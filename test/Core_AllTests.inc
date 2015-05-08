//#include <Windows.h>

#include <set>
#include <thread>

#include <e3_CommonMacros.h>
#include <core/Settings.h>
#include <core/ParameterShaper.h>
#include <core/Link.h>
#include <core/Module.h>
#include <core/Sink.h>
#include <core/Polyphony.h>
#include <core/Instrument.h>
#include <core/Bank.h>
#include <core/BankSerializer.h>
#include <core/CpuMeter.h>
#include <modules/ModuleFactory.h>
#include <modules/AudioOutTerminal.h>
#include <modules/SineOscillator.h>


namespace e3 {
    namespace testing {

        const char* testBank_valid_filename            = "TestBank.e3mb";
        const char* testBank_invalid_xml_filename      = "TestBank_invalid_xml.e3mb";
        const char* testBank_incompatible_xml_filename = "TestBank_incompatible_xml.e3mb";

        class TestableSettings : public Settings
        {
        public:
            using Settings::file_;

            bool testValid()
            {
                bool result = true;
                XmlElement* e = getElement( "Application" );
                result &= (nullptr != e);
                bool autosave = e->getBoolAttribute( "Autosave" );
                result &= autosave;

                return result;
            }
        };

        class SettingsTest : public ::testing::Test
        {
        public:
            SettingsTest() {
                settings_.setPath( "e3m_valid.settings" );   // load settings file from current dir
            }
            TestableSettings settings_;
        };

        TEST_F( SettingsTest, setPathAbsoluteOrRelative )
        {
            const char* filename = "e3m_valid.settings";
            File f1, f2;
            String path = File::getCurrentWorkingDirectory().getChildFile( filename ).getFullPathName();

            settings_.setPath( filename );
            f1 = settings_.file_;

            settings_.setPath( filename );
            f2 = settings_.file_;

            EXPECT_EQ( f1, f2 );
        }

        TEST_F( SettingsTest, loadFileNotExisting )
        {
            settings_.setPath( "gulufaz" );
            settings_.load();
            EXPECT_TRUE( settings_.testValid() );
        }

        TEST_F( SettingsTest, storeFileNotExisting )
        {
            settings_.setPath( "gulufaz" );
            settings_.load();
            settings_.store();

            File f = settings_.getPath();
            bool exists = f.existsAsFile();
            EXPECT_TRUE( exists );

            if (exists) {
                EXPECT_TRUE( f.deleteFile() );
                EXPECT_FALSE( f.existsAsFile() );
            }
        }

        TEST_F( SettingsTest, createDefaultFile )
        {
            File backup;
            settings_.setPath( "" );
            File f = settings_.getPath();

            if (f.existsAsFile()) {     // backup and remove existing settings file
                backup = File::getCurrentWorkingDirectory().getChildFile( "e3m_settings_backup_during_test" );
                f.copyFileTo( backup );
                f = settings_.getPath();
            }
            settings_.load();   // should create default settings file in default location
            EXPECT_TRUE( settings_.testValid() );
            EXPECT_TRUE( f.existsAsFile() );

            //  cleanup
            if (backup != File()) {
                backup.moveFileTo( f );
                EXPECT_TRUE( f.existsAsFile() );
            }
        }

        TEST_F( SettingsTest, invalidFile )
        {
            settings_.setPath( "e3m_invalid.settings" );
            settings_.load();   // parsing fails, but uses default settings
            EXPECT_TRUE( settings_.testValid() );
        }

        TEST_F( SettingsTest, hasStyle )
        {
            settings_.load();
            XmlElement* e = settings_.getStyleXml();
            ASSERT_NE( nullptr, e );
            EXPECT_TRUE( e->getStringAttribute( "name" ) == "Default" );
        }


        //-----------------------------------------------------------------------------
        // ParameterTest
        //-----------------------------------------------------------------------------

        TEST( ParameterTest, eqOperator )
        {
            Parameter p1( 17, 18, "test1", ControlSlider );
            Parameter p2( 17, 18, "test2", ControlBiSlider );
            EXPECT_EQ( p1, p2 );
        }

        //-----------------------------------------------------------------------------
        // ParameterSetTest
        //-----------------------------------------------------------------------------

        class ParameterSetTest : public ::testing::Test
        {
        public:
            ParameterSetTest() 
            {
                Parameter arr[] = { 
                    Parameter( 0, 2 ), Parameter( 1, 2 ),  Parameter( 2, 2 ),
                    Parameter( 0, 1 ), Parameter( 1, 1 ),  Parameter( 2, 1 ),
                    Parameter( 0, 0 ), Parameter( 1, 0 ),  Parameter( 2, 0 )
                };
                for (int i = 0; i < ARRAY_SIZE( arr ); i++) {
                    auto result = set_.insert( arr[i] );
                }
            }
            ParameterSet set_;
        };

        TEST_F( ParameterSetTest, sorting )
        {
            Parameter prev(-1, -1);
            for (auto it = set_.begin(); it != set_.end(); ++it)
            {
                const Parameter& next = *it;
                EXPECT_GE( next.id_, prev.id_ );
                EXPECT_GE( next.ownerId_, prev.ownerId_ );
            }
        }

        TEST_F( ParameterSetTest, add )
        {
            Parameter p1( 0, 3 );
            EXPECT_NO_THROW( set_.add( Parameter( 0, 3 ) ) );
            EXPECT_NO_THROW( set_.add( Parameter( 17, 138 ) ) );

            EXPECT_THROW( set_.add( Parameter( 0, 3 ) ), Assertion );
            EXPECT_THROW( set_.add( Parameter( 0, -1 ) ), Assertion );
            EXPECT_THROW( set_.add( Parameter( -1, -1 ) ), Assertion );
            EXPECT_THROW( set_.add( Parameter( -1, 0 ) ), Assertion );
        }


        TEST_F( ParameterSetTest, ownerFirstLast )
        {
            int count = 0;
            for (ParameterSet::iterator it = set_.ownerFirst( 1 ); it != set_.ownerLast( 1 ); ++it)
            {
                const Parameter& p = *it;
                EXPECT_EQ( 1, p.ownerId_ );
                count++;
            }
            EXPECT_EQ( 3, count );

            for (ParameterSet::iterator it = set_.ownerFirst( 17 ); it != set_.ownerLast( 17 ); ++it)
            {
                EXPECT_TRUE( false );
            }
        }

        TEST_F( ParameterSetTest, eraseAllByOwner )
        {
            set_.eraseAllByOwner( 0 );
            EXPECT_EQ( 6, set_.size() );
            set_.eraseAllByOwner( 17 );
            EXPECT_EQ( 6, set_.size() );
        }


        
        //-----------------------------------------------------------------------------
        // ParameterShaperTest
        //-----------------------------------------------------------------------------

        class TestableParameterShaper : public ParameterShaper
        {
        public:
            TestableParameterShaper( double min, double max, int16_t steps, int16_t factor ) :
                ParameterShaper( min, max, steps, factor )
            {}
            using ParameterShaper::normalize;
            using ParameterShaper::denormalize;
            using ParameterShaper::validate;
        };

        class ParameterShaperTest : public ::testing::Test
        {
        public:
            ParameterShaperTest() {}

            void LogarithmicAndLinearTest( double value, double min, double max )
            {
                ParameterShaper shaper( min, max );
                double exp = shaper.exponential( value );
                double lin = shaper.linear( exp );

                EXPECT_NEAR( value, lin, 0.0001 );
            }

            void NormalizeAndDenormalizeTest( double value, double min, double max )
            {
                TestableParameterShaper shaper( min, max, 100, 12 );
                double norm   = shaper.normalize( value );
                double denorm = shaper.denormalize( norm );

                EXPECT_EQ( value, denorm );
            }

            void OutOfRangeTest( double value, double min, double max )
            {
                TestableParameterShaper shaper( min, max, 100, 12 );
                double bounded = shaper.bound( value );
                double norm    = shaper.normalize( bounded );
                norm           = shaper.validate( norm );
                double denorm  = shaper.denormalize( norm );

                EXPECT_EQ( bounded, denorm );
            }


            void FactorTest( double value, int factor )
            {
                // error becomes bigger with factor
                double error = factor != 0 ? pow( abs( factor ), 2 ) * 0.00000000000001 : 0.00000000000001;
                double min = 0;
                double max = 10;

                // test positive factors
                ParameterShaper shaper( min, max, 100, factor );
                double exp1 = shaper.exponential( value );
                double lin1 = shaper.linear( exp1 );

                EXPECT_NEAR( value, lin1, error );

                // test negative factors
                if (factor < 9)
                {
                    ParameterShaper shaper( min, max, 100, -factor );
                    double exp2 = shaper.exponential( value );
                    double lin2 = shaper.linear( exp2 );

                    EXPECT_NEAR( value, lin2, error );
                }
            }
        };

        TEST_F( ParameterShaperTest, LinearAndLogarithmic )
        {
            LogarithmicAndLinearTest( 2, 0, 4 );
            LogarithmicAndLinearTest( 0, 0, 4 );
            LogarithmicAndLinearTest( 4, 0, 4 );
            LogarithmicAndLinearTest( 0, -10, 10 );
            LogarithmicAndLinearTest( 5, -10, 10 );
            LogarithmicAndLinearTest( -5, -10, 10 );
            LogarithmicAndLinearTest( -5, -7, 33 );
            LogarithmicAndLinearTest( 29, -7, 33 );
            LogarithmicAndLinearTest( -5, -10, 0 );
            LogarithmicAndLinearTest( -12, -12, -2 );
            LogarithmicAndLinearTest( -11, -12, -2 );
        }


        TEST_F( ParameterShaperTest, NormalizeAndDenormalize )
        {
            NormalizeAndDenormalizeTest( 2, 0, 4 );
            NormalizeAndDenormalizeTest( 0, 0, 4 );
            NormalizeAndDenormalizeTest( 4, 0, 4 );
            NormalizeAndDenormalizeTest( 0, -10, 10 );
            NormalizeAndDenormalizeTest( 5, -10, 10 );
            NormalizeAndDenormalizeTest( -5, -10, 10 );
            NormalizeAndDenormalizeTest( -5, -7, 33 );
            NormalizeAndDenormalizeTest( 29, -7, 33 );
            NormalizeAndDenormalizeTest( -5, -10, 0 );
            NormalizeAndDenormalizeTest( -12, -12, -2 );
            NormalizeAndDenormalizeTest( -11, -12, -2 );

            // min > max
            NormalizeAndDenormalizeTest( 2, 4, 0 );
            NormalizeAndDenormalizeTest( 0, 4, 0 );
            NormalizeAndDenormalizeTest( 4, 4, 0 );
            NormalizeAndDenormalizeTest( 0, 10, -10 );
            NormalizeAndDenormalizeTest( 5, 10, -10 );
            NormalizeAndDenormalizeTest( -5, 10, -10 );
            NormalizeAndDenormalizeTest( -5, 33, -7 );
            NormalizeAndDenormalizeTest( 29, 33, -7 );
            NormalizeAndDenormalizeTest( -5, 0, -10 );
            NormalizeAndDenormalizeTest( -12, -2, -12 );
            NormalizeAndDenormalizeTest( -11, -2, -12 );
        }


        TEST_F( ParameterShaperTest, OutOfRange )
        {
            OutOfRangeTest( 2, 0, 1 );       // val>max
            OutOfRangeTest( -2, 0, 1 );      // val<max
            OutOfRangeTest( 12, -4, 4 );     // val>max
            OutOfRangeTest( -12, -4, 4 );    // val<max

            OutOfRangeTest( 2, 0, 0 );       // min==max
        }

        TEST_F( ParameterShaperTest, Factor )
        {
            for (int f = 0; f < 100; f++) {
                for (double v = 0; v < 10; v++)
                {
                    FactorTest( v, f );
                }
            }

            EXPECT_THROW( FactorTest( 0, 10000 ), std::runtime_error );   // test numeric overflow
            EXPECT_THROW( FactorTest( 5, 10000 ), std::runtime_error );
            EXPECT_THROW( FactorTest( 10, 10000 ), std::runtime_error );
        }


        //----------------------------------------------------------------------------------------
        // MidiParameterValueTest
        //----------------------------------------------------------------------------------------

        //TEST(MidiParameterValueTest, Constructor)
        //{
        //    MidiParameterValue midiValue;
        //    EXPECT_EQ(midiValue.controllerId_, -1);

        //    ParameterShaper paramValue(17, 0, 17);
        //    midiValue = MidiParameterValue(paramValue);
        //    EXPECT_EQ(midiValue.getValue(), 17);
        //}

        //----------------------------------------------------------------------------------------
        // ParameterMapTest
        //----------------------------------------------------------------------------------------

        class ParameterMapTest : public ::testing::Test
        {
        public:
            ParameterMapTest() {
                Parameter p;
                p.id_ = id_;
                map_.add( p );
            }

            ParameterMap map_;
            uint16_t id_ = 17;
        };

        TEST_F( ParameterMapTest, InsertAndGet )
        {
            Parameter& p = map_.at( id_ );

            EXPECT_EQ( id_, p.id_ );
            EXPECT_THROW( map_.at( 0 ), std::out_of_range );
        }



        //----------------------------------------------------------------------------------------
        // LinkTest
        //----------------------------------------------------------------------------------------

        class LinkTest : public ::testing::Test
        {
        public:
            LinkTest() {
                link_.leftModule_  = 12;
                link_.rightModule_ = 45;
                link_.rightPort_   = 67;
                link_.leftPort_    = 89;
            }

            Link link_;
        };

        TEST_F( LinkTest, ConstructionAndAssignment )
        {
            EXPECT_EQ( 12, link_.leftModule_ );

            Link l1 = link_;
            EXPECT_EQ( 12, l1.leftModule_ );
        }

        TEST_F( LinkTest, OperatorEquals )
        {
            Link l1 = link_;
            EXPECT_EQ( link_, l1 );
            EXPECT_EQ( link_, &l1 );

            l1.leftModule_ = 0;
            EXPECT_NE( link_, l1 );
        }


        //----------------------------------------------------------------------------------------
        // LinkListTest
        //----------------------------------------------------------------------------------------
        class LinkListTest : public ::testing::Test
        {
        public:
            LinkListTest()
            {
                link_.leftModule_ = link_.rightModule_ = link_.rightPort_ = link_.leftPort_ = 3;

                Link l;
                for (int i = 1; i <= 4; i++) {
                    l.leftModule_ = l.rightModule_ = l.rightPort_ = l.leftPort_ = i;
                    list_.push_back( l );
                }
            }
            LinkList list_;
            Link link_;
        };

        TEST_F( LinkListTest, get )
        {
            Link l1 = list_.get( link_ );
            EXPECT_EQ( link_, l1 );
        }


        TEST_F( LinkListTest, contains )
        {
            EXPECT_TRUE( list_.contains( link_ ) );
        }

        TEST_F( LinkListTest, update )
        {
            link_.id_ = 117;
            list_.replace( link_ );
            EXPECT_TRUE( list_.contains( link_ ) );
        }

        TEST_F( LinkListTest, remove )
        {
            list_.remove( link_ );
            EXPECT_FALSE( list_.contains( link_ ) );
        }

        TEST_F( LinkListTest, getIndex )
        {
            EXPECT_EQ( 2, list_.getIndex( link_ ) );
        }



        //----------------------------------------------------------------------------------------
        // ModuleTest
        //----------------------------------------------------------------------------------------

        static ModuleType testModuleTypes[] ={
            ModuleTypeAudioOutTerminal,
            ModuleTypeMidiGate,
            ModuleTypeMidiFrequency,
            ModuleTypeMidiInput,
            ModuleTypeSineOscillator,
            ModuleTypeAdsrEnvelope,
            ModuleTypeDelay,
        };

        typedef std::map<ModuleType, std::vector<int>> ModuleTestMap;
        static ModuleTestMap moduleTestMap =
        {
            { testModuleTypes[0], { ProcessAudio, Monophonic, 1, 0, 1 } },
            { testModuleTypes[1], { ProcessEvent, Polyphonic, 0, 1, 0 } },
            { testModuleTypes[2], { ProcessEvent | ProcessControl, Polyphonic, 0, 1, 3 } },
            { testModuleTypes[3], { ProcessEvent | ProcessControl, Polyphonic, 0, 2, 3 } },
            { testModuleTypes[4], { ProcessAudio, Polyphonic, 2, 1, 2 } },
            { testModuleTypes[5], { ProcessAudio | ProcessControl, Polyphonic, 2, 1, 4 } },
            { testModuleTypes[6], { ProcessAudio, Polyphonic, 1, 1, 3 } },
        };

        class TestableModule : public Module
        {
        public:
            TestableModule( ModuleType type, VoicingType voicingType=Polyphonic ) : Module(
                type,
                "TestableModule",
                voicingType,
                ProcessAudio )
            {}

            using Module::selectVoiceAdapter;
        };

        class TestableSineOscil : public SineOscillator
        {
        public:
            using Module::init;
            using Module::connectPort;
            using Module::update;
            using Module::disconnectPorts;
        };

        class TestableAudioOutTerminal : public AudioOutTerminal
        {
        public:
            using Module::init;
            using Module::connectPort;
            using Module::update;
            using Module::disconnectPorts;
        };

        class ModuleTest : public ::testing::Test
        {
        public:
            ModuleTest() 
            {
                polyphony_.setNumVoices( 1 );
            }

            void testInitialization( ModuleType moduleType )
            {
                ScopedPointer<Module> module = ModuleFactory::create( moduleType );
                std::vector<int>& expected = moduleTestMap[moduleType];

                EXPECT_EQ( expected[0], module->processingType_ );
                EXPECT_EQ( expected[1], module->getVoicingType() );
                EXPECT_EQ( expected[2], module->getNumInports() );
                EXPECT_EQ( expected[3], module->getNumOutports() );
                EXPECT_EQ( expected[4], module->getNumParameters() );
            }

            ScopedPointer<TestableSineOscil> sine_;
            ScopedPointer<TestableAudioOutTerminal> audioOutTerminal_;
            Polyphony polyphony_;

            void connect()
            {
                sine_ = new TestableSineOscil();
                audioOutTerminal_ = new TestableAudioOutTerminal();
                EXPECT_NE( nullptr, sine_ );
                EXPECT_NE( nullptr, audioOutTerminal_ );

                sine_->setId( 1 );
                audioOutTerminal_->setId( 0 );
                sine_->init( 44100, 1, &polyphony_ );
                audioOutTerminal_->init( 44100, 1, &polyphony_ );

                Link link;
                link.value_ = 1;    // modulation value
                link.leftModule_ = 1;
                link.rightModule_ = 0;
                link.leftPort_ = 0;
                link.rightPort_ = 0;
                sine_->connectPort( audioOutTerminal_, link );

                sine_->update();
                audioOutTerminal_->update();
            }
        };

        TEST_F( ModuleTest, ModuleInitialization )
        {
            ModuleTestMap::const_iterator it = moduleTestMap.begin();
            for (; it != moduleTestMap.end(); it++)
            {
                testInitialization( it->first );
            }
        }

        TEST_F( ModuleTest, getDefaultParameters )
        {
            AudioOutTerminal t;
            t.setId( 0 );
            ParameterSet& set = t.getDefaultParameters();
            int size = set.size();
            EXPECT_EQ( size, t.getDefaultParameters().size() );
        }

        TEST_F( ModuleTest, ParameterMap_setDefaultValues )
        {
            ScopedPointer<Module> module = ModuleFactory::create( ModuleTypeAudioOutTerminal );
            module->setDefaultParameters();

            const Parameter& param = module->getParameter( 0 );
            EXPECT_EQ( param.defaultValue_, param.value_ );
        }

        TEST_F( ModuleTest, Parameter_ConstructAndCopy )
        {
            Parameter p1, p2;
            EXPECT_EQ( p1.id_, 0 );
            EXPECT_EQ( p1.defaultValue_, 1 );

            p1.id_ = 177;
            p1.defaultValue_ = -33;
            p2 = p1;

            EXPECT_EQ( p1.id_, p2.id_ );
            EXPECT_EQ( p1.defaultValue_, p2.defaultValue_ );
        }

        TEST_F( ModuleTest, selectVoiceAdapter )
        {
            TestableModule audioOut( ModuleTypeAudioOutTerminal, Monophonic );
            TestableModule sine( ModuleTypeSineOscillator );

            ASSERT_EQ( Monophonic, audioOut.getVoicingType() );
            ASSERT_EQ( Polyphonic, sine.getVoicingType() );

            EXPECT_EQ( AdapterNone, audioOut.selectVoiceAdapter( Monophonic ) );
            EXPECT_EQ( AdapterMonoToPoly, audioOut.selectVoiceAdapter( Polyphonic ) );

            EXPECT_EQ( AdapterNone, sine.selectVoiceAdapter( Polyphonic ) );
            EXPECT_EQ( AdapterPolyToMono, sine.selectVoiceAdapter( Monophonic ) );
        }

        TEST_F( ModuleTest, connect )
        {
            connect();
            Port* port = sine_->getOutport( 0 );
            EXPECT_TRUE( port->getNumConnections() > 0 );

            audioOutTerminal_->disconnectPorts();
            sine_->disconnectPorts();
            EXPECT_FALSE( port->getNumConnections() > 0 );
        }

        TEST_F( ModuleTest, sendThroughPorts )
        {
            connect();
            Outport* outport = sine_->getOutport( 0 );
            Inport* inport   = audioOutTerminal_->getInport( 0 );

            outport->putAudio( 17, 0 );
            double value = *inport->getAudioBuffer();
            EXPECT_EQ( 17, value );
        }



        //----------------------------------------------------------------------------------------
        // InstrumentTest
        //----------------------------------------------------------------------------------------

        class TestableInstrument : public Instrument
        {
        public:
            TestableInstrument() : Instrument() {}
            //TestableInstrument(const Instrument& other) : Instrument(other) {}

            using Instrument::getModule;
            using Instrument::createUniqueId;
            using Instrument::hasAudioOutTerminal;
        };

        class InstrumentTest : public ::testing::Test
        {
        public:
            InstrumentTest()
            {
                for (uint16_t i = 0; i < ARRAY_SIZE( testModuleTypes ); i++)
                {
                    instrument_.createAndAddModule( testModuleTypes[i] );
                }
                instrument_.name_ = "testInstrument";
            }

            bool testModuleIdsAreUnique( const Instrument& instrument )
            {
                std::set<int16_t> testSet, uniqueSet;

                for (int16_t i = 0; i < 5; i++) {
                    testSet.insert( (int16_t)(floor( i / 2.0 ) * 2) );
                }
                EXPECT_EQ( 3, testSet.size() );  // test if set stores unique values

                const ModuleList& list = instrument.getModules();
                for (ModuleList::const_iterator it = list.begin(); it != list.end(); it++) {
                    uniqueSet.insert( (*it)->getId() );
                }
                return instrument.getNumModules() == uniqueSet.size();
            }

            TestableInstrument instrument_;
        };


        TEST_F( InstrumentTest, deleteModules )
        {
            instrument_.deleteModules();
            EXPECT_EQ( 0, instrument_.getNumModules() );
        }


        TEST_F( InstrumentTest, getModule )
        {
            Module* m = instrument_.getModule( 2 );
            EXPECT_NE( nullptr, m );
        }

        TEST_F( InstrumentTest, createUniqueId )
        {
            Module* m = nullptr;

            uint16_t id = instrument_.createUniqueId( ModuleTypeAudioOutTerminal );
            EXPECT_EQ( ModuleTypeAudioOutTerminal, id );

            // erase single element and append new element
            int n = instrument_.getNumModules();
            for (uint16_t i = 1; i < n; i++)
            {
                m = instrument_.getModule( i );
                ASSERT_NE( nullptr, m );
                instrument_.deleteModule( m );
                m = instrument_.createAndAddModule( ModuleTypeMidiInput );

                EXPECT_EQ( i, m->getId() );
                EXPECT_TRUE( testModuleIdsAreUnique( instrument_ ) );
            }

            instrument_.deleteModules();
            m = instrument_.createAndAddModule( ModuleTypeMidiInput );

            ASSERT_NE( nullptr, m );
            EXPECT_EQ( 1, m->getId() );
        }

        TEST_F( InstrumentTest, hasAudioOutTerminal )
        {
            EXPECT_TRUE( instrument_.hasAudioOutTerminal() );
        }


        //---------------------------------------------------
        // BankSerializerTest
        //---------------------------------------------------
        class TestableBankSerializer : public BankSerializer
        {
        public:
            using BankSerializer::checkPath;
            using BankSerializer::checkRoot;
        };

        class BankSerializerTest : public ::testing::Test
        {};


        TEST_F( BankSerializerTest, checkPath )
        {
            File file;
            std::string path;

            file = TestableBankSerializer::checkPath( testBank_valid_filename );
            EXPECT_TRUE( file.existsAsFile() );

            path = File::getCurrentWorkingDirectory().getChildFile( testBank_valid_filename ).getFullPathName().toStdString();
            file = TestableBankSerializer::checkPath( path );
            EXPECT_TRUE( file.existsAsFile() );

            path = "slime.txt";
            EXPECT_THROW( TestableBankSerializer::checkPath( path ), std::invalid_argument );

            path = File::getCurrentWorkingDirectory().getChildFile( path.c_str() ).getFullPathName().toStdString();
            EXPECT_THROW( TestableBankSerializer::checkPath( path ), std::invalid_argument );
        }

        TEST_F( BankSerializerTest, parse_ok )
        {
            std::unique_ptr<XmlElement> xml;

            File file = File::getCurrentWorkingDirectory().getChildFile( testBank_valid_filename );
            xml.reset( XmlDocument::parse( file ) );
            EXPECT_NE( nullptr, xml.get() );
            TestableBankSerializer::checkRoot( xml.get() );
        }


        TEST_F( BankSerializerTest, parse_error )
        {
            File file = File::getCurrentWorkingDirectory().getChildFile( "BankSerializerTest_parse_nofile" );
            EXPECT_THROW( TestableBankSerializer::checkPath( file.getFullPathName().toStdString() ), std::invalid_argument );

            file = File::getCurrentWorkingDirectory().getChildFile( testBank_incompatible_xml_filename );
            EXPECT_THROW( TestableBankSerializer::readBank( file.getFullPathName().toStdString() ), std::runtime_error );

            file = File::getCurrentWorkingDirectory().getChildFile( testBank_invalid_xml_filename );
            EXPECT_THROW( TestableBankSerializer::readBank( file.getFullPathName().toStdString() ), std::runtime_error );
        }

        //---------------------------------------------------
        // BankTest
        //---------------------------------------------------
        class TestableBank : public Bank
        {
        public:
        };

        class BankTest : public ::testing::Test
        {
        public:
            BankTest() {
                Settings::getInstance().load();
            }

            std::string makeAbsolutePath( const std::string& filename )
            {
                return File::getCurrentWorkingDirectory().getChildFile( StringRef(filename)).getFullPathName().toStdString();
            }

            Polyphony polyphony_;
            Settings settings_;
            TestableBank bank_;

            void checkBank( Bank& bank )
            {
                EXPECT_EQ( "test", bank.getName() );
                EXPECT_NE( nullptr, bank.getXml() );
            }

            void checkInstrument( Instrument* i )
            {
                ASSERT_NE( nullptr, i );

                polyphony_.setNumVoices( i->numVoices_ );

                TestableInstrument* instrument = static_cast<TestableInstrument*>(i);
                instrument->initModules( 44100, 32, &polyphony_ );
                instrument->connectModules();

                EXPECT_EQ( "instrument_0", instrument->name_ );
                EXPECT_EQ( 32, instrument->numVoices_ );
                ASSERT_EQ( 4, instrument->getNumModules() );

                Module* audioOut = instrument->getModule( 0 );
                Module* input    = instrument->getModule( 1 );
                Module* sine     = instrument->getModule( 2 );
                Module* adsr     = instrument->getModule( 3 );

                EXPECT_NE( nullptr, audioOut );
                EXPECT_NE( nullptr, input );
                EXPECT_NE( nullptr, sine );
                EXPECT_NE( nullptr, adsr );

                EXPECT_EQ( Monophonic, audioOut->getVoicingType() );
                EXPECT_EQ( Polyphonic, input->getVoicingType() );
                EXPECT_EQ( Polyphonic, sine->getVoicingType() );
                EXPECT_EQ( Polyphonic, adsr->getVoicingType() );

                EXPECT_EQ( 1, audioOut->getNumInports() );
                EXPECT_EQ( 2, input->getNumOutports() );
                EXPECT_EQ( 2, sine->getNumInports() );
                EXPECT_EQ( 1, sine->getNumOutports() );
                EXPECT_EQ( 2, adsr->getNumInports() );
                EXPECT_EQ( 1, adsr->getNumOutports() );

                EXPECT_EQ( "AudioOut", audioOut->getLabel() );
                EXPECT_EQ( "MidiInput", input->getLabel() );
                EXPECT_EQ( "Sine", sine->getLabel() );
                EXPECT_EQ( "ADSR", adsr->getLabel() );

                EXPECT_EQ( ModuleTypeAudioOutTerminal, audioOut->moduleType_ );
                EXPECT_EQ( ModuleTypeMidiInput, input->moduleType_ );
                EXPECT_EQ( ModuleTypeSineOscillator, sine->moduleType_ );
                EXPECT_EQ( ModuleTypeAdsrEnvelope, adsr->moduleType_ );

                const LinkList& links = instrument->getLinks();
                ASSERT_EQ( links.size(), 4 );
                EXPECT_EQ( links[0].leftModule_, 3 );
                EXPECT_EQ( links[0].rightModule_, 0 );
                EXPECT_EQ( links[0].leftPort_, 0 );
                EXPECT_EQ( links[0].rightPort_, 0 );

                Module* m = instrument->getModule( links[0].leftModule_ );
                EXPECT_NE( nullptr, m );
                EXPECT_EQ( adsr, m );
                EXPECT_EQ( 3, m->getId() );
                EXPECT_EQ( ModuleTypeAdsrEnvelope, m->moduleType_ );

                //checkMidiParameters( input->getParameter(0) );
                //checkMidiParameters( input->getParameter(1) );
                //checkMidiParameters( input->getParameter(2) );
                //checkMidiParameters( sine ->getParameter(0) );
                //checkMidiParameters( sine ->getParameter(1) );
                //checkMidiParameters( instrument->getLinks()[1] );
                //checkMidiParameters( adsr->getParameter(0) );
                //checkMidiParameters( adsr->getParameter(1) );
                //checkMidiParameters( adsr->getParameter(2) );
                //checkMidiParameters( adsr->getParameter(3) );
                //checkMidiParameters( instrument->getLinks()[2] );
                //checkMidiParameters( instrument->getLinks()[3] );

                LinkList linksForAdsr;
                instrument->getLinksForModule( adsr->getId(), PortTypeUndefined, linksForAdsr );
                EXPECT_EQ( 3, linksForAdsr.size() );

                instrument->getLinksForModule( adsr->getId(), PortTypeInport, linksForAdsr );
                EXPECT_EQ( 2, linksForAdsr.size() );
                
                instrument->getLinksForModule( adsr->getId(), PortTypeOutport, linksForAdsr );
                EXPECT_EQ( 1, linksForAdsr.size() );

                Sink sink;
                EXPECT_NO_THROW( sink.compile( instrument ) );
                EXPECT_EQ( 4, sink.size() );
            }

            void checkMidiParameters( const Parameter& model )
            {
                EXPECT_EQ( -1, model.midiShaper_.getControllerId() );
                EXPECT_EQ( 0, model.midiShaper_.getControllerMin() );
                EXPECT_EQ( 127, model.midiShaper_.getControllerMax() );
                EXPECT_EQ( false, model.midiShaper_.getSoftTakeover() );
            }
        };

        TEST_F( BankTest, load_nofile )
        {
            EXPECT_THROW( bank_.load( "BankTest_load_error" ), std::invalid_argument );
        }

        TEST_F( BankTest, load_invalid_xml )
        {
            EXPECT_THROW( bank_.load( testBank_invalid_xml_filename ), std::runtime_error );
        }

        TEST_F( BankTest, load_incompatible_xml )
        {
            EXPECT_THROW( bank_.load( testBank_incompatible_xml_filename ), std::runtime_error );
        }

        TEST_F( BankTest, load_ok )
        {
            bank_.load( testBank_valid_filename );
            checkBank( bank_ );
        }

        TEST_F( BankTest, load_multiple )
        {
            for (int i = 0; i < 10; i++) {
                bank_.load( testBank_valid_filename );
                checkBank( bank_ );
            }
        }

        TEST_F( BankTest, load_instrument )
        {
            bank_.load( testBank_valid_filename );
            int id = bank_.getCurrentInstrumentId();
            ScopedPointer<Instrument> instrument = bank_.loadInstrument( id );
            checkInstrument( instrument );
        }

        TEST_F( BankTest, store )
        {
            std::string path = makeAbsolutePath( "TestBank_storeTest.e3mb" );

            bank_.load( testBank_valid_filename );
            bank_.save( path, false, false );
            bank_.load( path );
            checkBank( bank_ );

            ScopedPointer<Instrument> instrument = bank_.loadInstrument();
            ASSERT_NE( nullptr, instrument );
            instrument->deleteModules();
            bank_.saveInstrument( instrument );
            int id = instrument->id_;
            bank_.setCurrentInstrumentId( id );

            bank_.save( path, false, false );
            bank_.load( path );
            EXPECT_EQ( id, bank_.getCurrentInstrumentId() );

            instrument = bank_.loadInstrument();
            ASSERT_NE( nullptr, instrument );
            EXPECT_EQ( id, instrument->id_ );
            EXPECT_EQ( 0, instrument->getModules().size() );
            EXPECT_EQ( 0, instrument->getLinks().size() );
        }


        TEST_F( BankTest, saveInstrumentAttributes1 )
        {
            bank_.load( testBank_valid_filename );
            checkBank( bank_ );

            ScopedPointer<Instrument> instrument = bank_.loadInstrument();
            instrument->name_ = "testName";
            instrument->category_ = "testCategory";
            instrument->comment_ = "testComment";
            instrument->setNumVoices( 17 );
            instrument->setNumUnison( 18 );
            instrument->setUnisonSpread( 19 );
            instrument->setHold( true );
            instrument->setRetrigger( true );
            instrument->setLegato( true );
            bank_.saveInstrumentAttributes( instrument );

            std::string path = makeAbsolutePath( "TestBank_storeTest.e3mb" );
            File file( path );
            file.deleteFile();
            bank_.save( path, false, false );
            bank_.load( path );
            instrument = bank_.loadInstrument();

            ASSERT_NE( nullptr, instrument );
            EXPECT_EQ( "testName", instrument->name_ );
            EXPECT_EQ( "testCategory", instrument->category_ );
            EXPECT_EQ( "testComment", instrument->comment_ );
            EXPECT_EQ( 17, instrument->numVoices_ );
            EXPECT_EQ( 18, instrument->numUnison_ );
            EXPECT_EQ( 19, instrument->unisonSpread_ );
            EXPECT_TRUE( instrument->hold_ );
            EXPECT_TRUE( instrument->retrigger_ );
            EXPECT_TRUE( instrument->legato_ );

            instrument->name_ = "instrument_0";
            instrument->setNumVoices( 32 );
            checkInstrument( instrument );
        }



        //--------------------------------------------------------
        // class CpuMeterTest
        //--------------------------------------------------------
        //
        class CpuMeterTest : public ::testing::Test
        {
        public:
            CpuMeterTest() : sampleRate_( 44100 ) {}

            void sleepMicros( int64_t micros ) {
                std::this_thread::sleep_for( std::chrono::microseconds( micros ) );
            }

        protected:
            uint32_t sampleRate_;
            CpuMeter cpuMeter_;
        };


        TEST_F( CpuMeterTest, elapsed_time_1_second )
        {
            int64_t expected = 1000000;     // 1 second

            cpuMeter_.start();
            sleepMicros( expected );
            double elapsed = cpuMeter_.getElapsedSeconds() * 1000000;

            EXPECT_GE( elapsed, expected );
            EXPECT_LE( elapsed, expected + 20000 );
        }


        TEST_F( CpuMeterTest, elapsed_time_10_milliseconds )
        {
            double expected = 10000;     // 10 milliseconds

            cpuMeter_.start();
            sleepMicros( (int64_t)expected );
            double elapsed = cpuMeter_.getElapsedSeconds() * 1000000;

            EXPECT_GE( elapsed, expected );
            EXPECT_NEAR( elapsed, expected, 1000 );  // error within 1 millisecond
        }

        TEST_F( CpuMeterTest, elapsed_time_1_millisecond )
        {
            double expected = 1000;     // 1 millisecond

            cpuMeter_.start();
            sleepMicros( (int64_t)expected );
            double elapsed = cpuMeter_.getElapsedSeconds() * 1000000;

            EXPECT_GE( elapsed, expected );
            EXPECT_NEAR( elapsed, expected, 1000 );  // error within 1 millisecond
        }

        TEST_F( CpuMeterTest, cpu_percent_short_time )
        {
            bool ready;
            int runs = 1;
            int runsPerCycle = 1;
            int framesPerRun = sampleRate_ / runs;
            int microSeconds = 1000;
            cpuMeter_.setRunsPerCycle( runsPerCycle );

            for (int i = 0; i < runs; i++) {
                cpuMeter_.start();
                sleepMicros( microSeconds );
                ready = cpuMeter_.stop( framesPerRun );
            }
            double percent = ready ? cpuMeter_.getPercent() : 0;

            EXPECT_NEAR( 0.1, percent, 0.1 );
        }

        TEST_F( CpuMeterTest, cpu_percent_multiple_runs )
        {
            bool ready;
            int runs = 100;
            int runsPerCycle = 1;
            int framesPerRun = sampleRate_ / runs;
            int microSeconds = 500000 / runs;
            cpuMeter_.setRunsPerCycle( runsPerCycle );

            for (int i = 0; i < runs; i++) {
                cpuMeter_.start();
                sleepMicros( microSeconds );
                ready = cpuMeter_.stop( framesPerRun );
            }
            double percent = ready ? cpuMeter_.getPercent() : 0;

            EXPECT_NEAR( 50, percent, 15 );
        }

    } // namespace testing
} // namespace e3
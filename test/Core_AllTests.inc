//#include <Windows.h>

#include <set>
#include <thread>
#include <string>
#include <sstream>

#include <e3_CommonMacros.h>
#include <core/Settings.h>
#include <core/ParameterShaper.h>
#include <core/Link.h>
#include <core/Module.h>
#include <core/Sink.h>
#include <core/Polyphony.h>
#include <core/Instrument.h>
#include <core/Database.h>
#include <core/InstrumentSerializer.h>
#include <core/CpuMeter.h>
#include <modules/ModuleFactory.h>
#include <modules/AudioOutTerminal.h>
#include <modules/SineOscillator.h>


namespace e3 {
    namespace testing {

        const char* instrument_valid_file        = "TestInstrument.e3mi_";
        const char* instrument_invalid_file      = "TestInstrument_invalid.e3mi_";
        const char* instrument_incompatible_file = "TestInstrument_incompatible.e3mi_";

        class TestableSettings : public Settings
        {
        public:
            using Settings::file_;

            bool testValid()
            {
                bool result = true;
				XmlElement* e = nullptr;

				std::string tagNames[] = {
					"application", "style", "database", "standalone", "plugin",
					"standalone/window", "plugin/window", "standalone/audio-devices"
				};

				for( int i = 0; i < ARRAY_SIZE( tagNames ); i++ )
				{
					e = getElement( tagNames[i] );
					result &= ( e != nullptr );
				}

				e = getElement( "application" );
                bool autosavePresets = e->getBoolAttribute( "autosave-presets" );
                result &= autosavePresets;
				bool autosaveInstruments = e->getBoolAttribute( "autosave-instruments" );
				result &= autosaveInstruments;

				
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
            ParameterSet set;
            Parameter p1 = set.addModuleParameter( 17, 18, "test1", ControlSlider );
            EXPECT_THROW(Parameter p2 = set.addModuleParameter( 17, 18, "test2", ControlBiSlider ), e3::Assertion);
            Parameter p3 = set.addModuleParameter( 16, 18, "test3", ControlSlider );
            Parameter p4 = set.addLinkParameter( 15, 18 );

            EXPECT_NE( p1, p3 );
            EXPECT_NE( p1, p4 );
        }

        TEST( ParameterTest, ltOperator )
        {
            ParameterSet set;
            Parameter p1 = set.addLinkParameter( 1, 1 );
            Parameter p2 = set.addLinkParameter( 2, 1 );
            Parameter p3 = set.addLinkParameter( 1, 2 );
            Parameter p4 = set.addLinkParameter( 2, 2 );

            EXPECT_LT( p1, p2 );
            EXPECT_LT( p1, p3 );
            EXPECT_LT( p1, p4 );
        }

        TEST( ParameterTest, isValid )
        {
            ParameterSet set;
            Parameter p1 = set.addModuleParameter( 17, 18, "test1", ControlSlider );
            Parameter p2 = set.addLinkParameter( 18, 18 );

            EXPECT_EQ( 2, set.size() );
            EXPECT_TRUE( p1.isValid() );
            EXPECT_TRUE( p2.isValid() );
        }

        TEST( ParameterTest, targetType )
        {
            ParameterSet set;
            Parameter p1 = set.addModuleParameter( 17, 18, "test1", ControlSlider );
            Parameter p2 = set.addLinkParameter( 18, 18 );

            EXPECT_TRUE( p1.isModuleType() );
            EXPECT_TRUE( p2.isLinkType() );
        }


        //-----------------------------------------------------------------------------
        // ParameterSetTest
        //-----------------------------------------------------------------------------

        class ParameterSetTest : public ::testing::Test
        {
        public:
            ParameterSetTest() 
            {
                std::pair<int, int> arr[] = { 
                    { 0, 2 }, { 1, 2 }, { 2, 2 },
                    { 0, 1 }, { 1, 1 }, { 2, 1 },
                    { 0, 0 }, { 1, 0 }, { 2, 0 }
                };
                for (int i = 0; i < ARRAY_SIZE( arr ); i++) 
                {
                    set_.addLinkParameter( arr[i].first, arr[i].second );
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
                EXPECT_GE( next.getId(), prev.getId() );
                EXPECT_GE( next.getModuleId(), prev.getModuleId() );
            }
        }

        TEST_F( ParameterSetTest, add )
        {
            Parameter p1( 0, 3 );
            EXPECT_NO_THROW( set_.addLinkParameter( 0, 3 ) );
            EXPECT_NO_THROW( set_.addLinkParameter( 17, 138 ) );

            EXPECT_THROW( set_.addLinkParameter( 0, 3 ), Assertion );
            EXPECT_THROW( set_.addLinkParameter( 0, -1 ), Assertion );
            EXPECT_THROW( set_.addLinkParameter( -1, -1 ), Assertion );
            EXPECT_THROW( set_.addLinkParameter( -1, 0 ), Assertion );
        }


        TEST_F( ParameterSetTest, ownerFirstLast )
        {
            int count = 0;
            for (ParameterSet::iterator it = set_.moduleFirst( 1 ); it != set_.moduleLast( 1 ); ++it)
            {
                const Parameter& p = *it;
                EXPECT_EQ( 1, p.getModuleId() );
                count++;
            }
            EXPECT_EQ( 3, count );

            for (ParameterSet::iterator it = set_.moduleFirst( 17 ); it != set_.moduleLast( 17 ); ++it)
            {
                EXPECT_TRUE( false );
            }
        }

        TEST_F( ParameterSetTest, removeAllByModule )
        {
            set_.removeAllByModule( 0 );
            EXPECT_EQ( 6, set_.size() );
            set_.removeAllByModule( 17 );
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
        // LinkSetTest
        //----------------------------------------------------------------------------------------
        class LinkSetTest : public ::testing::Test
        {
        public:
            LinkSetTest()
            {
                Link l;
                for (int i = 0; i <= 3; i++) {
                    Link link( i, i, i, i, i );
                    set_.add( link );
                }
            }
            LinkSet set_;
        };

        TEST_F( LinkSetTest, getByReference )
        {
            Link l1( 3, 3, 3, 3, 3 );
            const Link& l2 = set_.get( l1 );
            EXPECT_EQ( l1, l2 );
        }

        TEST_F( LinkSetTest, getById )
        {
            Link link;
            for (size_t i = 0; i < set_.size(); i++)  
            {
                EXPECT_NO_THROW( link = set_.get( i ) );
                EXPECT_EQ( i, link.getId() );
            }
        }

        TEST_F( LinkSetTest, add )
        {
            size_t size = set_.size();
            Link link( -1, 4, 3, 2, 1 );
            size_t newSize = set_.add( link );
            EXPECT_EQ( size + 1, newSize );
            EXPECT_THROW( set_.add( link ), e3::Assertion );
        }

        TEST_F( LinkSetTest, contains )
        {
            Link link( 3, 3, 3, 3, 3 );
            EXPECT_TRUE( set_.contains( link ) );
        }

        TEST_F( LinkSetTest, removeByReference )
        {
            size_t size = set_.size();

            const Link& link = set_.get( 2 );
            EXPECT_EQ( 2, link.getId() );

            int newSize = set_.remove( link );
            EXPECT_EQ( size - 1, newSize );
            EXPECT_EQ( size - 1, set_.size() );
            EXPECT_FALSE( set_.contains( link ) );
            EXPECT_THROW( set_.get( link ), std::out_of_range );
        }


        TEST_F( LinkSetTest, removeById )
        {
            const Link& link = set_.get( 1 );
            size_t size = set_.size();

            size_t newSize = set_.remove( 1 );
            EXPECT_EQ( size - 1, newSize );
            EXPECT_EQ( size - 1, set_.size() );
            EXPECT_FALSE( set_.contains( link ) );
            EXPECT_THROW( set_.get( link ), std::out_of_range );
        }


        TEST_F( LinkSetTest, idsAreUnique )
        {
            set_.remove( 2 );
            int newId = set_.createUniqueId();
            EXPECT_EQ( 2, newId );

            Link link( newId, 1, 2, 3, 4 );
            set_.add( link );

            newId = set_.createUniqueId();
            EXPECT_EQ( set_.size(), newId );
        }


        //----------------------------------------------------------------------------------------
        // PresetSetTest
        //----------------------------------------------------------------------------------------
        class TestablePresetSet : public PresetSet
        {
        public:
            using PresetSet::createUniqueId;
        };

        class PresetSetTest : public ::testing::Test
        {
        public:
            PresetSetTest()
            {
                for (int i = 0; i <= 3; i++) {
                    set_.addPreset( i );
                }
            }
            TestablePresetSet set_;
        };


        TEST_F( PresetSetTest, initialization )
        {
            EXPECT_EQ( 4, set_.size() );
        }

        TEST_F( PresetSetTest, contains )
        {
            Preset p( 2 );
            EXPECT_TRUE( set_.contains( p.getId() ) );
        }

        TEST_F( PresetSetTest, remove )
        {
            size_t size = set_.size();
            set_.remove( 2 );
            EXPECT_EQ( size - 1, set_.size() );
        }

        TEST_F( PresetSetTest, createUniqueId )
        {
            int id = set_.createUniqueId();
            EXPECT_EQ( id, set_.size() );

            set_.remove( 1 );
            id = set_.createUniqueId();
            EXPECT_EQ( 1, id );
        }


        TEST_F( PresetSetTest, updateSelectedPreset )
        {
            set_.selectPreset(0);
            const Preset& preset1 = set_.getSelectedPreset();
            ParameterSet& params1 = preset1.getModuleParameters();
            int c1 = params1.size();
            params1.addModuleParameter( 3, 7, "Tune", ControlBiSlider, 0 );

            set_.updateSelectedPreset( preset1 );
            const Preset& preset2 = set_.getSelectedPreset();
            const ParameterSet& params2 = preset2.getModuleParameters();
            int c2 = params2.size();
            EXPECT_NE( c1, c2 );
        }

        TEST_F( PresetSetTest, copyPreset )
        {
            set_.selectPreset( 0 );
            const Preset& preset1 = set_.getSelectedPreset();
            ParameterSet& params1 = preset1.getModuleParameters();
            const Parameter& p1 = params1.addModuleParameter( 3, 7, "Tune", ControlBiSlider, 0 );
            int c1 = params1.size();

            Preset preset2 = preset1;
            ParameterSet& params2 = preset2.getModuleParameters();
            int c2 = params2.size();
            EXPECT_EQ( c1, c2 );

            const Parameter& p2 = params2.get( 3, 7 );
            EXPECT_EQ( p1, p2 );
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

        static ModuleType testAudioModuleTypes[] = {
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
            using Module::connect;
            using Module::update;
            using Module::disconnectPorts;
        };

        class TestableAudioOutTerminal : public AudioOutTerminal
        {
        public:
            using Module::init;
            using Module::connect;
            using Module::update;
            using Module::disconnectPorts;
        };

        class ModuleTest : public ::testing::Test
        {
        public:
            ModuleTest() 
            {
                polyphony_.setNumVoices( 1 );
                srand( static_cast<unsigned int>(time( nullptr )) );
            }

            void testInitialization( ModuleType moduleType )
            {
                ScopedPointer<Module> module = ModuleFactory::create( moduleType );
                std::vector<int>& expected = moduleTestMap[moduleType];

                EXPECT_EQ( expected[0], module->processingType_ );
                EXPECT_EQ( expected[1], module->getVoicingType() );
                EXPECT_EQ( expected[2], module->getNumInports() );
                EXPECT_EQ( expected[3], module->getNumOutports() );
            }

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

                PortData data;
                data.leftModule_ = 1;
                data.rightModule_ = 0;
                data.leftPort_ = 0;
                data.rightPort_ = 0;
                sine_->connect( audioOutTerminal_, data );

                sine_->update();
                audioOutTerminal_->update();
            }

            ScopedPointer<TestableSineOscil> sine_;
            ScopedPointer<TestableAudioOutTerminal> audioOutTerminal_;
            Polyphony polyphony_;
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

        TEST_F( ModuleTest, Parameter_ConstructAndCopy )
        {
            Parameter p1, p2;
            EXPECT_EQ( p1.getId(), -1 );
            EXPECT_EQ( p1.defaultValue_, 1 );

            p1 = Parameter( 177, -1 );
            p1.defaultValue_ = -33;
            p2 = p1;

            EXPECT_EQ( p1.getId(), p2.getId() );
            EXPECT_EQ( p1, p2 );
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


		//---------------------------------------------------
		// InstrumentSerializerTest
		//---------------------------------------------------

		class TestableInstrument : public Instrument
		{
		public:
			TestableInstrument() : Instrument() {}

			using Instrument::getModule;
			using Instrument::createUniqueId;
			using Instrument::hasAudioOutTerminal;
		};


		class TestableInstrumentSerializer : public InstrumentSerializer
		{
		public:
			using InstrumentSerializer::checkPath;
			using InstrumentSerializer::checkRoot;
		};

		class InstrumentSerializerTest : public ::testing::Test
		{
		public:
			Polyphony polyphony_;

			InstrumentSerializerTest()
			{
				Settings::getInstance().load();
			}

			std::string makeAbsolutePath( const std::string& filename )
			{
				return File::getCurrentWorkingDirectory().getChildFile( StringRef( filename ) ).getFullPathName().toStdString();
			}


			void checkInstrument( Instrument* i )
			{
				ASSERT_NE( nullptr, i );

				polyphony_.setNumVoices( i->numVoices_ );

				TestableInstrument* instrument = static_cast<TestableInstrument*>( i );
				instrument->initModules( 44100, 32, &polyphony_ );
				instrument->connectModules();

				EXPECT_EQ( "instrument_0", instrument->name_ );
				EXPECT_EQ( 32, instrument->numVoices_ );
				ASSERT_EQ( 4, instrument->getNumModules() );

				Module* audioOut = instrument->getModule( 0 );
				Module* input = instrument->getModule( 1 );
				Module* sine = instrument->getModule( 2 );
				Module* adsr = instrument->getModule( 3 );

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

				const LinkSet& links = instrument->getLinks();
				const Link& link = links.get( 0 );
				ASSERT_EQ( links.size(), 4 );
				EXPECT_EQ( link.leftModule_, 3 );
				EXPECT_EQ( link.rightModule_, 0 );
				EXPECT_EQ( link.leftPort_, 0 );
				EXPECT_EQ( link.rightPort_, 0 );

				Module* m = instrument->getModule( link.leftModule_ );
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


		TEST_F( InstrumentSerializerTest, load_nofile )
		{
			EXPECT_THROW( InstrumentSerializer::loadInstrument( "non_existing_file" ), std::invalid_argument );
		}

		TEST_F( InstrumentSerializerTest, load_invalid_xml )
		{
			EXPECT_EQ( nullptr, InstrumentSerializer::loadInstrument( instrument_invalid_file ));
		}

		TEST_F( InstrumentSerializerTest, load_incompatible_xml )
		{
			EXPECT_THROW( InstrumentSerializer::loadInstrument( instrument_incompatible_file ), std::runtime_error );
		}


		TEST_F( InstrumentSerializerTest, checkPath )
		{
			File file;
			std::string path;

			file = TestableInstrumentSerializer::checkPath( instrument_valid_file );
			EXPECT_TRUE( file.existsAsFile() );

			path = File::getCurrentWorkingDirectory().getChildFile( instrument_valid_file ).getFullPathName().toStdString();
			file = TestableInstrumentSerializer::checkPath( path );
			EXPECT_TRUE( file.existsAsFile() );

			path = "slime.txt";
			EXPECT_THROW( TestableInstrumentSerializer::checkPath( path ), std::invalid_argument );

			path = File::getCurrentWorkingDirectory().getChildFile( path.c_str() ).getFullPathName().toStdString();
			EXPECT_THROW( TestableInstrumentSerializer::checkPath( path ), std::invalid_argument );
		}

		TEST_F( InstrumentSerializerTest, parse_ok )
		{
			std::unique_ptr<XmlElement> xml;

			File file = File::getCurrentWorkingDirectory().getChildFile( instrument_valid_file );
			xml.reset( XmlDocument::parse( file ) );
			EXPECT_NE( nullptr, xml.get() );
			TestableInstrumentSerializer::checkRoot( xml.get() );
		}


		TEST_F( InstrumentSerializerTest, parse_error )
		{
			File file = File::getCurrentWorkingDirectory().getChildFile( "InstrumentSerializerTest_parse_nofile" );
			EXPECT_THROW( TestableInstrumentSerializer::checkPath( file.getFullPathName().toStdString()), std::invalid_argument);
			
			file = File::getCurrentWorkingDirectory().getChildFile( instrument_incompatible_file );
			EXPECT_THROW( TestableInstrumentSerializer::loadInstrument( file.getFullPathName().toStdString()), std::runtime_error); 
			
			file = File::getCurrentWorkingDirectory().getChildFile( instrument_invalid_file );
			EXPECT_EQ( nullptr, TestableInstrumentSerializer::loadInstrument( file.getFullPathName().toStdString()));
		}


		TEST_F( InstrumentSerializerTest, check_instrument )
		{
			ScopedPointer<Instrument> instrument = InstrumentSerializer::loadInstrument( instrument_valid_file );
			checkInstrument( instrument );
		}

		TEST_F( InstrumentSerializerTest, store )
		{
			std::string tempPath = makeAbsolutePath( "InstrumentTest_temp.e3mi_" );
			std::string validPath = makeAbsolutePath( instrument_valid_file );

			ScopedPointer<Instrument> instrument = InstrumentSerializer::loadInstrument( validPath );
			InstrumentSerializer::saveInstrument( instrument );
			instrument = InstrumentSerializer::loadInstrument( validPath );
			checkInstrument( instrument );
			File( tempPath ).deleteFile();
		}


		TEST_F( InstrumentSerializerTest, deleteAllModules )
		{
			std::string tempPath = makeAbsolutePath( "InstrumentTest_temp.e3mi_" );
			std::string validPath = makeAbsolutePath( instrument_valid_file );

			ScopedPointer<Instrument> instrument = InstrumentSerializer::loadInstrument( validPath );
			instrument->deleteModules();
			instrument->setFilePath( tempPath );
			InstrumentSerializer::saveInstrument( instrument );

			instrument = InstrumentSerializer::loadInstrument( tempPath );
			ASSERT_NE( nullptr, instrument );
			EXPECT_EQ( 0, instrument->getModules().size() );
			EXPECT_EQ( 0, instrument->getLinks().size() );

			XmlElement* xml = instrument->getXml();
			std::string tagNames[] = { "panel", "modules", "links" };
			for( int i = 0; i < ARRAY_SIZE( tagNames ); i++ )
			{
				XmlElement* e = xml->getChildByName( StringRef(tagNames[i]) );
				int numChildren = e->getNumChildElements();
				EXPECT_EQ( 0, numChildren );
			}

			File( tempPath ).deleteFile();
		}


		TEST_F( InstrumentSerializerTest, deleteSingleModule )
		{
			std::string tempPath = makeAbsolutePath( "InstrumentTest_temp.e3mi_" );
			std::string validPath = makeAbsolutePath( instrument_valid_file );
			int moduleId = 2;

			ScopedPointer<Instrument> instrument = InstrumentSerializer::loadInstrument( validPath );
			Module* module = instrument->getModule( moduleId );		// sine oscillator
			ASSERT_NE( nullptr, module );

			instrument->deleteModule( module );
			instrument->setFilePath( tempPath );
			InstrumentSerializer::saveInstrument( instrument );

			instrument = InstrumentSerializer::loadInstrument( tempPath );
			ASSERT_NE( nullptr, instrument );
			EXPECT_EQ( 3, instrument->getModules().size() );

			LinkList list;
			instrument->getLinksForModule( moduleId, PortTypeUndefined, list );
			EXPECT_EQ( 0, list.size() );

			ParameterSet& p = instrument->getCurrentPreset().getModuleParameters();
			for( ParameterSet::const_iterator it = p.moduleFirst( moduleId ); it != p.moduleLast( moduleId ); ) {
				THROW( std::runtime_error, "Not all module parameters for module were deleted" );
			}

			p = instrument->getCurrentPreset().getLinkParameters();
			for( ParameterSet::const_iterator it = p.moduleFirst( moduleId ); it != p.moduleLast( moduleId ); ) {
				THROW( std::runtime_error, "Not all link parameters for module were deleted" );
			}

			XmlElement* xml = instrument->getXml();
			XmlElement* e = xml->getChildByName( "panel" );
			int numChildren = e->getNumChildElements();
			EXPECT_EQ( 3, numChildren );

			File( tempPath ).deleteFile();
		}

		TEST_F( InstrumentSerializerTest, saveInstrumentAttributes1 )
		{
			std::string path = makeAbsolutePath( "InstrumentTest_temp.e3mi_" );

			ScopedPointer<Instrument> instrument = InstrumentSerializer::loadInstrument( instrument_valid_file );
			instrument->name_ = "testName";
			instrument->setNumVoices( 17 );
			instrument->setNumUnison( 18 );
			instrument->setUnisonSpread( 19 );
			instrument->setHold( true );
			instrument->setRetrigger( true );
			instrument->setLegato( true );
			instrument->setFilePath( path );

			InstrumentSerializer::saveAttributes( instrument );

			File file( path );
			file.deleteFile();
			InstrumentSerializer::saveInstrument( instrument );
			instrument = InstrumentSerializer::loadInstrument( path );

			ASSERT_NE( nullptr, instrument );
			EXPECT_EQ( "testName", instrument->name_ );
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



        //----------------------------------------------------------------------------------------
        // InstrumentTest
        //----------------------------------------------------------------------------------------

        class InstrumentTest : public ::testing::Test
        {
        public:
			TestableInstrument instrument_;


            InstrumentTest()  
			{
				for( uint16_t i = 0; i < ARRAY_SIZE( testModuleTypes ); i++ )
				{
					instrument_.createAndAddModule( testModuleTypes[i] );
				}
				instrument_.name_ = "testInstrument";
			}


			std::string makeAbsolutePath( const std::string& filename )
			{
				return File::getCurrentWorkingDirectory().getChildFile( StringRef( filename ) ).getFullPathName().toStdString();
			}


            bool testModuleIdsAreUnique( const Instrument& instrument )
            {
                std::set<int> testSet, uniqueSet;

                for (int16_t i = 0; i < 5; i++) {
                    testSet.insert( (int)(floor( i / 2.0 ) * 2) );
                }
                EXPECT_EQ( 3, testSet.size() );  // test if set stores unique values

                const ModuleList& list = instrument.getModules();
                for (ModuleList::const_iterator it = list.begin(); it != list.end(); it++) {
                    uniqueSet.insert( (*it)->getId() );
                }
                return instrument.getNumModules() == uniqueSet.size();
            }

            ModuleType getRandomAudioModuleType()
            {
                ModuleType types[] = { ModuleTypeSineOscillator, ModuleTypeAdsrEnvelope, ModuleTypeDelay };
                int r = rand() % ARRAY_SIZE( types ) ;
                ModuleType type( static_cast<ModuleType>(types[r]) );
                return type;
            }
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


        TEST_F( InstrumentTest, addAndRemoveLink )
        {
			const Preset& preset     = instrument_.getCurrentPreset();
            ParameterSet& parameters = preset.getLinkParameters();
            LinkSet& links           = instrument_.getLinks();

            for (size_t i = 0; i < 100; i++) {
                Link link( -1, i, i, i, i );
                try {
                    instrument_.addLink( link );
                }
                catch (const Assertion&) {}
                EXPECT_EQ( i+1, links.size() );
                EXPECT_EQ( i+1, parameters.size() );
            }

            size_t size = parameters.size();
            srand( static_cast<unsigned int>(time( nullptr )) );
            for (size_t i = 0; i < 100; i++)
            {
                int r = rand() % 100;
                try {
                    const Link& link = links.get( r );
                    instrument_.removeLink( link );
                    EXPECT_EQ( --size, parameters.size() );
                }
                catch (...) {}
            }
        }

        TEST_F( InstrumentTest, sendOverPortsManySources )
        {
			instrument_.deleteModules();
            Polyphony polyphony;

            Module* audioOut = instrument_.createAndAddModule( ModuleTypeAudioOutTerminal );

            for (int i = 0; i < 100; i++)
            {
                ModuleType type = getRandomAudioModuleType();
                Module* module = instrument_.createAndAddModule( type );
            }
            instrument_.initModules( 44100, 1, &polyphony );

            for (int i = 1; i < 101; i++) {
                Link link( -1, i, 0, audioOut->getId(), 0 );
                instrument_.addLink( link );
            }
            instrument_.connectModules();

            for (int i = 1; i < instrument_.getNumModules(); i++)
            {
                Module* module = instrument_.getModule( i );
                ASSERT_NE( nullptr, module );
                Outport* outport = module->getOutport( 0 );
                outport->putAudio( 1 );
            }
            Inport* inport = audioOut->getInport( 0 );
            double value   = *inport->getAudioBuffer();
            EXPECT_EQ( 100, value );
        }

        TEST_F( InstrumentTest, sendOverPortsManyTargets )
        {
			instrument_.deleteModules();
            Polyphony polyphony;

            Module* sine = instrument_.createAndAddModule( ModuleTypeSineOscillator );

            // create 100 AdsrEnvelopes
            for (int i = 0; i < 100; i++)  {
                Module* module = instrument_.createAndAddModule( ModuleTypeAdsrEnvelope );
            }
            instrument_.initModules( 44100, 1, &polyphony );

            // create 100 links from sine to the AdsrEnvelopes
            for (int i = 2; i < 102; i++)  {
                Link link( -1, sine->getId(), 0, i, 0 );
                instrument_.addLink( link );
            }
            instrument_.connectModules();

            // send some signals through the connections
            Outport* sineOutport = sine->getOutport( 0 );
            for (int i = 0; i < 1173; i++) {
                sineOutport->putAudio( 1 );
            }

            // check the signals
            for (int i = 2; i < instrument_.getNumModules(); i++)
            {
                Module* module = instrument_.getModule( i );
                ASSERT_NE( nullptr, module );
                Inport* inport = module->getInport( 0 );
                double value   = *inport->getAudioBuffer();
                EXPECT_EQ( 1173, value );
            }
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
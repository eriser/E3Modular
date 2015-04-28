#include <Windows.h>

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
#include <modules/Master.h>
#include <modules/SineOscil.h>


namespace e3 {
    namespace testing {

        const char* testBank_valid_filename            = "TestBank.e3mb";
        const char* testBank_invalid_xml_filename      = "TestBank_invalid_xml.e3mb";
        const char* testBank_incompatible_xml_filename = "TestBank_incompatible_xml.e3mb";

        class TestableSettings : public Settings
        {
        public:
            using Settings::file_;
            using Settings::getStyleXml;

            bool testValid() 
            {
                bool result = true;
                XmlElement* e = getElement("Application");
                result &= (nullptr != e);
                bool autosave = e->getBoolAttribute("Autosave");
                result &= autosave;

                return result;
            }
        };

        class SettingsTest : public ::testing::Test
        {
        public:
            SettingsTest() {
                settings_.setPath("e3m_valid.settings");   // load settings file from current dir
            }

            TestableSettings settings_;
        };

        TEST_F(SettingsTest, setPathAbsoluteOrRelative)
        {
            const char* filename = "e3m_valid.settings";
            File f1, f2;
            String path = File::getCurrentWorkingDirectory().getChildFile(filename).getFullPathName();

            settings_.setPath(filename);
            f1 = settings_.file_;

            settings_.setPath(filename);
            f2 = settings_.file_;

            EXPECT_EQ(f1, f2);
        }

        TEST_F(SettingsTest, loadFileNotExisting)
        {
            settings_.setPath("gulufaz");
            settings_.load();
            EXPECT_TRUE(settings_.testValid());
        }

        TEST_F(SettingsTest, storeFileNotExisting)
        {
            settings_.setPath("gulufaz");
            settings_.load();
            settings_.store();

            File f = settings_.getPath();
            bool exists = f.existsAsFile();
            EXPECT_TRUE(exists);

            if (exists) {
                EXPECT_TRUE(f.deleteFile());
                EXPECT_FALSE(f.existsAsFile());
            }
        }

        TEST_F(SettingsTest, createDefaultFile)
        {
            File backup;
            settings_.setPath("");
            File f = settings_.getPath();

            if (f.existsAsFile()) {     // backup and remove existing settings file
                backup = File::getCurrentWorkingDirectory().getChildFile("e3m_settings_backup_during_test");
                f.copyFileTo(backup);
                f = settings_.getPath();
            }
            settings_.load();   // should create default settings file in default location
            EXPECT_TRUE(settings_.testValid());
            EXPECT_TRUE(f.existsAsFile());

            //  cleanup
            if (backup != File()) {
                backup.moveFileTo(f);
                EXPECT_TRUE(f.existsAsFile());
            }
        }

        TEST_F(SettingsTest, invalidFile)
        {
            settings_.setPath("e3m_invalid.settings");
            settings_.load();   // parsing fails, but uses default settings
            EXPECT_TRUE(settings_.testValid());
        }

        TEST_F(SettingsTest, hasStyle)
        {
            settings_.load();
            XmlElement* e = settings_.getStyleXml();
            ASSERT_NE(nullptr, e);
            EXPECT_TRUE(e->getStringAttribute("name") == "Default");
        }

        TEST_F(SettingsTest, useDefaultStyle)
        {
            settings_.setPath("e3m_no_style.settings");
            settings_.load();
            XmlElement* e = settings_.getStyleXml();
            ASSERT_NE(nullptr, e);
            EXPECT_TRUE(e->getStringAttribute("name") == "Default");
        }


        //-----------------------------------------------------------------------------
        // ParameterShaperTest
        //-----------------------------------------------------------------------------

        class TestableParameterShaper : public ParameterShaper
        {
        public: 
            TestableParameterShaper(double min, double max, int16_t steps, int16_t factor) :
                ParameterShaper(min, max, steps, factor)
            {}
            using ParameterShaper::normalize;
            using ParameterShaper::denormalize;
            using ParameterShaper::validate;
        };

        class ParameterShaperTest : public ::testing::Test
        {
        public:
            ParameterShaperTest() {}

            void LogarithmicAndLinearTest(double value, double min, double max)
            {
                ParameterShaper shaper(min, max);
                double exp = shaper.exponential(value);
                double lin = shaper.linear(exp);

                EXPECT_NEAR(value, lin, 0.0001);
            }

            void NormalizeAndDenormalizeTest(double value, double min, double max)
            {
                TestableParameterShaper shaper(min, max, 100, 12);
                double norm   = shaper.normalize(value);
                double denorm = shaper.denormalize(norm);

                EXPECT_EQ(value, denorm);
            }

            void OutOfRangeTest(double value, double min, double max)
            {
                TestableParameterShaper shaper(min, max, 100, 12);
                double bounded = shaper.bound(value);
                double norm    = shaper.normalize(bounded);
                norm           = shaper.validate(norm);
                double denorm  = shaper.denormalize(norm);

                EXPECT_EQ(bounded, denorm);
            }


            void FactorTest(double value, int factor)
            {
                // error becomes bigger with factor
                double error = factor != 0 ? pow(abs(factor), 2) * 0.00000000000001 : 0.00000000000001;
                double min = 0;
                double max = 10;

                // test positive factors
                ParameterShaper shaper(min, max, 100, factor);
                double exp1 = shaper.exponential(value);
                double lin1 = shaper.linear(exp1);

                EXPECT_NEAR(value, lin1, error);

                // test negative factors
                if (factor < 9)
                {
                    ParameterShaper shaper(min, max, 100, -factor);
                    double exp2 = shaper.exponential(value);
                    double lin2 = shaper.linear(exp2);

                    EXPECT_NEAR(value, lin2, error);
                }
            }
        };

        TEST_F(ParameterShaperTest, LinearAndLogarithmic)
        {
            LogarithmicAndLinearTest(2, 0, 4);
            LogarithmicAndLinearTest(0, 0, 4);
            LogarithmicAndLinearTest(4, 0, 4);
            LogarithmicAndLinearTest(0, -10, 10);
            LogarithmicAndLinearTest(5, -10, 10);
            LogarithmicAndLinearTest(-5, -10, 10);
            LogarithmicAndLinearTest(-5, -7, 33);
            LogarithmicAndLinearTest(29, -7, 33);
            LogarithmicAndLinearTest(-5, -10, 0);
            LogarithmicAndLinearTest(-12, -12, -2);
            LogarithmicAndLinearTest(-11, -12, -2);
        }


        TEST_F(ParameterShaperTest, NormalizeAndDenormalize)
        {
            NormalizeAndDenormalizeTest(2, 0, 4);
            NormalizeAndDenormalizeTest(0, 0, 4);
            NormalizeAndDenormalizeTest(4, 0, 4);
            NormalizeAndDenormalizeTest(0, -10, 10);
            NormalizeAndDenormalizeTest(5, -10, 10);
            NormalizeAndDenormalizeTest(-5, -10, 10);
            NormalizeAndDenormalizeTest(-5, -7, 33);
            NormalizeAndDenormalizeTest(29, -7, 33);
            NormalizeAndDenormalizeTest(-5, -10, 0);
            NormalizeAndDenormalizeTest(-12, -12, -2);
            NormalizeAndDenormalizeTest(-11, -12, -2);

            // min > max
            NormalizeAndDenormalizeTest(2, 4, 0);
            NormalizeAndDenormalizeTest(0, 4, 0);
            NormalizeAndDenormalizeTest(4, 4, 0);
            NormalizeAndDenormalizeTest(0, 10, -10);
            NormalizeAndDenormalizeTest(5, 10, -10);
            NormalizeAndDenormalizeTest(-5, 10, -10);
            NormalizeAndDenormalizeTest(-5, 33, -7);
            NormalizeAndDenormalizeTest(29, 33, -7);
            NormalizeAndDenormalizeTest(-5, 0, -10);
            NormalizeAndDenormalizeTest(-12, -2, -12);
            NormalizeAndDenormalizeTest(-11, -2, -12);
        }


        TEST_F(ParameterShaperTest, OutOfRange)
        {
            OutOfRangeTest(2, 0, 1);       // val>max
            OutOfRangeTest(-2, 0, 1);      // val<max
            OutOfRangeTest(12, -4, 4);     // val>max
            OutOfRangeTest(-12, -4, 4);    // val<max

            OutOfRangeTest(2, 0, 0);       // min==max
        }

        TEST_F(ParameterShaperTest, Factor)
        {
            for (int f = 0; f < 100; f++) {
                for (double v = 0; v < 10; v++)
                {
                    FactorTest(v, f);
                }
            }

            EXPECT_THROW(FactorTest(0,  10000), std::runtime_error);   // test numeric overflow
            EXPECT_THROW(FactorTest(5,  10000), std::runtime_error);
            EXPECT_THROW(FactorTest(10, 10000), std::runtime_error);
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
                Parameter model;
                model.id_ = id_;
                map_.add(model);
            }

            ParameterMap map_;
            uint16_t id_ = 17;
        };

        TEST_F(ParameterMapTest, InsertAndGet)
        {
            Parameter& p = map_.at(id_);

            EXPECT_EQ(id_, p.id_);
            EXPECT_THROW(map_.at(0), std::out_of_range);
        }

        TEST_F(ParameterMapTest, SetModel)
        {
            std::string label = "testing";

            Parameter model;
            model.id_ = id_;
            model.label_ = label;
            map_.update(model);

            EXPECT_EQ(label, map_.at(id_).label_);

            model.id_ = 0;
            EXPECT_THROW(map_.update(model), std::out_of_range);
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

        TEST_F(LinkTest, ConstructionAndAssignment)
        {
            EXPECT_EQ(12, link_.leftModule_);
            
            Link l1 = link_;
            EXPECT_EQ(12, l1.leftModule_);
        }

        TEST_F(LinkTest, OperatorEquals)
        {
            Link l1 = link_;
            EXPECT_EQ(link_, l1);
            EXPECT_EQ(link_, &l1);

            l1.leftModule_ = 0;
            EXPECT_NE(link_, l1);
        }

        TEST_F(LinkTest, OperatorLess)
        {
            Link l1 = link_;
            l1.rightModule_ = 44;
            EXPECT_LT(l1, link_);
            EXPECT_GT(link_, l1);

            l1 = link_;
            l1.leftPort_ = 0;
            EXPECT_LT(l1, link_);
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
                    list_.push_back(l);
                }
            }
            LinkList list_;
            Link link_;
        };

        TEST_F(LinkListTest, get)
        {
            Link l1 = list_.get(link_);
            EXPECT_EQ(link_, l1);
        }


        TEST_F(LinkListTest, contains)
        {
            EXPECT_TRUE(list_.contains(link_));
        }

        TEST_F(LinkListTest, update)
        {
            link_.id_ = 117;
            list_.replace(link_);
            EXPECT_TRUE(list_.contains(link_));
        }

        TEST_F(LinkListTest, remove)
        {
            list_.remove(link_);
            EXPECT_FALSE(list_.contains(link_));
        }

        TEST_F(LinkListTest, getIndex)
        {
            EXPECT_EQ(2, list_.getIndex(link_));
        }



        //----------------------------------------------------------------------------------------
        // ModuleTest
        //----------------------------------------------------------------------------------------

        static ModuleType testModuleTypes[] = {
            kModuleMaster,
            kModuleMidiGate,
            kModuleMidiPitch,
            kModuleMidiInput,
            kModuleSineOscil,
            kModuleAdsrEnv,
            kModuleDelay,
        };

        typedef std::map<ModuleType, std::vector<int>> ModuleTestMap;
        static ModuleTestMap moduleTestMap =
        {
            { testModuleTypes[0], { kProcessAudio, kMonophonic, 1, 0, 1 } },
            { testModuleTypes[1], { kProcessEvent, kPolyphonic, 0, 1, 0 } },
            { testModuleTypes[2], { kProcessEvent | kProcessControl, kPolyphonic, 0, 1, 3 } },
            { testModuleTypes[3], { kProcessEvent | kProcessControl, kPolyphonic, 0, 2, 3 } },
            { testModuleTypes[4], { kProcessAudio, kPolyphonic, 3, 1, 4 } },
            { testModuleTypes[5], { kProcessAudio | kProcessControl, kPolyphonic, 2, 1, 6 } },
            { testModuleTypes[6], { kProcessAudio, kPolyphonic, 1, 1, 3 } },
        };      
        
        class TestableModule : public Module
        {
        public:
            TestableModule(ModuleType type, VoicingType voicingType=kPolyphonic) : Module(
                type,
                "TestableModule",
                voicingType,
                kProcessAudio)
            {}

            using Module::selectVoiceAdapter;
        };

        class TestableSineOscil : public SineOscil
        {
        public:
            using Module::connectPort;
            using Module::update;
            using Module::disconnectPorts;
        };

        class TestableMaster : public Master
        {
        public:
            using Module::connectPort;
            using Module::update;
            using Module::disconnectPorts;
        };

        class ModuleTest : public ::testing::Test
        {
        public:
            ModuleTest() {}

            void testInitialization(ModuleType moduleType)
            {
                ScopedPointer<Module> module = ModuleFactory::create(moduleType);
                std::vector<int>& expected = moduleTestMap[moduleType];

                EXPECT_EQ(expected[0], module->processingType_);
                EXPECT_EQ(expected[1], module->voicingType_);
                EXPECT_EQ(expected[2], module->inports_.size());
                EXPECT_EQ(expected[3], module->outports_.size());
                EXPECT_EQ(expected[4], module->parameters_.size());
            }

            ScopedPointer<TestableSineOscil> sine_;
            ScopedPointer<TestableMaster> master_;

            void connect()
            {
                sine_ = new TestableSineOscil();
                master_ = new TestableMaster();
                EXPECT_NE(nullptr, sine_);
                EXPECT_NE(nullptr, master_);

                sine_->id_ = 1;
                master_->id_ = 0;
                sine_->setNumVoices(1);
                master_->setNumVoices(1);
                sine_->update();
                master_->update();

                Link link;
                link.value_ = 1;    // modulation value
                link.leftModule_ = 1;
                link.rightModule_ = 0;
                link.leftPort_ = 0;
                link.rightPort_ = 0;
                master_->addLink(link);
                sine_->connectPort(master_, &link);
            }
        };


        TEST_F(ModuleTest, ModuleInitialization)
        {
            ModuleTestMap::const_iterator it = moduleTestMap.begin();
            for (; it != moduleTestMap.end(); it++)
            {
                testInitialization(it->first);
            }
        }

        TEST_F(ModuleTest, ParameterMap_setDefaultValues)
        {
            ScopedPointer<Module> module = ModuleFactory::create(kModuleMaster);
            module->parameters_.setDefaultValues();

            Parameter& param = module->parameters_.at(0);
            EXPECT_EQ(param.defaultValue_, param.value_);
        }

        TEST_F(ModuleTest, Parameter_ConstructAndCopy)
        {
            Parameter p1, p2;
            EXPECT_EQ(p1.id_, 0);
            EXPECT_EQ(p1.defaultValue_, 0);

            p1.id_ = 177;
            p1.defaultValue_ = -33;
            p2 = p1;

            EXPECT_EQ(p1.id_, p2.id_);
            EXPECT_EQ(p1.defaultValue_, p2.defaultValue_);
        }

        TEST_F(ModuleTest, addAndRemoveLink)
        {
            TestableModule module(kModuleMaster);
            module.id_ = 17;
            Link link;
            link.id_ = 19;
            module.addLink(link);

            EXPECT_EQ(1, module.links_.size());
            EXPECT_EQ(17, link.rightModule_);
            EXPECT_EQ(19, link.id_);

            module.removeLink(link);
            EXPECT_EQ(0, module.links_.size());
            EXPECT_EQ(1, module.removedLinks_.size());

            link.id_ = 0;
            EXPECT_TRUE(module.removedLinks_.contains(link));

            link = module.removedLinks_.get(link);
            EXPECT_EQ(19, link.id_);

            module.addLink(link);
            EXPECT_EQ(0, module.removedLinks_.size());
        }

        TEST_F(ModuleTest, selectVoiceAdapter)
        {
            TestableModule master(kModuleMaster, kMonophonic);
            TestableModule sine(kModuleSineOscil);

            ASSERT_EQ(kMonophonic, master.voicingType_);
            ASSERT_EQ(kPolyphonic, sine.voicingType_);
            
            EXPECT_EQ(kAdapterNone, master.selectVoiceAdapter(kMonophonic));
            EXPECT_EQ(kAdapterMonoToPoly, master.selectVoiceAdapter(kPolyphonic));

            EXPECT_EQ(kAdapterNone, sine.selectVoiceAdapter(kPolyphonic));
            EXPECT_EQ(kAdapterPolyToMono, sine.selectVoiceAdapter(kMonophonic));
        }

        TEST_F(ModuleTest, connect)
        {
            connect();
            Port* port = sine_->getOutport(0);
            EXPECT_TRUE(port->isConnected());

            master_->disconnectPorts();
            sine_->disconnectPorts();
            EXPECT_FALSE(port->isConnected());
        }

        TEST_F(ModuleTest, sendThroughPorts)
        {
            connect();
            AudioOutport* outport = dynamic_cast<AudioOutport*>(sine_->getOutport(0));
            AudioInport* inport   = dynamic_cast<AudioInport*>(master_->getInport(0));

            outport->putValue(17, 0);
            double value = *inport->getBuffer();
            EXPECT_EQ(17, value);
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
            using Instrument::createModuleId;
            using Instrument::hasMaster;
            using Instrument::connectModule;
        };

        class InstrumentTest : public ::testing::Test
        {
        public:
            InstrumentTest() 
            {
                for (uint16_t i = 0; i < ARRAY_SIZE(testModuleTypes); i++) 
                {
                    instrument_.createAndAddModule(testModuleTypes[i]);
                }
                instrument_.name_ = "testInstrument";
            }

            bool testModuleIdsAreUnique(const Instrument& instrument)
            {
                std::set<int16_t> testSet, uniqueSet;

                for (int16_t i = 0; i < 5; i++) {
                    testSet.insert((int16_t)(floor(i / 2.0) * 2));
                }
                EXPECT_EQ(3, testSet.size());  // test if set stores unique values

                const ModuleList& list = instrument.getModules();
                for (ModuleList::const_iterator it = list.begin(); it != list.end(); it++) {
                    uniqueSet.insert((*it)->id_);
                }
                return instrument.getNumModules() == uniqueSet.size();
            }
            
            TestableInstrument instrument_;
        };


        TEST_F(InstrumentTest, deleteModules)
        {
            instrument_.deleteModules();
            EXPECT_EQ(0, instrument_.getNumModules());
        }


        TEST_F(InstrumentTest, getModule)
        {
            Module* m = instrument_.getModule(2);
            EXPECT_NE(nullptr, m);
        }

        TEST_F(InstrumentTest, createModuleId)
        {
            Module* m = nullptr;

            uint16_t id = instrument_.createModuleId(kModuleMaster);
            EXPECT_EQ(kModuleMaster, id);

            // erase single element and append new element
            for (uint16_t i = 1; i < instrument_.getNumModules(); i++)
            {
                m = instrument_.getModule(i);
                ASSERT_NE(nullptr, m);
                instrument_.deleteModule(m);
                m = instrument_.createAndAddModule(kModuleMidiInput);

                EXPECT_EQ(i, m->id_);
                EXPECT_TRUE(testModuleIdsAreUnique(instrument_));
            }
            
            // erase all elements but kModuleMaster and append new element
            instrument_.deleteModules();
            m = instrument_.createAndAddModule(kModuleMidiInput);

            ASSERT_NE(nullptr, m);
            EXPECT_EQ(1, m->id_);
        }

        TEST_F(InstrumentTest, hasMaster)
        {
            EXPECT_TRUE(instrument_.hasMaster());
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


        TEST_F(BankSerializerTest, checkPath)
        {
            File file;
            std::string path;
            
            file = TestableBankSerializer::checkPath(testBank_valid_filename);
            EXPECT_TRUE(file.existsAsFile());

            path = File::getCurrentWorkingDirectory().getChildFile(testBank_valid_filename).getFullPathName().toStdString();
            file = TestableBankSerializer::checkPath(path);
            EXPECT_TRUE(file.existsAsFile());

            path = "slime.txt";
            EXPECT_THROW(TestableBankSerializer::checkPath(path), std::invalid_argument);

            path = File::getCurrentWorkingDirectory().getChildFile(path.c_str()).getFullPathName().toStdString();
            EXPECT_THROW(TestableBankSerializer::checkPath(path), std::invalid_argument);
        }

        TEST_F(BankSerializerTest, parse_ok)
        {
            std::unique_ptr<XmlElement> xml;

            File file = File::getCurrentWorkingDirectory().getChildFile(testBank_valid_filename);
            xml.reset(XmlDocument::parse(file));
            EXPECT_NE(nullptr, xml.get());
            TestableBankSerializer::checkRoot(xml.get());
        }


        TEST_F(BankSerializerTest, parse_error)
        {
            File file = File::getCurrentWorkingDirectory().getChildFile("BankSerializerTest_parse_nofile");
            EXPECT_THROW(TestableBankSerializer::checkPath(file.getFullPathName().toStdString()), std::invalid_argument);

            file = File::getCurrentWorkingDirectory().getChildFile(testBank_incompatible_xml_filename);
            EXPECT_THROW(TestableBankSerializer::readBank(file.getFullPathName().toStdString()), std::runtime_error);

            file = File::getCurrentWorkingDirectory().getChildFile(testBank_invalid_xml_filename);
            EXPECT_THROW(TestableBankSerializer::readBank(file.getFullPathName().toStdString()), std::runtime_error);
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

            Settings settings_;
            TestableBank bank_;

            void checkBank(Bank& bank)
            {
                EXPECT_EQ("test", bank.getName());
            }

            void checkInstrument(Instrument* i)
            {
                TestableInstrument* instrument = static_cast<TestableInstrument*>(i);
                Polyphony polyphony;
                polyphony.setNumVoices(32);
                instrument->initModules(&polyphony, 44100);

                EXPECT_EQ("instrument_0", instrument->name_);
                EXPECT_EQ(32, instrument->numVoices_);
                ASSERT_EQ(4, instrument->getNumModules());

                Module* master = instrument->getModule(0);
                Module* input  = instrument->getModule(1);
                Module* sine   = instrument->getModule(2);
                Module* adsr   = instrument->getModule(3);

                EXPECT_NE(nullptr, master);
                EXPECT_NE(nullptr, input);
                EXPECT_NE(nullptr, sine);
                EXPECT_NE(nullptr, adsr);

                EXPECT_EQ(kMonophonic, master->voicingType_);
                EXPECT_EQ(kPolyphonic, input->voicingType_);
                EXPECT_EQ(kPolyphonic, sine->voicingType_);
                EXPECT_EQ(kPolyphonic, adsr->voicingType_);

                EXPECT_EQ(1, master->inports_.size());
                EXPECT_EQ(2, input->outports_.size());
                EXPECT_EQ(3, sine->inports_.size());
                EXPECT_EQ(1, sine->outports_.size());
                EXPECT_EQ(2, adsr->inports_.size());
                EXPECT_EQ(1, adsr->outports_.size());

                EXPECT_EQ("Master", master->label_);
                EXPECT_EQ("MidiInput", input->label_);
                EXPECT_EQ("Sine", sine->label_);
                EXPECT_EQ("ADSR", adsr->label_);

                EXPECT_EQ(kModuleMaster, master->moduleType_);
                EXPECT_EQ(kModuleMidiInput, input->moduleType_);
                EXPECT_EQ(kModuleSineOscil, sine->moduleType_);
                EXPECT_EQ(kModuleAdsrEnv, adsr->moduleType_);

                ASSERT_EQ(1, master->links_.size());
                ASSERT_EQ(0, input->links_.size());
                ASSERT_EQ(1, sine->links_.size());
                ASSERT_EQ(2, adsr->links_.size());

                const LinkList& links = instrument->getLinks();
                ASSERT_EQ(links.size(), 4);
                EXPECT_EQ(links[0].leftModule_, 3);
                EXPECT_EQ(links[0].rightModule_, 0);
                EXPECT_EQ(links[0].leftPort_, 0);
                EXPECT_EQ(links[0].rightPort_, 0);

                Module* m = instrument->getModule(links[0].leftModule_);
                EXPECT_NE(nullptr, m);
                EXPECT_EQ(adsr, m);
                EXPECT_EQ(3, m->id_);
                EXPECT_EQ(kModuleAdsrEnv, m->moduleType_);

                checkMidiParameters(input->parameters_[0]);
                checkMidiParameters(input->parameters_[1]);
                checkMidiParameters(input->parameters_[2]);
                checkMidiParameters(sine->parameters_[0]);
                checkMidiParameters(sine->parameters_[1]);
                checkMidiParameters(sine->links_[0]);
                checkMidiParameters(adsr->parameters_[0]);
                checkMidiParameters(adsr->parameters_[1]);
                checkMidiParameters(adsr->parameters_[2]);
                checkMidiParameters(adsr->parameters_[3]);
                checkMidiParameters(adsr->links_[0]);
                checkMidiParameters(adsr->links_[1]);

                LinkPointerList linksForAdsr;
                instrument->getLinksForModule(adsr->id_, linksForAdsr);    
                EXPECT_EQ(2, linksForAdsr.size());
            }

            void checkMidiParameters(const Parameter& model)
            {
                EXPECT_EQ(-1,    model.midiShaper_.getControllerId());
                EXPECT_EQ(0,     model.midiShaper_.getControllerMin());
                EXPECT_EQ(127,   model.midiShaper_.getControllerMax());
                EXPECT_EQ(false, model.midiShaper_.getSoftTakeover());
            }
        };

        TEST_F(BankTest, load_nofile)
        {
            EXPECT_THROW(bank_.open("BankTest_load_error"), std::invalid_argument);
        }

        TEST_F(BankTest, load_invalid_xml)
        {
            EXPECT_THROW(bank_.open(testBank_invalid_xml_filename), std::runtime_error);
        }

        TEST_F(BankTest, load_incompatible_xml)
        {
            EXPECT_THROW(bank_.open(testBank_incompatible_xml_filename), std::runtime_error);
        }

        TEST_F(BankTest, load_ok)
        {
            bank_.open(testBank_valid_filename);
            checkBank(bank_);
        }

        TEST_F(BankTest, load_multiple)
        {
            for (int i = 0; i < 10; i++) {
                bank_.open(testBank_valid_filename);
                checkBank(bank_);
            }
        }

        TEST_F(BankTest, load_instrument)
        {
            bank_.open(testBank_valid_filename);
            int hash = bank_.getInstrumentHash();
            ScopedPointer<Instrument> instrument = bank_.loadInstrument(hash);
            ASSERT_NE(nullptr, instrument);
            checkInstrument(instrument);
        }

        TEST_F(BankTest, store)
        {
            bank_.open(testBank_valid_filename);
            bank_.save("TestBank_storeTest.e3mb", false, false);
            bank_.open("TestBank_storeTest.e3mb");
            checkBank(bank_);

            ScopedPointer<Instrument> instrument = bank_.loadInstrument();
            ASSERT_NE(nullptr, instrument);
            instrument->deleteModules();
            bank_.saveInstrument(instrument);
            int hash = instrument->hash_;
            bank_.setInstrumentHash(hash);
            bank_.save("TestBank_storeTest.e3mb", false, false);
            bank_.open("TestBank_storeTest.e3mb");
            EXPECT_EQ(hash, bank_.getInstrumentHash());

            instrument = bank_.loadInstrument();
            ASSERT_NE(nullptr, instrument);
            EXPECT_EQ(hash, instrument->hash_);
            EXPECT_EQ(0, instrument->getModules().size());
            EXPECT_EQ(0, instrument->getLinks().size());
        }



        TEST_F(BankTest, compile)
        {
            bank_.open(testBank_valid_filename);
            ScopedPointer<Instrument> instrument = bank_.loadInstrument();
            ASSERT_NE(nullptr, instrument);

            Polyphony polyphony;
            polyphony.setNumVoices(4);

            instrument->initModules(&polyphony, 44100);
            instrument->connectModules();

            Sink sink;
            sink.compile(instrument);
            EXPECT_EQ(4, sink.size());
        }


        //--------------------------------------------------------
        // class CpuMeterTest
        //--------------------------------------------------------
        //
        class CpuMeterTest : public ::testing::Test
        {
        public:
            CpuMeterTest() : sampleRate_(44100) {}

            void sleepMicros(int64_t micros) {
                std::this_thread::sleep_for(std::chrono::microseconds(micros));
            }

        protected:
            uint32_t sampleRate_;
            CpuMeter cpuMeter_;
        };


        TEST_F(CpuMeterTest, elapsed_time_1_second)
        {
            int64_t expected = 1000000;     // 1 second

            cpuMeter_.start();
            sleepMicros(expected);
            double elapsed = cpuMeter_.getElapsedSeconds() * 1000000;

            EXPECT_GE(elapsed, expected);
            EXPECT_LE(elapsed, expected + 20000);
        }


        TEST_F(CpuMeterTest, elapsed_time_10_milliseconds)
        {
            double expected = 10000;     // 10 milliseconds

            cpuMeter_.start();
            sleepMicros((int64_t)expected);
            double elapsed = cpuMeter_.getElapsedSeconds() * 1000000;

            EXPECT_GE(elapsed, expected);
            EXPECT_NEAR(elapsed, expected, 1000);  // error within 1 millisecond
        }

        TEST_F(CpuMeterTest, elapsed_time_1_millisecond)
        {
            double expected = 1000;     // 1 millisecond

            cpuMeter_.start();
            sleepMicros((int64_t)expected);
            double elapsed = cpuMeter_.getElapsedSeconds() * 1000000;

            EXPECT_GE(elapsed, expected);
            EXPECT_NEAR(elapsed, expected, 1000);  // error within 1 millisecond
        }

        TEST_F(CpuMeterTest, cpu_percent_short_time)
        {
            bool ready;
            int runs = 1;
            int runsPerCycle = 1;
            int framesPerRun = sampleRate_ / runs;
            int microSeconds = 1000;
            cpuMeter_.setRunsPerCycle(runsPerCycle);

            for (int i = 0; i < runs; i++) {
                cpuMeter_.start();
                sleepMicros(microSeconds);
                ready = cpuMeter_.stop(framesPerRun);
            }
            double percent = ready ? cpuMeter_.getPercent() : 0;

            EXPECT_NEAR(0.1, percent, 0.1);
        }

        TEST_F(CpuMeterTest, cpu_percent_multiple_runs)
        {
            bool ready;
            int runs = 100;
            int runsPerCycle = 1;
            int framesPerRun = sampleRate_ / runs;
            int microSeconds = 500000 / runs;
            cpuMeter_.setRunsPerCycle(runsPerCycle);

            for (int i = 0; i < runs; i++) {
                cpuMeter_.start();
                sleepMicros(microSeconds);
                ready = cpuMeter_.stop(framesPerRun);
            }
            double percent = ready? cpuMeter_.getPercent() : 0;

            EXPECT_NEAR(50, percent, 15);
        }

    } // namespace testing
} // namespace e3
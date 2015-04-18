#include <Windows.h>

#include <set>
#include <thread>

#include <e3_CommonMacros.h>
#include <core/Settings.h>
#include <core/ParameterShaper.h>
#include <core/Models.h>
#include <core/ModelCatalog.h>
#include <core/Module.h>
#include <core/Sink.h>
#include <core/Polyphony.h>
#include <core/Instrument.h>
#include <core/Bank.h>
#include <core/BankSerializer.h>
#include <core/CpuMeter.h>


namespace e3 {
    namespace testing {

        const char* testBank                  = "TestBank.e3m";
        const char* testBank_invalid_xml      = "TestBank_invalid_xml.e3m";
        const char* testBank_incompatible_xml = "TestBank_incompatible_xml.e3m";

        class TestableSettings : public Settings
        {
        public:
            using Settings::file_;

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
                settings_.setPath("e3m.settings");   // load settings file from current dir
            }

            TestableSettings settings_;
        };

        TEST_F(SettingsTest, setPathAbsoluteOrRelative)
        {
            const char* filename = "e3m.settings";
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
            settings_.load();
            EXPECT_TRUE(settings_.testValid());
        }

        TEST_F(SettingsTest, hasStyle)
        {
            settings_.load();
            XmlElement* e = settings_.getStyle();
            ASSERT_NE(nullptr, e);
            EXPECT_TRUE(e->getStringAttribute("name") == "Default");
        }

        TEST_F(SettingsTest, useDefaultStyle)
        {
            settings_.setPath("e3m_no_style.settings");
            settings_.load();
            XmlElement* e = settings_.getStyle();
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
        // ParameterModelMapTest
        //----------------------------------------------------------------------------------------

        class ParameterModelMapTest : public ::testing::Test
        {
        public:
            ParameterModelMapTest() {
                ParameterModel model;
                model.id_ = id_;
                map_.add(model);
            }

            ParameterModelMap map_;
            uint16_t id_ = 17;
        };

        TEST_F(ParameterModelMapTest, InsertAndGet)
        {
            ParameterModel& p = map_.at(id_);

            EXPECT_EQ(id_, p.id_);
            EXPECT_THROW(map_.at(0), std::out_of_range);
        }

        TEST_F(ParameterModelMapTest, SetModel)
        {
            std::string label = "testing";

            ParameterModel model;
            model.id_ = id_;
            model.label_ = label;
            map_.update(model);

            EXPECT_EQ(label, map_.at(id_).label_);

            model.id_ = 0;
            EXPECT_THROW(map_.update(model), std::out_of_range);
        }


        //----------------------------------------------------------------------------------------
        // LinkModelTest
        //----------------------------------------------------------------------------------------

        class LinkModelTest : public ::testing::Test
        {
        public:
            LinkModelTest() {
                link_.leftModule_  = 12;
                link_.rightModule_ = 45;
                link_.rightPort_   = 67;
                link_.leftPort_    = 89;
            }

            LinkModel link_;
        };

        TEST_F(LinkModelTest, ConstructionAndAssignment)
        {
            EXPECT_EQ(12, link_.leftModule_);
            
            LinkModel l1 = link_;
            EXPECT_EQ(12, l1.leftModule_);
        }

        TEST_F(LinkModelTest, OperatorEquals)
        {
            LinkModel l1 = link_;
            EXPECT_EQ(link_, l1);
            EXPECT_EQ(link_, &l1);

            l1.leftModule_ = 0;
            EXPECT_NE(link_, l1);
        }

        TEST_F(LinkModelTest, OperatorLess)
        {
            LinkModel l1 = link_;
            l1.rightModule_ = 44;
            EXPECT_LT(l1, link_);
            EXPECT_GT(link_, l1);

            l1 = link_;
            l1.leftPort_ = 0;
            EXPECT_LT(l1, link_);
        }


        //----------------------------------------------------------------------------------------
        // LinkModelListTest
        //----------------------------------------------------------------------------------------
        class LinkModelListTest : public ::testing::Test
        {
        public:
            LinkModelListTest() 
            {
                link_.leftModule_ = link_.rightModule_ = link_.rightPort_ = link_.leftPort_ = 3;

                LinkModel l;
                for (int i = 1; i <= 4; i++) {
                    l.leftModule_ = l.rightModule_ = l.rightPort_ = l.leftPort_ = i;
                    list_.push_back(l);
                }
            }
            LinkModelList list_;
            LinkModel link_;
        };

        TEST_F(LinkModelListTest, get)
        {
            LinkModel l1 = list_.get(link_);
            EXPECT_EQ(link_, l1);
        }


        TEST_F(LinkModelListTest, contains)
        {
            EXPECT_TRUE(list_.contains(link_));
        }

        TEST_F(LinkModelListTest, update)
        {
            link_.id_ = 117;
            list_.update(link_);
            EXPECT_TRUE(list_.contains(link_));
        }

        TEST_F(LinkModelListTest, remove)
        {
            list_.remove(link_);
            EXPECT_FALSE(list_.contains(link_));
        }

        TEST_F(LinkModelListTest, getIndex)
        {
            EXPECT_EQ(2, list_.getIndex(link_));
        }

        TEST_F(LinkModelListTest, hasLeftModule)
        {
            EXPECT_TRUE(list_.hasLeftModule(link_.leftModule_));
            EXPECT_FALSE(list_.hasLeftModule(117));
        }


        //----------------------------------------------------------------------------------------
        // ModuleCatalogTest
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
            { testModuleTypes[4], { kProcessAudio, kPolyphonic, 2, 1, 4 } },
            { testModuleTypes[5], { kProcessAudio | kProcessControl, kPolyphonic, 2, 1, 6 } },
            { testModuleTypes[6], { kProcessAudio, kPolyphonic, 4, 1, 3 } },
        };

        class ModuleCatalogTest : public ::testing::Test
        {
        public:
            ModuleCatalogTest() {}

            void testEqualsCatalog(ModuleType moduleType)
            {
                const ModelCatalog& catalog = ModelCatalog::instance();
                const ModuleModel& model = catalog.getModuleModel(moduleType);
                std::vector<int>& expected = moduleTestMap[moduleType];

                EXPECT_EQ(expected[0], catalog.getProcessingType(moduleType));
                EXPECT_EQ(expected[1], catalog.getVoicingType(moduleType));
                EXPECT_EQ(expected[2], model.inPortModels_.size());
                EXPECT_EQ(expected[3], model.outPortModels_.size());
                EXPECT_EQ(expected[4], model.parameters_.size());
            }
        };

        TEST_F(ModuleCatalogTest, ModelInitialization)
        {
            ModuleTestMap::const_iterator it = moduleTestMap.begin();
            for (; it != moduleTestMap.end(); it++)
            {
                testEqualsCatalog(it->first);
            }
        }

        TEST_F(ModuleCatalogTest, ParameterModel_ConstructAndCopy)
        {
            ParameterModel p1, p2;
            EXPECT_EQ(p1.id_, 0);
            EXPECT_EQ(p1.defaultValue_, 0);

            p1.id_ = 177;
            p1.defaultValue_ = -33;
            p2 = p1;

            EXPECT_EQ(p1.id_, p2.id_);
            EXPECT_EQ(p1.defaultValue_, p2.defaultValue_);
        }

        TEST_F(ModuleCatalogTest, ParameterModelMap_setDefaultValues)
        {
            ModuleModel model = ModelCatalog::instance().getModuleModel(kModuleMaster);
            model.parameters_.setDefaultValues();

            ParameterModel& param = model.parameters_.at(0);
            EXPECT_EQ(param.defaultValue_, param.value_);
        }


        //----------------------------------------------------------------------------------------
        // ModuleTest
        //----------------------------------------------------------------------------------------

        class TestableModule : public Module
        {
        public:
            TestableModule(ModuleType type) : Module(type) {}

            using Module::createModule;
            using Module::chooseAdapter;
        };

        class ModuleTest : public ::testing::Test
        {
        public:
            ModuleTest() {}

            void testEqualsCatalog(ModuleType moduleType)
            {
                Module* module = TestableModule::createModule(moduleType);
                std::vector<int>& expected = moduleTestMap[moduleType];

                EXPECT_EQ(expected[0], module->processingType_);
                EXPECT_EQ(expected[1], module->voicingType_);
                EXPECT_EQ(expected[2], module->inPortModels_.size());
                EXPECT_EQ(expected[3], module->outPortModels_.size());
                EXPECT_EQ(expected[4], module->parameters_.size());

                if (module)delete module;
            }
        };


        //TEST(ModelTest, ModulePointer_ZeroAfterCopy)
        //{
        //    ModuleModel m1;
        //    m1.module_ = (Module*)123;

        //    ModuleModel m2 = m1;
        //    EXPECT_EQ(nullptr, m2.module_);
        //    EXPECT_NE(nullptr, m1.module_);

        //    m1.module_ = nullptr;
        //}


        TEST_F(ModuleTest, createModule)
        {
            for (uint16_t t=0; t < ARRAY_SIZE(testModuleTypes); t++)
            {
                testEqualsCatalog(testModuleTypes[t]);
            }
        }


        TEST_F(ModuleTest, addAndRemoveLink)
        {
            TestableModule module(kModuleMaster);
            module.id_ = 17;
            LinkModel link;
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

        TEST_F(ModuleTest, chooseAdapter)
        {
            TestableModule master(kModuleMaster);
            TestableModule sine(kModuleSineOscil);

            ASSERT_EQ(kMonophonic, master.voicingType_);
            ASSERT_EQ(kPolyphonic, sine.voicingType_);
            
            EXPECT_EQ(kAdapterNone, master.chooseAdapter(kMonophonic));
            EXPECT_EQ(kAdapterMonoToPoly, master.chooseAdapter(kPolyphonic));

            EXPECT_EQ(kAdapterNone, sine.chooseAdapter(kPolyphonic));
            EXPECT_EQ(kAdapterPolyToMono, sine.chooseAdapter(kMonophonic));
        }
        

        //----------------------------------------------------------------------------------------
        // InstrumentTest
        //----------------------------------------------------------------------------------------

        class TestableInstrument : public Instrument
        {
        public:
            TestableInstrument() : Instrument() {}
            //TestableInstrument(const Instrument& other) : Instrument(other) {}

            using Instrument::findModule;
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

                for (Instrument::const_iterator it = instrument.begin(); it != instrument.end(); it++) {
                    uniqueSet.insert((*it)->id_);
                }
                return instrument.size() == uniqueSet.size();
            }
            
            TestableInstrument instrument_;
        };


        //TEST_F(InstrumentTest, CopyAssignment)
        //{
        //    TestableInstrument p = instrument_;

        //    EXPECT_EQ(instrument_.name_, p.name_);
        //    EXPECT_EQ(instrument_.size(), p.size());
        //    EXPECT_TRUE(p.hasMaster());
        //}

        TEST_F(InstrumentTest, deleteModules)
        {
            instrument_.deleteModules();
            EXPECT_EQ(0, instrument_.size());
        }


        TEST_F(InstrumentTest, findModule)
        {
            Module* m = instrument_.findModule(2);
            EXPECT_NE(nullptr, m);
        }

        TEST_F(InstrumentTest, createModuleId)
        {
            uint16_t id = instrument_.createModuleId(kModuleMaster);
            EXPECT_EQ(kModuleMaster, id);

            // erase single element and append new element
            for (uint16_t i = 1; i < instrument_.size(); i++)
            {
                delete instrument_[1];
                instrument_.erase(instrument_.begin() + 1);
                instrument_.createAndAddModule(kModuleMidiInput);

                EXPECT_EQ(i, instrument_.back()->id_);
                EXPECT_TRUE(testModuleIdsAreUnique(instrument_));
            }
            
            // erase all elements but kModuleMaster and append new element
            instrument_.deleteModules();
            instrument_.createAndAddModule(kModuleMidiInput);

            EXPECT_EQ(1, instrument_.back()->id_);

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
            using BankSerializer::parse;
        };

        class BankSerializerTest : public ::testing::Test
        {};


        TEST_F(BankSerializerTest, checkPath)
        {
            File file;
            std::string path;
            
            file = TestableBankSerializer::checkPath(testBank);
            EXPECT_TRUE(file.existsAsFile());

            path = File::getCurrentWorkingDirectory().getChildFile(testBank).getFullPathName().toStdString();
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

            File file = File::getCurrentWorkingDirectory().getChildFile(testBank);
            xml.reset(TestableBankSerializer::parse(file));
            EXPECT_NE(nullptr, xml.get());
        }


        TEST_F(BankSerializerTest, parse_error)
        {
            File file = File::getCurrentWorkingDirectory().getChildFile("BankSerializerTest_parse_nofile");
            EXPECT_THROW(TestableBankSerializer::parse(file), std::runtime_error);

            file = File::getCurrentWorkingDirectory().getChildFile(testBank_incompatible_xml);
            EXPECT_THROW(TestableBankSerializer::parse(file), std::runtime_error);

            file = File::getCurrentWorkingDirectory().getChildFile(testBank_invalid_xml);
            EXPECT_THROW(TestableBankSerializer::parse(file), std::runtime_error);
        }

        //---------------------------------------------------
        // BankTest
        //---------------------------------------------------
        class TestableBank : public Bank
        {
        public:
            using Bank::isValidId;
            //using Bank::path_;
            //using Bank::xmlDocument_;
        };

        class BankTest : public ::testing::Test
        {
        public:
            BankTest() {
                settings_.load();
                bank_.setSettings(&settings_);
            }

            Settings settings_;
            TestableBank bank_;

            void checkBank(Bank& bank)
            {
                EXPECT_EQ("test", bank.getName());
                EXPECT_EQ(0, bank.getInstrumentId());
                EXPECT_EQ(2, bank.getNumInstruments());
   
                //TestableInstrument* instrument = static_cast<TestableInstrument*>(bank.getInstrument(1));
                //ASSERT_NE(nullptr, instrument);
                //EXPECT_EQ("instrument_1", instrument->name_);
                //EXPECT_EQ(1, instrument->numVoices_);
            }

            void checkInstrument(Instrument* i)
            {
                TestableInstrument* instrument = static_cast<TestableInstrument*>(i);
                Polyphony polyphony;
                polyphony.setNumVoices(32);
                instrument->initModules(&polyphony);
                instrument->updateModules(44100, polyphony.getNumVoices());

                EXPECT_EQ("instrument_0", instrument->name_);
                EXPECT_EQ(32, instrument->numVoices_);
                ASSERT_EQ(4, instrument->size());

                Module* master = instrument->at(0);
                Module* input  = instrument->at(1);
                Module* sine   = instrument->at(2);
                Module* adsr   = instrument->at(3);

                EXPECT_NE(nullptr, master);
                EXPECT_NE(nullptr, input);
                EXPECT_NE(nullptr, sine);
                EXPECT_NE(nullptr, adsr);

                EXPECT_EQ(kMonophonic, master->voicingType_);
                EXPECT_EQ(kPolyphonic, input->voicingType_);
                EXPECT_EQ(kPolyphonic, sine->voicingType_);
                EXPECT_EQ(kPolyphonic, adsr->voicingType_);

                EXPECT_EQ(1, master->getInPorts().size());
                EXPECT_EQ(2, input->getOutPorts().size());
                EXPECT_EQ(2, sine->getInPorts().size());
                EXPECT_EQ(1, sine->getOutPorts().size());
                EXPECT_EQ(1, adsr->getInPorts().size());
                EXPECT_EQ(1, adsr->getOutPorts().size());

                EXPECT_EQ("Master", master->label_);
                EXPECT_EQ("Input", input->label_);
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

                EXPECT_EQ(3, master->links_[0].leftModule_);
                EXPECT_EQ(1, sine->links_[0].leftModule_);
                EXPECT_EQ(1, adsr->links_[0].leftModule_);
                EXPECT_EQ(2, adsr->links_[1].leftModule_);

                Module* m = instrument->findModule(master->links_[0].leftModule_);
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
            }

            void checkMidiParameters(const ParameterModel& model)
            {
                EXPECT_EQ(-1,    model.midiShaper_.getControllerId());
                EXPECT_EQ(0,     model.midiShaper_.getControllerMin());
                EXPECT_EQ(127,   model.midiShaper_.getControllerMax());
                EXPECT_EQ(false, model.midiShaper_.getSoftTakeover());
            }
        };

        TEST_F(BankTest, load_nofile)
        {
            EXPECT_THROW(bank_.load("BankTest_load_error"), std::invalid_argument);
        }

        TEST_F(BankTest, load_invalid_xml)
        {
            EXPECT_THROW(bank_.load(testBank_invalid_xml), std::runtime_error);
        }

        TEST_F(BankTest, load_incompatible_xml)
        {
            EXPECT_THROW(bank_.load(testBank_incompatible_xml), std::runtime_error);
        }

        TEST_F(BankTest, load_ok)
        {
            bank_.load(testBank);
            checkBank(bank_);
        }

        TEST_F(BankTest, load_multiple)
        {
            for (int i = 0; i < 10; i++) {
                bank_.load(testBank);
                checkBank(bank_);
            }
        }

        TEST_F(BankTest, load_instrument)
        {
            bank_.load(testBank);
            int hash = bank_.getInstrumentHash();
            Instrument* instrument = bank_.loadInstrument(hash);
            ASSERT_NE(nullptr, instrument);
            checkInstrument(instrument);
            //delete i;
        }

        TEST_F(BankTest, store_instrument)
        {
            bank_.load(testBank);
            int hash = bank_.getInstrumentHash();
            Instrument* instrument = bank_.loadInstrument();
            bank_.storeInstrument(instrument);
            EXPECT_EQ(hash, instrument->hash_);

            instrument->deleteModules();
            bank_.storeInstrument(instrument);
            EXPECT_NE(hash, instrument->hash_);
        }

        TEST_F(BankTest, store)
        {
            bank_.load(testBank);
            bank_.store("TestBank_storeTest.e3m", false, false);
            bank_.load("TestBank_storeTest.e3m");
            checkBank(bank_);

            Instrument* instrument = bank_.loadInstrument();
            instrument->deleteModules();
            bank_.storeInstrument(instrument);
            int hash = instrument->hash_;
            bank_.setInstrumentHash(hash);
            bank_.store("TestBank_storeTest.e3m", false, false);
            bank_.load("TestBank_storeTest.e3m");
            EXPECT_EQ(hash, bank_.getInstrumentHash());

            instrument = bank_.loadInstrument();
            EXPECT_EQ(hash, instrument->hash_);
        }

        TEST_F(BankTest, connect_modules)
        {
            bank_.load(testBank);
            TestableInstrument* instrument = static_cast<TestableInstrument*>(bank_.loadInstrument());

            Polyphony polyphony;
            polyphony.setNumVoices(4);

            instrument->initModules(&polyphony);
            instrument->updateModules(44100, polyphony.getNumVoices());

            Sink sink;
            sink.compile(instrument);
            EXPECT_EQ(4, sink.size());
            //instrument->connectModules();
            //checkConnections(instrument);
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
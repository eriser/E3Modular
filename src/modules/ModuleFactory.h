
#pragma once



namespace e3 {

    class Module;

    class ModuleFactory {
    public:
        static Module* create(ModuleType type);
    };

} // namespace e3


#pragma once

#include <map>
#include <string>
#include "core/Module.h"



namespace e3 {

    class Module;

    typedef std::map< ModuleType, std::string > ModuleCatalog;


    class ModuleFactory {
    public:
        static Module* create(ModuleType type);

        static ModuleCatalog catalog_;
    };

} // namespace e3

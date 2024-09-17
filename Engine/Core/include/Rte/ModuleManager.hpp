#pragma once

#include "Rte/IModule.hpp"

#include <map>
#include <memory>
#include <string>

class ModuleManager {
    public:
        ModuleManager() = default;
        ~ModuleManager();

        ModuleManager(const ModuleManager &) = delete;
        ModuleManager &operator=(const ModuleManager &) = delete;

        ModuleManager(ModuleManager &&) = default;
        ModuleManager &operator=(ModuleManager &&) = default;

        /**
         * @brief Load a module by its name (RteGraphic, RtePhysics, ...)
         *
         * @param moduleName Name of the module to load
         * @return std::unique_ptr<IModule> Module instance
         */
        std::unique_ptr<IModule> loadModule(const std::string &moduleName);


    private:
        static std::string findLibrary(const std::string &moduleName);

        std::map<const char *, void *> m_modules;
};

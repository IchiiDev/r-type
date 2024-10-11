#pragma once

#include "Rte/Ecs/Ecs.hpp"

#include <map>
#include <memory>
#include <string>

namespace Rte {

    /**
     * @brief Interface for modules
     * This interface is used to create modules that can be loaded dynamically
     * It is used to separate the engine in different parts (graphic, physics, ...)
     * It can be casted to a specific module type to access its specific methods
     */
    class IModule {
        public:
            IModule() = default;
            virtual ~IModule() = default;

            IModule(const IModule &) = delete;
            IModule &operator=(const IModule &) = delete;

            IModule(IModule &&) = default;
            IModule &operator=(IModule &&) = default;

            /**
             * @brief Initialize the module
             * /!\ This method must be called before any other method
             *
             * @param ecs Entity Component System instance to use
             */
            virtual void init(const std::shared_ptr<Ecs>& ecs) = 0;

            /**
             * @brief Update the module
             * This method should be called every frame
             * It will update the systems managed by the module
             */
            virtual void update() = 0;
    };


    /**
     * @brief Manager for modules
     * This class is used to load modules dynamically
     * It will load the module from a shared library and return an instance of it
     * It will also keep track of the loaded modules to unload them when the manager is destroyed
     */
    class ModuleManager {
        public:
            ModuleManager() = default;
            ~ModuleManager();

            ModuleManager(const ModuleManager &) = delete;
            ModuleManager &operator=(const ModuleManager &) = delete;

            ModuleManager(ModuleManager &&) = delete;
            ModuleManager &operator=(ModuleManager &&) = delete;

            /**
            * @brief Load a module by its name (RteGraphic, RtePhysics, ...)
            *
            * @param moduleName Name of the module to load
            * @return std::unique_ptr<IModule> Module instance
            */
            std::unique_ptr<IModule> loadModule(const std::string &moduleName);


        private:
            static std::string findLibrary(const std::string &moduleName);

            std::map<std::string, void *> m_modules;
    };

}   // namespace Rte

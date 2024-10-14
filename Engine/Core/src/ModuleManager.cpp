#include "Rte/ModuleManager.hpp"

#ifdef _WIN32
    #include <Windows.h>
#else
    #include <dlfcn.h>
#endif
#include <filesystem>
#include <memory>
#include <stdexcept>
#include <string>

using namespace Rte;

ModuleManager::~ModuleManager() {
    for (const auto &module : m_modules) {
        #ifdef _WIN32
            FreeLibrary(static_cast<HMODULE>(module.second));
        #else
            dlclose(module.second);
        #endif
    }
}

std::unique_ptr<IModule> ModuleManager::loadModule(const std::string& moduleName) {
    const std::string modulePath = findLibrary(moduleName);
    if (modulePath.empty())
        throw std::runtime_error("Failed to find module: " + moduleName);

    void *moduleHandle = nullptr;

    #ifdef _WIN32
        moduleHandle = LoadLibrary(modulePath.c_str());
        if (moduleHandle == nullptr)
            throw std::runtime_error("Failed to load module: \"" + moduleName + "\": " + std::to_string(GetLastError()));

        auto create = reinterpret_cast<IModule *(*)()>(GetProcAddress(static_cast<HMODULE>(moduleHandle), "createModule"));
        if (create == nullptr)
            throw std::runtime_error("Failed to find symbol: createModule");
    #else
        moduleHandle = dlopen(modulePath.c_str(), RTLD_LAZY | RTLD_GLOBAL);
        if (moduleHandle == nullptr)
            throw std::runtime_error(dlerror());

        auto create = reinterpret_cast<IModule *(*)()>(dlsym(moduleHandle, "createModule"));
        if (create == nullptr)
            throw std::runtime_error(dlerror());
    #endif

    m_modules[moduleName.c_str()] = moduleHandle;
    return std::unique_ptr<IModule>(create());
}

std::string ModuleManager::findLibrary(const std::string& moduleName) {
    for (const auto &entry : std::filesystem::recursive_directory_iterator(".")) {
        if (entry.is_regular_file()) {
            std::string path = entry.path().string();
            const std::string filename = entry.path().filename().string();
            const std::string basename = filename.substr(0, filename.find_last_of('.'));
            #ifdef _WIN32
                if (basename == moduleName && path.find(".dll") != std::string::npos)
                    return path;
            #else
                if ((basename == moduleName || basename == "lib" + moduleName) && path.find(".so") != std::string::npos)
                    return path;
            #endif
        }
    }
    return "";
}

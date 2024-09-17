#pragma once

class IModule {
    public:
        IModule() = default;
        virtual ~IModule() = default;

        IModule(const IModule &) = delete;
        IModule &operator=(const IModule &) = delete;

        IModule(IModule &&) = default;
        IModule &operator=(IModule &&) = default;

        virtual void update() = 0;
};

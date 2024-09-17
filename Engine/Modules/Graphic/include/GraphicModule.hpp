#pragma once

#include "Rte/IModule.hpp"

class GraphicModule : public IModule {
    public:
        void update() override;
};

extern "C" IModule* createModule();

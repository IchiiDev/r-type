#pragma once

#include "Rte/ModuleManager.hpp"
#include "Rte/Ecs/Ecs.hpp"

#include <memory>

namespace Rte {

    class GraphicModule : public IModule {
        public:
            void init(const std::shared_ptr<Ecs>& ecs) override;
            void update() override;

        private:
            std::shared_ptr<Ecs> m_ecs;
    };

}   // namespace Rte

extern "C" Rte::IModule* createModule();

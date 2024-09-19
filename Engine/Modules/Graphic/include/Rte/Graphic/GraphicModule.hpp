#pragma once

#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Graphic/Texture.hpp"
#include "Rte/ModuleManager.hpp"

#include <memory>

namespace Rte::Graphic {

    class GraphicModule : public IModule {
        public:
            void init(const std::shared_ptr<Ecs>& ecs) override = 0;
            void update() override = 0;

            [[nodiscard]] virtual std::unique_ptr<Texture> createTexture() const = 0;
    };

}   // namespace Rte::Graphic

extern "C" EXPORT Rte::IModule* createModule();

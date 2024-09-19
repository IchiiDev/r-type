#pragma once

#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Graphic/GraphicModule.hpp"
#include "Rte/Graphic/Texture.hpp"

#include <memory>

namespace Rte::Graphic {

    class GraphicModuleImpl : public GraphicModule {
        public:
            void init(const std::shared_ptr<Ecs>& ecs) override;
            void update() override;

            [[nodiscard]] std::unique_ptr<Texture> createTexture() const override;

        private:
            std::shared_ptr<Ecs> m_ecs;
    };

}   // namespace Rte::Graphic

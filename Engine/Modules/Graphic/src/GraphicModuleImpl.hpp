#pragma once

#include "RenderSystem.hpp"
#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Graphic/GraphicModule.hpp"
#include "Rte/Graphic/Texture.hpp"

#include "SFML/Graphics/RenderWindow.hpp"

#include <memory>
#include <string>

namespace Rte::Graphic {

    class GraphicModuleImpl : public GraphicModule {
        public:
            void init(const std::shared_ptr<Ecs>& ecs) override;
            void update() override;

            void setWindowTitle(const std::string& title) override;
            void setWindowSize(const Vec2<u16>& size) override;
            [[nodiscard]] Vec2<u16> getWindowSize() const override;

            [[nodiscard]] std::shared_ptr<Texture> createTexture() const override;

        private:
            std::shared_ptr<Ecs> m_ecs;

            sf::RenderWindow m_window;
            std::shared_ptr<RenderSystem> m_renderSystem;
    };

}   // namespace Rte::Graphic

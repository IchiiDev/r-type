#pragma once

#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/Graphic/GraphicModule.hpp"
#include "Rte/Graphic/Texture.hpp"

#include <functional>
#include <memory>

class Button {
    public:
        /**
         * @brief Construct a new Button object and add it to the ECS.
         *
         * @param ecs               ECS instance
         * @param graphicModule     Graphic module instance
         * @param texture           Texture of the button
         * @param offsetFromCenter  Offset from the center of the window
         * @param size              Size of the button
         * @param onClick           Function to call when the button is clicked
         */
        Button(const std::shared_ptr<Rte::Ecs>& ecs, const std::shared_ptr<Rte::Graphic::GraphicModule>& graphicModule, const std::shared_ptr<Rte::Graphic::Texture>& texture, const Rte::Vec2<float>& offsetFromCenter, const Rte::Vec2<float>& size, std::function<void()> onClick);
        ~Button();

        Button(const Button&) = delete;
        Button& operator=(const Button&) = delete;
        Button(Button&&) noexcept = default;
        Button& operator=(Button&&) noexcept = default;

        /**
         * @brief Update the button.
         * This function should be called every frame.
         * It will update change the size of the button when hovered and call the onClick function when clicked.
         */
        void update();

    private:
        void updateTransform(const Rte::Vec2<Rte::u16>& windowSize, const Rte::Vec2<float>& offsetFromCenter, const Rte::Vec2<float>& buttonSize) const;

        std::shared_ptr<Rte::Ecs> m_ecs;
        std::shared_ptr<Rte::Graphic::GraphicModule> m_graphicModule;

        Rte::Entity m_entity;
        std::function<void()> m_onClick;
        Rte::Graphic::ButtonState m_lastState{};

        Rte::Vec2<float> m_offsetFromCenter{};
        Rte::Vec2<float> m_size{};
        Rte::Vec2<float> m_growSize{};
};

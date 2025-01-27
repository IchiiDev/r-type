#pragma once

#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/Graphic/GraphicModule.hpp"

#include <functional>
#include <memory>

class Button {
    public:
        Button(const std::shared_ptr<Rte::Ecs>& ecs, const std::shared_ptr<Rte::Graphic::GraphicModule>& graphicModule, uint32_t texture, bool enlargeOnHover, const Rte::Vec2<float>& position, const Rte::Vec2<float>& size, const std::string& text, int textSize, std::function<void()> onClick);
        ~Button();

        Button(const Button&) = delete;
        Button& operator=(const Button&) = delete;
        Button(Button&&) noexcept = default;
        Button& operator=(Button&&) noexcept = default;

        void update();

    private:
        std::shared_ptr<Rte::Ecs> m_ecs;
        std::shared_ptr<Rte::Graphic::GraphicModule> m_graphicModule;

        Rte::Entity m_entity;
        std::function<void()> m_onClick;
        Rte::Graphic::ButtonState m_lastState{};

        Rte::Vec2<float> m_position{};
        Rte::Vec2<float> m_size{};
        Rte::Vec2<float> m_growSize{};
        int m_textSize{};
        bool m_enlargeOnHover{};
};

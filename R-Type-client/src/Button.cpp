#include "Button.hpp"

#include "Rte/BasicComponents.hpp"
#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Graphic/Components.hpp"
#include "Rte/Graphic/GraphicModule.hpp"
#include "Rte/Graphic/Texture.hpp"

#include <functional>
#include <memory>
#include <utility>

Button::Button(const std::shared_ptr<Rte::Ecs>& ecs, const std::shared_ptr<Rte::Graphic::GraphicModule>& graphicModule, const std::shared_ptr<Rte::Graphic::Texture>& texture, const Rte::Vec2<float>& offsetFromCenter, const Rte::Vec2<float>& size, std::function<void()> onClick) {
    m_ecs = ecs;                            // NOLINT(cppcoreguidelines-prefer-member-initializer)
    m_graphicModule = graphicModule;        // NOLINT(cppcoreguidelines-prefer-member-initializer)
    m_onClick = std::move(onClick);         // NOLINT(cppcoreguidelines-prefer-member-initializer)
    m_entity = ecs->createEntity();         // NOLINT(cppcoreguidelines-prefer-member-initializer)
    m_offsetFromCenter = offsetFromCenter;  // NOLINT(cppcoreguidelines-prefer-member-initializer)
    m_size = size;                          // NOLINT(cppcoreguidelines-prefer-member-initializer)

    m_growSize = {
        .x = static_cast<float>(size.x * 0.03),
        .y = static_cast<float>(size.y * 0.03)
    };

    ecs->addComponent<Rte::Graphic::Components::Sprite>(m_entity, Rte::Graphic::Components::Sprite(texture));
    ecs->addComponent<Rte::Graphic::Components::Button>(m_entity, Rte::Graphic::Components::Button{});
    ecs->addComponent<Rte::BasicComponents::Transform>(m_entity, Rte::BasicComponents::Transform{});
    updateTransform(m_graphicModule->getWindowSize(), offsetFromCenter, size);
}

Button::~Button() {
    m_ecs->destroyEntity(m_entity);
}

void Button::update() {
    const Rte::Graphic::Components::Button& buttonComponent = m_ecs->getComponent<Rte::Graphic::Components::Button>(m_entity);
    const Rte::Vec2<Rte::u16> windowSize = m_graphicModule->getWindowSize();

    if (buttonComponent.state == Rte::Graphic::ButtonState::HOVERED) {
        const Rte::Vec2<float> newSize = {
            .x = m_size.x + m_growSize.x,
            .y = m_size.y + m_growSize.y
        };

        updateTransform(windowSize, m_offsetFromCenter, newSize);
    } else {
        if (buttonComponent.state == Rte::Graphic::ButtonState::PRESSED && m_lastState == Rte::Graphic::ButtonState::HOVERED)
            m_onClick();

        updateTransform(windowSize, m_offsetFromCenter, m_size);
    }

    m_lastState = buttonComponent.state;
}

void Button::updateTransform(const Rte::Vec2<Rte::u16>& windowSize, const Rte::Vec2<float>& offsetFromCenter, const Rte::Vec2<float>& buttonSize) const {
    Rte::BasicComponents::Transform& buttonTransform = m_ecs->getComponent<Rte::BasicComponents::Transform>(m_entity);
    buttonTransform.position.x = (static_cast<float>(windowSize.x) / 2 - buttonSize.x / 2) + offsetFromCenter.x;
    buttonTransform.position.y = (static_cast<float>(windowSize.y) / 2 - buttonSize.y / 2) + offsetFromCenter.y;
    buttonTransform.scale = buttonSize;
}

#include "GraphicModuleImpl.hpp"
#include "RenderSystem.hpp"
#include "Rte/BasicComponents.hpp"
#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Ecs/Event.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/Graphic/Components.hpp"
#include "Rte/Graphic/GraphicModule.hpp"
#include "Rte/Graphic/Texture.hpp"
#include "Rte/ModuleManager.hpp"
#include "SFML/Graphics/Rect.hpp"
#include "SFML/Graphics/View.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/Event.hpp"
#include "SFML/Window/VideoMode.hpp"
#include "TextureImpl.hpp"

#include <memory>
#include <optional>
#include <string>

using namespace Rte::Graphic;

Rte::IModule *createModule() {
    return new GraphicModuleImpl(); // NOLINT(cppcoreguidelines-owning-memory)
}

void GraphicModuleImpl::init(const std::shared_ptr<Ecs>& ecs) {
    m_window = sf::RenderWindow(sf::VideoMode({800, 600}), "Rte window");
    m_ecs = ecs;

    // Register components
    ecs->registerComponent<Components::Sprite>();

    // Render system registration
    m_renderSystem = ecs->registerSystem<RenderSystem>();
    m_renderSystem->init(ecs);

    // Render system signature
    Signature signature;
    signature.set(ecs->getComponentType<Components::Sprite>());
    signature.set(ecs->getComponentType<BasicComponents::Transform>());
    ecs->setSystemSignature<RenderSystem>(signature);
}

void GraphicModuleImpl::update() {
    // Check events
    while (std::optional<sf::Event> event = m_window.pollEvent()) {

        // Check window closed
        if (event->is<sf::Event::Closed>()) {
            m_ecs->sendEvent(Events::Window::QUIT);
            m_window.close();
        }


        // Check resize
        if (const sf::Event::Resized* resized = event->getIf<sf::Event::Resized>()) {
            // Change view for the window
            const sf::FloatRect visibleArea(
                sf::Vector2<float>(0, 0),
                sf::Vector2<float>(
                    static_cast<float>(resized->size.x),
                    static_cast<float>(resized->size.y)
                )
            );
            m_window.setView(sf::View(visibleArea));


            // Send event with new size
            Event event(Events::Window::RESIZED);
            event.setParameter<Rte::u16>(Events::Window::Resized::WIDTH, resized->size.x);
            event.setParameter<Rte::u16>(Events::Window::Resized::HEIGHT, resized->size.y);
            m_ecs->sendEvent(event);
        }
    }


    // Clear & display
    m_window.clear();
    m_renderSystem->update(m_window);
    m_window.display();
}

std::shared_ptr<Texture> GraphicModuleImpl::createTexture() const { // NOLINT(readability-convert-member-functions-to-static)
    return std::make_shared<TextureImpl>();
}

void GraphicModuleImpl::setWindowTitle(const std::string& title) {
    m_window.setTitle(title);
}

void GraphicModuleImpl::setWindowSize(const Vec2<u16>& size) {
    m_window.setSize({size.x, size.y});
}

Rte::Vec2<Rte::u16> GraphicModuleImpl::getWindowSize() const {
    const sf::Vector2<u32> size = m_window.getSize();
    return {static_cast<u16>(size.x), static_cast<u16>(size.y)};
}

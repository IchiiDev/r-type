#include "ButtonSystem.hpp"
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
#include "SFML/Graphics/Text.hpp"
#include "TextureImpl.hpp"

#include "SFML/Graphics/Rect.hpp"
#include "SFML/Graphics/Shader.hpp"
#include "SFML/Graphics/View.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/Event.hpp"
#include "SFML/Window/Keyboard.hpp"
#include "SFML/Window/Mouse.hpp"
#include "SFML/Window/VideoMode.hpp"

#include <memory>
#include <optional>
#include <stdexcept>
#include <string>

using namespace Rte::Graphic;

Rte::IModule *createModule() {
    return new GraphicModuleImpl(); // NOLINT(cppcoreguidelines-owning-memory)
}

void GraphicModuleImpl::init(const std::shared_ptr<Ecs>& ecs) {
    m_window = sf::RenderWindow(sf::VideoMode({1920, 1080}), "Rte window");
    m_window.setFramerateLimit(60);
    m_ecs = ecs;


    // Check shaders availability
    if (!sf::Shader::isAvailable())
        throw std::runtime_error("Failed to load graphic module: Shaders are not available on this system.");


    // Daltonian filter shader (source: https://godotshaders.com/shader/colorblindness-correction-shader)
    const std::string fragmentSource = R"(
        uniform sampler2D texture;
        uniform int mode;

        void main() {
            vec4 color = texture2D(texture, gl_TexCoord[0].xy);
            if (mode == 0) {
                gl_FragColor = color;
                return;
            }

            float L = (17.8824 * color.r) + (43.5161 * color.g) + (4.11935 * color.b);
            float M = (3.45565 * color.r) + (27.1554 * color.g) + (3.86714 * color.b);
            float S = (0.0299566 * color.r) + (0.184309 * color.g) + (1.46709 * color.b);

            float l, m, s;
            if (mode == 1) {            // Protanopia
                l = 0.0 * L + 2.02344 * M + -2.52581 * S;
                m = 0.0 * L + 1.0 * M + 0.0 * S;
                s = 0.0 * L + 0.0 * M + 1.0 * S;
            } else if (mode == 2) {     // Deuteranopia
                l = 1.0 * L + 0.0 * M + 0.0 * S;
                m = 0.494207 * L + 0.0 * M + 1.24827 * S;
                s = 0.0 * L + 0.0 * M + 1.0 * S;
            } else if (mode == 3) {     // Tritanopia
                l = 1.0 * L + 0.0 * M + 0.0 * S;
                m = 0.0 * L + 1.0 * M + 0.0 * S;
                s = -0.395913 * L + 0.801109 * M + 0.0 * S;
            }

            vec4 error;
            error.r = (0.0809444479 * l) + (-0.130504409 * m) + (0.116721066 * s);
            error.g = (-0.0102485335 * l) + (0.0540193266 * m) + (-0.113614708 * s);
            error.b = (-0.000365296938 * l) + (-0.00412161469 * m) + (0.693511405 * s);
            error.a = 1.0;

            vec4 diff = color - error;

            vec4 correction;
            correction.r = 0.0;
            correction.g = (diff.r * 0.7) + (diff.g * 1.0);
            correction.b = (diff.r * 0.7) + (diff.b * 1.0);
            correction = color + correction;
            correction.a = color.a;

            gl_FragColor = correction;
        }
    )";

    if (!m_shader.loadFromMemory(fragmentSource, sf::Shader::Type::Fragment))
        throw std::runtime_error("Failed to load graphic module: Failed to load shader.");

    m_shader.setUniform("mode", 0);


    // Register components
    ecs->registerComponent<Components::Sprite>();
    ecs->registerComponent<Components::Button>();
    ecs->registerComponent<Components::Text>();


    // Render system registration
    m_renderSystem = ecs->registerSystem<RenderSystem>();
    m_renderSystem->init(ecs);

    Signature renderSystemSignature;
    renderSystemSignature.set(ecs->getComponentType<Components::Sprite>());
    renderSystemSignature.set(ecs->getComponentType<BasicComponents::Transform>());
    ecs->setSystemSignature<RenderSystem>(renderSystemSignature);


    // Button system registration
    m_buttonSystem = ecs->registerSystem<ButtonSystem>();
    m_buttonSystem->init(ecs);

    Signature buttonSystemSignature;
    buttonSystemSignature.set(ecs->getComponentType<Components::Button>());
    buttonSystemSignature.set(ecs->getComponentType<Components::Sprite>());
    buttonSystemSignature.set(ecs->getComponentType<BasicComponents::Transform>());
    ecs->setSystemSignature<ButtonSystem>(buttonSystemSignature);


    // Text system registration
    m_textSystem = ecs->registerSystem<TextSystem>();
    m_textSystem->init(ecs, m_font);

    Signature textSystemSignature;
    textSystemSignature.set(ecs->getComponentType<Components::Text>());
    textSystemSignature.set(ecs->getComponentType<BasicComponents::Transform>());
    ecs->setSystemSignature<TextSystem>(textSystemSignature);
}

void GraphicModuleImpl::update() {
    // Check events
    while (std::optional<sf::Event> event = m_window.pollEvent()) {

        // Check window closed
        if (event->is<sf::Event::Closed>()) {
            m_ecs->sendEvent(Events::QUIT);
            m_window.close();
        }


        // Check resize
        if (const sf::Event::Resized *resized = event->getIf<sf::Event::Resized>()) {
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
            Event event(Events::RESIZED);
            event.setParameter<Vec2<u16>>(Events::Params::NEW_WINDOW_SIZE, Vec2<u16>(resized->size.x, resized->size.y));
            m_ecs->sendEvent(event);
        }


        // Check for key pressed
        if (const sf::Event::KeyPressed *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            Event event(Events::KEY_PRESSED);
            event.setParameter<Key>(Events::Params::KEY_PRESSED, sfmlKeyToRteKey.at(keyPressed->code));
            m_ecs->sendEvent(event);
        }


        // Check for mouse button pressed
        if (const sf::Event::MouseButtonPressed *mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
            Event event(Events::MOUSE_BUTTON_PRESSED);
            event.setParameter<MouseButton>(Events::Params::MOUSE_BUTTON_PRESSED, sfmlMouseButtonToRteMouseButton.at(mouseButtonPressed->button));
            event.setParameter<Vec2<u16>>(Events::Params::MOUSE_BUTTON_PRESSED_POSITION, Vec2<u16>(mouseButtonPressed->position.x, mouseButtonPressed->position.y));
            m_ecs->sendEvent(event);
        }
    }


    // Clear & display
    m_window.clear();
    m_renderSystem->update(m_window, m_shader);
    m_buttonSystem->update(m_window);
    m_textSystem->update(m_window);
    m_window.display();
}

bool GraphicModuleImpl::isKeyPressed(Key key) const {
    return sf::Keyboard::isKeyPressed(rteKeyToSfmlKey.at(key));
}

bool GraphicModuleImpl::isMouseButtonPressed(MouseButton button) const {
    return sf::Mouse::isButtonPressed(rteMouseButtonToSfmlMouseButton.at(button));
}

Rte::Vec2<Rte::u16> GraphicModuleImpl::getMousePosition() const {
    const sf::Vector2<i32> position = sf::Mouse::getPosition(m_window);
    return {static_cast<u16>(position.x), static_cast<u16>(position.y)};
}

void GraphicModuleImpl::setDaltonismMode(DaltonismMode mode) {
    m_shader.setUniform("mode", static_cast<int>(mode));
}

std::unique_ptr<Texture> GraphicModuleImpl::createTexture() const { // NOLINT(readability-convert-member-functions-to-static)
    return std::make_unique<TextureImpl>();
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

void GraphicModuleImpl::loadFontFromMemory(const void *data, Rte::u32 size) {
    if (!m_font.openFromMemory(data, size))
        throw std::runtime_error("Failed to load font from memory.");
}

void GraphicModuleImpl::loadFontFromFile(const char *path) {
    if (!m_font.openFromFile(path))
        throw std::runtime_error("Failed to load font from file.");
}

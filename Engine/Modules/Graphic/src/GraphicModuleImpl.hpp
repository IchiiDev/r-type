#pragma once

#include "ButtonSystem.hpp"
#include "RenderSystem.hpp"
#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Graphic/GraphicModule.hpp"
#include "Types.hpp"

#include "SFML/Graphics/Font.hpp"
#include "SFML/Graphics/Image.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Shader.hpp"
#include "SFML/Window/Keyboard.hpp"
#include "SFML/Window/Mouse.hpp"
#include "TextSystem.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>

namespace Rte::Graphic {

    static const std::unordered_map<sf::Keyboard::Key, Key> sfmlKeyToRteKey = {
        {sf::Keyboard::Key::Unknown, Key::Unknown},

        {sf::Keyboard::Key::A, Key::A},
        {sf::Keyboard::Key::B, Key::B},
        {sf::Keyboard::Key::C, Key::C},
        {sf::Keyboard::Key::D, Key::D},
        {sf::Keyboard::Key::E, Key::E},
        {sf::Keyboard::Key::F, Key::F},
        {sf::Keyboard::Key::G, Key::G},
        {sf::Keyboard::Key::H, Key::H},
        {sf::Keyboard::Key::I, Key::I},
        {sf::Keyboard::Key::J, Key::J},
        {sf::Keyboard::Key::K, Key::K},
        {sf::Keyboard::Key::L, Key::L},
        {sf::Keyboard::Key::M, Key::M},
        {sf::Keyboard::Key::N, Key::N},
        {sf::Keyboard::Key::O, Key::O},
        {sf::Keyboard::Key::P, Key::P},
        {sf::Keyboard::Key::Q, Key::Q},
        {sf::Keyboard::Key::R, Key::R},
        {sf::Keyboard::Key::S, Key::S},
        {sf::Keyboard::Key::T, Key::T},
        {sf::Keyboard::Key::U, Key::U},
        {sf::Keyboard::Key::V, Key::V},
        {sf::Keyboard::Key::W, Key::W},
        {sf::Keyboard::Key::X, Key::X},
        {sf::Keyboard::Key::Y, Key::Y},
        {sf::Keyboard::Key::Z, Key::Z},
        {sf::Keyboard::Key::Num0, Key::Num0},
        {sf::Keyboard::Key::Num1, Key::Num1},
        {sf::Keyboard::Key::Num2, Key::Num2},
        {sf::Keyboard::Key::Num3, Key::Num3},
        {sf::Keyboard::Key::Num4, Key::Num4},
        {sf::Keyboard::Key::Num5, Key::Num5},
        {sf::Keyboard::Key::Num6, Key::Num6},
        {sf::Keyboard::Key::Num7, Key::Num7},
        {sf::Keyboard::Key::Num8, Key::Num8},
        {sf::Keyboard::Key::Num9, Key::Num9},

        {sf::Keyboard::Key::Escape, Key::Escape},
        {sf::Keyboard::Key::LControl, Key::LControl},
        {sf::Keyboard::Key::LShift, Key::LShift},
        {sf::Keyboard::Key::LAlt, Key::LAlt},
        {sf::Keyboard::Key::LSystem, Key::LSystem},
        {sf::Keyboard::Key::RControl, Key::RControl},
        {sf::Keyboard::Key::RShift, Key::RShift},
        {sf::Keyboard::Key::RAlt, Key::RAlt},
        {sf::Keyboard::Key::RSystem, Key::RSystem},
        {sf::Keyboard::Key::Menu, Key::Menu},
        {sf::Keyboard::Key::LBracket, Key::LBracket},
        {sf::Keyboard::Key::RBracket, Key::RBracket},
        {sf::Keyboard::Key::Semicolon, Key::Semicolon},
        {sf::Keyboard::Key::Comma, Key::Comma},
        {sf::Keyboard::Key::Period, Key::Period},
        {sf::Keyboard::Key::Apostrophe, Key::Apostrophe},
        {sf::Keyboard::Key::Slash, Key::Slash},
        {sf::Keyboard::Key::Backslash, Key::Backslash},
        {sf::Keyboard::Key::Grave, Key::Grave},
        {sf::Keyboard::Key::Equal, Key::Equal},
        {sf::Keyboard::Key::Hyphen, Key::Hyphen},
        {sf::Keyboard::Key::Space, Key::Space},
        {sf::Keyboard::Key::Enter, Key::Enter},
        {sf::Keyboard::Key::Backspace, Key::Backspace},
        {sf::Keyboard::Key::Tab, Key::Tab},
        {sf::Keyboard::Key::PageUp, Key::PageUp},
        {sf::Keyboard::Key::PageDown, Key::PageDown},
        {sf::Keyboard::Key::End, Key::End},
        {sf::Keyboard::Key::Home, Key::Home},
        {sf::Keyboard::Key::Insert, Key::Insert},
        {sf::Keyboard::Key::Delete, Key::Delete},
        {sf::Keyboard::Key::Add, Key::Add},
        {sf::Keyboard::Key::Subtract, Key::Subtract},
        {sf::Keyboard::Key::Multiply, Key::Multiply},
        {sf::Keyboard::Key::Divide, Key::Divide},

        {sf::Keyboard::Key::Left, Key::Left},
        {sf::Keyboard::Key::Right, Key::Right},
        {sf::Keyboard::Key::Up, Key::Up},
        {sf::Keyboard::Key::Down, Key::Down},

        {sf::Keyboard::Key::Numpad0, Key::Numpad0},
        {sf::Keyboard::Key::Numpad1, Key::Numpad1},
        {sf::Keyboard::Key::Numpad2, Key::Numpad2},
        {sf::Keyboard::Key::Numpad3, Key::Numpad3},
        {sf::Keyboard::Key::Numpad4, Key::Numpad4},
        {sf::Keyboard::Key::Numpad5, Key::Numpad5},
        {sf::Keyboard::Key::Numpad6, Key::Numpad6},
        {sf::Keyboard::Key::Numpad7, Key::Numpad7},
        {sf::Keyboard::Key::Numpad8, Key::Numpad8},
        {sf::Keyboard::Key::Numpad9, Key::Numpad9},

        {sf::Keyboard::Key::F1, Key::F1},
        {sf::Keyboard::Key::F2, Key::F2},
        {sf::Keyboard::Key::F3, Key::F3},
        {sf::Keyboard::Key::F4, Key::F4},
        {sf::Keyboard::Key::F5, Key::F5},
        {sf::Keyboard::Key::F6, Key::F6},
        {sf::Keyboard::Key::F7, Key::F7},
        {sf::Keyboard::Key::F8, Key::F8},
        {sf::Keyboard::Key::F9, Key::F9},
        {sf::Keyboard::Key::F10, Key::F10},
        {sf::Keyboard::Key::F11, Key::F11},
        {sf::Keyboard::Key::F12, Key::F12},
        {sf::Keyboard::Key::F13, Key::F13},
        {sf::Keyboard::Key::F14, Key::F14},
        {sf::Keyboard::Key::F15, Key::F15},
        {sf::Keyboard::Key::Pause, Key::Pause}
    };


    static const std::unordered_map<Key, sf::Keyboard::Key> rteKeyToSfmlKey = {
        {Key::Unknown, sf::Keyboard::Key::Unknown},

        {Key::A, sf::Keyboard::Key::A},
        {Key::B, sf::Keyboard::Key::B},
        {Key::C, sf::Keyboard::Key::C},
        {Key::D, sf::Keyboard::Key::D},
        {Key::E, sf::Keyboard::Key::E},
        {Key::F, sf::Keyboard::Key::F},
        {Key::G, sf::Keyboard::Key::G},
        {Key::H, sf::Keyboard::Key::H},
        {Key::I, sf::Keyboard::Key::I},
        {Key::J, sf::Keyboard::Key::J},
        {Key::K, sf::Keyboard::Key::K},
        {Key::L, sf::Keyboard::Key::L},
        {Key::M, sf::Keyboard::Key::M},
        {Key::N, sf::Keyboard::Key::N},
        {Key::O, sf::Keyboard::Key::O},
        {Key::P, sf::Keyboard::Key::P},
        {Key::Q, sf::Keyboard::Key::Q},
        {Key::R, sf::Keyboard::Key::R},
        {Key::S, sf::Keyboard::Key::S},
        {Key::T, sf::Keyboard::Key::T},
        {Key::U, sf::Keyboard::Key::U},
        {Key::V, sf::Keyboard::Key::V},
        {Key::W, sf::Keyboard::Key::W},
        {Key::X, sf::Keyboard::Key::X},
        {Key::Y, sf::Keyboard::Key::Y},
        {Key::Z, sf::Keyboard::Key::Z},
        {Key::Num0, sf::Keyboard::Key::Num0},
        {Key::Num1, sf::Keyboard::Key::Num1},
        {Key::Num2, sf::Keyboard::Key::Num2},
        {Key::Num3, sf::Keyboard::Key::Num3},
        {Key::Num4, sf::Keyboard::Key::Num4},
        {Key::Num5, sf::Keyboard::Key::Num5},
        {Key::Num6, sf::Keyboard::Key::Num6},
        {Key::Num7, sf::Keyboard::Key::Num7},
        {Key::Num8, sf::Keyboard::Key::Num8},
        {Key::Num9, sf::Keyboard::Key::Num9},

        {Key::Escape, sf::Keyboard::Key::Escape},
        {Key::LControl, sf::Keyboard::Key::LControl},
        {Key::LShift, sf::Keyboard::Key::LShift},
        {Key::LAlt, sf::Keyboard::Key::LAlt},
        {Key::LSystem, sf::Keyboard::Key::LSystem},
        {Key::RControl, sf::Keyboard::Key::RControl},
        {Key::RShift, sf::Keyboard::Key::RShift},
        {Key::RAlt, sf::Keyboard::Key::RAlt},
        {Key::RSystem, sf::Keyboard::Key::RSystem},
        {Key::Menu, sf::Keyboard::Key::Menu},
        {Key::LBracket, sf::Keyboard::Key::LBracket},
        {Key::RBracket, sf::Keyboard::Key::RBracket},
        {Key::Semicolon, sf::Keyboard::Key::Semicolon},
        {Key::Comma, sf::Keyboard::Key::Comma},
        {Key::Period, sf::Keyboard::Key::Period},
        {Key::Apostrophe, sf::Keyboard::Key::Apostrophe},
        {Key::Slash, sf::Keyboard::Key::Slash},
        {Key::Backslash, sf::Keyboard::Key::Backslash},
        {Key::Grave, sf::Keyboard::Key::Grave},
        {Key::Equal, sf::Keyboard::Key::Equal},
        {Key::Hyphen, sf::Keyboard::Key::Hyphen},
        {Key::Space, sf::Keyboard::Key::Space},
        {Key::Enter, sf::Keyboard::Key::Enter},
        {Key::Backspace, sf::Keyboard::Key::Backspace},
        {Key::Tab, sf::Keyboard::Key::Tab},
        {Key::PageUp, sf::Keyboard::Key::PageUp},
        {Key::PageDown, sf::Keyboard::Key::PageDown},
        {Key::End, sf::Keyboard::Key::End},
        {Key::Home, sf::Keyboard::Key::Home},
        {Key::Insert, sf::Keyboard::Key::Insert},
        {Key::Delete, sf::Keyboard::Key::Delete},
        {Key::Add, sf::Keyboard::Key::Add},
        {Key::Subtract, sf::Keyboard::Key::Subtract},
        {Key::Multiply, sf::Keyboard::Key::Multiply},
        {Key::Divide, sf::Keyboard::Key::Divide},

        {Key::Left, sf::Keyboard::Key::Left},
        {Key::Right, sf::Keyboard::Key::Right},
        {Key::Up, sf::Keyboard::Key::Up},
        {Key::Down, sf::Keyboard::Key::Down},

        {Key::Numpad0, sf::Keyboard::Key::Numpad0},
        {Key::Numpad1, sf::Keyboard::Key::Numpad1},
        {Key::Numpad2, sf::Keyboard::Key::Numpad2},
        {Key::Numpad3, sf::Keyboard::Key::Numpad3},
        {Key::Numpad4, sf::Keyboard::Key::Numpad4},
        {Key::Numpad5, sf::Keyboard::Key::Numpad5},
        {Key::Numpad6, sf::Keyboard::Key::Numpad6},
        {Key::Numpad7, sf::Keyboard::Key::Numpad7},
        {Key::Numpad8, sf::Keyboard::Key::Numpad8},
        {Key::Numpad9, sf::Keyboard::Key::Numpad9},

        {Key::F1, sf::Keyboard::Key::F1},
        {Key::F2, sf::Keyboard::Key::F2},
        {Key::F3, sf::Keyboard::Key::F3},
        {Key::F4, sf::Keyboard::Key::F4},
        {Key::F5, sf::Keyboard::Key::F5},
        {Key::F6, sf::Keyboard::Key::F6},
        {Key::F7, sf::Keyboard::Key::F7},
        {Key::F8, sf::Keyboard::Key::F8},
        {Key::F9, sf::Keyboard::Key::F9},
        {Key::F10, sf::Keyboard::Key::F10},
        {Key::F11, sf::Keyboard::Key::F11},
        {Key::F12, sf::Keyboard::Key::F12},
        {Key::F13, sf::Keyboard::Key::F13},
        {Key::F14, sf::Keyboard::Key::F14},
        {Key::F15, sf::Keyboard::Key::F15},
        {Key::Pause, sf::Keyboard::Key::Pause}
    };



    static const std::unordered_map<sf::Mouse::Button, MouseButton> sfmlMouseButtonToRteMouseButton = {
        {sf::Mouse::Button::Left, MouseButton::Left},
        {sf::Mouse::Button::Right, MouseButton::Right},
        {sf::Mouse::Button::Middle, MouseButton::Middle},
        {sf::Mouse::Button::Extra1, MouseButton::Extra1},
        {sf::Mouse::Button::Extra2, MouseButton::Extra2}
    };


    static const std::unordered_map<MouseButton, sf::Mouse::Button> rteMouseButtonToSfmlMouseButton = {
        {MouseButton::Left, sf::Mouse::Button::Left},
        {MouseButton::Right, sf::Mouse::Button::Right},
        {MouseButton::Middle, sf::Mouse::Button::Middle},
        {MouseButton::Extra1, sf::Mouse::Button::Extra1},
        {MouseButton::Extra2, sf::Mouse::Button::Extra2}
    };


    class GraphicModuleImpl : public GraphicModule {
        public:
            // Module methods
            void init(const std::shared_ptr<Ecs>& ecs) override;
            void update() override;


            // Specific methods
            void setWindowTitle(const std::string& title) override;
            void setWindowSize(const Vec2<u16>& size) override;
            void setDaltonismMode(DaltonismMode mode) override;
            void setLayerCount(int count) override;

            [[nodiscard]] Vec2<u16> getWindowSize() const override;

            void loadFontFromMemory(const void *data, u32 size) override;
            void loadFontFromFile(const char *path) override;


            // Texture methods
            uint32_t createTexture() override;
            void destroyTexture(uint32_t texture) override;

            bool loadTextureFromFile(uint32_t texture, const char *path) override;
            bool loadTextureFromMemory(uint32_t texture, const uint8_t *data, const Vec2<u16>& size) override;

            void setTextureSmooth(uint32_t texture, bool smooth) override;
            void setTextureRepeated(uint32_t texture, bool repeated) override;
            bool generateTextureMipMaps(uint32_t texture) override;

            Vec2<u16> getTextureSize(uint32_t texture) override;
            const u8 *getTexturePixels(uint32_t texture) override;


            // Input methods
            [[nodiscard]] bool isKeyPressed(Key key) const override;
            [[nodiscard]] bool isMouseButtonPressed(MouseButton button) const override;
            [[nodiscard]] Vec2<u16> getMousePosition() const override;


        private:
            std::shared_ptr<Ecs> m_ecs;

            sf::RenderWindow m_window;
            sf::Shader m_shader;
            sf::Font m_font;
            int m_layerCount = 1;

            std::shared_ptr<RenderSystem> m_renderSystem;
            std::shared_ptr<ButtonSystem> m_buttonSystem;
            std::shared_ptr<TextSystem> m_textSystem;

            std::map<uint32_t, TextureHandle> m_textures;
            uint32_t m_textureCounter = 0;
    };

}   // namespace Rte::Graphic

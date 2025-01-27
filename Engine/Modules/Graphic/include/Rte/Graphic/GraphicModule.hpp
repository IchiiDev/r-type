#pragma once

#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/ModuleManager.hpp"

#include <memory>
#include <string>

namespace Rte::Graphic {

    /**
     * @brief Enumeration of Daltonism modes.
     */
    enum class DaltonismMode : u8 {
        NONE = 0,
        PROTANOPIA = 1,
        DEUTERANOPIA = 2,
        TRITANOPIA = 3
    };


    /**
     * @brief Enumeration of button states.
     */
    enum class ButtonState : u8 {
        NONE = 0,
        HOVERED = 1,
        PRESSED = 2
    };


    /**
     * @brief Enumeration of key codes for keyboard input.
     */
    enum class Key : u8 {
        Unknown,

        A,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        I,
        J,
        K,
        L,
        M,
        N,
        O,
        P,
        Q,
        R,
        S,
        T,
        U,
        V,
        W,
        X,
        Y,
        Z,
        Num0,
        Num1,
        Num2,
        Num3,
        Num4,
        Num5,
        Num6,
        Num7,
        Num8,
        Num9,

        Escape,
        LControl,
        LShift,
        LAlt,
        LSystem,
        RControl,
        RShift,
        RAlt,
        RSystem,
        Menu,
        LBracket,   /* { */
        RBracket,   /* } */
        Semicolon,  /* ; */
        Comma,      /* , */
        Period,     /* . */
        Apostrophe, /* ' */
        Slash,      /* / */
        Backslash,  /* \ */
        Grave,      /* ` */
        Equal,      /* = */
        Hyphen,     /* - */
        Space,
        Enter,
        Backspace,
        Tab,
        PageUp,
        PageDown,
        End,
        Home,
        Insert,
        Delete,
        Add,
        Subtract,   /* - */
        Multiply,   /* * */
        Divide,     /* / */

        Left,
        Right,
        Up,
        Down,

        Numpad0,
        Numpad1,
        Numpad2,
        Numpad3,
        Numpad4,
        Numpad5,
        Numpad6,
        Numpad7,
        Numpad8,
        Numpad9,

        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        F11,
        F12,
        F13,
        F14,
        F15,
        Pause
    };


    /**
     * @brief Enumeration of mouse buttons code
     */
    enum class MouseButton : u8 {
        Left,
        Right,
        Middle,
        Extra1,
        Extra2
    };


    // Event types
    namespace Events {
        const EventType QUIT = "Events::QUIT"_hash;
        const EventType RESIZED = "Events::RESIZED"_hash;
        const EventType KEY_PRESSED = "Events::KEY_PRESSED"_hash;
        const EventType MOUSE_BUTTON_PRESSED = "Events::MOUSE_BUTTON_PRESSED"_hash;
    }   // namespace Events


    // Input event parameters
    namespace Events::Params {
        const ParamId NEW_WINDOW_SIZE = "Events::Params::NEW_WINDOW_SIZE"_hash;
        const ParamId KEY_PRESSED = "Events::Params::KEY_PRESSED"_hash;
        const ParamId MOUSE_BUTTON_PRESSED = "Events::Params::MOUSE_BUTTON_PRESSED"_hash;
        const ParamId MOUSE_BUTTON_PRESSED_POSITION = "Events::Params::MOUSE_BUTTON_PRESSED_POSITION"_hash;
    }   // namespace Events::Params


    /**
     * @brief Graphic module interface.
     * This module is responsible for rendering entities to the window.
     * It uses the RenderSystem to render all the entities that have a transform and a sprite component.
     */
    class GraphicModule : public IModule {
        public:
            GraphicModule() = default;
            ~GraphicModule() override = default;

            GraphicModule(const GraphicModule&) = delete;
            GraphicModule& operator=(const GraphicModule&) = delete;

            GraphicModule(GraphicModule&&) = default;
            GraphicModule& operator=(GraphicModule&&) = default;




        public: // Overridden methods
            /**
             * @brief Initializes the module.
             * /!\ This method must be called before any other method.
             *
             * @param ecs Pointer to the ECS.
             */
            void init(const std::shared_ptr<Ecs>& ecs) override = 0;

            /**
             * @brief Updates the module.
             * This method should be called every frame.
             * It will render all the entities to the window using the RenderSystem.
             */
            void update() override = 0;




        public: // Specific methods
            /**
             * @brief Sets the title of the window.
             *
             * @param title Title of the window.
             */
            virtual void setWindowTitle(const std::string& title) = 0;

            /**
             * @brief Set the size of the window.
             *
             * @param size Size of the window.
             */
            virtual void setWindowSize(const Vec2<u16>& size) = 0;

            /**
             * @brief Set the position of the window.
             *
             * @param position Position of the window.
             */
            virtual void setWindowPosition(const Vec2<int>& position) = 0;

            /**
             * @brief Set the Daltonism mode.
             *
             * @param mode Daltonism mode (NONE, PROTANOPIA, DEUTERANOPIA, TRITANOPIA).
             */
            virtual void setDaltonismMode(DaltonismMode mode) = 0;

            /**
             * @brief Set the Layer count of the window.
             * This value will be used to sort the entities to render them in the correct order.
             *
             * @param count Layer count.
             */
            virtual void setLayerCount(int count) = 0;

            /**
             * @brief Get the size of the window.
             *
             * @return Vec2<u16> Size of the window.
             */
            [[nodiscard]] virtual Vec2<u16> getWindowSize() const = 0;

            virtual void loadFontFromMemory(const void *data, u32 size) = 0;
            virtual void loadFontFromFile(const char *path) = 0;




        public: // Texture methods
            [[nodiscard]] virtual uint32_t createTexture() = 0;
            virtual void destroyTexture(uint32_t texture) = 0;

            [[nodiscard]] virtual bool loadTextureFromFile(uint32_t texture, const char *path) = 0;
            [[nodiscard]] virtual bool loadTextureFromMemory(uint32_t texture, const uint8_t *data, const Vec2<u16>& size) = 0;

            virtual void setTextureSmooth(uint32_t texture, bool smooth) = 0;
            virtual void setTextureRepeated(uint32_t texture, bool repeated) = 0;
            [[nodiscard]] virtual bool generateTextureMipMaps(uint32_t texture) = 0;

            [[nodiscard]] virtual Vec2<u16> getTextureSize(uint32_t texture) = 0;
            [[nodiscard]] virtual const u8 *getTexturePixels(uint32_t texture) = 0;




        public: // Direc draw methods
            /**
             * @brief Add a sprite to the draw list, and draw it when the update method is called.
             *
             * @param position Position of the sprite.
             * @param scale Scale of the sprite.
             * @param texture Texture of the sprite.
             */
            virtual void drawRectangle(const Vec2<int>& position, const Vec2<u16>& scale, uint32_t texture) =0;

            /**
             * @brief Add a text to the draw list, and draw it when the update method is called.
             *
             * @param position Position of the text.
             * @param text Text to draw.
             * @param characterSize Size of the characters.
             */
            virtual void drawText(const Vec2<int>& position, const std::string& text, u32 characterSize) =0;




        public:  // Input methods
            /**
             * @brief Check if a key is pressed.
             *
             * @param key Key to check.
             * @return true if the key is pressed, false otherwise.
             */
            [[nodiscard]] virtual bool isKeyPressed(Key key) const = 0;

            /**
             * @brief Check if a mouse button is pressed.
             *
             * @param button Mouse button to check.
             * @return true if the mouse button is pressed, false otherwise.
             */
            [[nodiscard]] virtual bool isMouseButtonPressed(MouseButton button) const = 0;

            /**
             * @brief Get the position of the mouse.
             *
             * @return Vec2<u16> Position of the mouse.
             */
            [[nodiscard]] virtual Vec2<u16> getMousePosition() const = 0;
    };

}   // namespace Rte::Graphic

extern "C" EXPORT Rte::IModule* createModule();

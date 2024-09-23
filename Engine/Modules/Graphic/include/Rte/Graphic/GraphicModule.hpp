#pragma once

#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/Graphic/Texture.hpp"
#include "Rte/ModuleManager.hpp"

#include <memory>
#include <string>

namespace Rte::Graphic {

    /**
     * @brief Enumeration of Daltonism modes.
     */
    enum DaltonismMode : u8 {
        NONE = 0,
        PROTANOPIA = 1,
        DEUTERANOPIA = 2,
        TRITANOPIA = 3
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
             * @brief Set the Daltonism mode.
             *
             * @param mode Daltonism mode (NONE, PROTANOPIA, DEUTERANOPIA, TRITANOPIA).
             */
            virtual void setDaltonismMode(DaltonismMode mode) = 0;

            /**
             * @brief Get the size of the window.
             *
             * @return Vec2<u16> Size of the window.
             */
            [[nodiscard]] virtual Vec2<u16> getWindowSize() const = 0;

            /**
             * @brief Create a texture.
             *
             * @return std::shared_ptr<Texture> Shared pointer to the texture.
             */
            [[nodiscard]] virtual std::shared_ptr<Texture> createTexture() const = 0;
    };


    // Event types
    namespace Events {
        const EventType QUIT = "Events::QUIT"_hash;
        const EventType RESIZED = "Events::RESIZED"_hash;
        const EventType KEY_PRESSED = "Events::KEY_PRESSED"_hash;
        const EventType TIMED_KEY_PRESSED = "Events::TIMED_KEY_PRESSED"_hash;
    }   // namespace Events


    // Resized event parameters
    namespace Events::Params::Resized {
        const ParamId WIDTH = "Events::Params::Resized::WIDTH"_hash;
        const ParamId HEIGHT = "Events::Params::Resized::HEIGHT"_hash;
    }   // namespace Events::Params::Resized


    // Input event parameters
    namespace Events::Params {
        const ParamId KEY_PRESSED = "Events::Params::KEY_PRESSED"_hash;
        const ParamId TIMED_KEY_PRESSED = "Events::Params::TIMED_KEY_PRESSED"_hash;
    }   // namespace Events::Params

}   // namespace Rte::Graphic

extern "C" EXPORT Rte::IModule* createModule();

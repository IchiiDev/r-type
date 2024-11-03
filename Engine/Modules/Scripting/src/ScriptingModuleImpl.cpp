#include "ScriptingModuleImpl.hpp"
#include "Rte/BasicComponents.hpp"
#include "Rte/Common.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/Graphic/GraphicModule.hpp"
#include "Rte/ModuleManager.hpp"

#include "Rte/Scripting/Components.hpp"
#include "sol/sol.hpp"
#include "sol/state.hpp"

#include <cassert>
#include <memory>
#include <unordered_map>

using namespace Rte::Scripting;




Rte::IModule *createModule() {
    return new ScriptingModuleImpl(); // NOLINT(cppcoreguidelines-owning-memory)
}

void ScriptingModuleImpl::init(const std::shared_ptr<Ecs>& ecs) {
    m_ecs = ecs;
    m_ecs->registerComponent<Components::Script>();

    m_lua.open_libraries(sol::lib::base);
    m_lua.open_libraries(sol::lib::math);
    m_lua.new_usertype<Vec2<float>>(
        "Vec2",
        "x", &Vec2<float>::x,
        "y", &Vec2<float>::y
    );

    m_lua.new_usertype<BasicComponents::Transform>(
        "Transform",
        "position", &BasicComponents::Transform::position,
        "rotation", &BasicComponents::Transform::rotation,
        "scale", &BasicComponents::Transform::scale
    );


    {   // Lua coroutine isKeyPressed using graphicModule
        m_lua.set_function("isKeyPressed", [this](const std::string& key) {
            Rte::Graphic::Key rteKey{};

            if (key == "Unknown") rteKey = Rte::Graphic::Key::Unknown;
            else if (key == "Unknown") rteKey =  Rte::Graphic::Key::Unknown;
            else if (key == "A") rteKey =  Rte::Graphic::Key::A;
            else if (key == "B") rteKey =  Rte::Graphic::Key::B;
            else if (key == "C") rteKey =  Rte::Graphic::Key::C;
            else if (key == "D") rteKey =  Rte::Graphic::Key::D;
            else if (key == "E") rteKey =  Rte::Graphic::Key::E;
            else if (key == "F") rteKey =  Rte::Graphic::Key::F;
            else if (key == "G") rteKey =  Rte::Graphic::Key::G;
            else if (key == "H") rteKey =  Rte::Graphic::Key::H;
            else if (key == "I") rteKey =  Rte::Graphic::Key::I;
            else if (key == "J") rteKey =  Rte::Graphic::Key::J;
            else if (key == "K") rteKey =  Rte::Graphic::Key::K;
            else if (key == "L") rteKey =  Rte::Graphic::Key::L;
            else if (key == "M") rteKey =  Rte::Graphic::Key::M;
            else if (key == "N") rteKey =  Rte::Graphic::Key::N;
            else if (key == "O") rteKey =  Rte::Graphic::Key::O;
            else if (key == "P") rteKey =  Rte::Graphic::Key::P;
            else if (key == "Q") rteKey =  Rte::Graphic::Key::Q;
            else if (key == "R") rteKey =  Rte::Graphic::Key::R;
            else if (key == "S") rteKey =  Rte::Graphic::Key::S;
            else if (key == "T") rteKey =  Rte::Graphic::Key::T;
            else if (key == "U") rteKey =  Rte::Graphic::Key::U;
            else if (key == "V") rteKey =  Rte::Graphic::Key::V;
            else if (key == "W") rteKey =  Rte::Graphic::Key::W;
            else if (key == "X") rteKey =  Rte::Graphic::Key::X;
            else if (key == "Y") rteKey =  Rte::Graphic::Key::Y;
            else if (key == "Z") rteKey =  Rte::Graphic::Key::Z;
            else if (key == "Num0") rteKey =  Rte::Graphic::Key::Num0;
            else if (key == "Num1") rteKey =  Rte::Graphic::Key::Num1;
            else if (key == "Num2") rteKey =  Rte::Graphic::Key::Num2;
            else if (key == "Num3") rteKey =  Rte::Graphic::Key::Num3;
            else if (key == "Num4") rteKey =  Rte::Graphic::Key::Num4;
            else if (key == "Num5") rteKey =  Rte::Graphic::Key::Num5;
            else if (key == "Num6") rteKey =  Rte::Graphic::Key::Num6;
            else if (key == "Num7") rteKey =  Rte::Graphic::Key::Num7;
            else if (key == "Num8") rteKey =  Rte::Graphic::Key::Num8;
            else if (key == "Num9") rteKey =  Rte::Graphic::Key::Num9;
            else if (key == "Escape") rteKey =  Rte::Graphic::Key::Escape;
            else if (key == "LControl") rteKey =  Rte::Graphic::Key::LControl;
            else if (key == "LShift") rteKey =  Rte::Graphic::Key::LShift;
            else if (key == "LAlt") rteKey =  Rte::Graphic::Key::LAlt;
            else if (key == "LSystem") rteKey =  Rte::Graphic::Key::LSystem;
            else if (key == "RControl") rteKey =  Rte::Graphic::Key::RControl;
            else if (key == "RShift") rteKey =  Rte::Graphic::Key::RShift;
            else if (key == "RAlt") rteKey =  Rte::Graphic::Key::RAlt;
            else if (key == "RSystem") rteKey =  Rte::Graphic::Key::RSystem;
            else if (key == "Menu") rteKey =  Rte::Graphic::Key::Menu;
            else if (key == "LBracket") rteKey =  Rte::Graphic::Key::LBracket;
            else if (key == "RBracket") rteKey =  Rte::Graphic::Key::RBracket;
            else if (key == "Semicolon") rteKey =  Rte::Graphic::Key::Semicolon;
            else if (key == "Comma") rteKey =  Rte::Graphic::Key::Comma;
            else if (key == "Period") rteKey =  Rte::Graphic::Key::Period;
            else if (key == "Apostrophe") rteKey =  Rte::Graphic::Key::Apostrophe;
            else if (key == "Slash") rteKey =  Rte::Graphic::Key::Slash;
            else if (key == "Backslash") rteKey =  Rte::Graphic::Key::Backslash;
            else if (key == "Grave") rteKey =  Rte::Graphic::Key::Grave;
            else if (key == "Equal") rteKey =  Rte::Graphic::Key::Equal;
            else if (key == "Hyphen") rteKey =  Rte::Graphic::Key::Hyphen;
            else if (key == "Space") rteKey =  Rte::Graphic::Key::Space;
            else if (key == "Enter") rteKey =  Rte::Graphic::Key::Enter;
            else if (key == "Backspace") rteKey =  Rte::Graphic::Key::Backspace;
            else if (key == "Tab") rteKey =  Rte::Graphic::Key::Tab;
            else if (key == "PageUp") rteKey =  Rte::Graphic::Key::PageUp;
            else if (key == "PageDown") rteKey =  Rte::Graphic::Key::PageDown;
            else if (key == "End") rteKey =  Rte::Graphic::Key::End;
            else if (key == "Home") rteKey =  Rte::Graphic::Key::Home;
            else if (key == "Insert") rteKey =  Rte::Graphic::Key::Insert;
            else if (key == "Delete") rteKey =  Rte::Graphic::Key::Delete;
            else if (key == "Add") rteKey =  Rte::Graphic::Key::Add;
            else if (key == "Subtract") rteKey =  Rte::Graphic::Key::Subtract;
            else if (key == "Multiply") rteKey =  Rte::Graphic::Key::Multiply;
            else if (key == "Divide") rteKey =  Rte::Graphic::Key::Divide;
            else if (key == "Left") rteKey =  Rte::Graphic::Key::Left;
            else if (key == "Right") rteKey =  Rte::Graphic::Key::Right;
            else if (key == "Up") rteKey =  Rte::Graphic::Key::Up;
            else if (key == "Down") rteKey =  Rte::Graphic::Key::Down;
            else if (key == "Numpad0") rteKey =  Rte::Graphic::Key::Numpad0;
            else if (key == "Numpad1") rteKey =  Rte::Graphic::Key::Numpad1;
            else if (key == "Numpad2") rteKey =  Rte::Graphic::Key::Numpad2;
            else if (key == "Numpad3") rteKey =  Rte::Graphic::Key::Numpad3;
            else if (key == "Numpad4") rteKey =  Rte::Graphic::Key::Numpad4;
            else if (key == "Numpad5") rteKey =  Rte::Graphic::Key::Numpad5;
            else if (key == "Numpad6") rteKey =  Rte::Graphic::Key::Numpad6;
            else if (key == "Numpad7") rteKey =  Rte::Graphic::Key::Numpad7;
            else if (key == "Numpad8") rteKey =  Rte::Graphic::Key::Numpad8;
            else if (key == "Numpad9") rteKey =  Rte::Graphic::Key::Numpad9;
            else if (key == "F1") rteKey =  Rte::Graphic::Key::F1;
            else if (key == "F2") rteKey =  Rte::Graphic::Key::F2;
            else if (key == "F3") rteKey =  Rte::Graphic::Key::F3;
            else if (key == "F4") rteKey =  Rte::Graphic::Key::F4;
            else if (key == "F5") rteKey =  Rte::Graphic::Key::F5;
            else if (key == "F6") rteKey =  Rte::Graphic::Key::F6;
            else if (key == "F7") rteKey =  Rte::Graphic::Key::F7;
            else if (key == "F8") rteKey =  Rte::Graphic::Key::F8;
            else if (key == "F9") rteKey =  Rte::Graphic::Key::F9;
            else if (key == "F10") rteKey =  Rte::Graphic::Key::F10;
            else if (key == "F11") rteKey =  Rte::Graphic::Key::F11;
            else if (key == "F12") rteKey =  Rte::Graphic::Key::F12;
            else if (key == "F13") rteKey =  Rte::Graphic::Key::F13;
            else if (key == "F14") rteKey =  Rte::Graphic::Key::F14;
            else if (key == "F15") rteKey =  Rte::Graphic::Key::F15;
            else if (key == "Pause") rteKey =  Rte::Graphic::Key::Pause;

            return m_graphicModule->isKeyPressed(rteKey);
        });
    }

    m_scriptingSystem = m_ecs->registerSystem<ScriptingSystem>();
    m_scriptingSystem->init(m_ecs, &m_lua);

    Signature signature;
    signature.set(m_ecs->getComponentType<Components::Script>());
    signature.set(m_ecs->getComponentType<BasicComponents::Transform>());
    m_ecs->setSystemSignature<ScriptingSystem>(signature);
}

void ScriptingModuleImpl::update() {
    m_scriptingSystem->update();
}

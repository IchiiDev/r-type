#include "ClientApp.hpp"

#include "Button.hpp"
#include "Rte/Common.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/Graphic/Components.hpp"
#include "Rte/Graphic/GraphicModule.hpp"
#include "Rte/Graphic/Texture.hpp"

#include <iostream>
#include <memory>
#include <vector>

void ClientApp::createMenuButtons() {
    m_menuButtons.push_back(std::make_unique<Button>(m_ecs, m_graphicModule, m_transparentTexture, true, Rte::Vec2<float>(0, -80), Rte::Vec2<float>(200, 50), "Play", 30, [&]() {
        std::cout << "Play button clicked\n";
        m_running = false;
    }));

    m_menuButtons.push_back(std::make_unique<Button>(m_ecs, m_graphicModule, m_transparentTexture, true, Rte::Vec2<float>(0, 0), Rte::Vec2<float>(200, 50), "Options", 30, [&]() {
        m_switchedOptionsState = true;
        m_showOptions = true;
    }));

    m_menuButtons.push_back(std::make_unique<Button>(m_ecs, m_graphicModule, m_transparentTexture, true, Rte::Vec2<float>(0, 80), Rte::Vec2<float>(200, 50), "Quit", 30, [&]() {
        m_running = false;
    }));
}

void ClientApp::createOptionsButtons() {
    // Resolution buttons
    m_menuButtons.push_back(std::make_unique<Button>(m_ecs, m_graphicModule, m_transparentTexture, false, Rte::Vec2<float>(0, -300), Rte::Vec2<float>(200, 50), "Resolution", 40, []() {}));

    m_menuButtons.push_back(std::make_unique<Button>(m_ecs, m_graphicModule, m_transparentTexture, true, Rte::Vec2<float>(0, -260), Rte::Vec2<float>(150, 30), "1920x1080", 30, [&]() {
        m_graphicModule->setWindowSize({1920, 1080});
    }));

    m_menuButtons.push_back(std::make_unique<Button>(m_ecs, m_graphicModule, m_transparentTexture, true, Rte::Vec2<float>(0, -230), Rte::Vec2<float>(150, 30), "1280x720", 30, [&]() {
        m_graphicModule->setWindowSize({1280, 720});
    }));

    m_menuButtons.push_back(std::make_unique<Button>(m_ecs, m_graphicModule, m_transparentTexture, true, Rte::Vec2<float>(0, -200), Rte::Vec2<float>(150, 30), "800x600", 30, [&]() {
        m_graphicModule->setWindowSize({800, 600});
    }));


    // Sound buttons (a series of buttons for sound volume)
    m_menuButtons.push_back(std::make_unique<Button>(m_ecs, m_graphicModule, m_transparentTexture, false, Rte::Vec2<float>(0, -100), Rte::Vec2<float>(200, 50), "Sound", 40, []() {}));

    for (int i = 0; i < 10; ++i) {
        m_menuButtons.push_back(std::make_unique<Button>(m_ecs, m_graphicModule, m_transparentTexture, true, Rte::Vec2<float>(-100.F + static_cast<float>(i) * 20, -50), Rte::Vec2<float>(15, 30), std::to_string(i), 30, [this, i]() {
            m_soundVolume = i;
        }));
    }


    // Daltonism mode buttons
    m_menuButtons.push_back(std::make_unique<Button>(m_ecs, m_graphicModule, m_transparentTexture, false, Rte::Vec2<float>(0, 50), Rte::Vec2<float>(200, 50), "Daltonism mode", 40, []() {}));

    m_menuButtons.push_back(std::make_unique<Button>(m_ecs, m_graphicModule, m_transparentTexture, true, Rte::Vec2<float>(-100, 100), Rte::Vec2<float>(200, 50), "None", 30, [&]() {
        m_graphicModule->setDaltonismMode(Rte::Graphic::DaltonismMode::NONE);
    }));

    m_menuButtons.push_back(std::make_unique<Button>(m_ecs, m_graphicModule, m_transparentTexture, true, Rte::Vec2<float>(100, 100), Rte::Vec2<float>(200, 50), "Protanopia", 30, [&]() {
        m_graphicModule->setDaltonismMode(Rte::Graphic::DaltonismMode::PROTANOPIA);
    }));

    m_menuButtons.push_back(std::make_unique<Button>(m_ecs, m_graphicModule, m_transparentTexture, true, Rte::Vec2<float>(-100, 150), Rte::Vec2<float>(200, 50), "Deuteranopia", 30, [&]() {
        m_graphicModule->setDaltonismMode(Rte::Graphic::DaltonismMode::DEUTERANOPIA);
    }));

    m_menuButtons.push_back(std::make_unique<Button>(m_ecs, m_graphicModule, m_transparentTexture, true, Rte::Vec2<float>(100, 150), Rte::Vec2<float>(200, 50), "Tritanopia", 30, [&]() {
        m_graphicModule->setDaltonismMode(Rte::Graphic::DaltonismMode::TRITANOPIA);
    }));


    // Back button
    m_menuButtons.push_back(std::make_unique<Button>(m_ecs, m_graphicModule, m_transparentTexture, true, Rte::Vec2<float>(0, 250), Rte::Vec2<float>(200, 50), "Back", 30, [&]() {
        m_switchedOptionsState = true;
        m_showOptions = false;
    }));
}

void ClientApp::menuLoop() {
    // 1*1 red texture for the buttons
    m_transparentTexture = m_graphicModule->createTexture();
    m_transparentTexture->loadFromMemory(std::vector<Rte::u8>{0, 0, 0, 0}.data(), {1, 1});

    std::shared_ptr<Rte::Graphic::Texture> backgroundTexture = m_graphicModule->createTexture();
    backgroundTexture->loadFromFile("../assets/test.jpg");
    Rte::Entity backgroundEntity = m_ecs->createEntity();
    m_ecs->addComponent<Rte::Graphic::Components::Sprite>(backgroundEntity, Rte::Graphic::Components::Sprite(backgroundTexture));
    m_ecs->addComponent<Rte::BasicComponents::Transform>(backgroundEntity, Rte::BasicComponents::Transform{.position = {0, 0}});

    createMenuButtons();

    while (m_running) {
        for (const auto& button : m_menuButtons)
            button->update();

        if (m_switchedOptionsState) {
            m_switchedOptionsState = false;
            m_menuButtons.clear();

            if (m_showOptions)
                createOptionsButtons();
            else
                createMenuButtons();
        }

        m_graphicModule->update();
    }
}

#include "Rte/BasicComponents.hpp"
#include "ServerApp.hpp"
#include "Utils/BinaryMap.hpp"

#include "Rte/Physics/Components.hpp"
#include <iostream>

void ServerApp::initScene() {
    // Init destruction map

    uint32_t round1 = m_graphicModule->createTexture();
    if (!m_graphicModule->loadTextureFromFile(round1, "../assets/round1.png"))
        throw std::runtime_error("Failed to load texture: \"../assets/round1.png\"");

    m_destructionMaps["round1"].first = m_graphicModule->getTextureSize(round1);
    m_destructionMaps["round1"].second = convertToBinary(m_graphicModule->getTexturePixels(round1), m_graphicModule->getTextureSize(round1));
    // Invisible walls

    // Left wall
    m_leftWall = m_ecs->createEntity();
    m_ecs->addComponent<Rte::BasicComponents::Transform>(m_leftWall, Rte::BasicComponents::Transform{{-10, 0}, {1, 100}, 0});
    m_ecs->addComponent<Rte::Physics::Components::Physics>(m_leftWall, Rte::Physics::Components::Physics{.shapeBody = m_physicsModule->createShapeBody(
        {100, 2000},
        1,
        0,
        {-100, 0},
        0,
        true,
        true,
        Rte::Physics::ShapeType::RECTANGLE
    )});

    // Right wall
    m_rightWall = m_ecs->createEntity();
    m_ecs->addComponent<Rte::BasicComponents::Transform>(m_rightWall, Rte::BasicComponents::Transform{{1910, 0}, {1, 100}, 0});
    m_ecs->addComponent<Rte::Physics::Components::Physics>(m_rightWall, Rte::Physics::Components::Physics{.shapeBody = m_physicsModule->createShapeBody(
        {100, 2000},
        1,
        0,
        {2020, 0},
        0,
        true,
        true,
        Rte::Physics::ShapeType::RECTANGLE
    )});

    // Top wall
    m_topWall = m_ecs->createEntity();
    m_ecs->addComponent<Rte::BasicComponents::Transform>(m_topWall, Rte::BasicComponents::Transform{{0, 0}, {100, 1}, 0});
    m_ecs->addComponent<Rte::Physics::Components::Physics>(m_topWall, Rte::Physics::Components::Physics{.shapeBody = m_physicsModule->createShapeBody(
        {2000, 100},
        1,
        0,
        {0, -100},
        0,
        true,
        true,
        Rte::Physics::ShapeType::RECTANGLE
    )});

    // Bottom wall
    m_bottomWall = m_ecs->createEntity();
    m_ecs->addComponent<Rte::BasicComponents::Transform>(m_bottomWall, Rte::BasicComponents::Transform{{0, 1070}, {100, 1}, 0});
    m_ecs->addComponent<Rte::Physics::Components::Physics>(m_bottomWall, Rte::Physics::Components::Physics{.shapeBody = m_physicsModule->createShapeBody(
        {2000, 100},
        1,
        0.01,
        {0, 1180},
        0,
        true,
        true,
        Rte::Physics::ShapeType::RECTANGLE
    )});

    // Breakables
    createBreakable({0, 1080 / 2 - 200}, "mushroom");

    // Terrain

    m_terrains.push_back(generateTerrainBlock(m_terrainBlockWidth, m_terrainBlockHeight, {-1920/2, static_cast<float>(1080 / 2 - m_terrainBlockHeight / 2 * 8)}));

    while (m_ecs->getComponent<Rte::BasicComponents::Transform>(m_terrains.at(m_terrains.size() - 1)).position.x < 1920) {
        m_terrains.push_back(generateTerrainBlock(
            m_terrainBlockWidth,
            m_terrainBlockHeight,
            {
                m_ecs->getComponent<Rte::BasicComponents::Transform>(m_terrains.at(m_terrains.size() - 1)).position.x + m_terrainBlockWidth * 8 - (1920 / 2) - 16,
                static_cast<float>(1080 / 2 - m_terrainBlockHeight / 2 * 8)
            }
        ));
    }
}

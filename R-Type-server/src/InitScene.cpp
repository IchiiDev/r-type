#include "Rte/Graphic/Texture.hpp"
#include "ServerApp.hpp"
#include "Utils/BinaryMap.hpp"

#include "Rte/Physics/Components.hpp"

void ServerApp::initScene() {
    // Init destruction map

    const std::shared_ptr<Rte::Graphic::Texture> round1 = m_graphicModule->createTexture();
    round1->loadFromFile("../assets/round1.png");
    
    m_destructionMaps["round1"].first = round1->getSize();
    m_destructionMaps["round1"].second = convertToBinary(round1->getPixels(), round1->getSize());
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
        0,
        {0, 1180},
        0,
        true,
        true,
        Rte::Physics::ShapeType::RECTANGLE
    )});

    // Breakables
    createBreakable({0, 1080 / 2 - 200}, "mushroom");
}
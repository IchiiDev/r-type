#include "ServerApp.hpp"

#include "Rte/Ecs/Types.hpp"
#include "Rte/Physics/Components.hpp"

void ServerApp::initScene() {
    // Invisible walls

    // Left wall
    const Rte::Entity leftWall = m_ecs->createEntity();
    m_ecs->addComponent<Rte::BasicComponents::Transform>(leftWall, Rte::BasicComponents::Transform{{-10, 0}, {1, 100}, 0});
    m_ecs->addComponent<Rte::Physics::Components::Physics>(leftWall, Rte::Physics::Components::Physics{.shapeBody = m_physicsModule->createShapeBody(
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
    const Rte::Entity topWall = m_ecs->createEntity();
    m_ecs->addComponent<Rte::BasicComponents::Transform>(topWall, Rte::BasicComponents::Transform{{0, 0}, {100, 1}, 0});
    m_ecs->addComponent<Rte::Physics::Components::Physics>(topWall, Rte::Physics::Components::Physics{.shapeBody = m_physicsModule->createShapeBody(
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
    const Rte::Entity bottomWall = m_ecs->createEntity();
    m_ecs->addComponent<Rte::BasicComponents::Transform>(bottomWall, Rte::BasicComponents::Transform{{0, 1070}, {100, 1}, 0});
    m_ecs->addComponent<Rte::Physics::Components::Physics>(bottomWall, Rte::Physics::Components::Physics{.shapeBody = m_physicsModule->createShapeBody(
        {2000, 100},
        1,
        0,
        {0, 1180},
        0,
        true,
        true,
        Rte::Physics::ShapeType::RECTANGLE
    )});
}
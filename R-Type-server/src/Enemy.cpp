#include "Enemy.hpp"
#include "Rte/BasicComponents.hpp"
#include "Rte/Common.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/Graphic/Components.hpp"
#include "Rte/Graphic/GraphicModule.hpp"
#include "Rte/Graphic/Texture.hpp"
#include "Rte/Physics/Components.hpp"
#include "Rte/Physics/PhysicsModule.hpp"
#include "Rte/Physics/ShapeBody.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <memory>
#include <numbers>



Enemy::Enemy(const std::shared_ptr<Rte::Ecs>& ecs, const std::shared_ptr<Rte::Graphic::GraphicModule>& graphicModule, const std::shared_ptr<Rte::Physics::PhysicsModule>& physicsModule, uint32_t uid, Rte::Vec2<float> pos) : m_enemy(ecs->createEntity()), m_ecs(ecs), m_graphicModule(graphicModule), m_physicsModule(physicsModule) {
    std::shared_ptr<Rte::Graphic::Texture> enemyTexture = m_graphicModule->createTexture();
    enemyTexture->loadFromFile("../assets/enemy.png");

    int randR = rand() % 255;
    int randG = rand() % 255;
    int randB = rand() % 255;
    std::vector<Rte::u8> randColorText(static_cast<size_t>(enemyTexture->getSize().x * enemyTexture->getSize().y * 4));
    for (size_t i = 0; i < randColorText.size(); i += 4) {
        randColorText[i] = std::clamp(enemyTexture->getPixels()[i] + randR, 0, 255);
        randColorText[i + 1] = std::clamp(enemyTexture->getPixels()[i + 1] + randG, 0, 255);
        randColorText[i + 2] = std::clamp(enemyTexture->getPixels()[i + 2] + randB, 0, 255);
        randColorText[i + 3] = enemyTexture->getPixels()[i + 3];
    }
    enemyTexture->loadFromMemory(randColorText.data(), enemyTexture->getSize());
    constexpr Rte::Vec2<float> enemyScale = {3, 3};
    

    m_ecs->addComponent<Rte::BasicComponents::UidComponents>(m_enemy, Rte::BasicComponents::UidComponents(uid));
    m_ecs->addComponent<Rte::Graphic::Components::Sprite>(m_enemy, Rte::Graphic::Components::Sprite(enemyTexture));
    m_ecs->addComponent<Rte::BasicComponents::Transform>(m_enemy, Rte::BasicComponents::Transform{pos, enemyScale, 0});
    m_ecs->addComponent<Rte::Physics::Components::Physics>(m_enemy, Rte::Physics::Components::Physics{.shapeBody = m_physicsModule->createShapeBody(
        {31 * 3, 47 * 3}, 1, 0,
        pos, 0, true, false,
        Rte::Physics::ShapeType::CAPSULE
    )});
}

Rte::Entity Enemy::getEntity() const {
    return m_enemy;
}

void Enemy::move(Rte::Vec2<float> direction) {
    m_physicsModule->move(m_ecs->getComponent<Rte::Physics::Components::Physics>(m_enemy).shapeBody, direction);
}

void Enemy::fly(Rte::Vec2<float> direction) {
    m_physicsModule->applyForce(m_ecs->getComponent<Rte::Physics::Components::Physics>(m_enemy).shapeBody, direction);
}

Rte::Entity Enemy::shoot(float angle) {
    //Shoot projectile
    if (m_mana < 20 || m_shootCooldown > 0)
        return 0;
    m_mana -= 20;
    Rte::Vec2<float> enemyPos = m_ecs->getComponent<Rte::BasicComponents::Transform>(m_enemy).position;
    const std::shared_ptr<Rte::Graphic::Texture> projectileTexture = m_graphicModule->createTexture();
    projectileTexture->loadFromFile("../assets/projectile.png");
    Rte::Entity projectile = m_ecs->createEntity();

    m_ecs->addComponent<Rte::Graphic::Components::Sprite>(projectile, Rte::Graphic::Components::Sprite(projectileTexture));
    m_ecs->addComponent<Rte::BasicComponents::Transform>(projectile, Rte::BasicComponents::Transform{
        .position = {static_cast<float>(cos(angle) * 100) + enemyPos.x, static_cast<float>(sin(angle) * 100) + enemyPos.y},
        .scale = {8, 8},
        .rotation = angle * 180 / std::numbers::pi_v<float>
    });
    m_ecs->addComponent<Rte::Physics::Components::Physics>(projectile, Rte::Physics::Components::Physics{.shapeBody = m_physicsModule->createShapeBody(
        {32, 0},
        0.05,
        0.3,
        {m_ecs->getComponent<Rte::BasicComponents::Transform>(projectile).position.x + m_graphicModule->getWindowSize().x / 2,
            m_ecs->getComponent<Rte::BasicComponents::Transform>(projectile).position.y + m_graphicModule->getWindowSize().y / 2},
        m_ecs->getComponent<Rte::BasicComponents::Transform>(projectile).rotation,
        false,
        false,
        Rte::Physics::ShapeType::CIRCLE
    )});
    float force = 0.05;
    m_physicsModule->applyForce(m_ecs->getComponent<Rte::Physics::Components::Physics>(projectile).shapeBody, {
        static_cast<float>(cos(angle) * force),
        static_cast<float>(sin(-angle) * force)
    });
    m_shootCooldown = 0.5;
    return projectile;
}

void Enemy::takeDamage() {
    m_health -= 100;
    if (m_health < 0)
        m_health = 0;
}

void Enemy::update() {
    m_mana += m_manaRegen;
    if (m_mana > m_maxMana)
        m_mana = m_maxMana;
    if (m_shootCooldown > 0)
        m_shootCooldown -= 0.01;
}

float Enemy::getHealth() const {
    return m_health;
}

float Enemy::getMana() const {
    return m_mana;
}

float Enemy::getFlightTime() const {
    return m_flightTime;
}

Rte::Vec2<float> Enemy::getPos() const {
    return m_ecs->getComponent<Rte::BasicComponents::Transform>(m_enemy).position;
}
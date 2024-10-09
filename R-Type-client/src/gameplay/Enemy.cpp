#include "Enemy.hpp"
#include "Rte/Common.hpp"
#include "Rte/Graphic/Texture.hpp"
#include <cmath>
#include <memory>
#include "Rte/Graphic/GraphicModule.hpp"
#include "Rte/Graphic/Components.hpp"
#include "Rte/Graphic/Texture.hpp"
#include "Rte/Physics/PhysicsModule.hpp"
#include "Rte/Physics/Components.hpp"

static float getRotFromPoints(const Rte::Vec2<float> p1, const Rte::Vec2<float> p2) {
    return atan2(p2.y - p1.y, p2.x - p1.x);
}

Enemy::Enemy(const std::shared_ptr<Rte::Ecs>& ecs, const std::shared_ptr<Rte::Graphic::GraphicModule>& graphicModule, const std::shared_ptr<Rte::Physics::PhysicsModule>& physicsModule) : m_enemy(ecs->createEntity()), m_ecs(ecs), m_graphicModule(graphicModule), m_physicsModule(physicsModule) {
    const std::shared_ptr<Rte::Graphic::Texture> enemyTexture = m_graphicModule->createTexture();
    enemyTexture->loadFromFile("../assets/enemy.png");

    constexpr Rte::Vec2<float> enemyScale = {8, 8};
    const Rte::Vec2<float> enemyPosition = {
        1700,
        800
    };

    m_ecs->addComponent<Rte::Graphic::Components::Sprite>(m_enemy, Rte::Graphic::Components::Sprite(enemyTexture));
    m_ecs->addComponent<Rte::BasicComponents::Transform>(m_enemy, Rte::BasicComponents::Transform{
        .position = enemyPosition,
        .scale = enemyScale,
        .rotation = 0
    });

    m_ecs->addComponent<Rte::Physics::Components::Physics>(m_enemy, Rte::Physics::Components::Physics{.shapeBody = m_physicsModule->createShapeBody(
        {32 * 8, 64 * 8},
        1,
        50,
        enemyPosition,
        0,
        true
    )});
}

void Enemy::move(Rte::Vec2<float> direction) {
    m_physicsModule->move(m_ecs->getComponent<Rte::Physics::Components::Physics>(m_enemy).shapeBody, direction);
}

void Enemy::fly(Rte::Vec2<float> direction) {
    m_physicsModule->applyForce(m_ecs->getComponent<Rte::Physics::Components::Physics>(m_enemy).shapeBody, direction);
}

void Enemy::shoot(Rte::Vec2<float> playerPos) {
    //Shoot projectile
    if (m_mana < 20)
        return;
    m_mana -= 20;
    Rte::Vec2<float> enemyPos = m_ecs->getComponent<Rte::BasicComponents::Transform>(m_enemy).position;
    float angle = getRotFromPoints(enemyPos, playerPos);
    const std::shared_ptr<Rte::Graphic::Texture> projectileTexture = m_graphicModule->createTexture();
    projectileTexture->loadFromFile("../assets/projectile.png");
    m_projectiles.push_back(m_ecs->createEntity());
    
    m_ecs->addComponent<Rte::Graphic::Components::Sprite>(m_projectiles.at(m_projectiles.size() - 1), Rte::Graphic::Components::Sprite(projectileTexture));
    m_ecs->addComponent<Rte::BasicComponents::Transform>(m_projectiles.at(m_projectiles.size() - 1), Rte::BasicComponents::Transform{
        .position = {static_cast<float>(cos(angle) * 300) + enemyPos.x, static_cast<float>(sin(angle) * 300) + enemyPos.y},
        .scale = {8, 8},
        .rotation = -angle * 180 / std::numbers::pi_v<float>
    });
    m_ecs->addComponent<Rte::Physics::Components::Physics>(m_projectiles.at(m_projectiles.size() - 1), Rte::Physics::Components::Physics{.shapeBody = m_physicsModule->createShapeBody(
        {32, 0},
        0.1,
        0.3,
        {m_ecs->getComponent<Rte::BasicComponents::Transform>(m_projectiles.at(m_projectiles.size() - 1)).position.x + m_graphicModule->getWindowSize().x / 2,
            m_ecs->getComponent<Rte::BasicComponents::Transform>(m_projectiles.at(m_projectiles.size() - 1)).position.y + m_graphicModule->getWindowSize().y / 2},
        m_ecs->getComponent<Rte::BasicComponents::Transform>(m_projectiles.at(m_projectiles.size() - 1)).rotation,
        false
    )});

    m_physicsModule->applyForce(m_ecs->getComponent<Rte::Physics::Components::Physics>(m_projectiles.at(m_projectiles.size() - 1)).shapeBody, {
        static_cast<float>(cos(angle) * 0.3),
        static_cast<float>(sin(-angle) * 0.3)
    });
}

void Enemy::update() {
    m_mana += .5F;
    if (m_mana > 100.F)
        m_mana = 100.F;
    m_health += 1.F;

    updateProjectiles();
}

float Enemy::getHealth() const {
    return m_health;
};

float Enemy::getMana() const {
    return m_mana;
};

float Enemy::getFlightTime() const {
    return m_flightTime;
};

Rte::Vec2<float> Enemy::getDestroyedProjectilePos() {
    if (m_destroyedProjectiles.empty())
        return {};
    Rte::Vec2<float> pos = m_destroyedProjectiles.at(0);
    m_destroyedProjectiles.erase(m_destroyedProjectiles.begin());
    return pos;
}

void Enemy::updateProjectiles() {
    for (int i = 0; i < m_projectiles.size(); i++) {
        if (m_physicsModule->colliding(m_ecs->getComponent<Rte::Physics::Components::Physics>(m_projectiles[i]).shapeBody)) {
            m_destroyedProjectiles.push_back(m_ecs->getComponent<Rte::BasicComponents::Transform>(m_projectiles[i]).position);
            m_ecs->destroyEntity(m_projectiles[i]);
            m_projectiles.erase(m_projectiles.begin() + i);
        }
    }
}
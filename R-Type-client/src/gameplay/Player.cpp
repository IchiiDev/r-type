#include "Player.hpp"
#include "Rte/Common.hpp"
#include "Rte/Graphic/Texture.hpp"
#include <cmath>
#include <cstdlib>
#include <memory>
#include "Rte/Graphic/GraphicModule.hpp"
#include "Rte/Graphic/Components.hpp"
#include "Rte/Graphic/Texture.hpp"
#include "Rte/Physics/PhysicsModule.hpp"
#include "Rte/Physics/Components.hpp"
#include "Rte/Physics/ShapeBody.hpp"

static float getRotFromPoints(const Rte::Vec2<float> p1, const Rte::Vec2<float> p2) {
    return atan2(p2.y - p1.y, p2.x - p1.x);
}

Player::Player(const std::shared_ptr<Rte::Ecs>& ecs, const std::shared_ptr<Rte::Graphic::GraphicModule>& graphicModule, const std::shared_ptr<Rte::Physics::PhysicsModule>& physicsModule) : m_player(ecs->createEntity()), m_ecs(ecs), m_graphicModule(graphicModule), m_physicsModule(physicsModule) {
    const std::shared_ptr<Rte::Graphic::Texture> playerTexture = m_graphicModule->createTexture();
    playerTexture->loadFromFile("../assets/player.png");

    constexpr Rte::Vec2<float> playerScale = {3, 3};
    const Rte::Vec2<float> playerPosition = {
        200,
        800
    };

    m_ecs->addComponent<Rte::Graphic::Components::Sprite>(m_player, Rte::Graphic::Components::Sprite(playerTexture));
    m_ecs->addComponent<Rte::BasicComponents::Transform>(m_player, Rte::BasicComponents::Transform{
        .position = playerPosition,
        .scale = playerScale,
        .rotation = 0
    });

    m_ecs->addComponent<Rte::Physics::Components::Physics>(m_player, Rte::Physics::Components::Physics{.shapeBody = m_physicsModule->createShapeBody(
        {31 * 3, 47 * 3},
        1,
        50,
        playerPosition,
        0,
        true,
        false,
        Rte::Physics::ShapeType::CAPSULE
    )});
}

void Player::move(Rte::Vec2<float> direction) {
    m_physicsModule->move(m_ecs->getComponent<Rte::Physics::Components::Physics>(m_player).shapeBody, direction);
}

void Player::fly(Rte::Vec2<float> direction) {
    m_physicsModule->applyForce(m_ecs->getComponent<Rte::Physics::Components::Physics>(m_player).shapeBody, direction);
}

void Player::shoot(Rte::Vec2<float> mousePos) {
    //Shoot projectile
    if (m_mana < 20)
        return;
    m_mana -= 20;
    Rte::Vec2<float> playerPos = m_ecs->getComponent<Rte::BasicComponents::Transform>(m_player).position;
    float angle = getRotFromPoints(playerPos, mousePos);
    const std::shared_ptr<Rte::Graphic::Texture> projectileTexture = m_graphicModule->createTexture();
    projectileTexture->loadFromFile("../assets/projectile.png");
    m_projectiles.push_back(m_ecs->createEntity());

    m_ecs->addComponent<Rte::Graphic::Components::Sprite>(m_projectiles.at(m_projectiles.size() - 1), Rte::Graphic::Components::Sprite(projectileTexture));
    m_ecs->addComponent<Rte::BasicComponents::Transform>(m_projectiles.at(m_projectiles.size() - 1), Rte::BasicComponents::Transform{
        .position = {static_cast<float>(cos(angle) * 100) + playerPos.x, static_cast<float>(sin(angle) * 100) + playerPos.y},
        .scale = {8, 8},
        .rotation = -angle * 180 / std::numbers::pi_v<float>
    });
    m_ecs->addComponent<Rte::Physics::Components::Physics>(m_projectiles.at(m_projectiles.size() - 1), Rte::Physics::Components::Physics{.shapeBody = m_physicsModule->createShapeBody(
        {32, 0},
        0.05,
        0.3,
        {m_ecs->getComponent<Rte::BasicComponents::Transform>(m_projectiles.at(m_projectiles.size() - 1)).position.x + m_graphicModule->getWindowSize().x / 2,
            m_ecs->getComponent<Rte::BasicComponents::Transform>(m_projectiles.at(m_projectiles.size() - 1)).position.y + m_graphicModule->getWindowSize().y / 2},
        m_ecs->getComponent<Rte::BasicComponents::Transform>(m_projectiles.at(m_projectiles.size() - 1)).rotation,
        false,
        false,
        Rte::Physics::ShapeType::CAPSULE
    )});
    float force = 0.4;
    m_physicsModule->applyForce(m_ecs->getComponent<Rte::Physics::Components::Physics>(m_projectiles.at(m_projectiles.size() - 1)).shapeBody, {
        static_cast<float>(cos(angle) * force),
        static_cast<float>(sin(-angle) * force)
    });
}

void Player::takeDamage() {
    m_health -= 10;
    if (m_health < 0)
        m_health = 0;
}


void Player::update() {
    m_mana += m_manaRegen;
    if (m_mana > m_maxMana)
        m_mana = m_maxMana;
    m_health += m_healthRegen;
    if (m_health > m_maxHealth)
        m_health = m_maxHealth;
    updateProjectiles();
}

float Player::getHealth() const {
    return m_health;
};

float Player::getMana() const {
    return m_mana;
};

float Player::getFlightTime() const {
    return m_flightTime;
};

Rte::Vec2<float> Player::getPos() const {
    return m_ecs->getComponent<Rte::BasicComponents::Transform>(m_player).position;
}

Rte::Vec2<float> Player::getDestroyedProjectilePos() {
    if (m_destroyedProjectiles.empty())
        return {};
    Rte::Vec2<float> pos = m_destroyedProjectiles.at(0);
    m_destroyedProjectiles.erase(m_destroyedProjectiles.begin());
    return pos;
}

void Player::updateProjectiles() {
    for (int i = 0; i < m_projectiles.size(); i++) {
        if (m_physicsModule->colliding(m_ecs->getComponent<Rte::Physics::Components::Physics>(m_projectiles[i]).shapeBody)) {
            m_destroyedProjectiles.push_back(m_ecs->getComponent<Rte::BasicComponents::Transform>(m_projectiles[i]).position);
            m_ecs->destroyEntity(m_projectiles[i]);
            m_projectiles.erase(m_projectiles.begin() + i);
        }
    }
}
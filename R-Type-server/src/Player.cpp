#include "Player.hpp"
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



Player::Player(const std::shared_ptr<Rte::Ecs>& ecs, const std::shared_ptr<Rte::Graphic::GraphicModule>& graphicModule, const std::shared_ptr<Rte::Physics::PhysicsModule>& physicsModule, uint32_t uid) : m_player(ecs->createEntity()), m_ecs(ecs), m_graphicModule(graphicModule), m_physicsModule(physicsModule) {
    std::shared_ptr<Rte::Graphic::Texture> playerTexture = m_graphicModule->createTexture();
    playerTexture->loadFromFile("../assets/player.png");

    int randR = rand() % 255;
    int randG = rand() % 255;
    int randB = rand() % 255;
    std::vector<Rte::u8> randColorText(static_cast<size_t>(playerTexture->getSize().x * playerTexture->getSize().y * 4));
    for (size_t i = 0; i < randColorText.size(); i += 4) {
        randColorText[i] = std::clamp(playerTexture->getPixels()[i] + randR, 0, 255);
        randColorText[i + 1] = std::clamp(playerTexture->getPixels()[i + 1] + randG, 0, 255);
        randColorText[i + 2] = std::clamp(playerTexture->getPixels()[i + 2] + randB, 0, 255);
        randColorText[i + 3] = playerTexture->getPixels()[i + 3];
    }
    playerTexture->loadFromMemory(randColorText.data(), playerTexture->getSize());
    constexpr Rte::Vec2<float> playerScale = {3, 3};
    const Rte::Vec2<float> playerPosition = {600, 400};

    m_ecs->addComponent<Rte::BasicComponents::UidComponents>(m_player, Rte::BasicComponents::UidComponents(uid));
    m_ecs->addComponent<Rte::Graphic::Components::Sprite>(m_player, Rte::Graphic::Components::Sprite(playerTexture));
    m_ecs->addComponent<Rte::BasicComponents::Transform>(m_player, Rte::BasicComponents::Transform{playerPosition, playerScale, 0});
    m_ecs->addComponent<Rte::Physics::Components::Physics>(m_player, Rte::Physics::Components::Physics{.shapeBody = m_physicsModule->createShapeBody(
        {31 * 3, 47 * 3}, 1, 50,
        playerPosition, 0, true, false,
        Rte::Physics::ShapeType::CAPSULE
    )});
}

Rte::Entity Player::getEntity() const {
    return m_player;
}

void Player::move(Rte::Vec2<float> direction) {
    m_physicsModule->move(m_ecs->getComponent<Rte::Physics::Components::Physics>(m_player).shapeBody, direction);
}

void Player::fly(Rte::Vec2<float> direction) {
    m_physicsModule->applyForce(m_ecs->getComponent<Rte::Physics::Components::Physics>(m_player).shapeBody, direction);
}

Rte::Entity Player::shoot(float angle) {
    //Shoot projectile
    if (m_mana < 20 || m_shootCooldown > 0)
        return 0;
    m_mana -= 20;
    Rte::Vec2<float> playerPos = m_ecs->getComponent<Rte::BasicComponents::Transform>(m_player).position;
    const std::shared_ptr<Rte::Graphic::Texture> projectileTexture = m_graphicModule->createTexture();
    projectileTexture->loadFromFile("../assets/projectile.png");
    Rte::Entity projectile = m_ecs->createEntity();

    m_ecs->addComponent<Rte::Graphic::Components::Sprite>(projectile, Rte::Graphic::Components::Sprite(projectileTexture));
    m_ecs->addComponent<Rte::BasicComponents::Transform>(projectile, Rte::BasicComponents::Transform{
        .position = {static_cast<float>(cos(angle) * 100) + playerPos.x, static_cast<float>(sin(angle) * 100) + playerPos.y},
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
    float force = 0.2;
    m_physicsModule->applyForce(m_ecs->getComponent<Rte::Physics::Components::Physics>(projectile).shapeBody, {
        static_cast<float>(cos(angle) * force),
        static_cast<float>(sin(-angle) * force)
    });
    m_shootCooldown = 0.5;
    return projectile;
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
    if (m_shootCooldown > 0)
        m_shootCooldown -= 0.01;
}

float Player::getHealth() const {
    return m_health;
}

float Player::getMana() const {
    return m_mana;
}

float Player::getFlightTime() const {
    return m_flightTime;
}

Rte::Vec2<float> Player::getPos() const {
    return m_ecs->getComponent<Rte::BasicComponents::Transform>(m_player).position;
}

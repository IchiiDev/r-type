#pragma once

#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/Graphic/GraphicModule.hpp"
#include "Rte/Physics/PhysicsModule.hpp"
#include <memory>
#include <vector>

class Player {
    public:
        Player(const std::shared_ptr<Rte::Ecs>& m_ecs, const std::shared_ptr<Rte::Graphic::GraphicModule>& m_graphicModule, const std::shared_ptr<Rte::Physics::PhysicsModule>& m_physicsModule, uint32_t uid);
        ~Player() = default;

        Player(const Player&) = delete;
        Player& operator=(const Player&) = delete;

        Player(Player&&) noexcept = default;
        Player& operator=(Player&&) noexcept = default;


        void setFriction(float friction);
        void move(Rte::Vec2<float> direction);
        void fly(Rte::Vec2<float> direction);
        Rte::Vec2<float> getVelocity() const;

        Rte::Entity shoot(float angle);
        void update();
        void takeDamage(float amount);
        void heal(float amount);

        [[nodiscard]] Rte::Entity getEntity() const;
        [[nodiscard]] float getHealth() const;
        [[nodiscard]] Rte::Vec2<float> getPos() const;
        [[nodiscard]] float getMana() const;
        [[nodiscard]] float getFlightTime() const;


    private:
        Rte::Entity m_player;
        std::shared_ptr<Rte::Ecs> m_ecs;
        std::shared_ptr<Rte::Graphic::GraphicModule> m_graphicModule;
        std::shared_ptr<Rte::Physics::PhysicsModule> m_physicsModule;

        bool is_jumping = false;

        float m_health = 100.F;
        float m_mana = 100.F;
        float m_flightTime = 0.F;

        float m_maxHealth = 100.F;
        float m_maxMana = 100.F;
        float m_maxFlightTime = 100.F;

        float m_healthRegen = 0.05F;
        float m_manaRegen = 0.5F;

        float m_shootCooldown = 0.0F;
};

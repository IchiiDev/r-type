#pragma once

#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/Graphic/GraphicModule.hpp"
#include "Rte/Physics/PhysicsModule.hpp"
#include <vector>

class Player {
    public:
        Player(const std::shared_ptr<Rte::Ecs>& m_ecs, const std::shared_ptr<Rte::Graphic::GraphicModule>& m_graphicModule, const std::shared_ptr<Rte::Physics::PhysicsModule>& m_physicsModule);
        ~Player() = default;
        
        Player(const Player&) = delete;
        Player& operator=(const Player&) = delete;
        Player(Player&&) noexcept = default;
        Player& operator=(Player&&) noexcept = default;

        void move(Rte::Vec2<float> direction);
        void fly(Rte::Vec2<float> direction);

        void shoot(Rte::Vec2<float> mousePos);
        void update();
        void takeDamage();

        [[nodiscard]] float getHealth() const;
        [[nodiscard]] Rte::Vec2<float> getPos() const;
        [[nodiscard]] float getMana() const;
        [[nodiscard]] float getFlightTime() const;
        [[nodiscard]] Rte::Vec2<float> getDestroyedProjectilePos();

    private:
        void updateProjectiles();
        
        Rte::Entity m_player;
        std::shared_ptr<Rte::Ecs> m_ecs;
        std::shared_ptr<Rte::Graphic::GraphicModule> m_graphicModule;
        std::shared_ptr<Rte::Physics::PhysicsModule> m_physicsModule;

        std::vector<Rte::Entity> m_projectiles;

        std::vector<Rte::Vec2<float>> m_destroyedProjectiles;

        float m_health = 100.F;
        float m_mana = 100.F;
        float m_flightTime = 0.F;

        float m_maxHealth = 100.F;
        float m_maxMana = 100.F;
        float m_maxFlightTime = 100.F;

        float m_healthRegen = 0.05F;
        float m_manaRegen = 0.5F;

};
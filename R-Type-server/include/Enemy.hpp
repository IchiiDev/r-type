#pragma once

#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/Graphic/GraphicModule.hpp"
#include "Rte/Physics/PhysicsModule.hpp"
#include <vector>

class Enemy {
    public:
        Enemy(const std::shared_ptr<Rte::Ecs>& m_ecs, const std::shared_ptr<Rte::Graphic::GraphicModule>& m_graphicModule, const std::shared_ptr<Rte::Physics::PhysicsModule>& m_physicsModule);
        ~Enemy() = default;
        
        Enemy(const Enemy&) = delete;
        Enemy& operator=(const Enemy&) = delete;
        Enemy(Enemy&&) noexcept = default;
        Enemy& operator=(Enemy&&) noexcept = default;

        void move(Rte::Vec2<float> direction);
        void fly(Rte::Vec2<float> direction);

        void shoot(Rte::Vec2<float> playerPos);
        void update();
        
        [[nodiscard]] Rte::Entity getEntity() const;
        [[nodiscard]] float getHealth() const;
        [[nodiscard]] float getMana() const;
        [[nodiscard]] float getFlightTime() const;
        [[nodiscard]] Rte::Vec2<float> getDestroyedProjectilePos();

    private:
        void updateProjectiles();
        
        Rte::Entity m_enemy;
        std::shared_ptr<Rte::Ecs> m_ecs;
        std::shared_ptr<Rte::Graphic::GraphicModule> m_graphicModule;
        std::shared_ptr<Rte::Physics::PhysicsModule> m_physicsModule;

        std::vector<Rte::Entity> m_projectiles;

        std::vector<Rte::Vec2<float>> m_destroyedProjectiles;

        float m_health = 100.F;
        float m_mana = 100.F;
        float m_flightTime = 0.F;
};
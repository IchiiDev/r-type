#pragma once

#include "Enemy.hpp"
#include "Player.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/Graphic/GraphicModule.hpp"
#include "Rte/ModuleManager.hpp"
#include "Rte/Network/NetworkModule.hpp"
#include "Rte/Network/NetworkModuleServer.hpp"
#include "Rte/Physics/PhysicsModule.hpp"
#include "Rte/Physics/Tool.hpp"

#include <array>
#include <cstdint>
#include <map>
#include <memory>
#include <vector>
#include <chrono>

class ServerApp {
    public:
        ServerApp();
        ~ServerApp() = default;

        ServerApp(const ServerApp&) = delete;
        ServerApp& operator=(const ServerApp&) = delete;
        ServerApp(ServerApp&&) noexcept = delete;
        ServerApp& operator=(ServerApp&&) noexcept = delete;

        void run();

    private:

        // Scene
        void initScene();
        void updateScene();
        
        // Players
        void updatePlayers();

        // Enemies
        void createEnemy(Rte::Vec2<float> pos);
        void updateEnemies();
        void destroyEnemy(const Rte::Entity& enemy);
        
        // Projectiles
        void createProjectile(Rte::Entity projectile);
        void updateProjectiles();
        void destroyProjectile(const Rte::Entity& projectile);

        // Powerups
        void createPowerup(Rte::Vec2<float> pos);
        void updatePowerups();
        void destroyPowerup(const Rte::Entity& powerup);
        
        // Breakables
        Rte::Entity createBreakable(Rte::Vec2<float> pos, std::string spritePath);
        Rte::Entity createBreakable(Rte::Vec2<float> pos, std::vector<Rte::u8> texture, std::vector<Rte::u8> material, Rte::Vec2<Rte::u16> size);
        Rte::Entity createBreakable(Rte::Entity breakable, std::vector<Rte::u8> texture, std::vector<Rte::u8> material, Rte::Vec2<Rte::u16> size);
        void updateBreakables();
        void fractureBreakable(const Rte::Vec2<Rte::u16>& position);
        void destroyBreakable(const Rte::Entity& breakable);

        // Terrain
        Rte::Entity generateTerrainBlock(int width, int maxHeight, Rte::Vec2<float> pos);
        void updateTerrain();
        
        Rte::ModuleManager moduleManager;
        std::shared_ptr<Rte::Ecs> m_ecs;

        bool m_running = true;

        float terrainPos = 0;

        std::shared_ptr<Rte::Physics::PhysicsModule> m_physicsModule;
        std::shared_ptr<Rte::Network::NetworkModule> m_networkModule;
        std::shared_ptr<Rte::Graphic::GraphicModule> m_graphicModule;
        std::shared_ptr<Rte::Network::NetworkModuleServer> m_networkModuleServer;

        std::map<uint32_t, std::unique_ptr<Player>> m_players;
        std::shared_ptr<std::vector<Rte::Entity>> m_entities;
        std::vector<std::unique_ptr<Rte::Entity>> m_projectiles;
        std::map<std::string, std::pair<Rte::Vec2<Rte::u16>, std::vector<int>>> m_destructionMaps;
        std::vector<std::unique_ptr<Rte::Entity>> m_powerups;
        std::vector<std::unique_ptr<Rte::Entity>> m_breakables;
        std::map<uint32_t, std::unique_ptr<Enemy>> m_enemies;
        Rte::Entity m_rightWall;
        Rte::Entity m_leftWall;
        Rte::Entity m_topWall;
        Rte::Entity m_bottomWall;
        std::map<Rte::Entity, Rte::Network::PackedTexture> m_newEntitiesTextures;

        std::map<std::string, std::vector<Rte::Physics::PixelCringe>> projectilesMaps;
        
        int m_terrainBlockWidth = 64;
        int m_terrainBlockHeight = 64;
        std::vector<Rte::Entity> m_terrains;

        std::chrono::time_point<std::chrono::high_resolution_clock> m_enemyClock;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_startTime;

        uint32_t m_currentUid = 0;
};

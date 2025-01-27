#pragma once

#include "Rte/Audio/AudioModule.hpp"
#include "Rte/Audio/Sound.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/Graphic/GraphicModule.hpp"
#include "Rte/ModuleManager.hpp"
#include <memory>

struct Rect {
    float x1;
    float y1;
    float x2;
    float y2;
};

class FlappyBird {
    public:
        FlappyBird();
        ~FlappyBird();

        FlappyBird(const FlappyBird &other) = delete;
        FlappyBird &operator=(const FlappyBird &other) = delete;
        FlappyBird(FlappyBird &&other) = delete;
        FlappyBird &operator=(FlappyBird &&other) = delete;

        void gravity();
        void titleScreen();
        void gameLoop();
        void endScreen();
        void pauseScreen(Rte::Audio::Sound &music);

        bool playerPipeCollide(const Rte::Entity &player, const Rte::Entity &pipe);
        bool playerScore(const Rte::Entity &player, std::tuple<Rect, Rte::Entity, bool> &pipe);

        void run();


    private:
        bool m_running = true;

        Rte::ModuleManager moduleManager;
        std::shared_ptr<Rte::Ecs> m_ecs;

        std::shared_ptr<Rte::Graphic::GraphicModule> m_graphicModule;
        std::shared_ptr<Rte::Audio::AudioModule> m_audioModule;
        Rte::Entity player;
        float g = 5.0;
        int score = 0;
        int highScore = 0;
        bool pauseMusic = false;
        bool pauseSound = false;
};

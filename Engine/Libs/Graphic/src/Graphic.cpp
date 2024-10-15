#include "Rte/Graphic/Graphic.hpp"

#include "raylib.h"
#include <cstdint>
#include <cstdlib>

#define MAX_BUILDINGS   100

// Random values generator
static int GetRandomValue(int min, int max) {
    return min + rand() % (max - min + 1);
}

int graphicFunction() {
    // Window init
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - 2d camera");
    SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second


    Camera2D camera = {
        .offset = Vector2{
            .x = screenWidth / 2.0F,
            .y = screenHeight / 2.0F
        },
        .target = Vector2{
            .x = 0,
            .y = 0
        },
        .rotation = 0.0F,
        .zoom = 1.0F
    };


    // Main game loop
    while (!WindowShouldClose()) {
        ClearBackground(RAYWHITE);

        BeginDrawing();

            BeginMode2D(camera);

                DrawRectangle(0, 0, 200, 200, RED);

            EndMode2D();

        EndDrawing();
    }


    CloseWindow();        // Close window and OpenGL context
    return 1;
}

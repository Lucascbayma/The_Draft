#include "raylib.h"
#include <stdio.h>
#include <math.h>

#define FRAME_COUNT 8   

int main() {
    InitWindow(0, 0, "Sonho de Papel - Rabisco em Movimento");
    ToggleFullscreen();

    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    Texture2D mapa = LoadTexture("mapa.png");

    Texture2D idle = LoadTexture("idle.png");
    
    Texture2D walkRight[FRAME_COUNT];
    for (int i = 0; i < FRAME_COUNT; i++) {
        char filename[32];
        sprintf(filename, "walk_right%d.png", i + 1);
        walkRight[i] = LoadTexture(filename);
    }

    int frame = 0;
    float frameTime = 0;
    float frameDelay = 1.0f / 8.0f; 

    Vector2 pos = { screenWidth / 2.0f, screenHeight / 2.0f };
    float escala = 0.2f;  
    float speed = 4.0f;  

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        Vector2 move = {0, 0};

        if (IsKeyDown(KEY_RIGHT)) move.x += 1;
        if (IsKeyDown(KEY_LEFT))  move.x -= 1;
        if (IsKeyDown(KEY_UP))    move.y -= 1;
        if (IsKeyDown(KEY_DOWN))  move.y += 1;

        float len = sqrtf(move.x * move.x + move.y * move.y);
        if (len > 0) {
            move.x /= len;
            move.y /= len;
        }

        pos.x += move.x * speed;
        pos.y += move.y * speed;

        bool movingRight = (move.x > 0 && fabs(move.x) >= fabs(move.y));

        frameTime += GetFrameTime();
        if (movingRight) {
            if (frameTime >= frameDelay) {
                frame = (frame + 1) % FRAME_COUNT;
                frameTime = 0;
            }
        } else {
            frame = 0;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawTexturePro(
            mapa,
            (Rectangle){0, 0, mapa.width, mapa.height},
            (Rectangle){0, 0, screenWidth, screenHeight},
            (Vector2){0, 0},
            0.0f,
            WHITE
        );

        if (movingRight)
            DrawTextureEx(walkRight[frame], pos, 0.0f, escala, WHITE);
        else
            DrawTextureEx(idle, pos, 0.0f, escala, WHITE);

        EndDrawing();
    }

    UnloadTexture(mapa);
    UnloadTexture(idle);
    for (int i = 0; i < FRAME_COUNT; i++) UnloadTexture(walkRight[i]);
    CloseWindow();
    return 0;
}

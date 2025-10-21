#include "raylib.h"
#include <stdio.h>
#include <math.h>

#define FRAME_RIGHT 8
#define FRAME_LEFT 8
#define FRAME_UP 9

int main() {
    InitWindow(0, 0, "Sonho de Papel - Rabisco em Movimento");
    ToggleFullscreen();

    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    Texture2D mapa = LoadTexture("mapa.png");
    Texture2D idle = LoadTexture("idle.png");

    Texture2D walkRight[FRAME_RIGHT];
    for (int i = 0; i < FRAME_RIGHT; i++) {
        char filename[32];
        sprintf(filename, "walk_right%d.png", i + 1);
        walkRight[i] = LoadTexture(filename);
    }

    Texture2D walkLeft[FRAME_LEFT];
    for (int i = 0; i < FRAME_LEFT; i++) {
        char filename[32];
        sprintf(filename, "walk_left%d.png", i + 1);
        walkLeft[i] = LoadTexture(filename);
    }

    Texture2D walkUp[FRAME_UP];
    for (int i = 0; i < FRAME_UP; i++) {
        char filename[32];
        sprintf(filename, "walk_up%d.png", i + 1);
        walkUp[i] = LoadTexture(filename);
    }

    int frame = 0;
    float frameTime = 0;
    float frameDelayRight = 1.0f / 8.0f;
    float frameDelayUp = 1.0f / 9.0f;

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

        if (pos.x < 0) pos.x = 0;
        if (pos.y < 0) pos.y = 0;
        if (pos.x > screenWidth - walkRight[0].width * escala) pos.x = screenWidth - walkRight[0].width * escala;
        if (pos.y > screenHeight - walkRight[0].height * escala) pos.y = screenHeight - walkRight[0].height * escala;

        bool movingRight = (move.x > 0 && fabs(move.x) >= fabs(move.y));
        bool movingLeft  = (move.x < 0 && fabs(move.x) >= fabs(move.y));
        bool movingUp    = (move.y < 0);
        bool isMoving    = (len > 0);


        frameTime += GetFrameTime();

        if (isMoving) {
            if (movingUp) {
                if (frameTime >= frameDelayUp) {
                    frame = (frame + 1) % FRAME_UP;
                    frameTime = 0;
                }
            } else if (movingRight || movingLeft) {
                if (frameTime >= frameDelayRight) {
                    frame = (frame + 1) % FRAME_RIGHT;
                    frameTime = 0;
                }
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

        if (movingUp)
            DrawTextureEx(walkUp[frame], pos, 0.0f, escala, WHITE);
        else if (movingRight)
            DrawTextureEx(walkRight[frame], pos, 0.0f, escala, WHITE);
        else if (movingLeft)
            DrawTextureEx(walkLeft[frame], pos, 0.0f, escala, WHITE);
        else
            DrawTextureEx(idle, pos, 0.0f, escala, WHITE);

        EndDrawing();
    }

    UnloadTexture(mapa);
    UnloadTexture(idle);
    for (int i = 0; i < FRAME_RIGHT; i++) UnloadTexture(walkRight[i]);
    for (int i = 0; i < FRAME_LEFT; i++)  UnloadTexture(walkLeft[i]);
    for (int i = 0; i < FRAME_UP; i++)    UnloadTexture(walkUp[i]);

    CloseWindow();
    return 0;
}

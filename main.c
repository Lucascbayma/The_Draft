#include "raylib.h"
#include <stdio.h>
#include <math.h>

#define FRAME_RIGHT 8
#define FRAME_LEFT 8
#define FRAME_UP 9

typedef enum { DIR_IDLE, DIR_RIGHT, DIR_LEFT, DIR_UP, DIR_DOWN } PlayerDirection;

int main() {
    InitWindow(0, 0, "Sonho de Papel - Rabisco em Movimento");
    ToggleFullscreen();

    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    Texture2D mapa = LoadTexture("images/mapa.png");
    Texture2D idle = LoadTexture("images/idle.png");

    Texture2D walkRight[FRAME_RIGHT];
    for (int i = 0; i < FRAME_RIGHT; i++) {
        char filename[32];
        sprintf(filename, "images/walk_right%d.png", i + 1);
        walkRight[i] = LoadTexture(filename);
    }

    Texture2D walkLeft[FRAME_LEFT];
    for (int i = 0; i < FRAME_LEFT; i++) {
        char filename[32];
        sprintf(filename, "images/walk_left%d.png", i + 1);
        walkLeft[i] = LoadTexture(filename);
    }

    Texture2D walkUp[FRAME_UP];
    for (int i = 0; i < FRAME_UP; i++) {
        char filename[32];
        sprintf(filename, "images/walk_up%d.png", i + 1);
        walkUp[i] = LoadTexture(filename);
    }

    int frame = 0;
    float frameTime = 0;
    float frameDelayRight = 1.0f / 8.0f;
    float frameDelayUp = 1.0f / 9.0f;
    PlayerDirection lastDir = DIR_IDLE;

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
        bool isMoving = (len > 0);

        if (isMoving) {
            move.x /= len;
            move.y /= len;
        }

        pos.x += move.x * speed;
        pos.y += move.y * speed;

        if (pos.x < 0) pos.x = 0;
        if (pos.y < 0) pos.y = 0;
        if (pos.x > screenWidth - walkRight[0].width * escala) pos.x = screenWidth - walkRight[0].width * escala;
        if (pos.y > screenHeight - walkRight[0].height * escala) pos.y = screenHeight - walkRight[0].height * escala;

        if (isMoving) {
            if (fabs(move.x) > fabs(move.y)) {
                if (move.x > 0) lastDir = DIR_RIGHT;
                else lastDir = DIR_LEFT;
            } else {
                if (move.y < 0) lastDir = DIR_UP;
                else lastDir = DIR_DOWN;
            }
        } else {
            lastDir = DIR_IDLE;
        }

        frameTime += GetFrameTime();

        bool movingVertical = (lastDir == DIR_UP || lastDir == DIR_DOWN);
        bool movingHorizontal = (lastDir == DIR_RIGHT || lastDir == DIR_LEFT);

        if (isMoving) {
            if (movingVertical) {
                if (frameTime >= frameDelayUp) {
                    frame = (frame + 1) % FRAME_UP;
                    frameTime = 0;
                }
            } else if (movingHorizontal) {
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

        Texture2D currentFrame = idle;

        if (lastDir == DIR_UP)
            currentFrame = walkUp[frame];
        else if (lastDir == DIR_RIGHT)
            currentFrame = walkRight[frame];
        else if (lastDir == DIR_LEFT)
            currentFrame = walkLeft[frame];
        else if (lastDir == DIR_DOWN)
            currentFrame = idle;
        else
            currentFrame = idle;

        DrawTextureEx(currentFrame, pos, 0.0f, escala, WHITE);

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
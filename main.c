#include "raylib.h"
#include "rabisco.h"
#include "stdio.h"

int main() {
    // --- Inicialização da Janela ---
    InitWindow(0, 0, "Sonho de Papel - Rabisco em Movimento");
    ToggleFullscreen();

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    // --- Carregamento do mapa ---
    Texture2D mapa = LoadTexture("images/mapa.png");

    // --- Inicializa o Rabisco ---
    Rabisco rabisco;
    InitRabisco(&rabisco, mapa.width / 2.0f, mapa.height / 2.0f);

    // --- Carrega fonte personalizada ---
    Font gameFont = LoadFont("assets/PatrickHandSC-Regular.ttf");

    // --- Configuração da Câmera ---
    Camera2D camera = { 0 };
    camera.target = rabisco.pos;
    camera.offset = (Vector2){ screenW / 2.0f, screenH / 2.0f };
    camera.rotation = 0.0f;

    float zoomX = (float)screenW / (float)mapa.width;
    float zoomY = (float)screenH / (float)mapa.height;
    camera.zoom = (zoomX < zoomY ? zoomX : zoomY);
    camera.zoom *= 1.7f;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        UpdateRabisco(&rabisco, mapa.width, mapa.height);

        Vector2 desiredTarget = rabisco.pos;
        camera.target.x += (desiredTarget.x - camera.target.x) * 0.15f;
        camera.target.y += (desiredTarget.y - camera.target.y) * 0.15f;

        float halfWidth = screenW / (2.0f * camera.zoom);
        float halfHeight = screenH / (2.0f * camera.zoom);

        if (camera.target.x < halfWidth) camera.target.x = halfWidth;
        if (camera.target.y < halfHeight) camera.target.y = halfHeight;
        if (camera.target.x > mapa.width - halfWidth) camera.target.x = mapa.width - halfWidth;
        if (camera.target.y > mapa.height - halfHeight) camera.target.y = mapa.height - halfHeight;

        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode2D(camera);
            DrawTexture(mapa, 0, 0, WHITE);
            DrawRabisco(&rabisco);
        EndMode2D();
        
        int padding = 20;

        // --- 1. HUD de Vida (Corações) ---
        int heartSize = 70;
        for (int i = 0; i < rabisco.maxVida; i++) {
            Texture2D heartTexture = (i < rabisco.vida) ? rabisco.heartFull : rabisco.heartBroken;
            float scaleFactor = (float)heartSize / heartTexture.width;
            DrawTextureEx(
                heartTexture,
                (Vector2){ padding + i * (heartSize + 8), padding },
                0.0f,
                scaleFactor,
                WHITE
            );
        }

        // --- 2. HUD de Moedas ---
        int coinSize = 60;
        int fontSize = 55;

        int coinPosY = padding + heartSize + padding / 2;
        float coinScale = (float)coinSize / rabisco.coinIcon.width;

        DrawTextureEx(
            rabisco.coinIcon,
            (Vector2){ padding, coinPosY },
            0.0f,
            coinScale,
            WHITE
        );

        // Desenha contagem com fonte personalizada
        DrawTextEx(
            gameFont,
            TextFormat("x %02d", rabisco.moedas),
            (Vector2){ padding + coinSize + 15, coinPosY + (coinSize - fontSize) / 3 },
            fontSize,
            2,
            WHITE
        );

        EndDrawing();
    }

    UnloadFont(gameFont);
    UnloadRabisco(&rabisco);
    UnloadTexture(mapa);
    CloseWindow();

    return 0;
}

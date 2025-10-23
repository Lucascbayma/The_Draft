#include "raylib.h"
#include "rabisco.h"

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

    // --- Configuração da Câmera ---
    Camera2D camera = { 0 };
    camera.target = rabisco.pos;
    camera.offset = (Vector2){ screenW / 2.5f, screenH / 2.5f };
    camera.rotation = 0.0f;

    // Zoom definido para preencher o máximo possível da tela sem mostrar o fundo além do mapa
    float zoomX = (float)screenW / (float)mapa.width;
    float zoomY = (float)screenH / (float)mapa.height;
    camera.zoom = (zoomX < zoomY ? zoomX : zoomY); // Mantém proporção, sem distorcer o mapa
    camera.zoom *= 2.25f; // Pequeno aumento para dar sensação de aproximação

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // --- Atualiza o Rabisco ---
        UpdateRabisco(&rabisco, mapa.width, mapa.height);

        // --- Atualiza a Câmera suavemente ---
        Vector2 desiredTarget = rabisco.pos;
        camera.target.x += (desiredTarget.x - camera.target.x) * 0.15f;
        camera.target.y += (desiredTarget.y - camera.target.y) * 0.15f;

        // --- Impede a câmera de mostrar fora do mapa ---
        float halfWidth = screenW / (2.0f * camera.zoom);
        float halfHeight = screenH / (2.0f * camera.zoom);

        if (camera.target.x < halfWidth) camera.target.x = halfWidth;
        if (camera.target.y < halfHeight) camera.target.y = halfHeight;
        if (camera.target.x > mapa.width - halfWidth) camera.target.x = mapa.width - halfWidth;
        if (camera.target.y > mapa.height - halfHeight) camera.target.y = mapa.height - halfHeight;

        // --- Renderização ---
        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode2D(camera);
            // O mapa agora ocupa toda a tela, sem mostrar o que há além dele
            DrawTexture(mapa, 0, 0, WHITE);
            DrawRabisco(&rabisco);
        EndMode2D();

        EndDrawing();
    }

    // --- Libera recursos ---
    UnloadRabisco(&rabisco);
    UnloadTexture(mapa);
    CloseWindow();

    return 0;
}

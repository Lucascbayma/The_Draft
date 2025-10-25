#include "raylib.h"
#include "rabisco.h"
#include "stdio.h" // para usar TextFormat

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
    camera.offset = (Vector2){ screenW / 2.0f, screenH / 2.0f };
    camera.rotation = 0.0f;

    // Zoom definido para preencher o máximo possível da tela sem mostrar o fundo além do mapa
    float zoomX = (float)screenW / (float)mapa.width;
    float zoomY = (float)screenH / (float)mapa.height;
    camera.zoom = (zoomX < zoomY ? zoomX : zoomY); // Mantém proporção, sem distorcer o mapa
    camera.zoom *= 1.7f; // Pequeno aumento para dar sensação de aproximação

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // --- Atualiza o Rabisco ---
        // Usamos mapa.width e mapa.height pois o rabisco está limitado ao tamanho do mapa, não da tela
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

        // Renderiza o Mundo (dentro da câmera)
        BeginMode2D(camera);
            DrawTexture(mapa, 0, 0, WHITE);
            DrawRabisco(&rabisco);
        EndMode2D();
        
        int padding = 20;
        
        // --- 1. HUD de Vida (Corações) ---
        int heartSize = 50; // Tamanho do coração na tela
        
        for (int i = 0; i < rabisco.maxVida; i++) {
            Texture2D heartTexture = (i < rabisco.vida) ? rabisco.heartFull : rabisco.heartBroken;
            
            // Desenha o coração, ajustando o tamanho pela escala
            float scaleFactor = (float)heartSize / heartTexture.width;
            
            DrawTextureEx(
                heartTexture,
                (Vector2){ padding + i * (heartSize + 5), padding }, // Posição (Canto superior esquerdo)
                0.0f,
                scaleFactor,
                WHITE
            );
        }
        
        // --- 2. HUD de Moedas ---
        int coinSize = 40; // Tamanho da moeda
        int fontSize = 35; // Tamanho da fonte para a contagem

        // Posição Y deve estar alinhada com os corações ou um pouco abaixo
        int coinPosY = padding + heartSize + padding/2;

        // Desenha o Ícone da Moeda (lado esquerdo)
        float coinScale = (float)coinSize / rabisco.coinIcon.width;
        DrawTextureEx(
            rabisco.coinIcon,
            (Vector2){ padding, coinPosY },
            0.0f,
            coinScale,
            WHITE
        );

        // Desenha a Contagem de Moedas
        DrawText(
            TextFormat("x %02d", rabisco.moedas),
            padding + coinSize + 10,
            coinPosY + (coinSize - fontSize) / 2, // para alinhar verticalmente
            fontSize,
            WHITE
        );
        
        EndDrawing();
    }

    // --- Libera recursos ---
    UnloadRabisco(&rabisco);
    UnloadTexture(mapa);
    CloseWindow();

    return 0;
}
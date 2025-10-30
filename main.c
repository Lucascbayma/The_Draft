#include "raylib.h"
#include "rabisco.h"
#include "raymath.h"
#include <math.h>
#include <stdio.h>

// Definindo o Inimigo de Teste AQUI
typedef struct {
    Vector2 pos;
    float escala;
    int vida;
    int maxVida;
    int dano;
    float velocidade;
    
    Rectangle bounds;
    bool active;
    Color tint;
    Texture2D textura;
} InimigoTeste;

#define MAX_INIMIGOS_TESTE 3

int main() {
    InitWindow(0, 0, "The Draft");
    ToggleFullscreen();

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    const int mapBorderTop = 45;
    const int mapBorderBottom = 120;
    const int mapBorderLeft = 100;
    const int mapBorderRight = 140;

    Texture2D mapa = LoadTexture("images/mapa.png");

    Rabisco rabisco;
    InitRabisco(&rabisco, mapa.width / 2.0f, mapa.height / 2.0f);

    // Carrega a textura do Inimigo de Teste
    Texture2D texInimigo = LoadTexture("images/idle.png");

    InimigoTeste inimigos[MAX_INIMIGOS_TESTE];
    
    for(int i = 0; i < MAX_INIMIGOS_TESTE; i++) {
        inimigos[i].pos = (Vector2){ (float)GetRandomValue(200, mapa.width - 200), (float)GetRandomValue(200, mapa.height - 200) };
        inimigos[i].escala = 0.10f;
        inimigos[i].maxVida = 3;
        inimigos[i].vida = inimigos[i].maxVida;
        inimigos[i].dano = 1;
        inimigos[i].velocidade = 2.0f;
        inimigos[i].active = true;
        inimigos[i].tint = WHITE;
        inimigos[i].textura = texInimigo;
        
        float texW = (float)inimigos[i].textura.width;
        float texH = (float)inimigos[i].textura.height;
        inimigos[i].bounds = (Rectangle){ inimigos[i].pos.x, inimigos[i].pos.y, texW * inimigos[i].escala, texH * inimigos[i].escala };
    }


    Camera2D camera = { 0 };
    camera.target = rabisco.pos;
    camera.offset = (Vector2){ screenW / 2.0f, screenH / 2.0f };
    camera.rotation = 0.0f;

    float zoomX = (float)screenW / (float)mapa.width;
    float zoomY = (float)screenH / (float)mapa.height;
    camera.zoom = (zoomX < zoomY ? zoomX : zoomY);
    camera.zoom *= 2.0f;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        
        bool rabiscoAtacou = UpdateRabisco(&rabisco, mapa.width, mapa.height, mapBorderTop, mapBorderBottom, mapBorderLeft, mapBorderRight); 

        // --- LÓGICA DO INIMIGO DE TESTE ---
        for (int i = 0; i < MAX_INIMIGOS_TESTE; i++) {
            InimigoTeste *e = &inimigos[i];
            if (!e->active) continue;

            if (e->vida <= 0) {
                e->active = false;
                rabisco.moedas++;
                continue;
            }

            float dist = Vector2Distance(e->pos, rabisco.pos);
            float chaseRadius = 400.0f;
            float attackDist = e->bounds.width;

            if (dist < chaseRadius && dist > attackDist) {
                Vector2 move = Vector2Subtract(rabisco.pos, e->pos);
                move = Vector2Normalize(move);

                float dx = move.x * e->velocidade;
                float dy = move.y * e->velocidade;
                
                e->pos.x += dx;
                e->pos.y += dy;

                e->bounds.x = e->pos.x;
                e->bounds.y = e->pos.y;
            }
        }

        // --- LÓGICA DE ATAQUE DO RABISCO ---
        if (rabiscoAtacou) {
            Rectangle attackBox = GetRabiscoAttackHitbox(&rabisco);
            
            for (int i = 0; i < MAX_INIMIGOS_TESTE; i++) {
                InimigoTeste *e = &inimigos[i];
                if (e->active) {
                    if (CheckCollisionRecs(attackBox, e->bounds)) {
                        e->vida -= rabisco.dano;
                        e->tint = RED;
                    }
                }
            }
        }
        
        for (int i = 0; i < MAX_INIMIGOS_TESTE; i++) {
            if (inimigos[i].tint.r == RED.r) {
                inimigos[i].tint = WHITE;
            }
        }

        // --- CÂMERA ---
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
            
            // --- DESENHA INIMIGOS DE TESTE ---
            for (int i = 0; i < MAX_INIMIGOS_TESTE; i++) {
                InimigoTeste *e = &inimigos[i];
                if (e->active) {
                    DrawTextureEx(e->textura, e->pos, 0.0f, e->escala, e->tint);
                    
                    float healthPercent = (float)e->vida / (float)e->maxVida;
                    if (healthPercent < 1.0f) {
                        DrawRectangle(e->pos.x, e->pos.y - 10, e->bounds.width, 5, RED);
                        DrawRectangle(e->pos.x, e->pos.y - 10, e->bounds.width * healthPercent, 5, GREEN);
                    }
                }
            }
            
            DrawRabisco(&rabisco);
        EndMode2D();
        
        // --- HUD ---
        int padding = 20;
        int heartSize = 70;
        
        for (int i = 0; i < rabisco.maxVida; i++) {
            Texture2D heartTexture = (i < rabisco.vida) ? rabisco.heartFull : rabisco.heartBroken;
            if (heartTexture.width > 0) { 
                float scaleFactor = (float)heartSize / heartTexture.width;
                DrawTextureEx(heartTexture, (Vector2){ padding + i * (heartSize + 5), padding }, 0.0f, scaleFactor, WHITE);
            }
        }

        int coinSize = 60;
        int fontSize = 40;
        float spacing = 5;
        int coinPosY = padding + heartSize + padding/2;
        int textPosX = padding + coinSize + 10;
        int textPosY = coinPosY + (coinSize - fontSize) / 2;
        
        const char* coinText = TextFormat("%02d", rabisco.moedas);

        if (rabisco.coinIcon.width > 0) {
            float coinScale = (float)coinSize / rabisco.coinIcon.width;
            DrawTextureEx(rabisco.coinIcon, (Vector2){ padding, coinPosY }, 0.0f, coinScale, WHITE);
        }

        if (rabisco.hudFont.texture.id > 0) {
            DrawTextEx(rabisco.hudFont, coinText, (Vector2){textPosX - 2, textPosY}, fontSize, spacing, BLACK);
            DrawTextEx(rabisco.hudFont, coinText, (Vector2){textPosX + 2, textPosY}, fontSize, spacing, BLACK);
            DrawTextEx(rabisco.hudFont, coinText, (Vector2){textPosX, textPosY - 2}, fontSize, spacing, BLACK);
            DrawTextEx(rabisco.hudFont, coinText, (Vector2){textPosX, textPosY + 2}, fontSize, spacing, BLACK);
            DrawTextEx(rabisco.hudFont, coinText, (Vector2){textPosX, textPosY}, fontSize, spacing, WHITE);
        }
        
        EndDrawing();
    }

    UnloadTexture(texInimigo);
    UnloadRabisco(&rabisco);
    UnloadTexture(mapa);
    CloseWindow();
    return 0;
}
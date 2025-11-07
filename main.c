#include "raylib.h"
#include "rabisco.h"
#include "inimigos.h"
#include "stdio.h"
#include "raymath.h"

#define MAX_INIMIGOS 3

typedef enum {
    TELA_TITULO,
    TELA_TRANSICAO,
    TELA_JOGO
} EstadoJogo;

int main() {
    InitWindow(0, 0, "The Draft");
    ToggleFullscreen();
    InitAudioDevice(); 

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();
    SetTargetFPS(60);

    Texture2D tituloFrames[4] = {
        LoadTexture("images/titulo1.png"),
        LoadTexture("images/titulo2.png"),
        LoadTexture("images/titulo3.png"),
        LoadTexture("images/titulo4.png")
    };
    Texture2D fundoPreto = LoadTexture("images/fundo_preto.png");
    
    const int tamanhoFonteTitulo = 30; 
    Font fontTitulo = LoadFontEx("assets/PatrickHandSC-Regular.ttf", tamanhoFonteTitulo, NULL, 0); 

    Music music = LoadMusicStream("audio/music/the_draft_music.mp3");
    music.looping = true;
    float musicVolume = 0.5f; 
    SetMusicVolume(music, musicVolume);
    PlayMusicStream(music);

    int frameAtual = 0;
    float tempoFrame = 0.0f;
    const float duracaoFrame = 0.25f;

    EstadoJogo estado = TELA_TITULO;
    float alphaTransicao = 0.0f; 

    const int mapBorderTop = 45;
    const int mapBorderBottom = 120;
    const int mapBorderLeft = 100;
    const int mapBorderRight = 140;

    Texture2D mapa = LoadTexture("images/mapa.png");
    Rabisco rabisco;
    InitRabisco(&rabisco, mapa.width / 2.0f, mapa.height / 2.0f);

    InitInimigoAssets(); 
    Inimigo inimigos[MAX_INIMIGOS]; 

    for (int i = 0; i < MAX_INIMIGOS; i++) {
        Vector2 pos = (Vector2){ (float)GetRandomValue(200, mapa.width - 200), (float)GetRandomValue(200, mapa.height - 200) };
        
        if (i == 0) {
            SpawnInimigo(&inimigos[i], TIPO_TANQUE, pos);
        } else if (i == 1) {
            SpawnInimigo(&inimigos[i], TIPO_ARANHA, pos);
        } else {
            SpawnInimigo(&inimigos[i], TIPO_PADRAO, pos);
        }
    }

    Camera2D camera = { 0 };
    camera.target = rabisco.pos;
    camera.offset = (Vector2){ screenW / 2.0f, screenH / 2.0f };
    camera.rotation = 0.0f;

    float zoomX = (float)screenW / (float)mapa.width;
    float zoomY = (float)screenH / (float)mapa.height;
    camera.zoom = (zoomX < zoomY ? zoomX : zoomY) * 2.0f;

    while (!WindowShouldClose()) {
        UpdateMusicStream(music);

        BeginDrawing();
        ClearBackground(BLACK);

        if (estado == TELA_TITULO) {
            tempoFrame += GetFrameTime();
            if (tempoFrame >= duracaoFrame) {
                tempoFrame = 0.0f;
                frameAtual = (frameAtual + 1) % 4;
            }

            Texture2D frame = tituloFrames[frameAtual];
            float scaleX = (float)screenW / frame.width;
            float scaleY = (float)screenH / frame.height;
            float scale = (scaleX > scaleY) ? scaleX : scaleY;
            float posX = (screenW - frame.width * scale) / 2;
            float posY = (screenH - frame.height * scale) / 2;
            DrawTextureEx(frame, (Vector2){ posX, posY }, 0.0f, scale, WHITE);

            const char *texto = "Press any button to start";
            Vector2 textSize = MeasureTextEx(fontTitulo, texto, tamanhoFonteTitulo, 5); 
            DrawTextEx(fontTitulo, texto,
                       (Vector2){ (screenW - textSize.x) / 3.1f, screenH - 100 }, 
                       tamanhoFonteTitulo, 9, (Color){150,150,150,255});

            if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT) ||
                GetKeyPressed() != 0) {
                estado = TELA_TRANSICAO;
                alphaTransicao = 0.0f;
            }
        }

        else if (estado == TELA_TRANSICAO) {
            Texture2D frame = tituloFrames[frameAtual];
            float scaleX = (float)screenW / frame.width;
            float scaleY = (float)screenH / frame.height;
            float scale = (scaleX > scaleY) ? scaleX : scaleY;
            float posX = (screenW - frame.width * scale) / 2;
            float posY = (screenH - frame.height * scale) / 2;
            DrawTextureEx(frame, (Vector2){ posX, posY }, 0.0f, scale, WHITE);

            alphaTransicao += GetFrameTime();
            if (alphaTransicao > 1.0f) alphaTransicao = 1.0f;

            DrawRectangle(0, 0, screenW, screenH, Fade(BLACK, alphaTransicao));

            if (alphaTransicao >= 1.0f) {
                estado = TELA_JOGO;
            }
        }

        else if (estado == TELA_JOGO) {
            if (IsKeyPressed(KEY_MINUS)) {
                musicVolume -= 0.1f;
                if (musicVolume < 0.0f) musicVolume = 0.0f;
                SetMusicVolume(music, musicVolume);
            }
            if (IsKeyPressed(KEY_EQUAL)) { 
                musicVolume += 0.1f;
                if (musicVolume > 1.0f) musicVolume = 1.0f;
                SetMusicVolume(music, musicVolume);
            }

            bool rabiscoAtacou = UpdateRabisco(&rabisco, mapa.width, mapa.height,
                                               mapBorderTop, mapBorderBottom, mapBorderLeft, mapBorderRight);

            for (int i = 0; i < MAX_INIMIGOS; i++) {
                UpdateInimigo(&inimigos[i], &rabisco, mapa.width, mapa.height,
                              mapBorderTop, mapBorderBottom, 
                              mapBorderLeft, mapBorderRight);
            }

            if (rabiscoAtacou) {
                Rectangle attackBox = GetRabiscoAttackHitbox(&rabisco);
                for (int i = 0; i < MAX_INIMIGOS; i++) {
                    Inimigo *e = &inimigos[i];
                    if (e->active && CheckCollisionRecs(attackBox, GetInimigoHitbox(e))) {
                        e->vida -= rabisco.dano;
                        e->tint = RED;
                    }
                }
            }

            for (int i = 0; i < MAX_INIMIGOS; i++) {
                if (inimigos[i].tint.r == RED.r) inimigos[i].tint = WHITE;
            }

            Vector2 desiredTarget = rabisco.pos;
            camera.target.x += (desiredTarget.x - camera.target.x) * 0.15f;
            camera.target.y += (desiredTarget.y - camera.target.y) * 0.15f;

            float halfWidth = screenW / (2.0f * camera.zoom);
            float halfHeight = screenH / (2.0f * camera.zoom);

            if (camera.target.x < halfWidth) camera.target.x = halfWidth;
            if (camera.target.y < halfHeight) camera.target.y = halfHeight;
            if (camera.target.x > mapa.width - halfWidth) camera.target.x = mapa.width - halfWidth;
            if (camera.target.y > mapa.height - halfHeight) camera.target.y = mapa.height - halfHeight;

            BeginMode2D(camera);
                DrawTexture(mapa, 0, 0, WHITE);
                
                for (int i = 0; i < MAX_INIMIGOS; i++) {
                    DrawInimigo(&inimigos[i]);
                }
                DrawRabisco(&rabisco);
            EndMode2D();

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
            int fontSize = 55;
            float spacing = 0;
            int coinPosY = padding + heartSize + padding / 2;
            int textPosX = padding + coinSize + 10;
            int textPosY = coinPosY + (coinSize - fontSize) / 2;
            const char *coinText = TextFormat("%02d", rabisco.moedas);
            float coinScale = (float)coinSize / rabisco.coinIcon.width;
            
            DrawTextureEx(rabisco.coinIcon, (Vector2){ padding, coinPosY }, 0.0f, coinScale, WHITE);
            
            DrawTextEx(rabisco.hudFont, coinText, (Vector2){textPosX - 2, textPosY}, fontSize, spacing, BLACK);
            DrawTextEx(rabisco.hudFont, coinText, (Vector2){textPosX + 2, textPosY}, fontSize, spacing, BLACK);
            DrawTextEx(rabisco.hudFont, coinText, (Vector2){textPosX, textPosY - 2}, fontSize, spacing, BLACK);
            DrawTextEx(rabisco.hudFont, coinText, (Vector2){textPosX, textPosY + 2}, fontSize, spacing, BLACK);
            DrawTextEx(rabisco.hudFont, coinText, (Vector2){textPosX, textPosY}, fontSize, spacing, WHITE);

            int statusFontSize = 60; 
            int iconStatusSize = 70; 
            float iconOffset = 5; 
            int lineHeight = statusFontSize + 10; 
            
            int statusGap = 80; 
            
            int statusY = coinPosY + coinSize + statusGap; 
            
            const char* damageValue = TextFormat("%d", rabisco.dano);
            
            Vector2 damageIconPos = (Vector2){ padding, statusY }; 
            Vector2 damageValuePos = (Vector2){ padding + iconStatusSize + 10, statusY };

            DrawTextureEx(rabisco.iconDamage, 
                          (Vector2){ damageIconPos.x, damageIconPos.y + iconOffset }, 
                          0.0f, (float)iconStatusSize / rabisco.iconDamage.width, WHITE); 

            DrawTextEx(rabisco.hudFont, damageValue, (Vector2){damageValuePos.x - 2, damageValuePos.y}, statusFontSize, spacing, BLACK);
            DrawTextEx(rabisco.hudFont, damageValue, (Vector2){damageValuePos.x + 2, damageValuePos.y}, statusFontSize, spacing, BLACK);
            DrawTextEx(rabisco.hudFont, damageValue, (Vector2){damageValuePos.x, damageValuePos.y - 2}, statusFontSize, spacing, BLACK);
            DrawTextEx(rabisco.hudFont, damageValue, (Vector2){damageValuePos.x, damageValuePos.y + 2}, statusFontSize, spacing, BLACK);
            DrawTextEx(rabisco.hudFont, damageValue, 
                       damageValuePos, 
                       statusFontSize, spacing, WHITE);
            
            const char* speedValue = TextFormat("%.1f", rabisco.velocidade);

            int speedY = statusY + lineHeight;
            
            Vector2 speedIconPos = (Vector2){ padding, speedY };
            Vector2 speedValuePos = (Vector2){ padding + iconStatusSize + 10, speedY };
            
            DrawTextureEx(rabisco.iconVel, 
                          (Vector2){ speedIconPos.x, speedIconPos.y + iconOffset }, 
                          0.0f, (float)iconStatusSize / rabisco.iconVel.width, WHITE);
                          
            DrawTextEx(rabisco.hudFont, speedValue, (Vector2){speedValuePos.x - 2, speedValuePos.y}, statusFontSize, spacing, BLACK);
            DrawTextEx(rabisco.hudFont, speedValue, (Vector2){speedValuePos.x + 2, speedValuePos.y}, statusFontSize, spacing, BLACK);
            DrawTextEx(rabisco.hudFont, speedValue, (Vector2){speedValuePos.x, speedValuePos.y - 2}, statusFontSize, spacing, BLACK);
            DrawTextEx(rabisco.hudFont, speedValue, (Vector2){speedValuePos.x, speedValuePos.y + 2}, statusFontSize, spacing, BLACK);
            DrawTextEx(rabisco.hudFont, speedValue, 
                       speedValuePos, 
                       statusFontSize, spacing, WHITE);

            const char* rangeValue = TextFormat("%.1f", rabisco.alcance);
            
            int rangeY = speedY + lineHeight;
            
            Vector2 rangeIconPos = (Vector2){ padding, rangeY };
            Vector2 rangeValuePos = (Vector2){ padding + iconStatusSize + 10, rangeY };
            
            DrawTextureEx(rabisco.iconRange,
                          (Vector2){ rangeIconPos.x, rangeIconPos.y + iconOffset }, 
                          0.0f, (float)iconStatusSize / rabisco.iconRange.width, WHITE);
                          
            DrawTextEx(rabisco.hudFont, rangeValue, (Vector2){rangeValuePos.x - 2, rangeValuePos.y}, statusFontSize, spacing, BLACK);
            DrawTextEx(rabisco.hudFont, rangeValue, (Vector2){rangeValuePos.x + 2, rangeValuePos.y}, statusFontSize, spacing, BLACK);
            DrawTextEx(rabisco.hudFont, rangeValue, (Vector2){rangeValuePos.x, rangeValuePos.y - 2}, statusFontSize, spacing, BLACK);
            DrawTextEx(rabisco.hudFont, rangeValue, (Vector2){rangeValuePos.x, rangeValuePos.y + 2}, statusFontSize, spacing, BLACK);
            DrawTextEx(rabisco.hudFont, rangeValue, 
                       rangeValuePos, 
                       statusFontSize, spacing, WHITE);
        }

        EndDrawing();
    }

    UnloadMusicStream(music); 
    for (int i = 0; i < 4; i++) UnloadTexture(tituloFrames[i]);
    UnloadTexture(fundoPreto);
    
    UnloadInimigoAssets(); 
    
    UnloadTexture(mapa);
    UnloadFont(fontTitulo);
    UnloadRabisco(&rabisco); 
    CloseAudioDevice(); 
    CloseWindow();
    return 0;
}
#include "raylib.h"
#include "rabisco.h"
#include "inimigos.h"
#include "stdio.h"
#include "raymath.h"

#define MAX_INIMIGOS 4
#define MAX_PROJETEIS 150

typedef enum {
    TELA_TITULO,
    TELA_TRANSICAO,
    TELA_JOGO,
    TELA_GAME_OVER 
} EstadoJogo;

typedef struct {
    Vector2 pos;
    Vector2 velocity; 
    float speed;
    int dano;
    bool active;
    Rectangle bounds;
    float escala; 
} Projetil;

typedef struct {
    Rectangle bounds;
    bool isPressed;
    float escala;
} Button;

Projetil projeteis[MAX_PROJETEIS];
Texture2D texProjetilBorracha; 

void SpawnProjetilAtirador(Vector2 startPos, Vector2 direction) {
    for (int i = 0; i < MAX_PROJETEIS; i++) {
        if (!projeteis[i].active) {
            
            projeteis[i].escala = 0.05f;
            
            projeteis[i].pos = Vector2Add(startPos, Vector2Scale(direction, 10.0f)); 
            
            projeteis[i].velocity = direction;
            projeteis[i].speed = 5.0f;
            projeteis[i].dano = 1;
            projeteis[i].active = true;
            
            float hitboxSize = 7.0f; 
            
            float visualW = texProjetilBorracha.width * projeteis[i].escala;
            float visualH = texProjetilBorracha.height * projeteis[i].escala;

            projeteis[i].bounds.width = hitboxSize;
            projeteis[i].bounds.height = hitboxSize;
            
            projeteis[i].bounds.x = projeteis[i].pos.x + (visualW / 2.0f) - (hitboxSize / 2.0f);
            projeteis[i].bounds.y = projeteis[i].pos.y + (visualH / 2.0f) - (hitboxSize / 2.0f);
            
            break;
        }
    }
}

void UpdateProjeteis(Rabisco *r, int mapW, int mapH, int borderTop, int borderBottom, int borderLeft, int borderRight) {
    for (int i = 0; i < MAX_PROJETEIS; i++) {
        if (projeteis[i].active) {
            
            projeteis[i].pos = Vector2Add(projeteis[i].pos, Vector2Scale(projeteis[i].velocity, projeteis[i].speed));
            
            float visualW = texProjetilBorracha.width * projeteis[i].escala;
            float visualH = texProjetilBorracha.height * projeteis[i].escala;
            
            projeteis[i].bounds.x = projeteis[i].pos.x + (visualW / 2.0f) - (projeteis[i].bounds.width / 2.0f);
            projeteis[i].bounds.y = projeteis[i].pos.y + (visualH / 2.0f) - (projeteis[i].bounds.height / 2.0f);
            
            if (CheckCollisionRecs(projeteis[i].bounds, GetRabiscoHitbox(r))) { 
                r->currentHitPoints -= projeteis[i].dano;
                projeteis[i].active = false;
            }
            
            if (projeteis[i].pos.x < borderLeft || projeteis[i].pos.x > mapW - borderRight ||
                projeteis[i].pos.y < borderTop || projeteis[i].pos.y > mapH - borderBottom) {
                projeteis[i].active = false;
            }
        }
    }
}

void DrawProjeteis() {
    for (int i = 0; i < MAX_PROJETEIS; i++) {
        if (projeteis[i].active) {
            DrawTextureEx(texProjetilBorracha, projeteis[i].pos, 0.0f, projeteis[i].escala, WHITE);
        }
    }
}

void ResetJogo(Rabisco *rabisco, Inimigo inimigos[], int maxInimigos, Texture2D mapa) {

    rabisco->pos = (Vector2){ mapa.width / 2.0f, mapa.height / 2.0f + 100.0f };
    rabisco->currentHitPoints = rabisco->maxHeartContainers * 2; 
    rabisco->moedas = 0;
    rabisco->facingDir = DIR_DOWN;
    rabisco->attackTimer = 0.0f;
    rabisco->attackDurationTimer = 0.0f;
    
    for (int i = 0; i < maxInimigos; i++) {
        inimigos[i].active = false;
    }
    
    for (int i = 0; i < MAX_PROJETEIS; i++) {
        projeteis[i].active = false;
    }
}


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

    Music music = LoadMusicStream("audio/music/the_draft_music.ogg");
    music.looping = true;
    float musicVolume = 0.5f; 
    SetMusicVolume(music, musicVolume);
    PlayMusicStream(music);

    int frameAtual = 0;
    float tempoFrame = 0.0f;
    const float duracaoFrame = 0.25f;

    EstadoJogo estado = TELA_TITULO;
    float alphaTransicao = 0.0f; 
    
    int opcaoGameOver = 0;

    const int mapBorderTop = 65;
    const int mapBorderBottom = 120;
    const int mapBorderLeft = 110;
    const int mapBorderRight = 110;

    Texture2D mapa = LoadTexture("images/mapa.png");
    Rabisco rabisco;
    InitRabisco(&rabisco, mapa.width / 2.0f, mapa.height / 2.0f);

    InitInimigoAssets(); 
    Inimigo inimigos[MAX_INIMIGOS]; 
    
    texProjetilBorracha = LoadTexture("images/inimigo_borracha_tiro.png");

    Texture2D texButtonUp = LoadTexture("images/button1.png");
    Texture2D texButtonDown = LoadTexture("images/button2.png");
    
    Button spawnButton;
    spawnButton.escala = 0.6f; 
    spawnButton.isPressed = false;
    spawnButton.bounds = (Rectangle){ 
        mapa.width / 2.0f - (texButtonUp.width * spawnButton.escala / 2.0f),
        mapa.height / 3.0f-200.0f,
        (float)texButtonUp.width * spawnButton.escala, 
        (float)texButtonUp.height * spawnButton.escala 
    };

    ResetJogo(&rabisco, inimigos, MAX_INIMIGOS, mapa);

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

            UpdateProjeteis(&rabisco, mapa.width, mapa.height, mapBorderTop, mapBorderBottom, mapBorderLeft, mapBorderRight);

            int activeEnemies = 0;
            for (int i = 0; i < MAX_INIMIGOS; i++) {
                if (inimigos[i].active) {
                    UpdateInimigo(&inimigos[i], &rabisco, mapa.width, mapa.height,
                                  mapBorderTop, mapBorderBottom, 
                                  mapBorderLeft, mapBorderRight);
                    activeEnemies++;
                }
            }
            
            
            float playerHitboxSize = 10.0f;
            Rectangle rabiscoBoundsForButton = {
                rabisco.pos.x + (rabisco.width / 2.0f) - (playerHitboxSize / 2.0f),
                rabisco.pos.y + rabisco.height - playerHitboxSize,
                playerHitboxSize,
                playerHitboxSize
            };
            
            // 2. Hitbox "justa" (apertada) para o Botão
            // (Ajuste 'buttonHitboxW' e 'buttonHitboxH' para o tamanho que você quer)
            float buttonHitboxW = 30.0f;
            float buttonHitboxH = 30.0f;
            Rectangle buttonHitbox = {
                spawnButton.bounds.x + (spawnButton.bounds.width / 2.0f) - (buttonHitboxW / 2.0f), // Centro X
                spawnButton.bounds.y + (spawnButton.bounds.height / 2.0f) - (buttonHitboxH / 2.0f), // Centro Y
                buttonHitboxW,
                buttonHitboxH
            };
            
            // 3. Verifica se a rodada acabou
            if (activeEnemies == 0) {
                spawnButton.isPressed = false;
            }

            // 4. Usa as DUAS hitboxes pequenas para a colisão
            if (!spawnButton.isPressed && CheckCollisionRecs(rabiscoBoundsForButton, buttonHitbox)) {
                spawnButton.isPressed = true;
                
                // ... (código de spawn dos 4 inimigos) ...
                Vector2 pos0 = (Vector2){ (float)GetRandomValue(200, mapa.width - 200), (float)GetRandomValue(200, mapa.height - 200) };
                Vector2 pos1 = (Vector2){ (float)GetRandomValue(200, mapa.width - 200), (float)GetRandomValue(200, mapa.height - 200) };
                Vector2 pos2 = (Vector2){ (float)GetRandomValue(200, mapa.width - 200), (float)GetRandomValue(200, mapa.height - 200) };
                Vector2 pos3 = (Vector2){ (float)GetRandomValue(200, mapa.width - 200), (float)GetRandomValue(200, mapa.height - 200) };

                SpawnInimigo(&inimigos[0], TIPO_PADRAO, pos0);
                SpawnInimigo(&inimigos[1], TIPO_TANQUE, pos1);
                SpawnInimigo(&inimigos[2], TIPO_ARANHA, pos2);
                SpawnInimigo(&inimigos[3], TIPO_ATIRADOR_BORRACHA, pos3);
            }
            // --- FIM DA LÓGICA DO BOTÃO ---

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

            float halfWidth = screenW / (1.85f * camera.zoom);
            float halfHeight = screenH / (1.85f * camera.zoom);

            if (camera.target.x < halfWidth) camera.target.x = halfWidth;
            if (camera.target.y < halfHeight) camera.target.y = halfHeight;
            if (camera.target.x > mapa.width - halfWidth) camera.target.x = mapa.width - halfWidth;
            if (camera.target.y > mapa.height - halfHeight) camera.target.y = mapa.height - halfHeight;

            BeginMode2D(camera);
                DrawTexture(mapa, 0, 0, WHITE);
                
                DrawProjeteis();
                
                Texture2D texBtn = spawnButton.isPressed ? texButtonDown : texButtonUp;
                DrawTextureEx(texBtn, (Vector2){spawnButton.bounds.x, spawnButton.bounds.y}, 0.0f, spawnButton.escala, WHITE);

                for (int i = 0; i < MAX_INIMIGOS; i++) {
                    DrawInimigo(&inimigos[i]);
                }
                DrawRabisco(&rabisco);
            EndMode2D();

            int padding = 20;
            int heartSize = 70;
            
            for (int i = 0; i < rabisco.maxHeartContainers; i++) {
                Texture2D heartTexture;
                int heartValue = i * 2;
                if (rabisco.currentHitPoints >= heartValue + 2) {
                    heartTexture = rabisco.heartFull;
                } else if (rabisco.currentHitPoints == heartValue + 1) {
                    heartTexture = rabisco.heartBroken;
                } else {
                    heartTexture = rabisco.hollowHeart;
                }
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
            DrawTextureEx(rabisco.iconDamage, (Vector2){ damageIconPos.x, damageIconPos.y + iconOffset }, 0.0f, (float)iconStatusSize / rabisco.iconDamage.width, WHITE); 
            DrawTextEx(rabisco.hudFont, damageValue, (Vector2){damageValuePos.x - 2, damageValuePos.y}, statusFontSize, spacing, BLACK);
            DrawTextEx(rabisco.hudFont, damageValue, (Vector2){damageValuePos.x + 2, damageValuePos.y}, statusFontSize, spacing, BLACK);
            DrawTextEx(rabisco.hudFont, damageValue, (Vector2){damageValuePos.x, damageValuePos.y - 2}, statusFontSize, spacing, BLACK);
            DrawTextEx(rabisco.hudFont, damageValue, (Vector2){damageValuePos.x, damageValuePos.y + 2}, statusFontSize, spacing, BLACK);
            DrawTextEx(rabisco.hudFont, damageValue, damageValuePos, statusFontSize, spacing, WHITE);
            const char* speedValue = TextFormat("%.1f", rabisco.velocidade);
            int speedY = statusY + lineHeight;
            Vector2 speedIconPos = (Vector2){ padding, speedY };
            Vector2 speedValuePos = (Vector2){ padding + iconStatusSize + 10, speedY };
            DrawTextureEx(rabisco.iconVel, (Vector2){ speedIconPos.x, speedIconPos.y + iconOffset }, 0.0f, (float)iconStatusSize / rabisco.iconVel.width, WHITE);
            DrawTextEx(rabisco.hudFont, speedValue, (Vector2){speedValuePos.x - 2, speedValuePos.y}, statusFontSize, spacing, BLACK);
            DrawTextEx(rabisco.hudFont, speedValue, (Vector2){speedValuePos.x + 2, speedValuePos.y}, statusFontSize, spacing, BLACK);
            DrawTextEx(rabisco.hudFont, speedValue, (Vector2){speedValuePos.x, speedValuePos.y - 2}, statusFontSize, spacing, BLACK);
            DrawTextEx(rabisco.hudFont, speedValue, (Vector2){speedValuePos.x, speedValuePos.y + 2}, statusFontSize, spacing, BLACK);
            DrawTextEx(rabisco.hudFont, speedValue, speedValuePos, statusFontSize, spacing, WHITE);
            const char* rangeValue = TextFormat("%.1f", rabisco.alcance);
            int rangeY = speedY + lineHeight;
            Vector2 rangeIconPos = (Vector2){ padding, rangeY };
            Vector2 rangeValuePos = (Vector2){ padding + iconStatusSize + 10, rangeY };
            DrawTextureEx(rabisco.iconRange, (Vector2){ rangeIconPos.x, rangeIconPos.y + iconOffset }, 0.0f, (float)iconStatusSize / rabisco.iconRange.width, WHITE);
            DrawTextEx(rabisco.hudFont, rangeValue, (Vector2){rangeValuePos.x - 2, rangeValuePos.y}, statusFontSize, spacing, BLACK);
            DrawTextEx(rabisco.hudFont, rangeValue, (Vector2){rangeValuePos.x + 2, rangeValuePos.y}, statusFontSize, spacing, BLACK);
            DrawTextEx(rabisco.hudFont, rangeValue, (Vector2){rangeValuePos.x, rangeValuePos.y - 2}, statusFontSize, spacing, BLACK);
            DrawTextEx(rabisco.hudFont, rangeValue, (Vector2){rangeValuePos.x, rangeValuePos.y + 2}, statusFontSize, spacing, BLACK);
            DrawTextEx(rabisco.hudFont, rangeValue, rangeValuePos, statusFontSize, spacing, WHITE);

            if (rabisco.currentHitPoints <= 0) {
                estado = TELA_GAME_OVER;
                opcaoGameOver = 0; 
            }
        }
        
        else if (estado == TELA_GAME_OVER) {

            if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
                opcaoGameOver = 0;
            }
            if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
                opcaoGameOver = 1;
            }
            
            if (IsKeyPressed(KEY_ENTER)) {
                ResetJogo(&rabisco, inimigos, MAX_INIMIGOS, mapa);
                spawnButton.isPressed = false; // Garante que o botão resete
                
                if (opcaoGameOver == 0) {
                    estado = TELA_JOGO;
                } else {
                    estado = TELA_TITULO;
                }
            }
            
            DrawRectangle(0, 0, screenW, screenH, Fade(BLACK, 0.7f)); 
            
            const char *tituloGO = "GAME OVER";
            Vector2 tamTitulo = MeasureTextEx(fontTitulo, tituloGO, 80, 5);
            DrawTextEx(fontTitulo, tituloGO, (Vector2){(screenW - tamTitulo.x) / 2, screenH/2 - 100}, 80, 5, RED);

            const char *opcao1 = "Jogar de novo";
            Vector2 tamOp1 = MeasureTextEx(fontTitulo, opcao1, 40, 5);
            DrawTextEx(fontTitulo, opcao1, (Vector2){(screenW - tamOp1.x) / 2, screenH/2 + 20}, 40, 5, 
                       (opcaoGameOver == 0 ? YELLOW : WHITE)); 

            const char *opcao2 = "Voltar ao Menu";
            Vector2 tamOp2 = MeasureTextEx(fontTitulo, opcao2, 40, 5);
            DrawTextEx(fontTitulo, opcao2, (Vector2){(screenW - tamOp2.x) / 2, screenH/2 + 80}, 40, 5,
                       (opcaoGameOver == 1 ? YELLOW : WHITE)); 
        }

        EndDrawing();
    }
    UnloadTexture(texProjetilBorracha);

    UnloadMusicStream(music); 
    for (int i = 0; i < 4; i++) UnloadTexture(tituloFrames[i]);
    UnloadTexture(fundoPreto);
    
    UnloadTexture(texButtonUp);
    UnloadTexture(texButtonDown);
    
    UnloadInimigoAssets(); 
    
    UnloadTexture(mapa);
    UnloadFont(fontTitulo);
    UnloadRabisco(&rabisco); 
    CloseAudioDevice(); 
    CloseWindow();
    return 0;
}
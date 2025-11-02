#include "raylib.h"
#include "rabisco.h" 
#include "raymath.h"
#include <math.h>
#include <stdio.h>

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

    // --- TÍTULO ---
    Texture2D tituloFrames[4] = {
        LoadTexture("images/titulo1.png"),
        LoadTexture("images/titulo2.png"),
        LoadTexture("images/titulo3.png"),
        LoadTexture("images/titulo4.png")
    };
    Texture2D fundoPreto = LoadTexture("images/fundo_preto.png");
    
    const int tamanhoFonteTitulo = 30; 
   
    Font fontTitulo = LoadFontEx("assets/PatrickHandSC-Regular.ttf", tamanhoFonteTitulo, NULL, 0); 

    // --- ADICIONADO: Configuração da Música ---
    Music music = LoadMusicStream("audio/music/the_draft_music.mp3");
    music.looping = true;
    float musicVolume = 0.5f; // <-- ADICIONADO: 0.0f (mudo) a 1.0f (máximo)
    SetMusicVolume(music, musicVolume);
    // --- FIM DA ADIÇÃO ---

    int frameAtual = 0;
    float tempoFrame = 0.0f;
    const float duracaoFrame = 0.25f;

    EstadoJogo estado = TELA_TITULO;
    float alphaTransicao = 0.0f; // fade do titulo

    // --- ELEMENTOS DO MAPA ---
    const int mapBorderTop = 45;
    const int mapBorderBottom = 120;
    const int mapBorderLeft = 100;
    const int mapBorderRight = 140;

    Texture2D mapa = LoadTexture("images/mapa.png");
    Rabisco rabisco;
    InitRabisco(&rabisco, mapa.width / 2.0f, mapa.height / 2.0f); 

    Texture2D texInimigo = LoadTexture("images/idle.png");
    InimigoTeste inimigos[MAX_INIMIGOS_TESTE];

    for (int i = 0; i < MAX_INIMIGOS_TESTE; i++) {
        inimigos[i].pos = (Vector2){ (float)GetRandomValue(200, mapa.width - 200), (float)GetRandomValue(200, mapa.height - 200) };
        inimigos[i].escala = 0.10f;
        inimigos[i].maxVida = 3;
        inimigos[i].vida = inimigos[i].maxVida;
        inimigos[i].dano = 1;
        inimigos[i].velocidade = 2.0f;
        inimigos[i].active = true;
        inimigos[i].tint = WHITE;
        inimigos[i].textura = texInimigo;
        inimigos[i].bounds = (Rectangle){ inimigos[i].pos.x, inimigos[i].pos.y, texInimigo.width * inimigos[i].escala, texInimigo.height * inimigos[i].escala };
    }

    Camera2D camera = { 0 };
    camera.target = rabisco.pos;
    camera.offset = (Vector2){ screenW / 2.0f, screenH / 2.0f };
    camera.rotation = 0.0f;

    float zoomX = (float)screenW / (float)mapa.width;
    float zoomY = (float)screenH / (float)mapa.height;
    camera.zoom = (zoomX < zoomY ? zoomX : zoomY) * 2.0f;

    while (!WindowShouldClose()) {
        UpdateMusicStream(music); // <-- ADICIONADO: Atualiza o buffer da música

        BeginDrawing();
        ClearBackground(BLACK);

        if (estado == TELA_TITULO) {
            tempoFrame += GetFrameTime();
            if (tempoFrame >= duracaoFrame) {
                tempoFrame = 0.0f;
                frameAtual = (frameAtual + 1) % 4;
            }

            // --- Ajusta imagem para preencher a tela ---
            Texture2D frame = tituloFrames[frameAtual];
            float scaleX = (float)screenW / frame.width;
            float scaleY = (float)screenH / frame.height;
            float scale = (scaleX > scaleY) ? scaleX : scaleY;
            float posX = (screenW - frame.width * scale) / 2;
            float posY = (screenH - frame.height * scale) / 2;
            DrawTextureEx(frame, (Vector2){ posX, posY }, 0.0f, scale, WHITE);

            // --- Texto centralizado ---
            const char *texto = "Press any button to start";
            Vector2 textSize = MeasureTextEx(fontTitulo, texto, tamanhoFonteTitulo, 5); 
            DrawTextEx(fontTitulo, texto,
                       (Vector2){ (screenW - textSize.x) / 2.55, screenH - 80 }, 
                       tamanhoFonteTitulo, 9, BLACK);

            if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT) ||
                GetKeyPressed() != 0) {
                estado = TELA_TRANSICAO;
                alphaTransicao = 0.0f;
            }
        }

        else if (estado == TELA_TRANSICAO) {
            // Repete último frame do título
            Texture2D frame = tituloFrames[frameAtual];
            float scaleX = (float)screenW / frame.width;
            float scaleY = (float)screenH / frame.height;
            float scale = (scaleX > scaleY) ? scaleX : scaleY;
            float posX = (screenW - frame.width * scale) / 2;
            float posY = (screenH - frame.height * scale) / 2;
            DrawTextureEx(frame, (Vector2){ posX, posY }, 0.0f, scale, WHITE);

            // Fade suave
            alphaTransicao += GetFrameTime();
            if (alphaTransicao > 1.0f) alphaTransicao = 1.0f;

            DrawRectangle(0, 0, screenW, screenH, Fade(BLACK, alphaTransicao));

            if (alphaTransicao >= 1.0f) {
                estado = TELA_JOGO;
                PlayMusicStream(music); // <-- ADICIONADO: Toca a música quando o jogo começa
            }
        }

        else if (estado == TELA_JOGO) {
            // Controles de Volume (use - e +) ---
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
                    Vector2 move = Vector2Normalize(Vector2Subtract(rabisco.pos, e->pos));
                    e->pos.x += move.x * e->velocidade;
                    e->pos.y += move.y * e->velocidade;
                    e->bounds.x = e->pos.x;
                    e->bounds.y = e->pos.y;
                }
            }

            if (rabiscoAtacou) {
                Rectangle attackBox = GetRabiscoAttackHitbox(&rabisco);
                for (int i = 0; i < MAX_INIMIGOS_TESTE; i++) {
                    InimigoTeste *e = &inimigos[i];
                    if (e->active && CheckCollisionRecs(attackBox, e->bounds)) {
                        e->vida -= rabisco.dano;
                        e->tint = RED;
                    }
                }
            }

            for (int i = 0; i < MAX_INIMIGOS_TESTE; i++) {
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
            int coinPosY = padding + heartSize + padding / 2;
            const char *coinText = TextFormat("%02d", rabisco.moedas);
            float coinScale = (float)coinSize / rabisco.coinIcon.width;
            DrawTextureEx(rabisco.coinIcon, (Vector2){ padding, coinPosY }, 0.0f, coinScale, WHITE);
            DrawTextEx(rabisco.hudFont, coinText, (Vector2){ padding + coinSize + 10, coinPosY + 10 }, fontSize, 5, WHITE);
        }

        EndDrawing();
    }

    // Liberação de recursos
    UnloadMusicStream(music); // Libera a música
    for (int i = 0; i < 4; i++) UnloadTexture(tituloFrames[i]);
    UnloadTexture(fundoPreto);
    UnloadTexture(texInimigo);
    UnloadTexture(mapa);
    UnloadFont(fontTitulo);
    UnloadRabisco(&rabisco); 
    CloseAudioDevice(); 
    CloseWindow();
    return 0;
}
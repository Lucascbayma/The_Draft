#include "gameloop.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// --- GLOBAL VARIABLE DEFINITIONS ---
Projetil projeteis[MAX_PROJETEIS];
Texture2D texProjetilBorracha; 
int ondaAtual = 0;
int subOnda = 0; 
float chanceBonusOnda = 0.35f;
ScoreEntry highScores[MAX_HIGH_SCORES];
int finalScore = 0; 
int currentScore = 0;
Pedestal pedestais[NUM_PEDESTAIS];
Texture2D texPedestalBase; 


// --- FUNÇÕES DE PONTUAÇÃO ---

int CompareScores(const void *a, const void *b) {
    ScoreEntry *scoreA = (ScoreEntry *)a;
    ScoreEntry *scoreB = (ScoreEntry *)b;
    return scoreB->score - scoreA->score;
}
void SaveHighScores(ScoreEntry scores[]) {
    FILE *file = fopen("scores.txt", "w");
    if (file == NULL) return;
    for (int i = 0; i < MAX_HIGH_SCORES; i++) {
        fprintf(file, "%s %d\n", scores[i].initials, scores[i].score);
    }
    fclose(file);
}
void LoadHighScores(ScoreEntry scores[]) {
    FILE *file = fopen("scores.txt", "r");
    if (file == NULL) {
        strcpy(scores[0].initials, "AAA"); scores[0].score = 50;
        strcpy(scores[1].initials, "BBB"); scores[1].score = 40;
        strcpy(scores[2].initials, "CCC"); scores[2].score = 30;
        strcpy(scores[3].initials, "DDD"); scores[3].score = 20;
        strcpy(scores[4].initials, "EEE"); scores[4].score = 10;
        SaveHighScores(scores);
        return;
    }
    for (int i = 0; i < MAX_HIGH_SCORES; i++) {
        if (fscanf(file, "%s %d", scores[i].initials, &scores[i].score) != 2) {
            strcpy(scores[i].initials, "NUL");
            scores[i].score = 0;
        }
    }
    fclose(file);
}
bool IsHighScore(ScoreEntry scores[], int newScore) {
    return newScore > scores[MAX_HIGH_SCORES - 1].score;
}
void AddNewScore(ScoreEntry scores[], char newInitials[], int newScore) {
    ScoreEntry tempScores[MAX_HIGH_SCORES + 1];
    for(int i=0; i < MAX_HIGH_SCORES; i++) tempScores[i] = scores[i];
    strcpy(tempScores[MAX_HIGH_SCORES].initials, newInitials);
    tempScores[MAX_HIGH_SCORES].score = newScore;
    qsort(tempScores, MAX_HIGH_SCORES + 1, sizeof(ScoreEntry), CompareScores);
    for(int i=0; i < MAX_HIGH_SCORES; i++) scores[i] = tempScores[i];
    SaveHighScores(scores);
}


// --- FUNÇÕES DE PROJÉTIL ---

void SpawnProjetilAtirador(Vector2 startPos, Vector2 direction) {
    for (int i = 0; i < MAX_PROJETEIS; i++) {
        if (!projeteis[i].active) {
            
            projeteis[i].escala = 0.04f;
            
            Vector2 scaledDir = Vector2Scale(direction, 10.0f);
            projeteis[i].pos = Vector2Add(startPos, scaledDir);
            
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
            
            Vector2 scaledVel = Vector2Scale(projeteis[i].velocity, projeteis[i].speed);
            projeteis[i].pos = Vector2Add(projeteis[i].pos, scaledVel);
            
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


// --- FUNÇÕES DE ITEM/PEDESTAL ---

ItemType GetRandomVidaConsumivelItem(void) {
    int r = GetRandomValue(0, 2);
    if (r == 0) return ITEM_CORACAO_PARTIDO;
    if (r == 1) return ITEM_CORACAO;
    return ITEM_CORACAO_DUPLO;
}
ItemType GetRandomUpgradeItem(void) {
    int r = GetRandomValue(0, 7);
    switch(r) {
        case 0: return ITEM_LAPIS_PARTIDO;
        case 1: return ITEM_APONTADOR;
        case 2: return ITEM_ESTILETE;
        case 3: return ITEM_COLA;
        case 4: return ITEM_BOTAS_DE_PAPEL;
        case 5: return ITEM_PENGOO;
        case 6: return ITEM_CORACAO_VAZIO;
        case 7: return ITEM_GRAMPEADOR;
        default: return ITEM_APONTADOR;
    }
}
void ShuffleTipos(ItemType array[], int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = GetRandomValue(0, i);
        ItemType temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

void ResolveStaticCollision(Rabisco *r, Rectangle obstacleBounds) { 
    Rectangle rabiscoHitbox = GetRabiscoHitbox(r);

    if(CheckCollisionRecs(rabiscoHitbox, obstacleBounds)){
        float overlapX = (rabiscoHitbox.width / 2.0f + obstacleBounds.width / 2.0f) - fabs((rabiscoHitbox.x + rabiscoHitbox.width/2.0f) - (obstacleBounds.x + obstacleBounds.width/2.0f));
        float overlapY = (rabiscoHitbox.height / 2.0f + obstacleBounds.height / 2.0f) - fabs((rabiscoHitbox.y + rabiscoHitbox.height/2.0f) - (obstacleBounds.y + obstacleBounds.height/2.0f));
        
        float tinyPush = 0.01f;

        if(overlapX < overlapY){
            if(rabiscoHitbox.x < obstacleBounds.x){
                r->pos.x -= overlapX + tinyPush; 
            }else{ 
                r->pos.x += overlapX + tinyPush; 
            }
        } 
        else{
            if(rabiscoHitbox.y < obstacleBounds.y){ 
                r->pos.y -= overlapY + tinyPush; 
            }else{ 
                r->pos.y += overlapY + tinyPush; 
            }
        }
    }
}

void SetupPedestais(int mapW, int mapBorderTop) {
    float scale = 0.15f; 
    float pW = texPedestalBase.width * scale;
    float pH = texPedestalBase.height * scale;
    
    if (pW == 0) pW = 50;
    if (pH == 0) pH = 70;
    
    float matrizPosicoes[NUM_PEDESTAIS][2] = {
        {130.0f,  50.0f},  
        {250.0f,  50.0f},  
        {615.0f,  50.0f},  
        {750.5f,  50.0f}   
    };

    for (int i = 0; i < NUM_PEDESTAIS; i++) {
        
        float posX = matrizPosicoes[i][0];
        float posY = matrizPosicoes[i][1];
        
        pedestais[i].pos = (Vector2){ posX, posY };
        
        pedestais[i].active = false; 
        pedestais[i].tipo = ITEM_NULO;
        pedestais[i].preco = 0;
        pedestais[i].floatTimer = GetRandomValue(0, 100) / 10.0f;
        pedestais[i].waitingForRestock = false;
        pedestais[i].restockTimer = 0.0f;

        pedestais[i].hitRect = (Rectangle){ 
            pedestais[i].pos.x + 10, 
            pedestais[i].pos.y + 20, 
            pW - 20, 
            pH - 20 
        };
        
        pedestais[i].triggerRect = (Rectangle){
            pedestais[i].hitRect.x - 10,
            pedestais[i].hitRect.y - 10,
            pedestais[i].hitRect.width + 20,
            pedestais[i].hitRect.height + 20
        };
    }
}

void RestockPedestais(void) {
    for(int i=0; i<NUM_PEDESTAIS; i++) {
        pedestais[i].waitingForRestock = false;
        pedestais[i].restockTimer = 0.0f;
    }

    pedestais[0].active = true; 
    pedestais[0].tipo = GetRandomVidaConsumivelItem();
    pedestais[0].preco = GetItemPrice(pedestais[0].tipo);

    ItemType up1 = GetRandomUpgradeItem();
    ItemType up2 = GetRandomUpgradeItem();
    pedestais[1].active = true; pedestais[1].tipo = up1; pedestais[1].preco = GetItemPrice(up1);
    pedestais[2].active = true; pedestais[2].tipo = up2; pedestais[2].preco = GetItemPrice(up2);

    pedestais[3].active = false; 
    pedestais[3].tipo = ITEM_NULO;
    pedestais[3].preco = 0;
}


void UpdatePedestais(Rabisco *r) {
    for (int i = 0; i < NUM_PEDESTAIS; i++) {
        ResolveStaticCollision(r, pedestais[i].hitRect);

        if (pedestais[i].waitingForRestock) {
            pedestais[i].restockTimer -= GetFrameTime();
            if (pedestais[i].restockTimer <= 0) {
                pedestais[i].waitingForRestock = false;
                
                if (i == 0) { 
                    pedestais[i].tipo = GetRandomVidaConsumivelItem();
                    pedestais[i].preco = GetItemPrice(pedestais[i].tipo);
                    pedestais[i].active = true; 
                } 
                else if (i == 1 || i == 2) { 
                    pedestais[i].tipo = GetRandomUpgradeItem();
                    pedestais[i].preco = GetItemPrice(pedestais[i].tipo);
                    pedestais[i].active = true;
                }
            }
            continue; 
        }

        if (!pedestais[i].active) continue; 
        
        pedestais[i].floatTimer += GetFrameTime();
        
        if (CheckCollisionRecs(GetRabiscoHitbox(r), pedestais[i].triggerRect)) {
            
            if (r->moedas >= pedestais[i].preco) {
                 
                 bool comprou = false;
                 
                 if (pedestais[i].tipo == ITEM_CORACAO_PARTIDO || pedestais[i].tipo == ITEM_CORACAO || pedestais[i].tipo == ITEM_CORACAO_DUPLO) {
                     if (r->currentHitPoints < r->maxHeartContainers * 2) {
                        if (pedestais[i].tipo == ITEM_CORACAO_PARTIDO) r->currentHitPoints += 1;
                        else if (pedestais[i].tipo == ITEM_CORACAO) r->currentHitPoints += 2;
                        else r->currentHitPoints += 4;
                        
                        if (r->currentHitPoints > r->maxHeartContainers * 2) r->currentHitPoints = r->maxHeartContainers * 2;
                        comprou = true;
                     }
                 } else {
                     AddItemAoInventario(r, pedestais[i].tipo);
                     AplicarStatsInventario(r);
                     if (pedestais[i].tipo == ITEM_GRAMPEADOR || pedestais[i].tipo == ITEM_CORACAO_VAZIO) {
                         if(pedestais[i].tipo == ITEM_GRAMPEADOR) r->currentHitPoints += 2;
                         if (pedestais[i].tipo == ITEM_CORACAO_VAZIO) r->currentHitPoints = r->maxHeartContainers * 2;
                     }
                     comprou = true;
                 }

                 if (comprou) {
                     r->moedas -= pedestais[i].preco;
                     pedestais[i].active = false; 
                     pedestais[i].tipo = ITEM_NULO;
                     
                     pedestais[i].waitingForRestock = true;
                     pedestais[i].restockTimer = 2.0f; 
                 }
            }
        }
    }
}

void DrawPedestais(Font font) {
    float scale = 0.15f;
    for (int i = 0; i < NUM_PEDESTAIS; i++) {
        
        DrawTextureEx(texPedestalBase, pedestais[i].pos, 0.0f, scale, WHITE);
        
        if (pedestais[i].active && pedestais[i].tipo != ITEM_NULO) {
            float floatY = sinf(pedestais[i].floatTimer * 2.0f) * 3.0f; 
            
            Texture2D itemTex = GetItemIconTexture(pedestais[i].tipo);
            
            if (itemTex.id > 0) {
                float itemScale = 0.1f; 
                Vector2 itemPos = { 
                    pedestais[i].pos.x + (texPedestalBase.width * scale / 2) - (itemTex.width * itemScale / 2), 
                    pedestais[i].pos.y + floatY + 20.0f 
                };
                
                DrawTextureEx(itemTex, itemPos, 0.0f, itemScale, WHITE);
            }
            
            const char* precoTexto = TextFormat("$%d", pedestais[i].preco);
            Vector2 txtSize = MeasureTextEx(font, precoTexto, 20, 2);
            Vector2 txtPos = { 
                pedestais[i].pos.x + (texPedestalBase.width * scale / 2) - (txtSize.x / 2), 
                pedestais[i].pos.y + (texPedestalBase.height * scale) - 35.0f
            };
            
            DrawTextEx(font, precoTexto, (Vector2){txtPos.x + 1, txtPos.y + 1}, 20, 2, BLACK); 
            DrawTextEx(font, precoTexto, txtPos, 20, 2, WHITE); 
        }
    }
}

// --- FUNÇÕES DE FLUXO DE JOGO ---

Vector2 GetRandomSpawnPosition(Rabisco *r, int mapW, int mapH, int bTop, int bBot, int bLeft, int bRight) {
    Vector2 pos;
    float safeRadius = 250.0f; 
    
    do {
        pos = (Vector2){ 
            (float)GetRandomValue(bLeft + 50, mapW - bRight - 50), 
            (float)GetRandomValue(bTop + 50, mapH - bBot - 50) 
        };
    } while (Vector2Distance(pos, r->pos) < safeRadius);
    
    return pos;
}

void SpawnWave(int onda, Inimigo inimigos[], Rabisco *r, int mapW, int mapH, int bTop, int bBot, int bLeft, int bRight) {
    
    for (int i = 0; i < MAX_INIMIGOS; i++) {
        inimigos[i].active = false;
    }
    
    switch (onda) {
        case 1:
        {
            SpawnInimigo(&inimigos[0], TIPO_PADRAO, GetRandomSpawnPosition(r, mapW, mapH, bTop, bBot, bLeft, bRight));
            SpawnInimigo(&inimigos[1], TIPO_PADRAO, GetRandomSpawnPosition(r, mapW, mapH, bTop, bBot, bLeft, bRight));
            break;
        }

        case 2:
        {
            SpawnInimigo(&inimigos[0], TIPO_PADRAO, GetRandomSpawnPosition(r, mapW, mapH, bTop, bBot, bLeft, bRight));
            SpawnInimigo(&inimigos[1], TIPO_PADRAO, GetRandomSpawnPosition(r, mapW, mapH, bTop, bBot, bLeft, bRight));
            SpawnInimigo(&inimigos[2], TIPO_TANQUE, GetRandomSpawnPosition(r, mapW, mapH, bTop, bBot, bLeft, bRight));
            break;
        }

        default:
        { 
            bool levaDificil = (GetRandomValue(0, 1) == 1);
            int inimigoIndex = 0;
            
            if (levaDificil) {
                int totalInimigos = GetRandomValue(3, 4);
                
                SpawnInimigo(&inimigos[inimigoIndex], (GetRandomValue(0, 1) == 0 ? TIPO_ARANHA : TIPO_ATIRADOR_BORRACHA), GetRandomSpawnPosition(r, mapW, mapH, bTop, bBot, bLeft, bRight));
                inimigoIndex++;
                
                for (int i = inimigoIndex; i < totalInimigos; i++) {
                    SpawnInimigo(&inimigos[i], (InimigoType)GetRandomValue(0, 3), GetRandomSpawnPosition(r, mapW, mapH, bTop, bBot, bLeft, bRight));
                    inimigos[i].active = true;
                }
            } else {
                int totalInimigos = GetRandomValue(4, 6);
                for (int i = 0; i < totalInimigos; i++) {
                    SpawnInimigo(&inimigos[i], (InimigoType)GetRandomValue(0, 1), GetRandomSpawnPosition(r, mapW, mapH, bTop, bBot, bLeft, bRight));
                    inimigos[i].active = true;
                }
            }
            break;
        }
    }
}

void ResetJogo(Rabisco *rabisco, Inimigo inimigos[], int maxInimigos, Texture2D mapa) {
    LimparInventario(rabisco);
    AplicarStatsInventario(rabisco);

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
    
    SetupPedestais(mapa.width, 65); 
    RestockPedestais(); 
    
    ondaAtual = 0;
    subOnda = 0;
    chanceBonusOnda = 0.35f;
    currentScore = 0; 
}
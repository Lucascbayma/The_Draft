#include "inimigos.h"
#include "raymath.h"
#include <math.h>
#include <stdio.h>

#define FRAME_COUNT_PADRAO 8
#define FRAME_COUNT_TANQUE 8
#define FRAME_COUNT_ARANHA 4
#define FRAME_COUNT_ATIRADOR 8

static Texture2D texPadraoIdle;
static Texture2D texPadraoLeft[FRAME_COUNT_PADRAO];
static Texture2D texPadraoRight[FRAME_COUNT_PADRAO];
static Texture2D texTanqueIdle;
static Texture2D texTanqueLeft[FRAME_COUNT_TANQUE];
static Texture2D texTanqueRight[FRAME_COUNT_TANQUE];
static Texture2D texAranhaIdle;
static Texture2D texAranhaLeft[FRAME_COUNT_ARANHA];
static Texture2D texAranhaRight[FRAME_COUNT_ARANHA];

static Texture2D texAtiradorIdle;
static Texture2D texAtiradorLeft[FRAME_COUNT_ATIRADOR];
static Texture2D texAtiradorRight[FRAME_COUNT_ATIRADOR];
static Texture2D texProjetilBorracha; 

void InitInimigoAssets(void) {
    texPadraoIdle = LoadTexture("images/inimigo_base_direita1.png");
    for (int i = 0; i < FRAME_COUNT_PADRAO; i++) {
        char f[64];
        sprintf(f, "images/inimigo_base_direita%d.png", i+1);
        texPadraoRight[i] = LoadTexture(f);
        sprintf(f, "images/inimigo_base_esquerda%d.png", i+1);
        texPadraoLeft[i] = LoadTexture(f);
    }
    
    texTanqueIdle = LoadTexture("images/inimigo_tank_direita1.png");
    for (int i = 0; i < FRAME_COUNT_TANQUE; i++) {
        char f[64];
        sprintf(f, "images/inimigo_tank_direita%d.png", i+1);
        texTanqueRight[i] = LoadTexture(f);
        sprintf(f, "images/inimigo_tank_esquerda%d.png", i+1);
        texTanqueLeft[i] = LoadTexture(f);
    }
    
    texAranhaIdle = LoadTexture("images/inimigo_aranha_direita1.png");
    for (int i = 0; i < FRAME_COUNT_ARANHA; i++) {
        char f[64];
        sprintf(f, "images/inimigo_aranha_direita%d.png", i+1);
        texAranhaRight[i] = LoadTexture(f);
        sprintf(f, "images/inimigo_aranha_esquerda%d.png", i+1);
        texAranhaLeft[i] = LoadTexture(f);
    }

    texAtiradorIdle = LoadTexture("images/inimigo_borracha_direita1.png");
    for (int i = 0; i < FRAME_COUNT_ATIRADOR; i++) {
        char f[64];
        sprintf(f, "images/inimigo_borracha_direita%d.png", i + 1);
        texAtiradorRight[i] = LoadTexture(f);
        sprintf(f, "images/inimigo_borracha_esquerda%d.png", i + 1);
        texAtiradorLeft[i] = LoadTexture(f);
    }
    texProjetilBorracha = LoadTexture("images/inimigo_borracha_tiro.png"); 
}

void UnloadInimigoAssets(void) {
    UnloadTexture(texPadraoIdle);
    for (int i = 0; i < FRAME_COUNT_PADRAO; i++) {
        UnloadTexture(texPadraoRight[i]);
        UnloadTexture(texPadraoLeft[i]);
    }
    
    UnloadTexture(texTanqueIdle);
    for (int i = 0; i < FRAME_COUNT_TANQUE; i++) {
        UnloadTexture(texTanqueRight[i]);
        UnloadTexture(texTanqueLeft[i]);
    }
    
    UnloadTexture(texAranhaIdle);
    for (int i = 0; i < FRAME_COUNT_ARANHA; i++) {
        UnloadTexture(texAranhaLeft[i]);
        UnloadTexture(texAranhaRight[i]);
    }
    
    UnloadTexture(texAtiradorIdle);
    for (int i = 0; i < FRAME_COUNT_ATIRADOR; i++) {
        UnloadTexture(texAtiradorLeft[i]);
        UnloadTexture(texAtiradorRight[i]);
    }
    UnloadTexture(texProjetilBorracha);
}

void SpawnInimigo(Inimigo *e, InimigoType tipo, Vector2 pos) {
    e->tipo = tipo;
    e->pos = pos;
    e->escala = 0.10f;
    e->tint = WHITE;
    e->active = true;
    e->attackTimer = 0;
    
    e->facingDir = DIR_DOWN;
    e->frame = 0;
    e->frameTime = 0.0f;
    e->frameDelay = 1.0f / 8.0f;
    float texW = (float)texPadraoIdle.width;
    float texH = (float)texPadraoIdle.height;

    switch (e->tipo) {
        case TIPO_PADRAO:
            e->escala = 0.10f;
            e->vida = 3;
            e->maxVida = 3;
            e->dano = 1; 
            e->velocidade = 2.0f; 
            e->distanciaAtaque = 40.0f;
            e->velAtaque = 1.5f;
            e->frameCount = FRAME_COUNT_PADRAO;
            texW = (float)texPadraoIdle.width;
            texH = (float)texPadraoIdle.height;
            break;
            
        case TIPO_TANQUE:
            e->escala = 0.13f;
            e->vida = 8;
            e->maxVida = 8;
            e->dano = 2; 
            e->velocidade = 1.0f; 
            e->distanciaAtaque = 50.0f;
            e->velAtaque = 2.5f;
            e->frameCount = FRAME_COUNT_TANQUE;
            texW = (float)texTanqueIdle.width;
            texH = (float)texTanqueIdle.height;
            break;
            
        case TIPO_ARANHA:
            e->escala = 0.05f;
            e->vida = 5;
            e->maxVida = 2;
            e->dano = 1; 
            e->velocidade = 10.5f; 
            e->distanciaAtaque = 60.0f;
            e->velAtaque = 0.8f;
            e->frameCount = FRAME_COUNT_ARANHA;
            e->frameDelay = 1.0f / 12.0f;
            texW = (float)texAranhaIdle.width;
            texH = (float)texAranhaIdle.height;
            
            e->spiderState = SPIDER_IDLE;
            e->attackTriggerTimer = 0.0f;
            e->movementTriggerTimer = 0.0f;
            e->actionTimer = 0.0f;
            e->actionDirection = (Vector2){0, 0};
            
            break;
            
        case TIPO_ATIRADOR_BORRACHA:
            e->escala = 0.10f;
            e->vida = 4;
            e->maxVida = 2;
            e->dano = 1;
            e->velocidade = 1.8f; 
            e->distanciaAtaque = 400.0f; 
            e->velAtaque = 2.75f; 
            e->frameCount = FRAME_COUNT_ATIRADOR;
            texW = (float)texAtiradorIdle.width;
            texH = (float)texAtiradorIdle.height;
            break;
    }

    e->maxVida = e->vida;
    e->bounds = (Rectangle){ e->pos.x, e->pos.y, texW * e->escala, texH * e->escala };
}

Rectangle GetInimigoHitbox(Inimigo *e) {
    e->bounds.x = e->pos.x;
    e->bounds.y = e->pos.y;
    return e->bounds;
}

void UpdateInimigo(Inimigo *e, Rabisco *r, int mapW, int mapH,int borderTop, int borderBottom,int borderLeft, int borderRight) 
{
    if (!e->active) return;

    if (e->vida <= 0) {
        e->active = false;
        r->moedas++;
        return;
    }

    if (e->attackTimer > 0) e->attackTimer -= GetFrameTime();
    
    float dist = Vector2Distance(e->pos, r->pos);
    float chaseRadius = 400.0f;
    float dt = GetFrameTime(); 
    
    Vector2 move = {0, 0};
    
    bool isMovingOrLooking = false;
    
    
    if (e->tipo == TIPO_PADRAO || e->tipo == TIPO_TANQUE) {
        if (dist < chaseRadius && dist > e->distanciaAtaque) {
            move = Vector2Normalize(Vector2Subtract(r->pos, e->pos));
            e->pos.x += move.x * e->velocidade; 
            e->pos.y += move.y * e->velocidade; 
            isMovingOrLooking = true;
        } else if (dist < e->distanciaAtaque && e->attackTimer <= 0) {
            r->currentHitPoints -= e->dano; 
            e->attackTimer = e->velAtaque;
        }
    }
    
    else if (e->tipo == TIPO_ARANHA) {
        
        // DANO DE CONTATO (40.0f)
        if (dist < 40.0f && e->attackTimer <= 0) {
            r->currentHitPoints -= e->dano; 
            e->attackTimer = e->velAtaque; 
        }
        
        e->attackTriggerTimer += dt;
        if (e->attackTriggerTimer >= 5.0f) {
            e->attackTriggerTimer = 0.0f; 
            
            if (GetRandomValue(0, 1) == 0) {
                e->spiderState = SPIDER_ATTACKING;
                e->actionTimer = 1.5f; 
                e->actionDirection = Vector2Normalize(Vector2Subtract(r->pos, e->pos));
            }
        }
        
        if (e->spiderState == SPIDER_IDLE) {
            e->movementTriggerTimer += dt;
            
            float texW = e->bounds.width;
            float texH = e->bounds.height;
            bool naParedeEsquerda = (e->pos.x <= borderLeft + e->velocidade); 
            bool naParedeDireita = (e->pos.x >= mapW - borderRight - texW - e->velocidade);
            bool naParedeCima = (e->pos.y <= borderTop + e->velocidade);
            bool naParedeBaixo = (e->pos.y >= mapH - borderBottom - texH - e->velocidade);


            if (e->movementTriggerTimer >= 1.0f) {
                e->movementTriggerTimer = 0.0f;
                
                int direcoesValidas[8] = {1, 2, 3, 4, 5, 6, 7, 8}; 
                
                if (naParedeEsquerda) { direcoesValidas[2] = 0; } 
                if (naParedeDireita) { direcoesValidas[3] = 0; }
                if (naParedeCima) { direcoesValidas[0] = 0; }
                if (naParedeBaixo) { direcoesValidas[1] = 0; }


                int choice;
                int attempts = 0;
                
                do {
                    int index = GetRandomValue(1, 8); 
                    choice = direcoesValidas[index - 1]; 
                    
                    attempts++;
                    if (attempts > 50) { 
                         choice = 5;
                         break;
                    }
                } while (choice == 0);

                
                switch (choice) {
                    case 1: e->spiderState = SPIDER_MOVING; e->actionTimer = 0.45f; e->actionDirection = (Vector2){0, -1}; break;
                    case 2: e->spiderState = SPIDER_MOVING; e->actionTimer = 0.45f; e->actionDirection = (Vector2){0, 1}; break;
                    case 3: e->spiderState = SPIDER_MOVING; e->actionTimer = 0.45f; e->actionDirection = (Vector2){-1, 0}; break;
                    case 4: e->spiderState = SPIDER_MOVING; e->actionTimer = 0.45f; e->actionDirection = (Vector2){1, 0}; break;
                    
                    case 5: 
                    case 6: 
                    case 7: 
                    case 8: 
                        e->spiderState = SPIDER_IDLE; 
                        e->actionTimer = 0.0f; 
                        e->actionDirection = (Vector2){0, 0}; 
                        break;
                }
            }
        }
        
        
        if (e->spiderState == SPIDER_ATTACKING || e->spiderState == SPIDER_MOVING) {
            e->actionTimer -= dt; 
            
            if (e->actionTimer > 0.0f) {

                move = e->actionDirection; 
                Vector2 novaPos = e->pos;
                novaPos.x += move.x * e->velocidade;
                novaPos.y += move.y * e->velocidade;
                
                float texW = e->bounds.width;
                float texH = e->bounds.height;

                if (novaPos.x < borderLeft) {
                    e->pos.x = borderLeft + e->velocidade; 
                    e->spiderState = SPIDER_IDLE;
                    e->movementTriggerTimer = 0.0f;
                    move = (Vector2){0, 0}; 
                } 
                else if (novaPos.x > mapW - borderRight - texW) {
                    e->pos.x = mapW - borderRight - texW - e->velocidade; 
                    e->spiderState = SPIDER_IDLE;
                    e->movementTriggerTimer = 0.0f;
                    move = (Vector2){0, 0};
                } 
                else if (novaPos.y < borderTop) {
                    e->pos.y = borderTop + e->velocidade; 
                    e->spiderState = SPIDER_IDLE;
                    e->movementTriggerTimer = 0.0f;
                    move = (Vector2){0, 0};
                } 
                else if (novaPos.y > mapH - borderBottom - texH) {
                    e->pos.y = mapH - borderBottom - texH - e->velocidade; 
                    e->spiderState = SPIDER_IDLE;
                    e->movementTriggerTimer = 0.0f;
                    move = (Vector2){0, 0};
                } else {
                    e->pos.x += move.x * e->velocidade;
                    e->pos.y += move.y * e->velocidade;
                }
                
                isMovingOrLooking = true; 
            } else {
                e->spiderState = SPIDER_IDLE;
                e->movementTriggerTimer = 0.0f; 
                move = (Vector2){0, 0};
            }
        }
    }
    
    // --- LÓGICA DO ATIRADOR BORRACHA  ---
    else if (e->tipo == TIPO_ATIRADOR_BORRACHA) {
        
        Vector2 dirToRabisco = Vector2Normalize(Vector2Subtract(r->pos, e->pos));
        float idealDistanceMin = 200.0f; 
        float idealDistanceMax = 350.0f; 
        
        // 1. DEFINIR PARA ONDE ESTÁ OLHANDO
        if (fabs(dirToRabisco.x) > fabs(dirToRabisco.y)) {
            e->facingDir = (dirToRabisco.x > 0) ? DIR_RIGHT : DIR_LEFT;
        } else {
            e->facingDir = (dirToRabisco.y > 0) ? DIR_DOWN : DIR_UP; 
        }
        isMovingOrLooking = true; 

        // 2. DETERMINAR O MOVIMENTO
        if (dist < idealDistanceMin) {
            // ZONA DE FUGA: Está muito perto
            move = Vector2Scale(dirToRabisco, -1.0f); 
        } else if (dist > idealDistanceMax) {
            // ZONA DE PERSEGUIÇÃO: Está muito longe
            move = dirToRabisco; 
        } else {
            // ZONA IDEAL: Para e atira.
            move = (Vector2){0, 0};
        }
        
        // 3. APLICAR MOVIMENTO
        e->pos.x += move.x * e->velocidade; 
        e->pos.y += move.y * e->velocidade;
        
        // 4. ATAQUE
        if (dist < e->distanciaAtaque && e->attackTimer <= 0) {
            SpawnProjetilAtirador(e->pos, dirToRabisco); 
            e->attackTimer = e->velAtaque; 
        }
    }
    
    // O bloco de limites final (abaixo) irá conter o atirador no mapa.
    
    if (Vector2Distance(move, (Vector2){0, 0}) != 0 || isMovingOrLooking) {
        
        // Se houver movimento, atualiza facingDir (para Padrão/Tanque/Aranha no estado MOVING)
        if (Vector2Distance(move, (Vector2){0, 0}) != 0 && e->tipo != TIPO_ATIRADOR_BORRACHA) {
            if (fabs(move.x) > fabs(move.y)) {
                e->facingDir = (move.x > 0) ? DIR_RIGHT : DIR_LEFT;
            } else {
                e->facingDir = (move.y > 0) ? DIR_DOWN : DIR_UP;
            }
        }
        
        e->frameTime += GetFrameTime();
        if (e->frameTime >= e->frameDelay) {
            e->frame = (e->frame + 1) % e->frameCount;
            e->frameTime = 0.0f;
        }
    } else {
        e->frame = 0;
        e->facingDir = DIR_IDLE;
    }

    // --- CHECAGEM DE LIMITES GERAL ---
    float texW = e->bounds.width;
    float texH = e->bounds.height;
    
    if (e->pos.x < borderLeft) e->pos.x = borderLeft;
    if (e->pos.x > mapW - borderRight - texW) e->pos.x = mapW - borderRight - texW;
    if (e->pos.y < borderTop) e->pos.y = borderTop;
    if (e->pos.y > mapH - borderBottom - texH) e->pos.y = mapH - borderBottom - texH;
    
    e->bounds.x = e->pos.x;
    e->bounds.y = e->pos.y;
}

void DrawInimigo(Inimigo *e) {
    if (!e->active) return;
    
    Texture2D currentFrame;

    if (e->tipo == TIPO_PADRAO) {
        switch (e->facingDir) {
            case DIR_LEFT:
                currentFrame = texPadraoLeft[e->frame];
                break;
            case DIR_RIGHT:
            case DIR_UP:
            case DIR_DOWN:
                currentFrame = texPadraoRight[e->frame];
                break;
            case DIR_IDLE:
            default:
                currentFrame = texPadraoIdle;
                break;
        }
    } 
    else if (e->tipo == TIPO_TANQUE) {
        switch (e->facingDir) {
            case DIR_LEFT:
                currentFrame = texTanqueLeft[e->frame];
                break;
            case DIR_RIGHT:
            case DIR_UP:
            case DIR_DOWN:
                currentFrame = texTanqueRight[e->frame];
                break;
            case DIR_IDLE:
            default:
                currentFrame = texTanqueIdle;
                break;
        }
    }
    else if (e->tipo == TIPO_ARANHA) {
        switch (e->facingDir) {
            case DIR_LEFT:
                currentFrame = texAranhaLeft[e->frame];
                break;
            case DIR_RIGHT:
            case DIR_UP:
            case DIR_DOWN:
                currentFrame = texAranhaRight[e->frame];
                break;
            case DIR_IDLE:
            default:
                currentFrame = texAranhaIdle;
                break;
        }
    }
    
    else if (e->tipo == TIPO_ATIRADOR_BORRACHA) { 
        
        if (e->facingDir == DIR_IDLE) {
            currentFrame = texAtiradorIdle;
        } else {
             switch (e->facingDir) {
                case DIR_LEFT:
                    currentFrame = texAtiradorLeft[e->frame];
                    break;
                case DIR_RIGHT:
                case DIR_UP: 
                case DIR_DOWN:
                    currentFrame = texAtiradorRight[e->frame]; 
                    break;
                default:
                    currentFrame = texAtiradorIdle;
                    break;
            }
        }
    }
    else { currentFrame = texPadraoIdle; }
    
    DrawTextureEx(currentFrame, e->pos, 0.0f, e->escala, e->tint);
    
    float healthPercent = (float)e->vida / (float)e->maxVida;
    if (healthPercent < 1.0f) {
        DrawRectangle(e->pos.x, e->pos.y - 10, e->bounds.width, 5, RED);
        DrawRectangle(e->pos.x, e->pos.y - 10, e->bounds.width * healthPercent, 5, GREEN);
    }
}
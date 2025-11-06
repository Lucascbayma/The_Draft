#include "inimigos.h"
#include "raymath.h"
#include <math.h>
#include <stdio.h>

#define INIMIGO_FRAME_COUNT 8

static Texture2D texPadraoIdle;
static Texture2D texPadraoLeft[INIMIGO_FRAME_COUNT];
static Texture2D texPadraoRight[INIMIGO_FRAME_COUNT];

static Texture2D texTanqueIdle;
static Texture2D texTanqueLeft[INIMIGO_FRAME_COUNT];
static Texture2D texTanqueRight[INIMIGO_FRAME_COUNT];

static Texture2D texAranha;
static Texture2D texAtirador;

void InitInimigoAssets(void) {
    texPadraoIdle = LoadTexture("images/inimigo_base_direita1.png");
    for (int i = 0; i < INIMIGO_FRAME_COUNT; i++) {
        char f[64];
        sprintf(f, "images/inimigo_base_direita%d.png", i+1);
        texPadraoRight[i] = LoadTexture(f);
        sprintf(f, "images/inimigo_base_esquerda%d.png", i+1);
        texPadraoLeft[i] = LoadTexture(f);
    }
    
    texTanqueIdle = LoadTexture("images/inimigo_tank_direita1.png");
    for (int i = 0; i < INIMIGO_FRAME_COUNT; i++) {
        char f[64];
        sprintf(f, "images/inimigo_tank_direita%d.png", i+1);
        texTanqueRight[i] = LoadTexture(f);
        sprintf(f, "images/inimigo_tank_esquerda%d.png", i+1);
        texTanqueLeft[i] = LoadTexture(f);
    }
    
    texAranha = LoadTexture("images/aranha.png");
    texAtirador = LoadTexture("images/atirador.png");
}

void UnloadInimigoAssets(void) {
    UnloadTexture(texPadraoIdle);
    for (int i = 0; i < INIMIGO_FRAME_COUNT; i++) {
        UnloadTexture(texPadraoRight[i]);
        UnloadTexture(texPadraoLeft[i]);
    }
    
    UnloadTexture(texTanqueIdle); 
    for (int i = 0; i < INIMIGO_FRAME_COUNT; i++) {
        UnloadTexture(texTanqueRight[i]);
        UnloadTexture(texTanqueLeft[i]);
    }
    
    UnloadTexture(texAranha);
    UnloadTexture(texAtirador);
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
            e->vida = 3;
            e->maxVida = 3;
            e->dano = 1;
            e->velocidade = 2.0f;
            e->distanciaAtaque = 40.0f;
            e->velAtaque = 1.5f;
            texW = (float)texPadraoIdle.width;
            texH = (float)texPadraoIdle.height;
            break;
            
        case TIPO_TANQUE:
            e->vida = 8;
            e->maxVida = 8;
            e->dano = 2;
            e->velocidade = 1.0f;
            e->distanciaAtaque = 50.0f;
            e->velAtaque = 2.5f;
            texW = (float)texTanqueIdle.width; // <-- MUDANÇA AQUI
            texH = (float)texTanqueIdle.height; // <-- MUDANÇA AQUI
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

void UpdateInimigo(Inimigo *e, Rabisco *r, int mapW, int mapH,
                   int borderTop, int borderBottom,
                   int borderLeft, int borderRight) 
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
    
    Vector2 move = {0, 0};
    
    if (e->tipo == TIPO_PADRAO) {
        if (dist < chaseRadius && dist > e->distanciaAtaque) {
            move = Vector2Normalize(Vector2Subtract(r->pos, e->pos));
            e->pos.x += move.x * e->velocidade;
            e->pos.y += move.y * e->velocidade;
            e->bounds.x = e->pos.x;
            e->bounds.y = e->pos.y;
        } else if (dist < e->distanciaAtaque && e->attackTimer <= 0) {
            r->vida -= e->dano;
            e->attackTimer = e->velAtaque;
        }
    }
    else if (e->tipo == TIPO_TANQUE) {
        if (dist < chaseRadius && dist > e->distanciaAtaque) {
            move = Vector2Normalize(Vector2Subtract(r->pos, e->pos));
            e->pos.x += move.x * e->velocidade;
            e->pos.y += move.y * e->velocidade;
            e->bounds.x = e->pos.x;
            e->bounds.y = e->pos.y;
        } else if (dist < e->distanciaAtaque && e->attackTimer <= 0) {
            r->vida -= e->dano;
            e->attackTimer = e->velAtaque;
        }
    }
    
    if (move.x != 0 || move.y != 0) {
        if (fabs(move.x) > fabs(move.y)) {
            e->facingDir = (move.x > 0) ? DIR_RIGHT : DIR_LEFT;
        } else {
            e->facingDir = (move.y > 0) ? DIR_DOWN : DIR_UP;
        }
        
        e->frameTime += GetFrameTime();
        if (e->frameTime >= e->frameDelay) {
            e->frame = (e->frame + 1) % INIMIGO_FRAME_COUNT;
            e->frameTime = 0.0f;
        }
    } else {
        e->frame = 0;
        e->facingDir = DIR_IDLE;
    }
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
                currentFrame = texPadraoRight[e->frame];
                break;
            case DIR_UP:
            case DIR_DOWN:
            case DIR_IDLE:
            default:
                currentFrame = texPadraoIdle;
                break;
        }
    } 
    // --- MUDANÇA AQUI ---
    else if (e->tipo == TIPO_TANQUE) {
        switch (e->facingDir) {
            case DIR_LEFT:
                currentFrame = texTanqueLeft[e->frame];
                break;
            case DIR_RIGHT:
                currentFrame = texTanqueRight[e->frame];
                break;
            case DIR_UP:
            case DIR_DOWN:
            case DIR_IDLE:
            default:
                currentFrame = texTanqueIdle;
                break;
        }
    }
    // --- FIM DA MUDANÇA ---
    else if (e->tipo == TIPO_ARANHA) { currentFrame = texAranha; }
    else if (e->tipo == TIPO_ATIRADOR_BORRACHA) { currentFrame = texAtirador; }
    else { currentFrame = texPadraoIdle; }
    
    DrawTextureEx(currentFrame, e->pos, 0.0f, e->escala, e->tint);
    
    float healthPercent = (float)e->vida / (float)e->maxVida;
    if (healthPercent < 1.0f) {
        DrawRectangle(e->pos.x, e->pos.y - 10, e->bounds.width, 5, RED);
        DrawRectangle(e->pos.x, e->pos.y - 10, e->bounds.width * healthPercent, 5, GREEN);
    }
}
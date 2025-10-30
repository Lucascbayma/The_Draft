#ifndef RABISCO_H
#define RABISCO_H

#include "raylib.h"

typedef enum { DIR_IDLE, DIR_RIGHT, DIR_LEFT, DIR_UP, DIR_DOWN } PlayerDirection;

typedef struct {
    Vector2 pos;
    float escala; 
    int vida;
    int maxVida;
    int moedas;
    int dano;
    float velocidade;
    float distanciaAtaque;
    float velAtaque;
    
    float width;
    float height;
    PlayerDirection facingDir;
    float attackTimer;
    float attackDurationTimer;
    float attackDuration;

    Texture2D heartFull;
    Texture2D heartBroken;
    Texture2D coinIcon;
    Font hudFont;
    
} Rabisco;

void InitRabisco(Rabisco *r, float x, float y);
bool UpdateRabisco(Rabisco *r, int mapW, int mapH,
    int borderTop, int borderBottom,
    int borderLeft, int borderRight);
void DrawRabisco(Rabisco *r);
void UnloadRabisco(Rabisco *r);

Rectangle GetRabiscoAttackHitbox(Rabisco *r);

#endif
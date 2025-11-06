#ifndef INIMIGOS_H
#define INIMIGOS_H

#include "raylib.h"
#include "rabisco.h" 

typedef enum {
    TIPO_PADRAO,
    TIPO_TANQUE,
    TIPO_ARANHA,
    TIPO_ATIRADOR_BORRACHA
} InimigoType;

typedef struct {
    Vector2 pos;
    float escala;
    int vida;
    int maxVida;
    int dano;
    float velocidade;
    float distanciaAtaque;
    float velAtaque;
    
    bool active;
    Rectangle bounds;
    float attackTimer;
    Color tint;
    
    InimigoType tipo;
    
    PlayerDirection facingDir;
    int frame;
    float frameTime;
    float frameDelay;
    
} Inimigo;

void InitInimigoAssets(void);
void UnloadInimigoAssets(void);
void SpawnInimigo(Inimigo *e, InimigoType tipo, Vector2 pos);
void UpdateInimigo(Inimigo *e, Rabisco *r, int mapW, int mapH, int borderTop, int borderBottom, int borderLeft, int borderRight);
void DrawInimigo(Inimigo *e);
Rectangle GetInimigoHitbox(Inimigo *e);

#endif
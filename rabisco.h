#ifndef RABISCO_H
#define RABISCO_H

#include "raylib.h"

typedef struct {
    Vector2 pos;              // posição do Rabisco na tela
    float escala;             // tamanho do personagem
    int vida;                 // vida atual
    int maxVida;              // vida máxima 
    int moedas;               // moedas coletadas
    int dano;                 // dano causado
    float velocidade;         // velocidade de movimento
    float distanciaAtaque;    // alcance do ataque
    float velAtaque;          // tempo entre ataques
    
    Texture2D heartFull;
    Texture2D heartBroken;
    Texture2D coinIcon;
    Font hudFont;
    
} Rabisco;

void InitRabisco(Rabisco *r, float x, float y);
void UpdateRabisco(Rabisco *r, int mapW, int mapH,
    int borderTop, int borderBottom,
    int borderLeft, int borderRight);
void DrawRabisco(Rabisco *r);
void UnloadRabisco(Rabisco *r);

#endif
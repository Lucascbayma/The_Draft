#ifndef RABISCO_H
#define RABISCO_H

#include "raylib.h"

typedef struct {
    Vector2 pos;              // posição do Rabisco na tela
    float escala;             // tamanho do personagem
    int vida;                 // vida atual
    int dano;                 // dano causado
    float velocidade;         // velocidade de movimento
    float distanciaAtaque;    // alcance do ataque
    float velAtaque;          // tempo entre ataques
} Rabisco;

// Funções principais
void InitRabisco(Rabisco *r, float x, float y);
void UpdateRabisco(Rabisco *r, int screenW, int screenH);
void DrawRabisco(Rabisco *r);
void UnloadRabisco(Rabisco *r);

#endif

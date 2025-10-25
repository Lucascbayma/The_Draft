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
    
    // Texturas para o HUD
    Texture2D heartFull;
    Texture2D heartBroken;
    Texture2D coinIcon;
    
} Rabisco;

// Funções principais
void InitRabisco(Rabisco *r, float x, float y);
void UpdateRabisco(Rabisco *r, int screenW, int screenH);
void DrawRabisco(Rabisco *r);
void UnloadRabisco(Rabisco *r);

#endif
#ifndef INIMIGOS_H
#define INIMIGOS_H

#include "raylib.h"
#include "rabisco.h" 



typedef struct {
    Vector2 pos;
    float escala;
    int vida;
    int dano;
    float velocidade;
    float distanciaAtaque;
    float velAtaque;
    
    bool active;
    Rectangle bounds;
    float attackTimer;
    Color tint;
} Negro;

typedef struct {
    Vector2 pos;
    float escala;
    int vida;
    int dano;
    float velocidade;
    float distanciaAtaque;
    float velAtaque;
    
    
    bool active;
    Rectangle bounds;
    float attackTimer;
    Color tint;
} Gordo;



#endif 
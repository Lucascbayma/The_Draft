#ifndef INIMIGOS_H
#define INIMIGOS_H

#include "raylib.h"
#include "rabisco.h" 



typedef struct {
    // --- Campos Base ---
    Vector2 pos;
    float escala;
    int vida;
    int dano;
    float velocidade;
    float distanciaAtaque;
    float velAtaque;
    
    // --- Campos de "Estado" ---
    bool active;
    Rectangle bounds;
    float attackTimer;
    Color tint;
} Negro;

typedef struct {
    // --- Campos Base ---
    Vector2 pos;
    float escala;
    int vida;
    int dano;
    float velocidade;
    float distanciaAtaque;
    float velAtaque;
    
    // --- Campos de "Estado" ---
    bool active;
    Rectangle bounds;
    float attackTimer;
    Color tint;
} Gordo;

typedef struct {
    // --- Campos Base ---
    Vector2 pos;
    float escala;            
    int vida;                 
    int dano;                 
    float velocidade;         
    float distanciaAtaque;    
    float velAtaque;          

    // --- Campos de "Estado" ---
    bool active;
    Rectangle bounds;         
    float attackTimer;
    Color tint;

} Aranha;

typedef struct {
    // --- Campos Base ---
    Vector2 pos;
    float escala;
    int vida;
    int dano;                 
    float velocidade;         
    float distanciaAtaque;    
    float velAtaque;          

    // --- Campos de "Estado" ---
    bool active;
    Rectangle bounds;
    float attackTimer;        
    Color tint;

} AtiradorBorracha;

#endif 
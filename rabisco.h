#ifndef RABISCO_H
#define RABISCO_H

#include "raylib.h"
#include "itens.h" 

typedef enum { DIR_RIGHT = 0, DIR_LEFT, DIR_UP, DIR_DOWN, DIR_IDLE } PlayerDirection;


typedef struct InventarioNode {
    ItemType tipo;
    struct InventarioNode *next; 
} InventarioNode;

typedef struct {
    Vector2 pos;
    float escala;
    
    float hitboxWidth;
    float hitboxHeight;
    float hitboxOffsetX;
    float hitboxOffsetY;
    
    int maxHeartContainers; 
    int currentHitPoints; 
    int dano;
    float velocidade;
    
    int baseMaxHeartContainers;
    int baseDano;
    float baseVelocidade;
    
    int moedas;
    float alcance; 
    float distanciaAtaque;
    float velAtaque;
    float width;
    float height;
    PlayerDirection facingDir;
    float attackTimer;
    float attackDuration;
    float attackDurationTimer;
    float attackOffsetY;
    float attackOffsetX;
    
    InventarioNode *inventarioHead; 
    
    Texture2D heartFull;
    Texture2D heartBroken;
    Texture2D hollowHeart; 
    Texture2D coinIcon;
    Font hudFont;
    Texture2D iconVel;
    Texture2D iconDamage;
    Texture2D iconRange;
    
} Rabisco;

void InitRabisco(Rabisco *r, float x, float y);
bool UpdateRabisco(Rabisco *r, int mapW, int mapH, int borderTop, int borderBottom, int borderLeft, int borderRight, Rectangle pedestals[4]); 
void DrawRabisco(Rabisco *r);
void UnloadRabisco(Rabisco *r);
Rectangle GetRabiscoHitbox(Rabisco *r);
Rectangle GetRabiscoAttackHitbox(Rabisco *r);
void AddItemAoInventario(Rabisco *r, ItemType tipo);
void LimparInventario(Rabisco *r);
void AplicarStatsInventario(Rabisco *r);

#endif
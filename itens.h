#ifndef ITENS_H
#define ITENS_H

#include "raylib.h"
typedef enum {
    ITEM_NULO = 0,
    
    // Itens de Ataque
    ITEM_LAPIS_PARTIDO,
    ITEM_APONTADOR,
    ITEM_ESTILETE,
    
    // Itens de Velocidade
    ITEM_COLA,
    ITEM_BOTAS_DE_PAPEL,
    ITEM_PENGOO,
    
    // Itens de Vida
    ITEM_CORACAO_VAZIO,
    ITEM_GRAMPEADOR,
    
    // Itens de Vida
    ITEM_CORACAO_PARTIDO,
    ITEM_CORACAO,
    ITEM_CORACAO_DUPLO
    
} ItemType;


typedef struct {
    Vector2 pos;
    bool active;
    Rectangle bounds; 
    ItemType tipo;
    Color cor; 
} ItemChao;


#endif
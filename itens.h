#ifndef ITENS_H
#define ITENS_H

#include "raylib.h"
typedef enum {
    ITEM_NULO = 0,
    
    ITEM_LAPIS_PARTIDO,
    ITEM_APONTADOR,
    ITEM_ESTILETE,
    
    ITEM_COLA,
    ITEM_BOTAS_DE_PAPEL,
    ITEM_PENGOO,
    
    ITEM_CORACAO_VAZIO,
    ITEM_GRAMPEADOR,
    
    ITEM_CORACAO_PARTIDO,
    ITEM_CORACAO,
    ITEM_CORACAO_DUPLO
    
} ItemType;

void InitItensAssets(void);
void UnloadItensAssets(void);
Texture2D GetItemIconTexture(ItemType tipo); 
int GetItemPrice(ItemType tipo);

#endif
#include "itens.h"

static Texture2D texColaIcon, texBotaIcon, texPengooIcon;
static Texture2D texCorVazioIcon, texGrampeadorIcon;
static Texture2D texCorPartidoIcon, texCorIcon, texCorDuploIcon;

void InitItensAssets(void) {
    texColaIcon = LoadTexture("images/item_cola.png");
    texBotaIcon = LoadTexture("images/item_bota_de_papel.png");
    texPengooIcon = LoadTexture("images/item_pengoo.png");
    texCorVazioIcon = LoadTexture("images/item_hollow-heart.png");
    texGrampeadorIcon = LoadTexture("images/item_grampeador.png");
    texCorPartidoIcon = LoadTexture("images/item_half-heart.png");
    texCorIcon = LoadTexture("images/item_heart.png");
    texCorDuploIcon = LoadTexture("images/item_double-heart.png");
}

void UnloadItensAssets(void) {
    UnloadTexture(texColaIcon); UnloadTexture(texBotaIcon); UnloadTexture(texPengooIcon);
    UnloadTexture(texCorVazioIcon); UnloadTexture(texGrampeadorIcon);
    UnloadTexture(texCorPartidoIcon); UnloadTexture(texCorIcon); UnloadTexture(texCorDuploIcon);
}

Texture2D GetItemIconTexture(ItemType tipo) {
    switch (tipo) {

        case ITEM_COLA: return texColaIcon;
        case ITEM_BOTAS_DE_PAPEL: return texBotaIcon;
        case ITEM_PENGOO: return texPengooIcon;
        case ITEM_CORACAO_VAZIO: return texCorVazioIcon;
        case ITEM_GRAMPEADOR: return texGrampeadorIcon;
        case ITEM_CORACAO_PARTIDO: return texCorPartidoIcon;
        case ITEM_CORACAO: return texCorIcon;
        case ITEM_CORACAO_DUPLO: return texCorDuploIcon;
        default: return (Texture2D){0};
    }
}

int GetItemPrice(ItemType tipo) {
    switch (tipo) {
        case ITEM_CORACAO_PARTIDO: return 7;
        case ITEM_CORACAO: return 7;
        case ITEM_CORACAO_DUPLO: return 7;
        
        case ITEM_COLA: return 15;          
        
        case ITEM_BOTAS_DE_PAPEL: return 15;
        case ITEM_CORACAO_VAZIO: return 15;
        
        
        case ITEM_PENGOO: return 15;
        case ITEM_GRAMPEADOR: return 15;
        
        default: return 0;
    }
}
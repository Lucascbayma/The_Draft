#ifndef GAMELOOP_H
#define GAMELOOP_H

#include "raylib.h"
#include "rabisco.h"
#include "inimigos.h"
#include "itens.h"

#define MAX_INIMIGOS 6
#define MAX_PROJETEIS 150
#define MAX_HIGH_SCORES 5
#define NUM_PEDESTAIS 4


typedef struct {
    Vector2 pos;
    Vector2 velocity; 
    float speed;
    int dano;
    bool active;
    Rectangle bounds;
    float escala; 
} Projetil;

typedef struct {
    Rectangle bounds;
    bool isPressed;
    float escala;
} Button;

typedef struct {
    char initials[4];
    int score;
} ScoreEntry;

typedef struct {
    Vector2 pos;
    Rectangle hitRect;      
    Rectangle triggerRect;  
    bool active;            
    ItemType tipo;
    int preco;
    float floatTimer;  
    bool waitingForRestock; 
    float restockTimer;     
} Pedestal;


extern Projetil projeteis[MAX_PROJETEIS];
extern Texture2D texProjetilBorracha; 
extern int ondaAtual;
extern int subOnda; 
extern float chanceBonusOnda;
extern ScoreEntry highScores[MAX_HIGH_SCORES];
extern int finalScore; 
extern int currentScore;
extern Pedestal pedestais[NUM_PEDESTAIS];
extern Texture2D texPedestalBase; 

// Projectile 
void SpawnProjetilAtirador(Vector2 startPos, Vector2 direction);
void UpdateProjeteis(Rabisco *r, int mapW, int mapH, int borderTop, int borderBottom, int borderLeft, int borderRight);
void DrawProjeteis(void);

// Pedestal/Shop 
void ResolveStaticCollision(Rabisco *r, Rectangle obstacleBounds);
void SetupPedestais(int mapW, int mapBorderTop);
void RestockPedestais(void);
void UpdatePedestais(Rabisco *r);
void DrawPedestais(Font font);

// Score 
void LoadHighScores(ScoreEntry scores[]);
bool IsHighScore(ScoreEntry scores[], int newScore);
void AddNewScore(ScoreEntry scores[], char newInitials[], int newScore);

// Item Sorteio e Stats
ItemType GetRandomVidaConsumivelItem(void);
ItemType GetRandomUpgradeItem(void);
void ShuffleTipos(ItemType array[], int n);

// Onda/Game 
void ResetJogo(Rabisco *rabisco, Inimigo inimigos[], int maxInimigos, Texture2D mapa);
Vector2 GetRandomSpawnPosition(Rabisco *r, int mapW, int mapH, int bTop, int bBot, int bLeft, int bRight);
void SpawnWave(int onda, Inimigo inimigos[], Rabisco *r, int mapW, int mapH, int bTop, int bBot, int bLeft, int bRight);


#endif
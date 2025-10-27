#include "rabisco.h"
#include "raymath.h"
#include <math.h>
#include <stdio.h>

#define FRAME_RIGHT 8
#define FRAME_LEFT 8
#define FRAME_UP 9

static Texture2D idle;
static Texture2D walkRight[FRAME_RIGHT];
static Texture2D walkLeft[FRAME_LEFT];
static Texture2D walkUp[FRAME_UP];

static int frame = 0;
static float frameTime = 0;
static float frameDelay = 1.0f / 8.0f;

typedef enum { DIR_IDLE, DIR_RIGHT, DIR_LEFT, DIR_UP, DIR_DOWN } PlayerDirection;
static PlayerDirection lastDir = DIR_IDLE;

void InitRabisco(Rabisco *r, float x, float y) {
    r->pos = (Vector2){x, y};
    r->escala = 0.10f; 
    
    r->maxVida = 4;
    r->vida = r->maxVida;
    r->moedas = 0;
    
    r->dano = 2;
    r->velocidade = 4.0f;
    r->distanciaAtaque = 40.0f;
    r->velAtaque = 0.8f;

    idle = LoadTexture("images/idle.png");

    for (int i = 0; i < FRAME_RIGHT; i++) {
        char filename[64];
        sprintf(filename, "images/walk_right%d.png", i + 1);
        walkRight[i] = LoadTexture(filename);
    }
    for (int i = 0; i < FRAME_LEFT; i++) {
        char filename[64];
        sprintf(filename, "images/walk_left%d.png", i + 1);
        walkLeft[i] = LoadTexture(filename);
    }
    for (int i = 0; i < FRAME_UP; i++) {
        char filename[64];
        sprintf(filename, "images/walk_up%d.png", i + 1);
        walkUp[i] = LoadTexture(filename);
    }

    r->heartFull = LoadTexture("images/heart.png");
    r->heartBroken = LoadTexture("images/heart_broken.png");
    r->coinIcon = LoadTexture("images/moeda.png");
    r->hudFont = LoadFontEx("assets/PatrickHandSC-Regular.ttf", 40, 0, 0); 
}

void UpdateRabisco(Rabisco *r, int mapW, int mapH, int mapBorderOffsetX, int mapBorderOffsetY) { 
    Vector2 move = {0, 0};

    if (IsKeyDown(KEY_RIGHT)) move.x += 1;
    if (IsKeyDown(KEY_LEFT))  move.x -= 1;
    if (IsKeyDown(KEY_UP))    move.y -= 1;
    if (IsKeyDown(KEY_DOWN))  move.y += 1;

    float len = sqrtf(move.x * move.x + move.y * move.y);
    bool isMoving = (len > 0);

    if (isMoving) {
        move.x /= len;
        move.y /= len;
    }
    
    float dx = move.x * r->velocidade;
    float dy = move.y * r->velocidade;
    
    // Dimensões do personagem escalado
    float rabiscoW = walkRight[0].width * r->escala;
    float rabiscoH = walkRight[0].height * r->escala;
    
    // Limites MÍNIMOS Superior e Esquerdo
    float minX = (float)mapBorderOffsetX;
    float minY = (float)mapBorderOffsetY;

    // Limites MÁXIMOS Direito e Inferior
    float maxX = (float)mapW - (float)mapBorderOffsetX - rabiscoW; 
    float maxY = (float)mapH - (float)mapBorderOffsetY - rabiscoH;
    
    float nextX = r->pos.x + dx;
    float nextY = r->pos.y + dy;

    r->pos.x = Clamp(nextX, minX, maxX);
    r->pos.y = Clamp(nextY, minY, maxY);


    // Direção
    if(isMoving){
        if(fabs(move.x) > fabs(move.y)){
            lastDir = (move.x > 0) ? DIR_RIGHT : DIR_LEFT;
        }else{
            lastDir = (move.y < 0) ? DIR_UP : DIR_DOWN;
        }
    }else{
        lastDir = DIR_IDLE;
    }

    frameTime += GetFrameTime();
    if(frameTime >= frameDelay){
        frame++;
        frameTime = 0;
    }

    if(lastDir == DIR_UP)
        frame %= FRAME_UP;
    else
        frame %= FRAME_RIGHT;
}

void DrawRabisco(Rabisco *r){
    Texture2D currentFrame = idle;

    if(lastDir == DIR_UP)
        currentFrame = walkUp[frame];
    else if(lastDir == DIR_RIGHT)
        currentFrame = walkRight[frame];
    else if(lastDir == DIR_LEFT)
        currentFrame = walkLeft[frame];
    else if(lastDir == DIR_DOWN)
        currentFrame = idle;
    else
        currentFrame = idle;

    DrawTextureEx(currentFrame, r->pos, 0.0f, r->escala, WHITE);
}

void UnloadRabisco(Rabisco *r){
    UnloadTexture(idle);
    for(int i = 0; i < FRAME_RIGHT; i++) UnloadTexture(walkRight[i]);
    for(int i = 0; i < FRAME_LEFT; i++)  UnloadTexture(walkLeft[i]);
    for(int i = 0; i < FRAME_UP; i++)    UnloadTexture(walkUp[i]);

    UnloadTexture(r->heartFull);
    UnloadTexture(r->heartBroken);
    UnloadTexture(r->coinIcon);
    UnloadFont(r->hudFont);
}
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

static PlayerDirection lastDir = DIR_IDLE;

void InitRabisco(Rabisco *r, float x, float y) {
    r->pos = (Vector2){x, y};
    r->escala = 0.10f; 
    
    r->maxVida = 4;
    r->vida = r->maxVida;
    r->moedas = 0;
    
    r->dano = 2;
    r->velocidade = 4.0f;
    r->distanciaAtaque = 30.0f;
    r->velAtaque = 0.8f;

    idle = LoadTexture("images/idle.png");

    r->width = idle.width * r->escala;
    r->height = idle.height * r->escala;
    r->facingDir = DIR_DOWN;
    r->attackTimer = 0.0f;
    r->attackDuration = 0.25f;
    r->attackDurationTimer = 0.0f;

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

bool UpdateRabisco(Rabisco *r, int mapW, int mapH, 
    int borderTop, int borderBottom, 
    int borderLeft, int borderRight) 
{ 
    Vector2 move = {0, 0};
    bool attackJustStarted = false;

    if (r->attackTimer > 0) r->attackTimer -= GetFrameTime();
    if (r->attackDurationTimer > 0) r->attackDurationTimer -= GetFrameTime();

    if (IsKeyPressed(KEY_SPACE) && r->attackTimer <= 0) {
        r->attackTimer = r->velAtaque;
        r->attackDurationTimer = r->attackDuration;
        attackJustStarted = true;
    }

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

    float minX = (float)borderLeft;
    float maxX = (float)mapW - (float)borderRight - r->width;
    float minY = (float)borderTop;
    float maxY = (float)mapH - (float)borderBottom - r->height;

    float nextX = r->pos.x + dx;
    float nextY = r->pos.y + dy;

    r->pos.x = Clamp(nextX, minX, maxX);
    r->pos.y = Clamp(nextY, minY, maxY);

    if(isMoving){
        if(fabs(move.x) > fabs(move.y)){
            lastDir = (move.x > 0) ? DIR_RIGHT : DIR_LEFT;
        }else{
            lastDir = (move.y < 0) ? DIR_UP : DIR_DOWN;
        }
        r->facingDir = lastDir;
    }else{
        lastDir = DIR_IDLE;
    }

    frameTime += GetFrameTime();
    if(frameTime >= frameDelay){
        frame++;
        frameTime = 0;
    }

    if (lastDir == DIR_UP) {
        frame %= FRAME_UP;
    } else if (lastDir == DIR_LEFT) {
        frame %= FRAME_LEFT;
    } else if (lastDir == DIR_RIGHT) {
        frame %= FRAME_RIGHT;
    } else {
        frame = 0;
    }
    
    return attackJustStarted;
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

    if (r->attackDurationTimer > 0) {
        Rectangle attackBox = GetRabiscoAttackHitbox(r);
        DrawRectangleRec(attackBox, (Color){255, 255, 0, 100}); 
    }
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

Rectangle GetRabiscoAttackHitbox(Rabisco *r) {
    Rectangle hitbox;
    
    float range = r->distanciaAtaque; 
    
    float swingWidth = r->width * 0.3f;   
    float swingHeight = r->height * 0.3f; 

    switch (r->facingDir) {
        
        case DIR_RIGHT:
            hitbox = (Rectangle){ 
                r->pos.x + r->width, 
                r->pos.y - (swingHeight - r->height) / 2,
                range/0.8, 
                swingHeight 
            };
            break;
            
        
        case DIR_LEFT:
            hitbox = (Rectangle){ 
                r->pos.x - range, 
                r->pos.y - (swingHeight - r->height) / 2,
                range/0.8,
                swingHeight 
            };
            break;
            
        case DIR_UP:
            hitbox = (Rectangle){ 
                r->pos.x - (swingWidth - r->width) / 2,
                r->pos.y - range, 
                swingWidth, 
                range
            };
            break;
            
        case DIR_DOWN:
        case DIR_IDLE:
        default:
            hitbox = (Rectangle){ 
                r->pos.x - (swingWidth - r->width) / 2,
                r->pos.y + r->height, 
                swingWidth, 
                range
            };
            break;
    }
    return hitbox;
}
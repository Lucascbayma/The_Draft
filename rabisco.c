#include "rabisco.h"
#include "raymath.h"
#include <math.h>
#include <stdio.h>

#define FRAME_RIGHT 8
#define FRAME_LEFT 8
#define FRAME_UP 9
#define FRAME_ATTACK 8

static Texture2D idle;
static Texture2D walkRight[FRAME_RIGHT];
static Texture2D walkLeft[FRAME_LEFT];
static Texture2D walkUp[FRAME_UP];
static Texture2D attackFrames[FRAME_ATTACK];

static int frame = 0;
static float frameTime = 0;
static float frameDelay = 1.0f / 8.0f;

static int attackFrame = 0;
static float attackFrameTime = 0;
static float attackFrameDelay = 1.0f / 15.0f;

static PlayerDirection lastDir = DIR_IDLE;

void InitRabisco(Rabisco *r, float x, float y) {
    r->pos = (Vector2){x, y};
    r->escala = 0.10f; 
    
    // --- SISTEMA DE VIDA ATUALIZADO ---
    r->maxHeartContainers = 3; // Total de "recipientes"
    r->currentHitPoints = r->maxHeartContainers * 2; 
    
    r->moedas = 0;
    
    r->dano = 2;
    r->velocidade = 4.0f;
    r->distanciaAtaque = 30.0f;
    r->velAtaque = 0.8f;
    r->alcance = 1.5f;

    idle = LoadTexture("images/idle.png");
    float heightFactor = 0.8f;
    r->width = idle.width * r->escala;
    r->height = (idle.height * r->escala) * heightFactor;
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

    for (int i = 0; i < FRAME_ATTACK; i++) {
        char filename[64];
        sprintf(filename, "images/ataque%d_up.png", i + 1);
        attackFrames[i] = LoadTexture(filename);
    }

    //  TEXTURAS DA HUD 
    r->heartFull = LoadTexture("images/heart.png");
    r->heartBroken = LoadTexture("images/heart_broken.png");
    r->hollowHeart = LoadTexture("images/hollow_heart.png"); 
    r->coinIcon = LoadTexture("images/moeda.png");
    r->iconDamage = LoadTexture("images/hud_dano.png");
    r->iconVel = LoadTexture("images/hud_velocidade.png");
    r->iconRange = LoadTexture("images/hud_alcance.png"); 
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

    int attackKey = 0;
    if (IsKeyPressed(KEY_UP)) attackKey = KEY_UP;
    else if (IsKeyPressed(KEY_DOWN)) attackKey = KEY_DOWN;
    else if (IsKeyPressed(KEY_LEFT)) attackKey = KEY_LEFT;
    else if (IsKeyPressed(KEY_RIGHT)) attackKey = KEY_RIGHT;

    if (r->attackTimer <= 0 && attackKey != 0) 
    {
        r->attackTimer = r->velAtaque;
        r->attackDurationTimer = (float)FRAME_ATTACK * attackFrameDelay;
        attackJustStarted = true;
        attackFrame = 0;

        switch (attackKey) {
            case KEY_UP: r->facingDir = DIR_UP; break;
            case KEY_DOWN: r->facingDir = DIR_DOWN; break;
            case KEY_LEFT: r->facingDir = DIR_LEFT; break;
            case KEY_RIGHT: r->facingDir = DIR_RIGHT; break;
            default: break; 
        }
    }

    if (IsKeyDown(KEY_D)) move.x += 1;
    if (IsKeyDown(KEY_A))  move.x -= 1;
    if (IsKeyDown(KEY_W))  move.y -= 1;
    if (IsKeyDown(KEY_S))  move.y += 1;

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
        
        if (r->attackDurationTimer <= 0) { 
             r->facingDir = lastDir;
        }
    } else if (r->attackDurationTimer <= 0){
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

    if (r->attackDurationTimer > 0) {
        attackFrameTime += GetFrameTime();
        if (attackFrameTime >= attackFrameDelay) {
            if (attackFrame < FRAME_ATTACK - 1) {
                attackFrame++;
            }
            attackFrameTime = 0;
        }
    } else {
        attackFrame = 0;
    }
    
    return attackJustStarted;
}

void DrawRabisco(Rabisco *r){
    Texture2D currentFrame = idle;
    bool isMoving = (lastDir != DIR_IDLE);
    
    if (isMoving) {
        switch (r->facingDir) {
            case DIR_UP:    currentFrame = walkUp[frame % FRAME_UP]; break;
            case DIR_LEFT:  currentFrame = walkLeft[frame % FRAME_LEFT]; break;
            case DIR_RIGHT: currentFrame = walkRight[frame % FRAME_RIGHT]; break;
            case DIR_DOWN:
            case DIR_IDLE:
            default:        currentFrame = idle;
        }
    } else {
        switch (r->facingDir) {
            case DIR_UP:    currentFrame = walkUp[0]; break;
            case DIR_LEFT:  currentFrame = walkLeft[0]; break;
            case DIR_RIGHT: currentFrame = walkRight[0]; break;
            case DIR_DOWN:
            case DIR_IDLE:
            default:        currentFrame = idle;
        }
    }
    
    
    Rectangle srcRectRabisco = { 0, 0, (float)currentFrame.width, (float)currentFrame.height };
    Rectangle destRectRabisco = { r->pos.x, r->pos.y, r->width, r->height };
    Vector2 originRabisco = { 0, 0 };
    DrawTexturePro(currentFrame, srcRectRabisco, destRectRabisco, originRabisco, 0.0f, WHITE);


  
    if (r->attackDurationTimer > 0) {
        Texture2D atk = attackFrames[attackFrame];
        Vector2 pos = r->pos;
        
        float heightFactor = r->height / (idle.height * r->escala); 
        float atkScaledW = atk.width * r->escala;
        float atkScaledH = (atk.height * r->escala) * heightFactor; 

        float offsetX = 5;
        float offsetY = 5;

        float rotation = 0.0f;
        bool flip = false;

        switch (r->facingDir) {
            case DIR_UP:
                rotation = 0.0f;
                flip = false;
                offsetX = (r->width / 2) - (atkScaledW / 2); 
                offsetY = -atkScaledH+20.0f; 
                break;
            case DIR_DOWN:
                rotation = 180.0f;
                flip = false;
                offsetX = (r->width / 2) + (atkScaledW / 2); 
                offsetY = r->height + atkScaledH-20.0f; 
                break;
            case DIR_LEFT:
                rotation = -90.0f;
                flip = false;
                offsetX = -(atkScaledH / 2) + (r->width / 2)-60.0f;
                offsetY = (r->height / 2) + (atkScaledW / 2)-20.0f; 
                break;
            case DIR_RIGHT:
            default:
                rotation = 90.0f;
                flip = true;
                offsetX = (r->width / 2) + (atkScaledH / 2)+60.0f;
                offsetY = (r->height / 2) - (atkScaledW / 2)+20.0f; 
                break;
        }
        
        if (r->facingDir == DIR_LEFT || r->facingDir == DIR_RIGHT) {
            float temp = atkScaledW;
            atkScaledW = atkScaledH;
            atkScaledH = temp;
        }

        Rectangle src = { 0, 0, atk.width * (flip ? -1 : 1), atk.height };
        Rectangle dest = { pos.x + offsetX, pos.y + offsetY, atkScaledW, atkScaledH };
        Vector2 origin = { 0, 0 };

        DrawTexturePro(atk, src, dest, origin, rotation, WHITE);
    }
}

void UnloadRabisco(Rabisco *r){
    UnloadTexture(idle);
    for(int i = 0; i < FRAME_RIGHT; i++) UnloadTexture(walkRight[i]);
    for(int i = 0; i < FRAME_LEFT; i++)  UnloadTexture(walkLeft[i]);
    for(int i = 0; i < FRAME_UP; i++)    UnloadTexture(walkUp[i]);
    for(int i = 0; i < FRAME_ATTACK; i++) UnloadTexture(attackFrames[i]);


    UnloadTexture(r->heartFull);
    UnloadTexture(r->heartBroken);
    UnloadTexture(r->hollowHeart); 
    UnloadTexture(r->coinIcon);
    UnloadTexture(r->iconDamage);
    UnloadTexture(r->iconVel);
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
                range,
                swingHeight 
            };
            break;
        case DIR_LEFT:
            hitbox = (Rectangle){ 
                r->pos.x - range, 
                r->pos.y - (swingHeight - r->height) / 2,
                range,
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
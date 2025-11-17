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
    r->maxHeartContainers = 3; 
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
    r->hitboxWidth = 25.0f;
    r->hitboxHeight = 35.0f;
    r->hitboxOffsetX = (r->width - r->hitboxWidth) / 2.0f;
    r->hitboxOffsetY = (r->height - r->hitboxHeight) / 2.0f;
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
    int gamepad = 0; // O primeiro controle conectado

    if (r->attackTimer > 0) r->attackTimer -= GetFrameTime();
    if (r->attackDurationTimer > 0) r->attackDurationTimer -= GetFrameTime();

    // --- 1. LÓGICA DE ATAQUE (TECLADO & GAMEPAD) ---
    int attackKey = 0;
    
    // Leitura do Teclado para Ataque (SETA TECLADO)
    if (IsKeyPressed(KEY_UP)) attackKey = KEY_UP;
    else if (IsKeyPressed(KEY_DOWN)) attackKey = KEY_DOWN;
    else if (IsKeyPressed(KEY_LEFT)) attackKey = KEY_LEFT;
    else if (IsKeyPressed(KEY_RIGHT)) attackKey = KEY_RIGHT;

    // Leitura do Gamepad para Ataque (Botões Faciais)
    if (IsGamepadAvailable(gamepad)) {
        if (IsGamepadButtonDown(gamepad, GAMEPAD_BUTTON_RIGHT_FACE_UP)) attackKey = KEY_UP; // Triângulo
        else if (IsGamepadButtonDown(gamepad, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) attackKey = KEY_DOWN; // X
        else if (IsGamepadButtonDown(gamepad, GAMEPAD_BUTTON_RIGHT_FACE_LEFT)) attackKey = KEY_LEFT; // Quadrado
        else if (IsGamepadButtonDown(gamepad, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) attackKey = KEY_RIGHT; // Bola
    }

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

    // --- 2. LÓGICA DE MOVIMENTO (WASD, SETINHA e ANALÓGICO) ---
    
    // Leitura do Teclado (WASD)
    if (IsKeyDown(KEY_D)) move.x += 1;
    if (IsKeyDown(KEY_A))  move.x -= 1;
    if (IsKeyDown(KEY_W))  move.y -= 1;
    if (IsKeyDown(KEY_S))  move.y += 1;
    
    // Leitura para Movimento (Controle)
    if (IsGamepadAvailable(gamepad)) {
        float axisX = GetGamepadAxisMovement(gamepad, GAMEPAD_AXIS_LEFT_X);
        float axisY = GetGamepadAxisMovement(gamepad, GAMEPAD_AXIS_LEFT_Y);

        const float deadzone = 0.2f; 
        
        // Analógico Esquerdo
        if (fabs(axisX) > deadzone) move.x += axisX;
        if (fabs(axisY) > deadzone) move.y += axisY;

        // Setinhas
        if (IsGamepadButtonDown(gamepad, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) move.x += 1;
        if (IsGamepadButtonDown(gamepad, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) move.x -= 1;
        if (IsGamepadButtonDown(gamepad, GAMEPAD_BUTTON_LEFT_FACE_UP)) move.y -= 1;
        if (IsGamepadButtonDown(gamepad, GAMEPAD_BUTTON_LEFT_FACE_DOWN)) move.y += 1;
    }

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

    // --- 3. LÓGICA DE ANIMAÇÃO E DIREÇÃO ---
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

        currentFrame = idle;
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
Rectangle GetRabiscoHitbox(Rabisco *r) {
    return (Rectangle){ 
        r->pos.x + r->hitboxOffsetX, 
        r->pos.y + r->hitboxOffsetY, 
        r->hitboxWidth, 
        r->hitboxHeight 
    };
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
#include "raylib.h"

struct AnimData {
    Rectangle rec;
    Vector2 pos;
    int frame;
    float updateTime;
    float runningTime;
};

bool isOnGround(AnimData data, int windowHeight) {
    return (data.pos.y >= windowHeight - data.rec.height);
}

AnimData updateAnimData(AnimData data, float deltaTime, int maxFrame) {
    // update running time
    data.runningTime += deltaTime;
    if (data.runningTime >= data.updateTime) {
        data.runningTime = 0.0;
        // update animation frame
        data.rec.x = data.frame * data.rec.width;
        data.frame++;
        if (data.frame > maxFrame) {
            data.frame = 0;
        }
    }
    return data;
}

int main() {

    // window dimensions
    const int windowWidth = 512;
    const int windowHeight = 380;
    int windowDimensions[2] = {windowWidth, windowHeight};

    // initialize the window
    InitWindow(windowDimensions[0], windowDimensions[1], "Dapper-Dasher");

    // jumping mechanic
    const int gravity = 1000;
    const int jumpVel = 600; // pixels/s
    bool isInAir = false;


    // Scarfy Data
    Texture2D scarfy = LoadTexture("textures/scarfy.png");
    AnimData scarfyData;
    scarfyData.rec.width = scarfy.width/6;
    scarfyData.rec.height= scarfy.height;
    scarfyData.rec.x = 0;
    scarfyData.rec.y = 0;
    scarfyData.pos.x = windowWidth / 2 - scarfyData.rec.width / 2;
    scarfyData.pos.y = windowHeight - scarfyData.rec.height;
    scarfyData.frame = 0;
    scarfyData.updateTime = 1.0/12.0;
    scarfyData.runningTime = 0.0; 

    // scarfy  animation velocity
    int velocity = 0;

    

    // Nebula Data
    Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png");

    const int sizeOfNebulae = 10;
    AnimData nebulae[sizeOfNebulae] = {};

    for (int i = 0; i < sizeOfNebulae; i++) {
        nebulae[i].rec.x = 0.0;
        nebulae[i].rec.y = 0.0;
        nebulae[i].rec.width = nebula.width / 8;
        nebulae[i].rec.height = nebula.height / 8;
        nebulae[i].pos.x = windowWidth + 300 * i;
        nebulae[i].pos.y = windowHeight - nebula.height / 8;
        nebulae[i].frame = 0;
        nebulae[i].runningTime = 0.0;
        nebulae[i].updateTime = 0.0;
    }

    // Nebula velocity
    int nebVel = -200; // (px/s)

    // Finish Line
    float finishLine = nebulae[sizeOfNebulae - 1].pos.x;

    // Loading background
    Texture2D background = LoadTexture("textures/far-buildings.png");
    float bgX = 0;
    Texture2D midground = LoadTexture("textures/back-buildings.png");
    float mgX = 0;
    Texture2D foreground = LoadTexture("textures/foreground.png");
    float fgX = 0;

    bool collision = false;

    SetTargetFPS(60);
    while(!WindowShouldClose()) {
        // DeltaTime (time since last frame)
        const float dT = GetFrameTime();

        // start drawing
        BeginDrawing();
        ClearBackground(WHITE);
        
        // BACKGROUND SCROLLING
        bgX -= 20 * dT;
        if (bgX <= -background.width * 2) {
            bgX = 0.0;
        }
        mgX -= 40 * dT;
        if (mgX <= -midground.width * 2) {
            mgX = 0.0;
        }
        fgX -= 80 * dT;
        if (fgX <= -foreground.width * 2) {
            fgX = 0.0;
        }


        // draw the background
        Vector2 bg1Pos{bgX, 0.0};
        Vector2 bg2Pos{bgX + background.width * 2, 0.0};
        DrawTextureEx(background, bg1Pos, 0.0, 2.0, WHITE);
        DrawTextureEx(background, bg2Pos, 0.0, 2.0, WHITE);

        Vector2 mg1Pos{mgX, 0.0};
        Vector2 mg2Pos{mgX + midground.width * 2, 0.0};
        DrawTextureEx(midground, mg1Pos, 0.0, 2.0, WHITE);
        DrawTextureEx(midground, mg2Pos, 0.0, 2.0, WHITE);

        Vector2 fg1Pos{fgX, 0.0};
        Vector2 fg2Pos{fgX + foreground.width * 2, 0.0};
        DrawTextureEx(foreground, fg1Pos, 0.0, 2.0, WHITE);
        DrawTextureEx(foreground, fg2Pos, 0.0, 2.0, WHITE);

        if (isOnGround(scarfyData, windowHeight)) {
            // scarfy is on the ground
            velocity = 0;
            isInAir = false;
        }
        else {
            // rectangle is on the air
            // apply gravity
            velocity += gravity * dT;
            isInAir = true;
        }
        
        if ((IsKeyPressed(KEY_SPACE)) && (!isInAir)) {
            // check for jumping
            velocity -= jumpVel;
        }

        for (int i = 0; i < sizeOfNebulae; i++) {
            // update nebulae positions
            nebulae[i].pos.x += nebVel * dT;
        }

        // update scarfy position
        scarfyData.pos.y += velocity * dT;

        // update finish line position
        finishLine += nebVel * dT;

        // scarfy animation pattern
        if (!isInAir) {
            scarfyData = updateAnimData(scarfyData, dT, 5);
        }
        //nebulae animation pattern
        for (int i = 0; i < sizeOfNebulae; i++) { 
            nebulae[i] = updateAnimData(nebulae[i], dT, 7);
        }

        
        for (AnimData nebula : nebulae) {
            float pad = 50;
            Rectangle nebRec{
                nebula.pos.x + pad, 
                nebula.pos.y + pad,
                nebula.rec.width - 2 * pad,
                nebula.rec.height - 2 * pad
            };
            Rectangle scarfyRec {
                scarfyData.pos.x,
                scarfyData.pos.y,
                scarfyData.rec.width,
                scarfyData.rec.height,
            };
            if (CheckCollisionRecs(nebRec, scarfyRec)) {
                collision = true;
            }
        }
        if (collision) {
        // LOSE THE GAME
        DrawText("GAME OVER!", windowDimensions[0]/2 - 100, windowDimensions[1]/2, 32, WHITE);
        }
        else if (scarfyData.pos.x >= finishLine) {
            DrawText("YOU WIN!", windowDimensions[0]/2 - 75, windowDimensions[1]/2, 32, WHITE);
        }
        else {
            // draw nebulae
            for (int i = 0; i < sizeOfNebulae; i++) {
                DrawTextureRec(nebula, nebulae[i].rec, nebulae[i].pos, LIGHTGRAY);
            }
            // draw scarfy
            DrawTextureRec(scarfy, scarfyData.rec, scarfyData.pos, WHITE);
        }

        EndDrawing();
        // stop drawing
    }

    UnloadTexture(background);
    UnloadTexture(midground);
    UnloadTexture(foreground);
    UnloadTexture(scarfy);
    UnloadTexture(nebula);
    CloseWindow();
    return 0;
}
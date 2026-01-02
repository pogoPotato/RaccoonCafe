#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h> // ttf font make sure its sdl3
#include "Customer.h"
#include "Coin.h"
#include <string>


enum class GameState { SPLASH, PLAYING };// class for splash screen

struct Game
{
    // 1. CAPACITY & UPGRADES
    int currentMaxSlots;           // level 1 = 1 slot/customer, Level 2 = 2 slots type shi
    static const int MAX_LIMIT = 5; //max slots may be 5??

    //max limit ?? to store
    Customer customers[MAX_LIMIT];
    bool menuSlots[MAX_LIMIT];
    bool slotPreparing[MAX_LIMIT];
    float slotTimers[MAX_LIMIT];
    int slotTargetIndex[MAX_LIMIT];

    // paisai paisa with saman
    int coinsCollected;
    int stock;                 // current food supply
    int upgradeCost;           // cost to level up store
    int stockCost;             // cost for +15? 20 ?  food
    bool isShopOpen;           // toggle for the upgrade menu

    // 4. STORE VISUALS & UI
    SDL_FRect store;
    SDL_Texture* backgroundTexture;  //bg texture
    SDL_Texture* storeTexture;
    TTF_Font* font;            //font shit

    // spawn and shit
    float spawnTimer;
    float nextSpawnTime;
    float gameTime;

    //coin update
    Coin activeCoins[MAX_LIMIT];

    //falt shit for UI
    SDL_FRect shopButton;
    SDL_FRect restockButton;
    SDL_FRect upgradeButton;
    SDL_FRect resetButton;  //reset to start // idk? was confused how to build game without storeing satas for fresh game so added this might remove this shit..

    // wave system dead calm ->> this shit wiorks aliek dead so yelpp.. busy and rush--> yeah feels like actual work shit
    float waveTimer;
    float waveTimeRemaining;
    int currentWaveType; // 0=calm, 1=busy, 2=rush, 3=dead
    int customersToSpawn;

    // splash screen
    GameState state = GameState::SPLASH;
    int splashStage = 0; //logo and shit for this game 

    SDL_Texture* studioSplash = nullptr; // logo andd shit 
    SDL_Texture* gameSplash = nullptr;   // same logo and shit

    float splashTimer = 0.0f;
    float splashAlpha = 0.0f;

    //customer texture
    SDL_Texture* customerTexture = nullptr;
};

//improving some UIshit
void drawText(SDL_Renderer* renderer, TTF_Font* font, std::string text, float x, float y, SDL_Color color, bool centered, bool withShadow);

//cycle
void initGame(Game& game, SDL_Renderer* renderer);
void cleanupGame(Game& game);

// input and logic?? nawhh dude
void handleGameClick(Game& game, float mx, float my);
void updateGame(Game& game, float dt);
void spawnCustomer(Game& game, int slot);

//states 
void saveGameState(const Game& game);
void loadGameState(Game& game);

// actual rendering shit
void renderGame(Game& game, SDL_Renderer* renderer);

// image loading yelpp..
SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer);
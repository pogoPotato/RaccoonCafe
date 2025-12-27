#include "Game.h" // game header

#include <SDL3_image/SDL_image.h> // same image shit


//u should know remaining 
#include <iostream>

#include <fstream>

#include <cstdlib>

#include <ctime>

#include <cmath>

#include <algorithm>

#include <string>



// text helper fonts and shit ?

void drawText(SDL_Renderer* renderer, TTF_Font* font, std::string text, float x, float y, SDL_Color color, bool centered, bool withShadow) {

    if (!font || text.empty()) return;



    // for better Ui like shader border? 

    if (withShadow) {

        SDL_Color shadowColor = { 0, 0, 0, 180 };

        SDL_Surface* shadowSurface = TTF_RenderText_Blended(font, text.c_str(), 0, shadowColor);

        if (shadowSurface) {

            SDL_Texture* shadowTexture = SDL_CreateTextureFromSurface(renderer, shadowSurface);

            if (shadowTexture) {

                float shadowX = centered ? x - (shadowSurface->w / 2.0f) + 2 : x + 2;

                SDL_FRect shadowRect = { shadowX, y + 2, (float)shadowSurface->w, (float)shadowSurface->h };

                SDL_RenderTexture(renderer, shadowTexture, nullptr, &shadowRect);

                SDL_DestroyTexture(shadowTexture);

            }

            SDL_DestroySurface(shadowSurface);

        }

    }



    //main text it is drawn after shadow

    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), 0, color);

    if (!surface) return;



    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    if (texture) {

        float finalX = centered ? x - (surface->w / 2.0f) : x;

        SDL_FRect dstRect = { finalX, y, (float)surface->w, (float)surface->h };

        SDL_RenderTexture(renderer, texture, nullptr, &dstRect);

        SDL_DestroyTexture(texture);

    }

    SDL_DestroySurface(surface);

}



//image loader 

SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer) {

    SDL_Texture* texture = IMG_LoadTexture(renderer, path);

    if (!texture) {

        std::cout << "CRITICAL ERROR: Could not load image at: " << path << std::endl;

        std::cout << "SDL_image Error: " << SDL_GetError() << std::endl;

        return nullptr;

    }

    return texture;

}



// game data and state shit coin .. level yeha stored here

void saveGameState(const Game& game) {

    std::ofstream file("save.dat");

    if (file.is_open()) {

        file << game.coinsCollected << " " << game.currentMaxSlots << " " << game.stock;

        file.close();

    }

}



void loadGameState(Game& game) {

    std::ifstream file("save.dat");

    if (file.is_open()) {

        file >> game.coinsCollected >> game.currentMaxSlots >> game.stock;

        file.close();

        // recalculate ur level cost

        game.upgradeCost = (int)(100 * pow(2.5, game.currentMaxSlots - 1));

    }

    else {

        //nothing found sorry man u lost it all need for a fresh start

        game.coinsCollected = 0;

        game.currentMaxSlots = 1;

        game.stock = 15;

        game.upgradeCost = 100;

    }

}



//queu system 

int getServingSlots(const Game& game) {

    int count = 0;

    for (int i = 0; i < Game::MAX_LIMIT; i++) {

        if (game.customers[i].active &&

            (game.customers[i].state == CustomerState::ORDER_TAKEN ||

                game.customers[i].state == CustomerState::ORDER_READY ||

                game.customers[i].state == CustomerState::PAYING)) {

            count++;

        }

    }

    return count;

}


// checkaing if that shit is in queu

bool isInQueue(const Game& game, int index) {

    if (!game.customers[index].active) return false;

    if (game.customers[index].state != CustomerState::WAITING) return false;

    int servingCount = getServingSlots(game);

    return servingCount >= game.currentMaxSlots;

}



// actual gameplay no logic 

void spawnCustomer(Game& game, int slot) {

    float custW = 80.0f;

    float custH = 160.0f;

    float floorY = game.store.y + game.store.h - custH;

    int side = rand() % 2;

    game.customers[slot].rect = { (side == 0) ? -200.0f : 1400.0f, floorY, custW, custH };

    game.customers[slot].direction = (side == 0) ? 1 : -1;

    float centerX = game.store.x + (game.store.w / 2.0f);

    game.customers[slot].targetX = centerX - 40.0f + (slot * 110.0f) - ((Game::MAX_LIMIT - 1) * 55.0f);

    game.customers[slot].state = CustomerState::ENTERING;

    game.customers[slot].speed = 220.0f;

    game.customers[slot].active = true;

    game.customers[slot].patience = 30.0f;

    game.customers[slot].maxPatience = 30.0f;

    game.customers[slot].waitTimer = 0.0f;

}



void initGame(Game& game, SDL_Renderer* renderer) {

    srand((unsigned int)time(NULL));



    // ttf fonts init

    if (TTF_Init() == -1) {

        std::cout << "TTF_Init Error: " << SDL_GetError() << std::endl;

    }

    //loading it with bold format

    game.font = TTF_OpenFont("assets/Orchidea Couture.ttf", 28);

    if (!game.font) {

        std::cout << "TTF_OpenFont Error: " << SDL_GetError() << std::endl;

    }

    else {

        TTF_SetFontStyle(game.font, TTF_STYLE_BOLD);

    }



    game.store = { 400.0f, 180.0f, 480.0f, 420.0f };

    loadGameState(game);

    game.isShopOpen = false;

    game.upgradeCost = 100 * game.currentMaxSlots;

    game.stockCost = 25;

    game.shopButton = { 20, 620, 160, 70 };

    game.restockButton = { 400, 250, 480, 80 };

    game.upgradeButton = { 400, 350, 480, 80 };

    game.resetButton = { 400, 450, 480, 80 };



    const char* bgPath = "assets/background.png";

    game.backgroundTexture = loadTexture(bgPath, renderer);

    if (game.backgroundTexture == nullptr) {
       //if ur path is broken this will let u know
        std::cout << "Failed to load background texture!" << std::endl;
    }



    const char* path = "assets/store.png";

    game.storeTexture = loadTexture(path, renderer);



    for (int i = 0; i < Game::MAX_LIMIT; i++) {

        game.customers[i].active = false;

        game.menuSlots[i] = false;

        game.slotPreparing[i] = false;

        game.activeCoins[i].active = false;

        game.activeCoins[i].value = 0;

        game.slotTimers[i] = 0.0f;

        game.slotTargetIndex[i] = -1;

    }

    game.spawnTimer = 0.0f;

    game.waveTimer = 0.0f;

    game.currentWaveType = 0;

    game.waveTimeRemaining = 8.0f + (rand() % 7);

    game.customersToSpawn = 0;

}


// updating every frame 
void updateGame(Game& game, float dt) {

    if (game.isShopOpen) return;

    game.waveTimer += dt;

    game.waveTimeRemaining -= dt;

    if (game.waveTimeRemaining <= 0) {

        game.currentWaveType = rand() % 4;

        switch (game.currentWaveType) {

        case 0: game.waveTimeRemaining = 10.0f + (rand() % 8); game.customersToSpawn = 2 + (rand() % 2); break;

        case 1: game.waveTimeRemaining = 8.0f + (rand() % 5); game.customersToSpawn = 3 + (rand() % 2); break;

        case 2: game.waveTimeRemaining = 6.0f + (rand() % 4); game.customersToSpawn = 4 + (rand() % 2); break;

        case 3: game.waveTimeRemaining = 5.0f + (rand() % 6); game.customersToSpawn = 0; break;

        }

        game.waveTimer = 0.0f;

    }

    game.spawnTimer += dt;

    int totalCustomers = 0;

    for (int i = 0; i < Game::MAX_LIMIT; i++) { if (game.customers[i].active) totalCustomers++; }

    float spawnInterval;

    switch (game.currentWaveType) {

    case 0: spawnInterval = 4.0f; break;

    case 1: spawnInterval = 2.5f; break;

    case 2: spawnInterval = 1.0f; break;

    case 3: spawnInterval = 999.0f; break;

    }

    if (game.spawnTimer >= spawnInterval && totalCustomers < Game::MAX_LIMIT && game.customersToSpawn > 0) {

        for (int i = 0; i < Game::MAX_LIMIT; i++) {

            if (!game.customers[i].active) {

                spawnCustomer(game, i);

                game.spawnTimer = 0.0f;

                game.customersToSpawn--;

                break;

            }

        }

    }

    for (int i = 0; i < Game::MAX_LIMIT; i++) {

        if (game.customers[i].active && game.customers[i].state == CustomerState::LEAVING) {

            if (game.customers[i].rect.x < -300 || game.customers[i].rect.x > 1500) game.customers[i].active = false;

        }

    }

    for (int i = 0; i < Game::MAX_LIMIT; i++) {

        if (!game.customers[i].active) continue;

        updateCustomer(game.customers[i], dt);

        if (game.customers[i].state == CustomerState::WAITING) {

            game.customers[i].patience -= dt;

            if (game.customers[i].patience <= 0) game.customers[i].state = CustomerState::LEAVING;

        }

        if (game.customers[i].state == CustomerState::ORDER_TAKEN) {

            game.customers[i].patience -= dt * 0.5f;

            if (game.customers[i].patience <= 0) {

                for (int m = 0; m < game.currentMaxSlots; m++) {

                    if (game.slotTargetIndex[m] == i) {

                        game.menuSlots[m] = false; game.slotPreparing[m] = false;

                        game.slotTargetIndex[m] = -1; game.slotTimers[m] = 0.0f;

                    }

                }

                game.customers[i].state = CustomerState::LEAVING;

            }

        }

        if (game.customers[i].state == CustomerState::PAYING) {

            game.customers[i].waitTimer += dt;

            if (game.customers[i].waitTimer >= 0.5f) {  //time gap between coin drop

                // random coin between 1 to 5

                int coinValue = 1 + (rand() % 5);

                game.activeCoins[i].rect = { game.customers[i].rect.x + 20, game.customers[i].rect.y + 120, 45, 45 };

                game.activeCoins[i].active = true;

                game.activeCoins[i].value = coinValue;

                game.customers[i].state = CustomerState::LEAVING;  //pay --> it leaves

            }

        }

    }

    for (int m = 0; m < game.currentMaxSlots; m++) {

        if (game.slotPreparing[m]) {

            game.slotTimers[m] += dt;

            if (game.slotTimers[m] >= 2.5f) {

                game.slotPreparing[m] = false;

                game.slotTimers[m] = 0.0f;

                int targetIndex = game.slotTargetIndex[m];

                if (targetIndex >= 0 && targetIndex < Game::MAX_LIMIT) {

                    if (game.customers[targetIndex].active) game.customers[targetIndex].state = CustomerState::ORDER_READY;

                }

            }

        }

    }

}



void handleGameClick(Game& game, float mx, float my) {
    // shop button
    if (mx >= game.shopButton.x && mx <= game.shopButton.x + game.shopButton.w &&
        my >= game.shopButton.y && my <= game.shopButton.y + game.shopButton.h) {
        game.isShopOpen = !game.isShopOpen;
        return;
    }

    // 2. SHOP MENU LOGIC
    if (game.isShopOpen) {
        if (mx >= game.restockButton.x && mx <= game.restockButton.x + game.restockButton.w &&
            my >= game.restockButton.y && my <= game.restockButton.y + game.restockButton.h) {
            if (game.coinsCollected >= game.stockCost) {
                game.coinsCollected -= game.stockCost;
                game.stock += 15;
                saveGameState(game);
            }
        }
        if (mx >= game.upgradeButton.x && mx <= game.upgradeButton.x + game.upgradeButton.w &&
            my >= game.upgradeButton.y && my <= game.upgradeButton.y + game.upgradeButton.h) {
            if (game.coinsCollected >= game.upgradeCost && game.currentMaxSlots < Game::MAX_LIMIT) {
                game.coinsCollected -= game.upgradeCost;
                game.currentMaxSlots++;
                game.upgradeCost = (int)(100 * pow(2.5, game.currentMaxSlots - 1));
                saveGameState(game);
            }
        }
        if (mx >= game.resetButton.x && mx <= game.resetButton.x + game.resetButton.w &&
            my >= game.resetButton.y && my <= game.resetButton.y + game.resetButton.h) {
            game.coinsCollected = 0;
            game.currentMaxSlots = 1;
            game.stock = 15;
            game.upgradeCost = 100;
            saveGameState(game);
        }
        return;
    }

    //menu slot like the place u receive and prepare order
    for (int m = 0; m < game.currentMaxSlots; m++) {
        float menuY = 200.0f + (m * 120.0f);
        //my window format is fucked so need to render Ui carfully
        if (mx >= 1060 && mx <= 1160 && my >= menuY && my <= menuY + 100) {
            if (game.menuSlots[m] && !game.slotPreparing[m]) {
                game.slotPreparing[m] = true;
                return; //return so we dont trigger click behinf box
            }
        }
    }

    //collect coin and shi
    for (int i = 0; i < Game::MAX_LIMIT; i++) {
        if (game.activeCoins[i].active) {
            if (mx >= game.activeCoins[i].rect.x && mx <= game.activeCoins[i].rect.x + 45 &&
                my >= game.activeCoins[i].rect.y && my <= game.activeCoins[i].rect.y + 45) {
                game.activeCoins[i].active = false;
                game.coinsCollected += game.activeCoins[i].value;
                game.activeCoins[i].value = 0;
                saveGameState(game);
                return;
            }
        }
    }

    //those shit interaction takeen take order
    for (int i = 0; i < Game::MAX_LIMIT; i++) {
        if (!game.customers[i].active) continue;

        if (mx >= game.customers[i].rect.x && mx <= game.customers[i].rect.x + 80 &&
            my >= game.customers[i].rect.y && my <= game.customers[i].rect.y + 160) {

            if (isInQueue(game, i)) return;

            //if taking oerder
            if (game.customers[i].state == CustomerState::WAITING && game.stock > 0) {
                int servingCount = getServingSlots(game);
                if (servingCount >= game.currentMaxSlots) return;

                for (int m = 0; m < game.currentMaxSlots; m++) {
                    if (!game.menuSlots[m] && !game.slotPreparing[m]) {
                        game.customers[i].state = CustomerState::ORDER_TAKEN;
                        game.menuSlots[m] = true;
                        game.slotTargetIndex[m] = i;
                        game.stock--;
                        return;
                    }
                }
            }

            //if order taken
            if (game.customers[i].state == CustomerState::ORDER_READY) {
                game.customers[i].state = CustomerState::PAYING;
                for (int m = 0; m < game.currentMaxSlots; m++) {
                    if (game.slotTargetIndex[m] == i) {
                        game.menuSlots[m] = false;
                        game.slotTargetIndex[m] = -1;
                    }
                }
                return;
            }
        }
    }
}


void renderGame(Game& game, SDL_Renderer* renderer) {

    // clean screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    //background
    if (game.backgroundTexture) {
        //it fit whole screen
        SDL_FRect screenRect = { 0.0f, 0.0f, 1280.0f, 720.0f };
        SDL_RenderTexture(renderer, game.backgroundTexture, nullptr, &screenRect);
    }
    else {
        //if no image loads it will siaplay some black grey gradient
        for (int y = 0; y < 720; y++) {
            int shade = 20 + (y * 15 / 720);
            SDL_SetRenderDrawColor(renderer, shade, shade, shade + 5, 255);
            SDL_FRect line = { 0, (float)y, 1280, 1 };
            SDL_RenderFillRect(renderer, &line);
        }
    }

    //now draw actual stroe above bg image
    if (game.storeTexture) {
        SDL_RenderTexture(renderer, game.storeTexture, nullptr, &game.store);
    }

    else { SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); SDL_RenderFillRect(renderer, &game.store); }



    SDL_Color white = { 255, 255, 255, 255 };

    SDL_Color black = { 0, 0, 0, 255 };

    SDL_Color gold = { 255, 215, 0, 255 };

    SDL_Color brightGold = { 255, 240, 100, 255 };



    for (int i = 0; i < Game::MAX_LIMIT; i++) {

        if (!game.customers[i].active) continue;

        bool inQueue = isInQueue(game, i);

        if (inQueue) {

            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 180);

            SDL_FRect dim = game.customers[i].rect;

            SDL_RenderFillRect(renderer, &dim);

        }

        drawCustomer(renderer, game.customers[i]);

        if (game.customers[i].state == CustomerState::WAITING || game.customers[i].state == CustomerState::ORDER_TAKEN) {

            float pct = game.customers[i].patience / game.customers[i].maxPatience;

            SDL_FRect barBg = { game.customers[i].rect.x, game.customers[i].rect.y - 15, 80, 10 };

            SDL_FRect barFill = { game.customers[i].rect.x, game.customers[i].rect.y - 15, 80 * pct, 10 };

            if (inQueue) SDL_SetRenderDrawColor(renderer, 80, 0, 0, 255);

            else SDL_SetRenderDrawColor(renderer, 150, 0, 0, 255);

            SDL_RenderFillRect(renderer, &barBg);

            if (inQueue) SDL_SetRenderDrawColor(renderer, 255, 100, 0, 255);

            else SDL_SetRenderDrawColor(renderer, 0, 255, 100, 255);

            SDL_RenderFillRect(renderer, &barFill);

        }

        if (inQueue) {

            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

            SDL_FRect queueMark = { game.customers[i].rect.x + 30, game.customers[i].rect.y - 30, 20, 10 };

            SDL_RenderFillRect(renderer, &queueMark);

        }

    }



    //draw coins with number inside

    for (int i = 0; i < Game::MAX_LIMIT; i++) {

        if (game.activeCoins[i].active) {

            //outer glow or shit 

            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

            SDL_SetRenderDrawColor(renderer, 255, 215, 0, 100);

            SDL_FRect glow = { game.activeCoins[i].rect.x - 5, game.activeCoins[i].rect.y - 5, 55, 55 };

            SDL_RenderFillRect(renderer, &glow);



            // main coin

            SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);

            SDL_RenderFillRect(renderer, &game.activeCoins[i].rect);



            //little improvement in visuals 

            SDL_SetRenderDrawColor(renderer, 255, 255, 200, 255);

            SDL_FRect highlight = { game.activeCoins[i].rect.x + 5, game.activeCoins[i].rect.y + 5, 35, 35 };

            SDL_RenderFillRect(renderer, &highlight);



            //coin value like those number we show thme now

            std::string coinText = std::to_string(game.activeCoins[i].value);

            drawText(renderer, game.font, coinText, game.activeCoins[i].rect.x + 22.5f, game.activeCoins[i].rect.y + 10, black, true, false);

        }

    }



    // menu slot

    for (int m = 0; m < game.currentMaxSlots; m++) {

        float menuY = 200.0f + (m * 120.0f);

        SDL_FRect slotRect = { 1060, menuY, 100, 100 };

        SDL_FRect slotBorder = { 1057, menuY - 3, 106, 106 };



        if (game.slotPreparing[m]) {

            SDL_SetRenderDrawColor(renderer, 255, 180, 0, 255);

            SDL_RenderFillRect(renderer, &slotBorder);

            SDL_SetRenderDrawColor(renderer, 255, 140, 0, 255);

            SDL_RenderFillRect(renderer, &slotRect);

            float p = game.slotTimers[m] / 2.5f;

            SDL_FRect cookBar = { 1060, menuY + 88, 100 * p, 12 };

            SDL_SetRenderDrawColor(renderer, 100, 255, 100, 255);

            SDL_RenderFillRect(renderer, &cookBar);

        }

        else if (game.menuSlots[m]) {

            SDL_SetRenderDrawColor(renderer, 0, 220, 255, 255);

            SDL_RenderFillRect(renderer, &slotBorder);

            SDL_SetRenderDrawColor(renderer, 0, 180, 220, 255);

            SDL_RenderFillRect(renderer, &slotRect);

        }

        else {

            SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);

            SDL_RenderFillRect(renderer, &slotBorder);

            SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);

            SDL_RenderFillRect(renderer, &slotRect);

        }

    }



    if (game.isShopOpen) {

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 230);

        SDL_FRect full = { 0, 0, 1280, 720 };

        SDL_RenderFillRect(renderer, &full);



        // restock button

        SDL_FRect restockBorder = { 395, 245, 490, 90 };

        SDL_SetRenderDrawColor(renderer, 0, 255, 150, 255);

        SDL_RenderFillRect(renderer, &restockBorder);

        SDL_SetRenderDrawColor(renderer, 0, 200, 100, 255);

        SDL_RenderFillRect(renderer, &game.restockButton);

        drawText(renderer, game.font, "RESTOCK +15", 640, 260, white, true, true);

        drawText(renderer, game.font, "COST: 25", 640, 295, gold, true, true);



        //buttons with better border

        bool canUpgrade = game.currentMaxSlots < Game::MAX_LIMIT;

        SDL_FRect upgradeBorder = { 395, 345, 490, 90 };

        SDL_SetRenderDrawColor(renderer, canUpgrade ? 255 : 100, canUpgrade ? 200 : 100, 0, 255);

        SDL_RenderFillRect(renderer, &upgradeBorder);

        SDL_SetRenderDrawColor(renderer, canUpgrade ? 200 : 80, canUpgrade ? 120 : 80, 0, 255);

        SDL_RenderFillRect(renderer, &game.upgradeButton);



        if (canUpgrade) {

            drawText(renderer, game.font, "UPGRADE LEVEL", 640, 360, white, true, true);

            drawText(renderer, game.font, "COST: " + std::to_string((int)game.upgradeCost), 640, 395, gold, true, true);

        }

        else {

            drawText(renderer, game.font, "MAX LEVEL REACHED", 640, 375, white, true, true);

        }



        //reset button if u wnat to start fresh

        SDL_FRect resetBorder = { 395, 445, 490, 90 };

        SDL_SetRenderDrawColor(renderer, 255, 80, 80, 255);

        SDL_RenderFillRect(renderer, &resetBorder);

        SDL_SetRenderDrawColor(renderer, 200, 50, 50, 255);

        SDL_RenderFillRect(renderer, &game.resetButton);

        drawText(renderer, game.font, "RESET PROGRESS", 640, 460, white, true, true);

        drawText(renderer, game.font, "START FRESH", 640, 495, white, true, true);

    }



    // TOP Ui like level coin and shit 

    drawText(renderer, game.font, "COINS: " + std::to_string(game.coinsCollected), 40, 30, brightGold, false, true);

    drawText(renderer, game.font, "STOCK: " + std::to_string(game.stock), 40, 75, white, false, true);

    drawText(renderer, game.font, "LEVEL: " + std::to_string(game.currentMaxSlots), 40, 120, white, false, true);



    int serving = getServingSlots(game);

    drawText(renderer, game.font, "SERVING: " + std::to_string(serving) + "/" + std::to_string(game.currentMaxSlots), 640, 30, brightGold, true, true);



    std::string waveText;

    SDL_Color waveColor = { 255, 255, 255, 255 };

    switch (game.currentWaveType) {

    case 0: waveText = "CALM"; waveColor = { 100, 200, 255, 255 }; break;

    case 1: waveText = "BUSY"; waveColor = { 255, 180, 0, 255 }; break;

    case 2: waveText = "RUSH!"; waveColor = { 255, 50, 50, 255 }; break;

    case 3: waveText = "DEAD"; waveColor = { 150, 150, 150, 255 }; break;

    }

    drawText(renderer, game.font, "WAVE: " + waveText, 1050, 30, waveColor, false, true);

    SDL_FRect waveBarBg = { 1050, 75, 200, 18 };

    SDL_FRect waveBarFill = { 1052, 77, 196 * (game.waveTimeRemaining / 15.0f), 14 };

    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);

    SDL_RenderFillRect(renderer, &waveBarBg);

    SDL_SetRenderDrawColor(renderer, waveColor.r, waveColor.g, waveColor.b, 255);

    SDL_RenderFillRect(renderer, &waveBarFill);



    // shop button

    SDL_FRect shopBorder = { 17, 617, 166, 76 };

    SDL_SetRenderDrawColor(renderer, 120, 120, 255, 255);

    SDL_RenderFillRect(renderer, &shopBorder);

    SDL_SetRenderDrawColor(renderer, 80, 80, 220, 255);

    SDL_RenderFillRect(renderer, &game.shopButton);

    drawText(renderer, game.font, game.isShopOpen ? "BACK" : "SHOP", 100, 638, white, true, true);

}


//cleanup again
void cleanupGame(Game& game) {

    if (game.backgroundTexture) SDL_DestroyTexture(game.backgroundTexture);

    if (game.storeTexture) SDL_DestroyTexture(game.storeTexture);

    if (game.font) TTF_CloseFont(game.font);

    TTF_Quit();

}
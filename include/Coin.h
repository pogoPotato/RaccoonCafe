#pragma once
#include <SDL3/SDL.h>

struct Coin
{
    SDL_FRect rect;
    bool active = false;
    int value = 0;  //coin value that goes from 1 -5
};

bool isCoinClicked(
    const Coin& coin, float mx, float my
);

void drawCoin(
    SDL_Renderer* renderer, const Coin& coin
);
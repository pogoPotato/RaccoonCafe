#include "Coin.h"
// coin and shit yeah its a rectangle so what?

bool isCoinClicked(const Coin& coin, float mx, float my)
{
	return coin.active &&
		mx >= coin.rect.x &&
		mx <= coin.rect.x + coin.rect.w &&
		my >= coin.rect.y &&
		my <= coin.rect.y + coin.rect.h;
}

void drawCoin(SDL_Renderer* renderer, const Coin& coin)
{
	if (!coin.active) return;

	SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
	SDL_RenderFillRect(renderer, & coin.rect);
 }
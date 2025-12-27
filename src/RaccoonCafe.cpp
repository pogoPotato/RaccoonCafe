//STUPA
//ENGINE
//2D?? not really
#include <SDL3/SDL.h>// main lib
#include <SDL3/SDL_main.h>// min libs main
#include <SDL3_image/SDL_image.h> // for image and shit
#include <iostream> // u dont know this ??
#include "Game.h" // actual game code

//states
struct SDLState
{
	SDL_Window* window;
	SDL_Renderer* renderer;
};
//cleanup
void cleanup(SDLState& state);

//init sdl u nned to preapare food before u eat
bool initSDL()
{
	return SDL_Init(SDL_INIT_VIDEO);
}

//make a windwo?
bool createWindow(SDLState& state, int width, int height)
{
	state.window = SDL_CreateWindow("Racoon Cafe", width, height, 0);
	return state.window != nullptr;
}


//render shit ?
bool createRenderer(SDLState& state)
{
	state.renderer = SDL_CreateRenderer(state.window, nullptr);
	return state.renderer != nullptr;
}

bool loop(SDLState& state, Game& game, float dt)
{
	// event handeling and shit
	SDL_Event event{ 0 };
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_EVENT_QUIT)
			return false;

		if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
		{
			// chcek state if playing or not and appoly clicks 
			if (game.state == GameState::PLAYING) {
				float mx = event.button.x;
				float my = event.button.y;
				handleGameClick(game, mx, my);
			}
			else {
				//click and splash gone
				game.splashTimer = 6.0f;
			}
		}
	}

	// logic -->> nawh dude its freaking 3am
	if (game.state == GameState::SPLASH) {
		game.splashTimer += dt;

		// 0-2s: fade In .. 2-4s: stay .. 4-6s: fade Out .. basic shit
		if (game.splashTimer < 2.0f) {
			game.splashAlpha = (game.splashTimer / 2.0f) * 255.0f;
		}
		else if (game.splashTimer < 4.0f) {
			game.splashAlpha = 255.0f;
		}
		else if (game.splashTimer < 6.0f) {
			game.splashAlpha = 255.0f - ((game.splashTimer - 4.0f) / 2.0f) * 255.0f;
		}
		else {
			// transition between 2 of them 
			if (game.splashStage == 0) {
				game.splashStage = 1;
				game.splashTimer = 0.0f;
			}
			else {
				game.state = GameState::PLAYING;
			}
		}
	}
	else {
		// update only when playing .. 
		updateGame(game, dt);
	}

	// actual render 
	SDL_SetRenderDrawColor(state.renderer, 0, 0, 0, 255); // transition
	SDL_RenderClear(state.renderer);

	if (game.state == GameState::SPLASH) {
		SDL_Texture* currentSplash = (game.splashStage == 0) ? game.studioSplash : game.gameSplash;
		if (currentSplash) {
			SDL_SetTextureAlphaMod(currentSplash, (Uint8)game.splashAlpha);
			SDL_FRect screenRect = { 0, 0, 1300, 720 };
			SDL_RenderTexture(state.renderer, currentSplash, nullptr, &screenRect);
		}
	}
	else {
		renderGame(game, state.renderer);
	}

	SDL_RenderPresent(state.renderer);

	return true;
}

int main(int argc, char* argv[])
{
	SDLState state;
	state.window = nullptr;
	state.renderer = nullptr;

	//init block
	if (!initSDL())
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "SDL3 not initialized", nullptr);
		return 1;
	}

	// window and shit
	int height = 720;
	int width = 1280;
	if (!createWindow(state, width, height))
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Window Not created!", nullptr);
		cleanup(state);
		return 1;
	}

	// rendering render whatever
	if (!createRenderer(state))
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Renderer is fucked RN", nullptr);
		cleanup(state);
		return 1;
	}

	//game shit
	Game game;
	initGame(game, state.renderer);

	//png of splatscreen shit
	game.studioSplash = IMG_LoadTexture(state.renderer, "assets/studiosplat.png");
	game.gameSplash = IMG_LoadTexture(state.renderer, "assets/gamesplat.png");

	Uint64 lastTicks = SDL_GetTicks();

	//actual loop
	bool isRunning = true;
	while (isRunning)
	{
		Uint64 currentTicks = SDL_GetTicks();
		float dt = (currentTicks - lastTicks) / 1000.0f;
		lastTicks = currentTicks;

		isRunning = loop(state, game, dt);
		SDL_Delay(1); // changed to 1ms to allow dt to be more fluid -> confused ?  study then.. dt -> deltatime.. 
	}

	//cleanup call cpp is trash
	if (game.studioSplash) SDL_DestroyTexture(game.studioSplash);
	if (game.gameSplash) SDL_DestroyTexture(game.gameSplash);
	cleanupGame(game);
	cleanup(state);
	return 0;
}


//actual cleanup
void cleanup(SDLState& state)
{
	if (state.renderer)
		SDL_DestroyRenderer(state.renderer);
	if (state.window)
		SDL_DestroyWindow(state.window);
	SDL_Quit();
}
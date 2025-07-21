#include <SDL2/SDL.h>
#include "game.h"

#define PI 3.14159265

#undef main

int main() {
    Game::Game game;
    game.setup();
    game.run();
    SDL_DestroyRenderer(game.renderer);
    SDL_DestroyWindow(game.window);
    return 0;
}
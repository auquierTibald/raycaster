#ifndef TEXTURE_LOADER_H
#define TEXTURE_LOADER_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_log.h>
#include <string>

#undef main

namespace TextureLoader {
    SDL_Texture* loadTexture(SDL_Renderer* renderer, const std::string& filePath) {
        SDL_Surface* surface = SDL_LoadBMP(filePath.c_str());
        if (!surface) {
            SDL_Log("Failed to load texture: %s, SDL_image Error: %s", filePath.c_str(), IMG_GetError());
            return nullptr;
        }
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture) {
            SDL_Log("Failed to create texture from surface: %s, SDL Error: %s", filePath.c_str(), SDL_GetError());
        }
        SDL_FreeSurface(surface);
        return texture;
    }
}

#endif // TEXTURE_LOADER_H
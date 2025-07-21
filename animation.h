#ifndef ANIMATION_H
#define ANIMATION_H
#include <SDL2/SDL.h>
#include "settings.h"

#undef main

namespace Animation
{

    SDL_Rect plaexis_bruiser_walk[Settings::max_animation_frames] = {{12, 0, 25, 29}, {12, 39, 25, 37}, {12, 77, 26, 42}, {12, 120, 29, 38}};
    SDL_Rect plaexis_bruiser_attack[Settings::max_animation_frames] = {{12, 162, 32, 35}, {12, 197, 31, 37}};
    SDL_Rect plaexis_bruiser_death[Settings::max_animation_frames] = {{12, 379, 30, 36}, {42, 379, 27, 36}, {69, 379, 33, 34}, {103, 379, 29, 35}, {133, 379, 39, 36}};
    SDL_Rect plaexis_bruiser_pain[Settings::max_animation_frames] = {{14, 340, 27, 35}};
    SDL_Rect skeleton_walk[Settings::max_animation_frames] = {{0, 0, 128, 128}};
    SDL_Rect skeleton_attack[Settings::max_animation_frames] = {{128, 0, 128, 128}, {256, 0, 128, 128}, {384, 0, 128, 128}};
    SDL_Rect skeleton_pain[Settings::max_animation_frames] = {{512, 0, 128, 128}};
    SDL_Rect skeleton_death[Settings::max_animation_frames] = {{640, 0, 128, 128}, {768, 0, 128, 128}, {896, 0, 128, 128}};

    class Animation
    {
        public:
        SDL_Rect srcRect[Settings::max_animation_frames];
        int num_frames;
        Animation()
        {
            this->num_frames = 0;
            for (int i = 0; i < Settings::max_animation_frames; i++)
            {
                this->srcRect[i] = {0, 0, 0, 0};
            }
        }
        void setAnimation(int num_frames, SDL_Rect srcRect[Settings::max_animation_frames])
        {
            this->num_frames = num_frames;
            for (int i = 0; i < num_frames; i++)
            {
                this->srcRect[i] = srcRect[i];
            }
        }
    };
}

#endif
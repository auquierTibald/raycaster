#ifndef SPRITE_H
#define SPRITE_H
#include <SDL2/SDL.h>
#include <map>
#include <cmath>
#include <iostream>
#include <ctime>
#include <vector>
#include <random>
#include "settings.h"
#include "textureLoader.h"
#include "player.h"
#include "animation.h"
#include "raycasting.h"

#undef main

#define PI 3.14159265

using namespace std;

namespace Sprite {

    class Sprite{
        public:
            Player::Player *player;
            float x, y, px, py, angle; // Position of the sprite
            float image_ratio;
            int width, height;
            int screenx, screeny;
            float theta, delta, delta_ray;
            double dist;
            float posx, posy, proj, scale = 1; // Dimensions of the sprite
            int ray;
            bool isStatic = true, onscreen = false, disabled = false;
            SDL_Texture* texture = nullptr; // Texture of the sprite
            SDL_Renderer* renderer; // Renderer for drawing the sprite
            SDL_Rect srcRect; // Rectangle for the sprite
            SDL_Rect destRect;
            std::string filePath;
            

            Sprite(Player::Player *player, SDL_Renderer* renderer, const std::string& filePath, float x, float y, int width, int height) {
                this->player = player;
                this->renderer = renderer;
                this->x = x;
                this->y = y;
                this->px = player->x;
                this->py = player->y;
                this->angle = player->angle; // Angle of the sprite
                this->width = width;
                this->height = height;
                this->image_ratio = height / width;
                this->filePath = filePath;
                texture = TextureLoader::loadTexture(renderer, filePath);
            }

            void draw2D()
            {
                if(this->filePath == "assets/86extreme.bmp")
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            else
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                SDL_Rect dest = {player->camX + (int)(x * Settings::SCALE - Settings::SCALE /4 ), player->camY + (int)(y * Settings::SCALE - Settings::SCALE /4), (int)(Settings::SCALE /2), (int)(Settings::SCALE /2)};
                SDL_RenderFillRect(renderer, &dest);
            }

            void setScale(float scale)
            {
                this->scale = scale;
            }

            void setSrcRect(int x, int y, int w, int h) {
                destRect.x = x;
                destRect.y = y;
                destRect.w = w;
                destRect.h = h;
            }

            void draw(double depths[Settings::num_rays], SDL_Texture* texture[Settings::num_rays], SDL_Rect destRect[Settings::num_rays], SDL_Rect srcRect[Settings::num_rays], bool isSprite[Settings::num_rays])
            {
                
                if(onscreen)
                {
                    int w = (int)(this->srcRect.w / Settings::colW);
                    bool flag = false;
                    for(int x = -2; x < w+2; x++)
                    {
                        ray = (int)((posx + x * Settings::colW) / Settings::colW);
                        if(ray > 0 && ray < Settings::num_rays && isSprite[ray] && depths[ray] < dist) 
                        {
                            flag = true;
                            break;
                        }
                    }

                    if(!disabled && !flag)
                    {
                        int shade = (int)(255 / (1 + pow(this->dist,5)*0.000002));
                        SDL_SetTextureColorMod(this->texture, (int)(255*(1-shade)), (int)(255*(1-shade)), (int)(255*(1-shade)));
                        SDL_RenderCopy(renderer, this->texture, &this->destRect, &this->srcRect);
                    }
                    for(int x = -2; x < w+2; x++)
                    {
                        ray = (int)((posx + x * Settings::colW) / Settings::colW);
                        //SDL_Rect tempcol = {(int)(posx + x * Settings::colW), (int)(posy - player->zOffset), Settings::colW, (int)proj};
                        //SDL_Rect tempdest = {this->destRect.x + x * wd, this->destRect.y, wd, this->destRect.h};
                        if(ray > 0 && ray < Settings::num_rays && depths[ray] < dist && depths[ray] != 0)
                        {
                            //this->srcRect = {(int)posx, (int)(posy - player->zOffset), (int)(proj * image_ratio), (int)proj}; caca
                            if(!disabled && !flag)
                            {
                                int shade = (int)(255 / (1 + pow(depths[ray],5)*0.000002));
                                SDL_SetTextureColorMod(texture[ray], (int)(255*(1-shade)), (int)(255*(1-shade)), (int)(255*(1-shade)));
                                SDL_RenderCopy(renderer, texture[ray], &destRect[ray], &srcRect[ray]);
                            }
                        }
                        else if(ray > 0 && ray < Settings::num_rays && !disabled)
                        {
                            depths[ray] = dist;
                            destRect[ray] = this->destRect; srcRect[ray] = this->srcRect;
                            texture[ray] = this->texture;
                            isSprite[ray] = true;
                        }
                    }
                }
            }

            void update(double depths[Settings::num_rays]) {
                this->px = player->x;
                this->py = player->y;
                this->angle = player->angle;

                float dx = x - px; float dy = y - py;
                float theta = atan2(dy, dx);
                float delta = theta - angle;
                if ( (dx > 0 && angle > PI ) || ( dx < 0 && dy < 0) ) {
                    delta += PI*2;
                }
                //if(delta > PI*2) {delta -= PI*2;} else if (delta < 0) {delta +=2*PI;}

                delta_ray = delta / Settings::delta_angle;
                screenx = (int)((Settings::half_num_rays + delta_ray) * Settings::colW);
                dist = sqrt(dx * dx + dy * dy);
                float norm_dist = dist * cos(delta);

                if (dist < 1 && !this->disabled)
                {
                    if(this->filePath == "assets/86extreme.bmp")
                    {
                        this->disabled = true;
                        player->num_86++;
                    }
                }

                if ( -(int)(this->width) < screenx && screenx < (Settings::WIDTH + this->width) && dist > 0.1f && dist < 40)
                {
                    onscreen = true;
                    proj = Settings::SCREEN_DIST / norm_dist; // Projection of the sprite
                    float sprite_half_width = (int)(proj / 2); float scaledproj = proj * scale;
                    posx = screenx - sprite_half_width - (scaledproj - proj); posy = Settings::HALF_HEIGHT - (int)(proj / 2) - ( ( scaledproj - proj ) * image_ratio );
                    this->srcRect = {(int)posx, (int)(posy - player->zOffset), (int)(scaledproj * image_ratio), (int)scaledproj};
                    //std::cout << "sprite "<< index <<" onscreen"<< std::endl;
                }
                else
                {
                    onscreen = false;
                    //this->srcRect = {0, 0, 0, 0};
                    //std::cout << "sprite "<< index <<" offscreen"<< std::endl;
                }

            }
        };

    class AnimatedSprite : public Sprite {
    public:
        double animation_time, animation_time_prev;    // Accumulated time
        bool animation_trigger, animation_finished = false;
        Animation::Animation animations[Settings::max_animations];
        int current = 0;


        AnimatedSprite(Player::Player *player, SDL_Renderer* renderer, const std::string& filePath, float x, float y, int width, int height, double animation_time = 120)
            : Sprite(player, renderer, filePath, x, y, width, height)
        {
            this->animation_time = animation_time;
            this->animation_time_prev = (double)SDL_GetTicks();
            this->animation_trigger = false;
        }

        void draw2D()
        {
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
            SDL_Rect dest = {player->camX + (int)(x * Settings::SCALE - Settings::SCALE /4 ), player->camY + (int)(y * Settings::SCALE - Settings::SCALE /4), (int)(Settings::SCALE /2), (int)(Settings::SCALE /2)};
            SDL_RenderFillRect(renderer, &dest);
        }

        void addAnimation(int index, int num_frames, SDL_Rect srcRect[Settings::max_animation_frames])
        {
            this->animations[index].setAnimation(num_frames, srcRect);
        }

        void animate(Animation::Animation animation, bool once = false)
        {
            if(animation_trigger && animation.num_frames != 0)
            {
                int i;
                bool iFound = false;
                for(i = 0; i < animation.num_frames; i++)
                {
                    if (this->destRect.x == animation.srcRect[i].x && this->destRect.y == animation.srcRect[i].y && this->destRect.w == animation.srcRect[i].w && this->destRect.h == animation.srcRect[i].h)
                    {
                        iFound = true;
                        break;
                    }
                }
                if(once == false)
                {
                    if( !iFound || i == animation.num_frames-1)
                    {
                        this->destRect = animation.srcRect[0];
                    }
                    else
                    {
                        this->destRect = animation.srcRect[i+1];
                    }
                }
                else if(!iFound && !animation_finished)
                {
                    this->destRect = animation.srcRect[0];
                }
                else if(i == animation.num_frames-1 && !animation_finished)
                {
                    this->destRect = {0, 0, 0, 0};
                    this->animation_finished = true;
                }
                else if(!animation_finished)
                {
                    this->destRect = animation.srcRect[i+1];
                }
            }
        }

        void check_animation_time()
        {
            animation_trigger = false;
            double time_now = (double)SDL_GetTicks();
            if (time_now - animation_time_prev > animation_time)
            {
                animation_time_prev = time_now;
                animation_trigger = true;
            }
        }

        void update(double depths[Settings::num_rays]) {
            // Call base update for position/projection
            Sprite::update(depths);
            this->check_animation_time();
            if(!isStatic)
            {
                this->animate(animations[current]);
            }

        }
    };
    
    class NPC : public AnimatedSprite{
        public:
        int health;
        int damage = 20, attack_cooldown = 0;
        bool isAlive = true;
        bool pain = false;
        double attack_dist = 3, damage_dist = 2, trigger_dist = 10;

        NPC(Player::Player* player, SDL_Renderer* renderer, const std::string& filePath, float x, float y, int width, int height, double animation_time, int health, int damage)
            : AnimatedSprite(player, renderer, filePath, x, y, width, height, animation_time)
            {
                this->animation_time = 120;
                this->health = health;
                this->damage = damage;
            }

        void draw2D(int map[Settings::mapHeight][Settings::mapWidth])
        {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_Rect dest = {player->camX + (int)(x * Settings::SCALE - Settings::SCALE /4 ), player->camY + (int)(y * Settings::SCALE - Settings::SCALE /4), (int)(Settings::SCALE /2), (int)(Settings::SCALE /2)};
            SDL_RenderFillRect(renderer, &dest);
        }

        void move(double deltaTime, int map[Settings::mapHeight][Settings::mapWidth])
            {
                float dx = px - x; float dy = py - y;
                float scale = Settings::enemy_size / deltaTime;
                int mapx = (int)x, mapy = (int)y;
                dx = dx * Settings::enemy_speed;
                dy = dy * Settings::enemy_speed;
                if(map[(int)mapy][(int)(x + dx * scale)] == 0 || map[(int)mapy][(int)(x + dx * scale)] == -1)
                {
                    x += dx;
                }
                if(map[(int)(y + dy * scale)][(int)mapx] == 0 || map[(int)(y + dy * scale)][(int)mapx] == -1)
                {
                    y += dy;
                }
            }

        void update(double deltaTime, int map[Settings::mapHeight][Settings::mapWidth], double depths[Settings::num_rays])
        {
            if(attack_cooldown > 0) {attack_cooldown--;}
            this->check_animation_time();
            Sprite::update(depths);
            if(health < 1)
            {
                isAlive = false;
                //onscreen = false;
                current = 2;
                //this->srcRect = {0, 0, 0, 0};
                this->animate(this->animations[current], true);
            }
            if(isAlive && dist < trigger_dist)
            {
                this->move(deltaTime, map);
                this->animate(this->animations[current]);
            }
            if(isAlive && this->dist > damage_dist && this->dist < attack_dist)
            {
                this->current = 1;
                srand(time(0));
                if(attack_cooldown == 0)
                {
                    attack_cooldown= 30;
                    player->health -= this->damage;
                }
            }
            else if(isAlive && this->dist < damage_dist)
            {
                if(player->attacking)
                {
                    this->health -= player->damages[player->weaponId];
                    current = 3;
                    this->animate(this->animations[current], true);
                }
                this->current = 0;
            }
            else if(isAlive)
            {
                this->current = 0;
            }
        }
    };


    //templates

}
#endif
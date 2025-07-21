#ifndef RAYCASTING_H
#define RAYCASTING_H
#include <SDL2/SDL.h>
#include <math.h>
#include <windows.h>
#include "player.h"
#include "settings.h"
#include "textureLoader.h"
#include "sprite.h"
#include <bits/stdc++.h>
#include <string>
#include <vector>
#define PI 3.14159265358979323846

#undef main

using namespace std;

namespace Raycasting {
    class Raycasting {
    public:
        SDL_Renderer* renderer;
        SDL_Texture* textures[Settings::num_textures]; // Array of textures for each ray

        SDL_Rect destRect[Settings::num_rays];
        SDL_Rect srcRect[Settings::num_rays];
        SDL_Texture* texture[Settings::num_rays];

        bool spriteflag;
        bool isSprite[Settings::num_rays];
        int mapx[Settings::num_rays], mapy[Settings::num_rays];
        vector<float> rangles, texturesoffset;
        float ox, oy;
        vector<int> raytiles, projX;
        double depths[Settings::num_rays]; // Distances to wall hit
        Raycasting(SDL_Renderer* renderer) {
            this->renderer = renderer;
        }

        void addTexture(int index, const std::string& filePath) {
            if (index < 0 || index >= Settings::num_textures) {
                SDL_Log("Texture index out of bounds: %d", index);
                return;
            }
            textures[index] = TextureLoader::loadTexture(renderer, filePath);
            if (!textures[index]) {
                SDL_Log("Failed to load texture: %s", filePath.c_str());
            }
        }

        void raycast(int map[Settings::mapHeight][Settings::mapWidth], float px, float py, float x_map, float y_map, float angle) {
            this->rangles.clear();
            this->texturesoffset.clear();
            this->raytiles.clear();

            ox = px; oy = py;
            int mapx = (int)px, mapy = (int)py, tile, tilex, tiley;
            float ray_angle = angle - Settings::half_fov + 0.0001f;
            bool wallHit = false, doorHit = false;
            float sideDistX, sideDistY;
            double depth;

            for (int ray = 0; ray < Settings::num_rays; ray++)
            {
                //system("cls");
                depth = 0;
                wallHit = false;
                doorHit = false;
                int side, tile = 0, dist = 0;

                float rayDirX = cos(ray_angle), rayDirY = sin(ray_angle);
                //if(rayDirX == 0) {std::cout<<"cos = 0"<<std::endl; rayDirX+=0.001f;} if(rayDirY == 0) {std::cout<<"sin = 0"<<std::endl; rayDirY+=0.001f;}
                float deltaDistX = sqrt(1 + (rayDirY / rayDirX) * (rayDirY / rayDirX)), deltaDistY = sqrt(1 + (rayDirX / rayDirY) * (rayDirX / rayDirY));
                int stepX, stepY;
                mapx = (int)ox; mapy = (int)oy;

                if(rayDirX < 0)
                {
                    stepX = -1;
                    sideDistX = (ox - mapx) * deltaDistX;
                }
                else
                    {
                    stepX = 1;
                    sideDistX = (mapx + 1.0 - ox) * deltaDistX;
                }
                if(rayDirY < 0)
                {
                    stepY = -1;
                    sideDistY = (oy - mapy) * deltaDistY;
                }
                else
                {
                    stepY = 1;
                    sideDistY = (mapy + 1.0 - oy) * deltaDistY;
                }

                //perform DDA
                while(!wallHit && dist < Settings::MAX_DEPTH)
                {
                    //jump to next map square, either in x-direction, or in y-direction
                    if(sideDistX < sideDistY)
                    {
                        sideDistX += deltaDistX;
                        mapx += stepX;
                        side = 0;
                    }
                    else
                    {
                        sideDistY += deltaDistY;
                        mapy += stepY;
                        side = 1;
                    }
                    if(mapy > 0 && mapy < Settings::mapHeight && mapx > 0 && mapx < Settings::mapWidth ) {tile = map[mapy][mapx];}
                    //Check if ray has hit a wall
                    if(tile  > 0)
                    { 
                        wallHit = true;
                    }
                    dist++;
                }

                if(side == 1)
                {
                    if(wallHit) {depths[ray] = (sideDistY - deltaDistY);} else {depths[ray] = 0;}
                    float tx = (ox + depths[ray] * rayDirX);
                    texturesoffset.push_back(tx - (int)tx);
                    raytiles.push_back(tile);
                }
                else
                {
                    if(wallHit) {depths[ray] = (sideDistX - deltaDistX);} else {depths[ray] = 0;}
                    float ty = (oy + depths[ray] * rayDirY);
                    texturesoffset.push_back(ty - (int)ty);
                    raytiles.push_back(tile);
                }
                this->mapy[ray] = mapy;
                this->mapx[ray] = mapx;
                projX.push_back((int)(ray * Settings::colW));
                rangles.push_back(ray_angle);

                ray_angle += Settings::delta_angle;
                //std::cout <<" ray x :"<<rayx[ray]<<" y : "<<rayy[ray]<<std::endl;
                //std::cout << "cos : " << cos_a << " sin : " << sin_a << std::endl;
                //std::cout << "ray " << ray << " depth : " << depths[ray] << " angle : " << rangles[ray] << " t_offset : " << texturesoffset[ray] << " tx : "<< tx<< " ty : " << ty << std::endl;
                if(ray_angle > 2*PI) {ray_angle = 0;} if(ray_angle < 0) {ray_angle += 2*PI;}
            }
            
        }

        void drawfloor(int floor, float px, float py, float pz, float angle)
        {
            float ray0_angle = angle - Settings::half_fov; 
            float ray1_angle = angle + Settings::half_fov;
            float rayDirX0 = cos(ray0_angle), rayDirY0 = sin(ray0_angle);
            float rayDirX1 = cos(ray1_angle), rayDirY1 = sin(ray1_angle);
            for(int y = Settings::HALF_HEIGHT + 10; y < Settings::HEIGHT + pz; y += Settings::colW)
            {
                int p = y - Settings::HALF_HEIGHT;

                float rowDistance = Settings::HALF_HEIGHT / p;

                float floorStepX = rowDistance * (rayDirX1 - rayDirX0) / (Settings::WIDTH / Settings::colW);
                float floorStepY = rowDistance * (rayDirY1 - rayDirY0) / (Settings::WIDTH / Settings::colW);

                float floorX = px + rowDistance * rayDirX0;
                float floorY = py + rowDistance * rayDirY0;

                for(int x = 0; x < Settings::WIDTH; x += Settings::colW)
                {
                    int cellX = (int)floorX, cellY = (int)floorY;
                    int tx = (int)(Settings::floor_texture_size * (floorX - cellX)) & (Settings::texture_size - 1);
                    int ty = (int)(Settings::floor_texture_size * (floorY - cellY)) & (Settings::texture_size - 1);
                    int texture = floor;
                    //if(cellX >= 0 && cellX < Maps::Height && cellX < Maps::Width && cellY >= 0) {texture = Maps::floor1[cellY][cellX];}
                    
                    floorX += floorStepX;
                    floorY += floorStepY;
                    int shade = (int)(255 / (1 + pow(rowDistance,5)*0.0000002));
                    SDL_SetTextureColorMod(textures[texture], (int)(255*(1-shade)), (int)(255*(1-shade)), (int)(255*(1-shade)));
                    SDL_Rect dest = {tx, ty, 1, 1};
                    SDL_Rect src = {x, (int)(y-pz), Settings::colW, Settings::colW};
                    SDL_RenderCopy(renderer, textures[texture], &dest, &src);
                }

            }
        }

        void draw2D()
        {
            SDL_SetRenderDrawColor(renderer, 55, 55, 55, 255); // Gray color for rays
            for (int ray = 0; ray < Settings::num_rays; ray++)
            {
                SDL_RenderDrawLine(renderer, ox * Settings::SCALE, 
                                            oy* Settings::SCALE,
                                            (int)((ox * Settings::SCALE) + cos(rangles[ray])*depths[ray] * Settings::SCALE),
                                            (int)((oy * Settings::SCALE) + sin(rangles[ray])*depths[ray] * Settings::SCALE));
            }
        }

        void projection(int map[Settings::mapHeight][Settings::mapWidth], float playerangle, float pz)
        {

            for (int ray = 0; ray < Settings::num_rays; ray++)
            {
                isSprite[ray] = false;
                float norm_depth = depths[ray] * cos(playerangle - rangles[ray]); // Correct depth for perspective
                float proj_height = (int)(Settings::SCREEN_DIST / norm_depth);
                float hor_offset = (Settings::HEIGHT - proj_height) / 2;
                int projY = (int)(hor_offset + proj_height - (map[mapy[ray]][mapx[ray]] * proj_height)) - pz;
                int shade = (int)(255 / (1 + pow(depths[ray],5)*0.000002));
                SDL_Rect col = {projX[ray], projY, Settings::colW, (int)(proj_height*map[mapy[ray]][mapx[ray]])};
                SDL_Rect dest = {(int)(texturesoffset[ray] * Settings::texture_size), 0, 1, Settings::texture_size * map[mapy[ray]][mapx[ray]]};
                if(raytiles[ray] != 0)
                {
                    destRect[ray] = dest;
                    srcRect[ray] = col;
                    texture[ray] = this->textures[raytiles[ray]];
                    SDL_SetTextureColorMod(texture[ray], (int)(255*(1-shade)), (int)(255*(1-shade)), (int)(255*(1-shade)));
                    SDL_RenderCopy(renderer, texture[ray], &destRect[ray], &srcRect[ray]);
                }
            }
    }
        
    };
}


#endif // RAYCASTING_H
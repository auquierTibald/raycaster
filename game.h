#ifndef GAME_H
#define GAME_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include "settings.h"
#include "player.h"
#include "raycasting.h"
#include "textureLoader.h"
#include "sprite.h"
#include "animation.h"
#include <vector>
#include <string>
#include <fstream>

#undef main

namespace Game {

class Game {
public:


    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Event event;

    bool running = true, heightMapSelection = false;
    double deltaTime = 0.0;
    double lastFrameTime = 0.0;
    double currentFrameTime = 0.0;
    int gameState = 0, level = 0;
    float skyoffset = 0;

    //editor
    int currentTexture = 1, currentObject = 0;
    int objectType = 0;
    SDL_Color White = {255, 255, 255};
    TTF_Font* minecraft;



    Player::Player *player = nullptr;
    Raycasting::Raycasting *raycasting = nullptr;

    SDL_Texture* skybox;
    SDL_Texture* button;

    int currentmap[Settings::mapHeight][Settings::mapWidth], currentHeight[Settings::mapHeight][Settings::mapWidth];

    vector<Sprite::Sprite> sprite;
    vector<Sprite::AnimatedSprite> animatedsprite;
    vector<Sprite::NPC> npc;

    Game() {
        SDL_Init(SDL_INIT_VIDEO);
        SDL_Window* window = SDL_CreateWindow("BOMBORAYCLAT", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Settings::WIDTH, Settings::HEIGHT, SDL_WINDOW_SHOWN);
        if (!window) {
            std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            SDL_Quit();
        }
        SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (!renderer) {
            std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            SDL_DestroyWindow(window);
            SDL_Quit();
        }
        this->window = window;
        this->renderer = renderer;
        player = new Player::Player(renderer, window);
        raycasting = new Raycasting::Raycasting(renderer);
        minecraft = TTF_OpenFont("minecraft.ttf", 20);

        Sprite::Sprite tree_template(player, renderer, "assets/tree.bmp", 0, 0, 685, 784);
        Sprite::NPC plaexis_bruiser_template(player, renderer, "assets/plaexisbrusier_spritesheet.bmp", 0, 0, 25, 29, 40, 100, 10);

        tree_template.setSrcRect(0, 0, 685, 784);
        plaexis_bruiser_template.setSrcRect(12, 0, 25, 29); 
        plaexis_bruiser_template.addAnimation(0, 4, Animation::plaexis_bruiser_walk);
        plaexis_bruiser_template.addAnimation(1, 2, Animation::plaexis_bruiser_attack);
        plaexis_bruiser_template.addAnimation(2, 5, Animation::plaexis_bruiser_death);
        plaexis_bruiser_template.addAnimation(3, 1, Animation::plaexis_bruiser_pain);
    }

    void mapSetup()
    {
        for (int y = 0; y < Settings::mapHeight; ++y) {
            for (int x = 0; x < Settings::mapWidth; ++x) {
                currentmap[y][x] = 0;
                currentHeight[y][x] = 1;
            }
        }
    }

    void addSprite(const std::string& filePath, float x, float y, int width, int height) {
        Sprite::Sprite s(this->player, renderer, filePath, x, y, width, height);
        sprite.push_back(s);
    }

    void addAnimatedSprite(const std::string& filePath, float x, float y, int width, int height) {
        Sprite::AnimatedSprite a(this->player, renderer, filePath, x, y, width, height);
        animatedsprite.push_back(a);
    }

    void addNPC(const std::string& filePath, float x, float y, int width, int height, int health, int damage) {
        Sprite::NPC n(this->player, renderer, filePath, x, y, width, height, 120, health, damage);
        npc.push_back(n);
    }

    void setup() {

        this->mapSetup();

        player->swordanimation[0] = TextureLoader::loadTexture(renderer, "assets/sword1.bmp");
        player->swordanimation[1] = TextureLoader::loadTexture(renderer, "assets/sword2.bmp");
        player->swordanimation[2] = TextureLoader::loadTexture(renderer, "assets/sword3.bmp");
        player->swordanimation[3] = TextureLoader::loadTexture(renderer, "assets/sword4.bmp");  
        player->currentweaponframe = player->swordanimation[0];

        raycasting->addTexture(1, "assets/wall.bmp");
        raycasting->addTexture(2, "assets/door.bmp");
        raycasting->addTexture(3, "assets/floor.bmp");
        raycasting->addTexture(4, "assets/floor2.bmp");
        raycasting->addTexture(5, "assets/double_wall.bmp");
        raycasting->addTexture(6, "assets/triple_wall.bmp");
        raycasting->addTexture(7, "assets/big_door_left.bmp");
        raycasting->addTexture(8, "assets/big_door_left3.bmp");
        raycasting->addTexture(9, "assets/big_door_right.bmp");
        raycasting->addTexture(10, "assets/big_door_right3.bmp");
        raycasting->addTexture(11, "assets/door2.bmp");
        raycasting->addTexture(12, "assets/door3.bmp");
        

        skybox = TextureLoader::loadTexture(renderer, "assets/skybox.bmp");
        button = TextureLoader::loadTexture(renderer, "assets/button.bmp");

        /*
        this->addSprite("assets/spritesheet.bmp", 2.5f, 3.5f, 34, 35);
        sprite.back().setSrcRect(136, 307, 34, 35); // Set source rectangle for the sprite

        this->addSprite("assets/spritesheet.bmp", 5.5f, 2.5f, 34, 35);
        sprite.back().setSrcRect(136, 307, 34, 35); // Set source rectangle for the sprite

        this->addSprite("assets/spritesheet.bmp", 10.5f, 5.5f, 34, 35);
        sprite.back().setSrcRect(136, 307, 34, 35); // Set source rectangle for the sprite

        this->addSprite("assets/spritesheet.bmp", 8.5f, 10.5f, 34, 35);
        sprite.back().setSrcRect(136, 307, 34, 35); // Set source rectangle for the sprite

        this->addNPC("assets/plaexisbrusier_spritesheet.bmp", 10.5f, 2.5f, 28, 40, 100, 10);
        npc.back().setSrcRect(12, 0, 25, 29);
        npc.back().isStatic = false;
        npc.back().addAnimation(0, 4, Animation::plaexis_bruiser_walk);

        this->addNPC("assets/plaexisbrusier_spritesheet.bmp", 25.5f, 10.5f, 28, 40, 100, 10);
        npc.back().setSrcRect(12, 0, 25, 29); 
        npc.back().isStatic = false;
        npc.back().addAnimation(0, 4, Animation::plaexis_bruiser_walk);
        */
    }

    void loadmap()
    {
        this->sprite.clear();
        this->animatedsprite.clear();
        this->npc.clear();

        int num_sprite, num_animatedsprite, num_npc;
        int x, y, width, height;
        float scale;
        std::string texturepath;
        std::string filename;
        std::cout << "Enter filename : ";
        std::cin >> filename;

        ifstream file(filename);

        if(file.is_open())
        {
            //PLAYER COORDS
            file.ignore(1, '\n');
            file >> player->x;
            file.ignore(1, ' ');
            file >> player->y;

            file.ignore(1, '\n');

            //SIZES
            file.ignore(1, '\n');

            file >> num_sprite;
            file.ignore(1, ' ');
            file >> num_animatedsprite;
            file.ignore(1, ' ');
            file >> num_npc;
            file.ignore(1, '\n');

            //MAP
            file.ignore(1, '\n');

            for (int i = 0; i < Settings::mapHeight; i++)
            {
                for (int j = 0; j < Settings::mapWidth; j++)
                {
                    file >> currentmap[i][j];
                    file.ignore(1, ' ');
                }
                file.ignore(1, '\n');
            }
            //HEIGHT MAP
            //MAP
            file.ignore(1, '\n');

            for (int i = 0; i < Settings::mapHeight; i++)
            {
                for (int j = 0; j < Settings::mapWidth; j++)
                {
                    file >> currentHeight[i][j];
                    file.ignore(1, ' ');
                }
                file.ignore(1, '\n');
            }
            
            //SPRITE
            file.ignore(1, '\n');
            for(int i = 0; i < num_sprite; i++)
            {
                file >> texturepath; file.ignore(1, ' ');
                file >> x; file.ignore(1, ' ');
                file >> y; file.ignore(1, ' ');
                file >> width; file.ignore(1, ' ');
                file >> height; file.ignore(1, ' ');
                file >> scale;
                this->addSprite(texturepath, x, y, width, height);
                this->sprite.back().setScale(scale);
                this->sprite.back().setSrcRect(0, 0, width, height);
                file.ignore(1, '\n');
            }

            //ANIMATED SPRITE
            file.ignore(1, '\n');
            for(int i = 0; i < num_animatedsprite; i++)
            {
                file >> texturepath; file.ignore(1, ' ');
                file >> x; file.ignore(1, ' ');
                file >> y; file.ignore(1, ' ');
                file >> width; file.ignore(1, ' ');
                file >> height; file.ignore(1, ' ');
                file >> scale;
                this->addAnimatedSprite(texturepath, x, y, width, height);
                this->animatedsprite.back().setScale(scale);
                this->animatedsprite.back().setSrcRect(0, 0, width, height);
                file.ignore(1, '\n');
            }

            //NPC
            file.ignore(1, '\n');
            for(int i = 0; i < num_npc; i++)
            {
                file >> texturepath; file.ignore(1, ' ');
                file >> x; file.ignore(1, ' ');
                file >> y;
                if(texturepath == "plaexis_brusier")
                {
                    this->addNPC("assets/plaexisbrusier_spritesheet.bmp", x, y, 28, 40, 100, 10);
                    npc.back().setScale(0.7);
                    npc.back().setSrcRect(12, 0, 25, 29);
                    npc.back().addAnimation(0, 4, Animation::plaexis_bruiser_walk);
                    npc.back().addAnimation(1, 2, Animation::plaexis_bruiser_attack);
                    npc.back().addAnimation(2, 5, Animation::plaexis_bruiser_death);
                    npc.back().addAnimation(3, 1, Animation::plaexis_bruiser_pain);
                }
                else if(texturepath == "skeleton")
                {
                    this->addNPC("assets/skeleton.bmp", x, y, 128, 128, 150, 9);
                    npc.back().setSrcRect(0, 0, 128, 128);
                    npc.back().addAnimation(0, 1, Animation::skeleton_walk);
                    npc.back().addAnimation(1, 3, Animation::skeleton_attack);
                    npc.back().addAnimation(2, 3, Animation::skeleton_death);
                    npc.back().addAnimation(3, 1, Animation::skeleton_pain);
                }
                file.ignore(1, '\n');
            }

        }

        file.close();

    }

    void savemap()
    {
        std::string filename;
        std::cout << "Enter filename : ";
        std::cin >> filename;

        ofstream file(filename);

        if(file.is_open())
        {
            //PLAYER COORDS
            file << std::endl;

            file << player->x << " " << player->y << std::endl;
            //SIZES
            file << std::endl;

            file << sprite.size() << " " << animatedsprite.size() << " " << npc.size() << std::endl;

            //MAP
            file <<std::endl;

            for (int i = 0; i < Settings::mapHeight; i++)
            {
                for (int j = 0; j < Settings::mapWidth; j++)
                {
                    file << currentmap[i][j] << " ";
                }
                file << std::endl;
            }

            //HEIGHT MAP
            file <<std::endl;

            for (int i = 0; i < Settings::mapHeight; i++)
            {
                for (int j = 0; j < Settings::mapWidth; j++)
                {
                    file << currentHeight[i][j] << " ";
                }
                file << std::endl;
            }

            //SPRITE
            file << std::endl;

            for(int i = 0; i < sprite.size(); i++)
            {
                file << sprite[i].filePath << " " << sprite[i].x  << " " << sprite[i].y  << " " << sprite[i].width  << " " << sprite[i].height << " " << sprite[i].scale << std::endl;
            }

            //ANIMATED SPRITE
            file << std::endl;

            for(int i = 0; i < animatedsprite.size(); i++)
            {
                file << animatedsprite[i].filePath << " " << animatedsprite[i].x  << " " << animatedsprite[i].y  << " " << animatedsprite[i].width  << " " << animatedsprite[i].height << " " << animatedsprite[i].scale << std::endl;
            }

            //NPC
            file << std::endl;

            for(int i = 0; i < npc.size(); i++)
            {
                if(npc[i].filePath == "assets/plaexisbrusier_spritesheet.bmp")
                {
                    file << "plaexis_brusier " << npc[i].x << " " <<  npc[i].y << std::endl;
                }
                else if(npc[i].filePath == "assets/skeleton.bmp")
                {
                    file << "skeleton " << npc[i].x << " " <<  npc[i].y << std::endl;
                }
            }
            
        }
        file.close();

    }

    void editorinputs(SDL_Event event)
    {
        int x, y;

        const Uint8* keystate = SDL_GetKeyboardState(NULL);

            if(keystate[SDL_SCANCODE_LCTRL] && keystate[SDL_SCANCODE_Z])
            {
                if(objectType == 1)
                {
                    sprite.pop_back();
                }
                else if(objectType == 2)
                {
                    npc.pop_back();
                }
                else if(objectType == 3)
                {
                    animatedsprite.pop_back();
                }
            }


        if(event.type == SDL_MOUSEBUTTONDOWN)
        {
            SDL_GetMouseState(&x, &y);
            int mapX = (int)((x - player->camX) / Settings::SCALE);
            int mapY = (int)((y - player->camY) / Settings::SCALE);
            if(event.button.button == SDL_BUTTON_LEFT)
            {   
                if(x > Settings::WIDTH - 100 && y < 100)
                {
                    if(currentTexture >= Settings::raycasting_textures && objectType == 0)
                    {
                        currentTexture = 0;
                    }
                    else if (objectType == 0)
                    {
                        currentTexture++;
                    }
                    if(objectType == 1)
                    {
                        currentObject++;
                    }
                    if(objectType == 2)
                    {
                        currentObject++;
                    } 
                    std::cout << currentObject << std::endl;
                }
                else if(x > Settings::WIDTH - 100 && y > 105 && y < 155)
                {
                    if(objectType == 3)
                    {
                        objectType = 0;
                    }
                    else
                    {
                        objectType++;
                    }
                }
                else if(x > Settings::WIDTH - 100 && y > 155 && y < 205)
                {
                    this->savemap();
                }
                else if(x > Settings::WIDTH - 100 && y > 205 && y < 255)
                {
                    this->loadmap();
                }
                else if(x > Settings::WIDTH - 100 && y > 255 && y < 305)
                {
                    if(heightMapSelection) {heightMapSelection = false;} else {heightMapSelection = true;}
                }
                else if(heightMapSelection)
                {
                    std::cout << currentHeight[(int)((y - player->camY )/Settings::SCALE)][(int)((x - player->camX )/Settings::SCALE)] << std::endl;
                    currentHeight[(int)((y - player->camY )/Settings::SCALE)][(int)((x - player->camX )/Settings::SCALE)]++;
                }
                else
                {
                    if(objectType == 0)
                    {
                        currentmap[mapY][mapX] = currentTexture;
                    }
                    else if (objectType == 1)
                    {
                        if(currentObject == 0)
                        {
                            this->addSprite("assets/tree.bmp", (x - player->camX )/Settings::SCALE, (y - player->camY )/Settings::SCALE, 685, 784);
                            this->sprite.back().setScale(2);
                            this->sprite.back().setSrcRect(0, 0, 685, 784);
                        }
                        else if(currentObject == 1)
                        {
                            this->addSprite("assets/tree2.bmp", (x - player->camX )/Settings::SCALE, (y - player->camY )/Settings::SCALE, 350, 450);
                            this->sprite.back().setScale(1.7);
                            this->sprite.back().setSrcRect(0, 0, 350, 450);
                        }
                        else if(currentObject == 2)
                        {
                            this->addSprite("assets/86extreme.bmp", (x - player->camX )/Settings::SCALE, (y - player->camY )/Settings::SCALE, 130, 130);
                            this->sprite.back().setScale(0.5);
                            this->sprite.back().setSrcRect(0, 0, 130, 130);
                        }
                    }
                    else if (objectType == 2)
                    {
                        if(currentObject == 0)
                        {
                            this->addNPC("assets/plaexisbrusier_spritesheet.bmp", (x - player->camX )/Settings::SCALE, (y - player->camY )/Settings::SCALE, 28, 40, 100, 10);
                            npc.back().setScale(0.7);
                            npc.back().setSrcRect(12, 0, 25, 29); 
                            npc.back().addAnimation(0, 4, Animation::plaexis_bruiser_walk);
                            npc.back().addAnimation(1, 2, Animation::plaexis_bruiser_attack);
                            npc.back().addAnimation(2, 5, Animation::plaexis_bruiser_death);
                            npc.back().addAnimation(3, 1, Animation::plaexis_bruiser_pain);
                        }
                        else if(currentObject == 1)
                        {
                            this->addNPC("assets/skeleton.bmp", (x - player->camX )/Settings::SCALE, (y - player->camY )/Settings::SCALE, 128, 128, 150, 9);
                            npc.back().setSrcRect(0, 0, 128, 128);
                            npc.back().addAnimation(0, 1, Animation::skeleton_walk);
                            npc.back().addAnimation(1, 3, Animation::skeleton_attack);
                            npc.back().addAnimation(2, 3, Animation::skeleton_death);
                            npc.back().addAnimation(3, 1, Animation::skeleton_pain);
                        }

                    }
                }
                
            }
            else
            {
                if(x > Settings::WIDTH - 100 && y < 100)
                {
                        if(currentTexture >= Settings::raycasting_textures && objectType == 0)
                        {
                            currentTexture = 0;
                        }
                        else if (objectType == 0)
                        {
                            currentTexture--;
                        }
                        if(objectType == 1)
                        {
                            currentObject--;
                        }
                        if(objectType == 2)
                        {
                            currentObject--;
                        } 
                }
                else
                {
                    if(heightMapSelection)
                    {
                        std::cout << currentHeight[(int)((y - player->camY )/Settings::SCALE)][(int)((x - player->camX )/Settings::SCALE)] << std::endl;
                        currentHeight[(int)((y - player->camY )/Settings::SCALE)][(int)((x - player->camX )/Settings::SCALE)]--;
                    }
                    else
                    {
                        currentmap[mapY][mapX] = 0;
                    }
                }
            }
        }
        else if(event.type == SDL_MOUSEWHEEL)
        {
            if(event.wheel.y > 0)
            {
                Settings::SCALE += 2;
            }
            else
            {
                Settings::SCALE -= 2;
            }
        }
    }

    void editordraw()
    {
        int x, y;

        SDL_Rect current = {Settings::WIDTH - 100, 0, 100, 100};
        SDL_Rect button1 =  {Settings::WIDTH - 100, 105, 100, 50};
        SDL_Rect button2 =  {Settings::WIDTH - 100, 160, 100, 50};
        SDL_Rect button3 =  {Settings::WIDTH - 100, 215, 100, 50};
        SDL_Rect button4 =  {Settings::WIDTH - 100, 270, 100, 50};

        SDL_Rect src1 = {12, 0, 25, 29};
        SDL_Rect src2 = {0, 0, 128, 128};
        SDL_SetTextureColorMod(raycasting->textures[currentTexture], 255, 255, 255);
        if(objectType == 0) {SDL_RenderCopy(renderer, raycasting->textures[currentTexture], NULL, &current);}
        else if(objectType == 1) 
        {
            if(currentObject == 0)
            {
                SDL_RenderCopy(renderer, TextureLoader::loadTexture(renderer, "assets/tree.bmp"), NULL, &current);
            } 
            else if(currentObject == 1)
            {
                SDL_RenderCopy(renderer, TextureLoader::loadTexture(renderer, "assets/tree2.bmp"), NULL, &current);
            }
            else if(currentObject == 2)
            {
                SDL_RenderCopy(renderer, TextureLoader::loadTexture(renderer, "assets/86extreme.bmp"), NULL, &current);
            }

        }
        else if(objectType == 2) 
        {
            if(currentObject == 0)
            {
                SDL_RenderCopy(renderer, TextureLoader::loadTexture(renderer, "assets/plaexisbrusier_spritesheet.bmp"), &src1, &current);
            }
            else if(currentObject == 1)
            {
                SDL_RenderCopy(renderer, TextureLoader::loadTexture(renderer, "assets/skeleton.bmp"), &src2, &current);
            }
        
        }

        SDL_RenderCopy(renderer, button, NULL, &button1);
        SDL_RenderCopy(renderer, button, NULL, &button2);
        SDL_RenderCopy(renderer, button, NULL, &button3);
        SDL_RenderCopy(renderer, button, NULL, &button4);

        SDL_GetMouseState(&x, &y);
        //std::cout<<"x : "<<x<<" y : "<<y<<std::endl;
        if(x > Settings::WIDTH - 100 &&  y > 105 && y < 155)
        {
            SDL_SetRenderDrawColor(renderer, 125, 125, 125, 255);
            SDL_RenderFillRect(renderer, &button1);
        }
        else if(x > Settings::WIDTH - 100 &&  y > 160 && y < 210)
        {
            SDL_SetRenderDrawColor(renderer, 125, 125, 125, 255);
            SDL_RenderFillRect(renderer, &button2);
        }
        else if(x > Settings::WIDTH - 100 &&  y > 215 && y < 265)
        {
            SDL_SetRenderDrawColor(renderer, 125, 125, 125, 255);
            SDL_RenderFillRect(renderer, &button3);
        }
        else if(x > Settings::WIDTH - 100 &&  y > 270 && y < 320)
        {
            SDL_SetRenderDrawColor(renderer, 125, 125, 125, 255);
            SDL_RenderFillRect(renderer, &button4);
        }


    }

    void update() {
        // deltaTime calculation
        lastFrameTime = currentFrameTime;
        currentFrameTime = SDL_GetPerformanceCounter();
        deltaTime = (double)((currentFrameTime - lastFrameTime)*1000 / (double)SDL_GetPerformanceFrequency() );
        if (gameState == 0) {
            switch (level)
            {
            case 0:
                player->update(deltaTime, currentmap);
                raycasting->raycast(currentmap, player->x, player->y, player->mapx, player->mapy, player->angle);
                /*
                for(int i = 0; i < npc.size(); i++)
                {
                    npc[i].update(deltaTime, currentmap, raycasting->depths);
                }
                */
                break;
            }
        }
        if (gameState == 1) {
            switch (level)
            {
            case 0:
                player->update(deltaTime,currentmap);
                raycasting->raycast(currentmap, player->x, player->y, player->mapx, player->mapy, player->angle);
                for(int i = 0; i < sprite.size(); i++)
                {
                    sprite[i].update(raycasting->depths);
                }
                for(int i = 0; i < animatedsprite.size(); i++)
                {
                    animatedsprite[i].update(raycasting->depths);
                }
                for(int i = 0; i < npc.size(); i++)
                {
                    npc[i].update(deltaTime, currentmap, raycasting->depths);
                }
                //this->objectrenderer->update(sprite, animatedsprite, npc);
                break;
            }
        }
           
    }
    void render() {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Clear the screen with black color
        SDL_RenderClear(renderer);
        if (gameState == 0) {
            switch (level)
            {
            case 0:
                //raycasting->draw2D();
                this->draw2DMap(currentmap);
                player->draw();
                for(int i = 0; i < sprite.size(); i++)
                {
                    sprite[i].draw2D();
                }
                for(int i = 0; i < animatedsprite.size(); i++)
                {
                    animatedsprite[i].draw2D();
                }
                for(int i = 0; i < npc.size(); i++)
                {
                    npc[i].draw2D(currentmap);
                }
                this->editordraw();
                break;
            }
  
        }
        else if(gameState == 1)
        {
            //this->draw3Dbackground();
            raycasting->drawfloor(4, player->x, player->y, player->zOffset, player->angle);
            raycasting->projection(currentHeight, player->angle, player->zOffset);
            for(int i = 0; i < sprite.size(); i++)
            {
                sprite[i].draw(raycasting->depths, raycasting->texture, raycasting->destRect, raycasting->srcRect, raycasting->isSprite);
            }
            for(int i = 0; i < animatedsprite.size(); i++)
            {
                animatedsprite[i].draw(raycasting->depths, raycasting->texture, raycasting->destRect, raycasting->srcRect, raycasting->isSprite);
            }
            for(int i = 0; i < npc.size(); i++)
            {
                npc[i].draw(raycasting->depths, raycasting->texture, raycasting->destRect, raycasting->srcRect, raycasting->isSprite);
            }
            
            //this->objectrenderer->draw();
            player->drawWeapon();
            player->drawGUI();
        }
        SDL_RenderPresent(renderer);
    }

    void draw2DMap(int map[Settings::mapHeight][Settings::mapWidth]) {
        for (int y = 0; y < Settings::mapHeight; y++) {
            for (int x = 0; x < Settings::mapWidth; x++) {
                if (map[y][x] > 0 ) {
                    SDL_Rect rect = { player->camX + x * Settings::SCALE, player->camY + y * Settings::SCALE, Settings::SCALE, Settings::SCALE };
                    SDL_SetTextureColorMod(raycasting->textures[map[y][x]], 255*(this->currentHeight[y][x]+1)/10, 255*(this->currentHeight[y][x]+1)/10, 255*(this->currentHeight[y][x]+1)/10); // Set color to white
                    SDL_RenderCopy(renderer, this->raycasting->textures[map[y][x]], NULL, &rect);
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Red color for the map grid
                    SDL_RenderDrawRect(renderer, &rect); // Draw the rectangle for the map element
                }
                // Add more conditions for different map elements
            }
        }
    }

    void draw3Dbackground()
    {
        skyoffset = (player->angle / (2 * M_PI) * Settings::WIDTH);

        SDL_Rect rect1 = { -(int)skyoffset, 0, 600 * 2, Settings::HEIGHT};
        SDL_Rect rect2 = { -(int)skyoffset + Settings::WIDTH, 0, 600 * 2, Settings::HEIGHT};

        //SDL_Rect floor = {0, Settings::HEIGHT / 2, Settings::WIDTH, Settings::HEIGHT / 2};

        SDL_RenderCopy(renderer, skybox, NULL, &rect1);
        SDL_RenderCopy(renderer, skybox, NULL, &rect2);

        //SDL_SetRenderDrawColor(renderer, 55, 55, 55, 255); // Sky blue color
        //SDL_RenderFillRect(renderer, &floor);
    }

    void events() {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            if(event.type == SDL_KEYDOWN) {
                if(event.key.keysym.sym == SDLK_m) {
                    if (gameState == 0) {
                        gameState = 1; // Switch to 3D mode
                        SDL_ShowCursor(SDL_DISABLE);
                    } else {
                        gameState = 0; // Switch to 2D mode
                        SDL_ShowCursor(SDL_ENABLE);
                    }
                }
                else if(event.key.keysym.sym == SDLK_x) {
                    running = false; // Exit the game
                }
            }
            if(event.type == SDL_MOUSEBUTTONDOWN)
            {
                player->attacking = true;
            }
            if(gameState == 0)
            {
                this->editorinputs(event);
            }
            // Handle other events like keyboard input, mouse input, etc.
        }
    }
    void run() {
        while(running && player->alive) {
            events();
            update();
            render();
            SDL_Delay(10);
        }
    }

};

}

#endif
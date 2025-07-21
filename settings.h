#ifndef SETTINGS_H
#define SETTINGS_H
#include <math.h>
#define PI 3.14159265358979323846f

#undef main

namespace Settings {

    // Define the game settings
    const int WIDTH = 1200;  // Width of the game window
    const int HEIGHT = 900; // Height of the game window

    const long mapWidth = 80;
    const long mapHeight = 80;

    float HALF_WIDTH = WIDTH / 2; // Half of the width
    float HALF_HEIGHT = HEIGHT / 2; // Half of the height
    int SCALE = 40;   // Scale factor for rendering
    const int FPS = 60;     // Frames per second
    const int MAX_DEPTH = 40;

    float px = 8.5f; // Initial player x position
    float py = 2.5f; // Initial player y position
    float zspeed = 0.3f;
    float pa = PI; // Player's angle
    float ps = 0.002; // Player's speed
    float pruns = 0.005; // Player's running speed
    float pss = 0.001; // Player's strafe speed
    float prs = 0.002; // Player's rotation speed
    float psize = 150.0f;

    float enemy_speed = 0.008f;
    float enemy_size = 60.0f;


    float DR = 0.0174533f; // Degrees to radians conversion factor
    float fov = 60*DR; // Field of view in radians
    float half_fov = fov / 2; // Half of the field of view
    const int colW = 4; // Width of each column in the raycasting
    const int num_rays = (int)(WIDTH / colW);
    int half_num_rays = (int)num_rays / 2;
    float delta_angle = fov / num_rays; // Angle between rays

    float SCREEN_DIST = HALF_WIDTH / tan(half_fov); // Distance from the player to the screen
    int texture_size = 512; // Size of the texture
    int floor_texture_size = 64;
    const int num_textures = 13; // Number of textures used in the game

    const float mouse_sensivity = 0.05f;


    const long num_weapons = 1;
    const long max_animation_frames = 10;
    const long max_animations = 10;

    int raycasting_textures = 12;

    int num_sprite_templates = 1;
    int num_npc_templates = 1;

}


#endif // SETTINGS_H
/**
Mariottini Matteo 4AI

TO DO:
*   IDEAS :)

DOING:
*   ...

IDEAS:
*   timer (300s ?)
*   more levels
*/

#include <iostream>
#include <ctime>
#include <vector>
#include <cstdio>
#include <cstring>
#include <string>
#include <fstream>
#include <process.h>
#include "vsgl2.h"

using namespace std;
using namespace vsgl2;
using namespace vsgl2::general;
using namespace vsgl2::video;
using namespace vsgl2::utils;
using namespace vsgl2::io;
using namespace vsgl2::audio;
using namespace vsgl2::ttf_fonts;

const int DIM = 64;
const int WIDTH = 704;
const int HEIGHT = 704;
const int MAX_X = (WIDTH / DIM) - (DIM*2 / DIM);
const int MAX_Y = (HEIGHT / DIM) - (DIM*2 / DIM);
const int N_LAUNCHERS = 9;
const int MAX_BULLETS = 9;
const int UPDATE_JUMP = 0;
const int UPDATE_MOVE = 20;

int max_time = 300;
int time_jump = ms_time();
int time_move = ms_time();
float dy = 0;
float dx = 0;
bool JUMP = false;
int c_dir;
int last_shot_time = 0;
int lives = 3;
float bullet_speed = 2;
int recharging_time = 700;
int launchers_alive = N_LAUNCHERS;

enum Movement {RIGHT, LEFT};

struct Character
{
    int dim;
    string image;
    int x;
    int y;
    int active;
};

struct Element
{
    int w;
    int h;
    string image;
    int x;
    int y;
    int active;
};

Character character;
Element platform;
Element launchers[N_LAUNCHERS];
Element bullets[MAX_BULLETS];

void init_character(string image)
{
    character.dim = DIM;
    character.x = WIDTH / 2 - DIM / 2;
    character.y = HEIGHT - 60 - DIM;
    character.image = image;
    character.active = 1;
}

void draw_character ()
{
    draw_image(character.image, character.x, character.y,
               character.dim, character.dim, 255);
}

void init_element(Element &e, int x, int y, int w, int h,
                   string image, int a)
{
    e.w = w;
    e.h = h;
    e.x = x;
    e.y = y;
    e.image = image;
    e.active = a;
}

void draw_element(Element e)
{
    draw_image(e.image, e.x, e.y,
               e.w, e.h, 255);
}

void moving_character(string image1, string image2)
{
    if (is_pressed(VSGL_A)){
        dx = -4;
        c_dir = LEFT;
        if (!JUMP) character.image = image2;
    }
    if (is_pressed(VSGL_D)){
        c_dir = RIGHT;
        dx = 4;
        if (!JUMP) character.image = image1;
    }
    if (ms_time() - time_move > UPDATE_MOVE){
        character.x += dx;
        dx = 0;
        time_move = ms_time();
    }
    //keep character between borders
    if (character.x > 598)  character.x = 598;
    if (character.x < 44)   character.x = 44;
}

void jump(string image3, string image4)
{
    if (!JUMP && is_pressed(VSGL_SPACE)){
        dy = -5;
        JUMP = true;
        play_sound("assets/sounds/super-mario_jump.wav");
    }
    if (JUMP && c_dir == RIGHT){
        character.image = image3;
    }
    if (JUMP && c_dir == LEFT){
        character.image = image4;
    }
    if (ms_time() - time_jump > UPDATE_JUMP){
        character.y += dy;
        dy += 0.1;
        time_jump = ms_time();
    }
    if (character.y > (HEIGHT - 60 - DIM)){
        character.y = HEIGHT - 60 - DIM;
        dy = 0;
        JUMP = false;
    }
}

void game_graphics()
{
    draw_image("assets/images/background.png", 0, 0, WIDTH, HEIGHT - DIM, 255);
    int t_y = DIM;
    for (int i = 0; i < MAX_Y; i++){
        draw_image("assets/images/tube.png", 5, t_y, DIM - 9, DIM, 255);
        t_y += DIM;
    }
    draw_image("assets/images/headed_tube.png", 0, 0, DIM, DIM, 255);
    t_y = DIM;
    for (int i = 0; i < MAX_Y; i++){
        draw_image("assets/images/tube.png", WIDTH - DIM + 5, t_y, DIM - 9, DIM, 255);
        t_y += DIM;
    }
    draw_image("assets/images/headed_tube.png", WIDTH - DIM, 0, DIM, DIM, 255);
    int g_x = 0;
    for (int i = 0; i < MAX_X + 2; i++){
        draw_image("assets/images/ground.png", g_x, HEIGHT - DIM, DIM, DIM, 255);
        g_x += DIM;
    }
    int l_x1 = WIDTH - DIM*3;
    for (int i = 0; i < 3; i++){
        draw_image("assets/images/mario_head.png", l_x1, (DIM/2) - (DIM/4), DIM/2, DIM/2, 50);
        l_x1 += (DIM/2) + 5;
    }
    int l_x2 = WIDTH - DIM*3;
    for (int i = 0; i < lives; i++){
        draw_image("assets/images/mario_head.png", l_x2, (DIM/2) - (DIM/4), DIM/2, DIM/2, 255);
        l_x2 += (DIM/2) + 5;
    }
    string n_launchers = "= " + std::to_string(launchers_alive);
    string time = std::to_string(max_time);
    draw_image("assets/images/small_launcher.png", (DIM*2) - 5, 11, 34/1.5, DIM/1.5, 255);
    draw_text("gamer.ttf", 50, n_launchers, WIDTH/4 - 15, 17, Color(0, 0, 0, 255)); //show number launchers still alive
    draw_text("gamer.ttf", 70, time ,(WIDTH/2) - 32, 0, Color(0, 0, 0, 255)); //countdown (300s)
    draw_filled_rect(DIM, DIM - 4, WIDTH - DIM*2, 4, Color(0, 0, 0, 255));
}

void init_platforms(int x, int y)
{
    init_element(platform, x, y, platform.w, platform.h, "assets/images/platform.jpg", 1);
}

void draw_platforms(int x, int y)
{
    platform.x = x;
    platform.y = y;
    draw_element(platform);
}

void on_platform(int x, int y)
{
    if (platform.active)
        if (character.x >= x - 40 && character.x <= x + platform.w - 29 &&
            character.y >= y - character.dim + 5 && character.y < y){
            character.y = y - character.dim + 5;
            dy = 0;
            JUMP = false;
        }
}

void init_launchers()
{
    for (int i = 0; i < N_LAUNCHERS; i++){
        launchers[i].x = (i+1)*DIM + 17;
        launchers[i].y = DIM;
        launchers[i].w = 34;
        launchers[i].h = DIM;
        launchers[i].image = "assets/images/launcher.png";
        launchers[i].active = 1;
    }
}

void draw_launchers(Element e[])
{
    for (int i = 0; i < N_LAUNCHERS; i++){
        if (e[i].active){
            draw_element(e[i]);
        }
    }
}

int launchers_collision()
{
    for (int i = 0; i < N_LAUNCHERS; i++)
        if (character.active && launchers[i].active &&
            character.y <= (launchers[i].y + launchers[i].h) &&
           (character.x + character.dim) >= launchers[i].x &&
            character.x <= (launchers[i].x + launchers[i].w)){
                return i;
            }
        return -1;
}

void update_launcher()
{
    int lc = launchers_collision();
    if (lc == -1)
        return;
    else
        launchers_alive--;
    launchers[lc].active = 0;

}

void init_bullets()
{
    for (int i = 0; i < MAX_BULLETS; i++){
        bullets[i].w = 30;
        bullets[i].h = 35;
        bullets[i].image = "assets/images/bullet.png";
        bullets[i].active = 0;
    }
}

void draw_bullet()
{
    for (int i = 0; i < MAX_BULLETS; i++)
        if (bullets[i].active)
            draw_element(bullets[i]);
}

bool recharged()
{
    if (ms_time() - last_shot_time > recharging_time)
        return true;
    return false;
}

int which_launcher_shooting()
{
    int l = rand()%launchers_alive + (N_LAUNCHERS - launchers_alive);
    int sum = 0;
    //check if at least one launcher is still alive
    for (int i = 0; i < N_LAUNCHERS; i++)
        sum += launchers[l].active;
    //launcher selected isn't alive
    if (sum == 0)
        return -1;
    while (!launchers[l].active)
        l = rand()%launchers_alive + (N_LAUNCHERS - launchers_alive);
    //return selected launcher
    cout << l << endl;
    return l;
}

void launcher_shot()
{
    if (!recharged())
        return;
    last_shot_time = ms_time();
    int l = which_launcher_shooting();
    if (l == -1)
        return;
    for (int i = 0; i < MAX_BULLETS; i++){
        if (!bullets[i].active && launchers[i].active){
            bullets[i].x = l * DIM + DIM + 19;
            bullets[i].y = launchers[0].y + launchers[0].h;
            bullets[i].active = 1;
            play_sound("assets/sounds/super-mario_shot.wav");
            return;
        }
    }
}

void update_bullet()
{
    for (int i = 0; i < MAX_BULLETS; i++)
        if (bullets[i].active)
            bullets[i].y += bullet_speed;
}

void bullets_collision(int x, int y)
{
    for (int i = 0; i < MAX_BULLETS; i++){
        //ground
        if (bullets[i].active &&
            bullets[i].y >= (HEIGHT - DIM - bullets[0].h))
                bullets[i].active = 0;
        //platforms
        if (bullets[i].active &&
            bullets[i].y >= y - bullets[0].h &&
            bullets[i].x >= x - bullets[0].w + 5 &&
            bullets[i].x <= x + platform.w - 5)
                bullets[i].active = 0;
        //character
        if (bullets[i].active &&
            bullets[i].y >= character.y - bullets[i].h &&
            bullets[i].y + bullets[i].h <= character.y + character.dim &&
            bullets[i].x >= character.x - bullets[i].w &&
            bullets[i].x + bullets[i].w <= character.x + character.dim){
                lives--;
                bullets[i].active = 0;
                init_character("assets/images/mario_right.png");
            }
    }
}

bool level_completed()
{
    if (launchers_alive == 0)
        return true;
    return false;
}

void game_over()
{
    game_graphics();
    draw_character();
    moving_character("assets/images/mario_right.png", "assets/images/mario_left.png");
    jump("assets/images/mario_right_jump.png", "assets/images/mario_left_jump.png");
    draw_text("gamer.ttf", DIM*3, "GAME", DIM*3, HEIGHT/2 - DIM*3, Color(255, 0, 0, 255));
    draw_text("gamer.ttf", DIM*3, "OVER", DIM*3 + 10, HEIGHT/2 - DIM, Color(255, 0, 0, 255));
    update();
}

int main(int argc, char* argv[])
{
    //init the library
    init();

    //set platforms dimensions
    platform.h = 14;
    platform.w = DIM;

    //set platforms coordinates
    int p1_x = 450;
    int p1_y = 500;
    int p2_x = 220;
    int p2_y = 420;
    int p3_x = 130;
    int p3_y = 290;
    int p4_x = 550;
    int p4_y = 350;
    int p5_x = 350;
    int p5_y = 270;
    int p6_x = 90;
    int p6_y = 510;

    set_window(WIDTH, HEIGHT,"Game"); //create the window and show it

    play_sound("assets/sounds/super-mario_its-me-mario.wav");
    play_music("assets/sounds/super-mario_theme.wav");

    init_character("assets/images/mario_right.png");
    init_platforms(p1_x, p1_y);
    init_launchers();
    init_bullets();

    lives = 3;

    srand(time(NULL));
    //main loop
    while (!done() && lives > 0){
        game_graphics();
        draw_character();
        moving_character("assets/images/mario_right.png", "assets/images/mario_left.png");
        jump("assets/images/mario_right_jump.png", "assets/images/mario_left_jump.png");
        draw_platforms(p1_x, p1_y);
        draw_platforms(p2_x, p2_y);
        draw_platforms(p3_x, p3_y);
        draw_platforms(p4_x, p4_y);
        draw_platforms(p5_x, p5_y);
        draw_platforms(p6_x, p6_y);
        on_platform(p1_x, p1_y);
        on_platform(p2_x, p2_y);
        on_platform(p3_x, p3_y);
        on_platform(p4_x, p4_y);
        on_platform(p5_x, p5_y);
        on_platform(p6_x, p6_y);
        draw_launchers(launchers);
        launchers_collision();
        update_launcher();
        launcher_shot();
        draw_bullet();
        update_bullet();
        bullets_collision(p1_x, p1_y);
        bullets_collision(p2_x, p2_y);
        bullets_collision(p3_x, p3_y);
        bullets_collision(p4_x, p4_y);
        bullets_collision(p5_x, p5_y);
        bullets_collision(p6_x, p6_y);
        /**
        if(level_completed()){
            init_launchers();
            init_bullets();
            JUMP = false;
            lives = 3;
            recharging_time -= 50;
            bullet_speed += 0.5;
            launchers_alive == N_LAUNCHERS;
        }*/
        update();
    }
    stop_music();
    play_sound("assets/sounds/super-mario_game-over.wav");
    while (!done())
        game_over();
    close();
    return 0;
}

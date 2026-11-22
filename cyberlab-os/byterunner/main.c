/*
 * CyberLab OS - ByteRunner Retro Arcade Game (SDL2)
 * Author: Vishnu C (25BTRCL282) - JAIN (Deemed-to-be University)
 *
 * Player controls a data packet runner dodging malicious firewall traps
 * and collecting memory cache fragments across overclocked lanes.
 */

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define PLAYER_SIZE 32
#define MAX_OBSTACLES 8
#define MAX_FRAGMENTS 6

typedef struct {
    float x, y;
    float vy;
    bool is_grounded;
} Player;

typedef struct {
    float x, y;
    float w, h;
    float speed;
    bool active;
} Entity;

static int load_high_score(void) {
    FILE *f = fopen("highscore.dat", "r");
    if (!f) return 0;
    int s = 0;
    fscanf(f, "%d", &s);
    fclose(f);
    return s;
}

static void save_high_score(int score) {
    FILE *f = fopen("highscore.dat", "w");
    if (f) {
        fprintf(f, "%d", score);
        fclose(f);
    }
}

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "CyberLab OS :: ByteRunner Arcade v1.0",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN
    );
    if (!window) {
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    srand(time(NULL));

    Player player = { 100, 450, 0, true };
    Entity obstacles[MAX_OBSTACLES];
    Entity fragments[MAX_FRAGMENTS];

    for (int i = 0; i < MAX_OBSTACLES; i++) {
        obstacles[i].active = true;
        obstacles[i].x = 800 + i * 220;
        obstacles[i].y = 480 - (rand() % 40);
        obstacles[i].w = 28;
        obstacles[i].h = 40;
        obstacles[i].speed = 5.0f;
    }

    for (int i = 0; i < MAX_FRAGMENTS; i++) {
        fragments[i].active = true;
        fragments[i].x = 900 + i * 180;
        fragments[i].y = 380 - (rand() % 60);
        fragments[i].w = 16;
        fragments[i].h = 16;
        fragments[i].speed = 5.0f;
    }

    int score = 0;
    int high_score = load_high_score();
    bool running = true;
    bool paused = false;
    bool game_over = false;

    Uint32 last_time = SDL_GetTicks();

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                } else if (event.key.keysym.sym == SDLK_p) {
                    paused = !paused;
                } else if (event.key.keysym.sym == SDLK_SPACE || event.key.keysym.sym == SDLK_UP) {
                    if (game_over) {
                        // Restart
                        score = 0;
                        game_over = false;
                        player.y = 450;
                        player.vy = 0;
                        player.is_grounded = true;
                        for (int i = 0; i < MAX_OBSTACLES; i++) {
                            obstacles[i].x = 800 + i * 220;
                        }
                    } else if (player.is_grounded && !paused) {
                        player.vy = -14.0f;
                        player.is_grounded = false;
                    }
                }
            }
        }

        if (!paused && !game_over) {
            // Physics
            player.y += player.vy;
            player.vy += 0.8f; // Gravity
            if (player.y >= 450) {
                player.y = 450;
                player.vy = 0;
                player.is_grounded = true;
            }

            // Update Obstacles
            for (int i = 0; i < MAX_OBSTACLES; i++) {
                obstacles[i].x -= obstacles[i].speed;
                if (obstacles[i].x < -50) {
                    obstacles[i].x = SCREEN_WIDTH + (rand() % 150);
                    score += 10;
                    if (score > high_score) {
                        high_score = score;
                        save_high_score(high_score);
                    }
                }

                // Collision Check
                if (player.x + PLAYER_SIZE > obstacles[i].x &&
                    player.x < obstacles[i].x + obstacles[i].w &&
                    player.y + PLAYER_SIZE > obstacles[i].y &&
                    player.y < obstacles[i].y + obstacles[i].h) {
                    game_over = true;
                }
            }

            // Update Memory Fragments
            for (int i = 0; i < MAX_FRAGMENTS; i++) {
                fragments[i].x -= fragments[i].speed;
                if (fragments[i].x < -50) {
                    fragments[i].x = SCREEN_WIDTH + (rand() % 200);
                    fragments[i].active = true;
                }

                if (fragments[i].active &&
                    player.x + PLAYER_SIZE > fragments[i].x &&
                    player.x < fragments[i].x + fragments[i].w &&
                    player.y + PLAYER_SIZE > fragments[i].y &&
                    player.y < fragments[i].y + fragments[i].h) {
                    fragments[i].active = false;
                    score += 25;
                }
            }
        }

        // Render Frame
        // Background: Deep Charcoal (#0d0e15)
        SDL_SetRenderDrawColor(renderer, 13, 14, 21, 255);
        SDL_RenderClear(renderer);

        // Grid lines (Futuristic Cyber Aesthetic)
        SDL_SetRenderDrawColor(renderer, 30, 34, 53, 255);
        for (int y = 0; y < SCREEN_HEIGHT; y += 40) {
            SDL_RenderDrawLine(renderer, 0, y, SCREEN_WIDTH, y);
        }

        // Ground Line (Electric Blue)
        SDL_SetRenderDrawColor(renderer, 0, 240, 255, 255);
        SDL_Rect ground = { 0, 482, SCREEN_WIDTH, 4 };
        SDL_RenderFillRect(renderer, &ground);

        // Player (Electric Purple #7928CA)
        SDL_SetRenderDrawColor(renderer, 121, 40, 202, 255);
        SDL_Rect pRect = { (int)player.x, (int)player.y, PLAYER_SIZE, PLAYER_SIZE };
        SDL_RenderFillRect(renderer, &pRect);

        // Obstacles: Red Firewall Traps
        SDL_SetRenderDrawColor(renderer, 255, 50, 75, 255);
        for (int i = 0; i < MAX_OBSTACLES; i++) {
            SDL_Rect obs = { (int)obstacles[i].x, (int)obstacles[i].y, (int)obstacles[i].w, (int)obstacles[i].h };
            SDL_RenderFillRect(renderer, &obs);
        }

        // Fragments: Cyan Memory Packets
        SDL_SetRenderDrawColor(renderer, 0, 240, 255, 255);
        for (int i = 0; i < MAX_FRAGMENTS; i++) {
            if (fragments[i].active) {
                SDL_Rect frag = { (int)fragments[i].x, (int)fragments[i].y, (int)fragments[i].w, (int)fragments[i].h };
                SDL_RenderFillRect(renderer, &frag);
            }
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16); // ~60 FPS
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

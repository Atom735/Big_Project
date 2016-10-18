#ifndef H_C_TEST
#define H_C_TEST

#include <SDL.h>
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")

#include <cstdio>
#include <cstdlib>

void rTestInit(SDL_Renderer *r);
void rTestRelease();
void rTest(SDL_Renderer *r, SDL_Scancode sc=SDL_SCANCODE_UNKNOWN);

#endif

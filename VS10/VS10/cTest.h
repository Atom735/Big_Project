#ifndef H_C_TEST
#define H_C_TEST

#include <SDL.h>
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")
#include <ft2build.h>
#ifdef _DEBUG
#pragma comment(lib, "freetype27d.lib")
#else
#pragma comment(lib, "freetype27.lib")
#endif
#include <cstdio>
#include <cstdlib>

void rTest(SDL_Renderer *r);

#endif

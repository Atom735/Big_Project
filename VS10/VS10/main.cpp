#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")

#include <SDL.h>
#include <cstdio>
#include <cstdlib>
#include <vector>
#define _USE_MATH_DEFINES
#include <cmath>

SDL_Window    *g_Window            = 0;
SDL_Surface   *g_ScreenSurface     = 0;
SDL_Thread    *g_ThreadVideo       = 0;
SDL_Thread    *g_ThreadVideoSwaper = 0;
int           *g_ScreenBufferBack  = 0;
int           *g_ScreenBufferFront = 0;
typedef void (* gVideoFunc) (void);
std::vector<gVideoFunc>
               g_VideoList;

float r_Time()
{return ((float)(SDL_GetPerformanceCounter()))/((float)(SDL_GetPerformanceFrequency()));}

void r_SreenBuffersSwap()
{int *b=g_ScreenBufferBack;g_ScreenBufferBack=g_ScreenBufferFront;g_ScreenBufferFront=b;}

void r_ScreenBufferClear()
{memset(g_ScreenBufferBack, 2048*2048*sizeof(int), 0);}

void r_ScreenLine()
{
	for(int y=0;y<600;y++)
		for(int x=0;x<800;x++)
			g_ScreenBufferBack[y*2048+x]=0xff*(!(int(r_Time())&1));
}

int r_ThreadVideoMain(void *ptr)
{
	int *run = (int*)(ptr);	
	while(*run)
	{
		for(int i=0; i<g_VideoList.size(); i++)
			g_VideoList[i]();		
		r_SreenBuffersSwap();
	}
	return 0;
}
int r_ThreadVideoSwaperMain(void *ptr)
{
	int *run = (int*)(ptr);
	g_ScreenBufferBack = (int*)malloc(2048*2048*sizeof(int));
	g_ScreenBufferFront = (int*)malloc(2048*2048*sizeof(int));
	r_ScreenBufferClear();
	r_SreenBuffersSwap();
	r_ScreenBufferClear();
	g_VideoList.clear();
	g_VideoList.push_back(r_ScreenBufferClear);
	g_VideoList.push_back(r_ScreenLine);
	g_ThreadVideo = SDL_CreateThread(r_ThreadVideoMain, "ThreadVideoDrawer", ptr);
	while(*run)
	{
		g_ScreenSurface = SDL_GetWindowSurface( g_Window );
		int w=g_ScreenSurface->w;
		int h=g_ScreenSurface->h;
		int *p=(int*)(g_ScreenSurface->pixels);
		SDL_LockSurface(g_ScreenSurface);
		for(int y=0;y<h;y++)
		{
			memcpy(p+y*w, g_ScreenBufferFront+2048*y, sizeof(int)*w);
		}
		SDL_UnlockSurface(g_ScreenSurface);
		SDL_UpdateWindowSurface( g_Window );
	}
	SDL_WaitThread(g_ThreadVideo,run);
	free(g_ScreenBufferFront);
	free(g_ScreenBufferBack);
	g_ScreenBufferBack=0;
    return 0;
}


int wmain(int argc, char const *argv[])
{
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_EVENTS ) < 0 )
	{
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );		
		abort();
		exit(-1);
    }

	g_Window = SDL_CreateWindow( "My SDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
		200, 200, SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE );
    if( g_Window == NULL )
    {
        printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
		abort();
		exit(-1);
    }


	int l_ReturnValue=1;
	g_ThreadVideoSwaper = SDL_CreateThread(r_ThreadVideoSwaperMain, "ThreadVideoSwaper", (void*)(&l_ReturnValue));

	SDL_Event e;
	while(l_ReturnValue)
	{
		while(SDL_PollEvent(&e))
		{
			if (e.type == SDL_WINDOWEVENT)
			{
				switch (e.window.event)
				{
					case SDL_WINDOWEVENT_CLOSE:
						l_ReturnValue = 0;
						break;
				};
			}
		}
	}

	SDL_WaitThread(g_ThreadVideoSwaper,&l_ReturnValue);
	SDL_UpdateWindowSurface( g_Window );
    SDL_DestroyWindow( g_Window );
    SDL_Quit();
	return 0;
}
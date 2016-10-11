#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")

#include <SDL.h>
#include <cstdio>
#include <cstdlib>
#define _USE_MATH_DEFINES
#include <cmath>

SDL_Window    *g_Window           = 0;
SDL_Surface   *g_ScreenSurface    = 0;
SDL_Thread    *g_ThreadVideo      = 0;

static double r_Time()
{
	return ((double)(SDL_GetPerformanceCounter()))/((double)(SDL_GetPerformanceFrequency()));
}

float exponent = 1.0;
float rings = 10.0;
float speed = 10.1;
float time = 0;


static int r_Color(int ix, int iy, const float iw, const float ih)
{
	float x = (((((float)(ix))+0.5f)/((float)(iw)))-0.5f)*2.0f;
	float y = (((((float)(iy))+0.5f)/((float)(ih)))-0.5f)*2.0f;
	
	float ang = atan2(x, y);
	float rad = pow(x*x+y*y, exponent);

	return int((0.5f * (1.0f + sin(ang + rings * rad + speed * time)))*255.0f);
}


static int r_ThreadVideoMain(void *ptr)
{
	while(*(int*)(ptr))
	{
		g_ScreenSurface = SDL_GetWindowSurface( g_Window );
		int w=g_ScreenSurface->w;
		int h=g_ScreenSurface->h;
		int *p=(int*)(g_ScreenSurface->pixels);
		
		float time1 = r_Time();
		time = r_Time();
		SDL_LockSurface(g_ScreenSurface);
		for(int y=0;y<h && (time1+0.5f > time);y++)
		{
			if(y%3==2)
			{
				SDL_UnlockSurface(g_ScreenSurface);
				SDL_UpdateWindowSurface( g_Window );
				SDL_LockSurface(g_ScreenSurface);
				time = r_Time();
			}
			for(int x=0;x<w;x++)
			{
				p[y*w+x] = r_Color(x,y,w,h);
			}
		}
		SDL_UnlockSurface(g_ScreenSurface);
		SDL_UpdateWindowSurface( g_Window );
	}
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
	g_ThreadVideo = SDL_CreateThread(r_ThreadVideoMain, "ThreadVideos", (void*)(&l_ReturnValue));

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

	SDL_WaitThread(g_ThreadVideo,&l_ReturnValue);
	SDL_UpdateWindowSurface( g_Window );
    SDL_DestroyWindow( g_Window );
    SDL_Quit();
	return 0;
}
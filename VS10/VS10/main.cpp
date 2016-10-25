#include "cGame.h"
#include <Windows.h>

#include "cFT.h"
#include "c2048.h"
#include "cTetris.h"

SDL_Window      *g_Window        = 0;
SDL_Renderer    *g_Renderer      = 0;

//cGameTetris g_Tetris;
cGameBase       *g_Game = 0;

INT APIENTRY WinMain(HINSTANCE hInst,HINSTANCE hPrev,LPSTR cmd,INT showcmd)
{
	AllocConsole();
	if(SDL_Init(SDL_INIT_VIDEO))
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
		abort();
	}
	if (SDL_CreateWindowAndRenderer(1024, 768, SDL_WINDOW_SHOWN, &g_Window, &g_Renderer))
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window and renderer: %s", SDL_GetError());
		abort();
    }

	rFT_Init(g_Renderer);

	g_Game = new cGame2048();
	g_Game->Init(g_Renderer);

    SDL_Event e;
	while (1) 
	{
		SDL_PumpEvents();
        while(SDL_PeepEvents(&e,1,SDL_GETEVENT,SDL_FIRSTEVENT,SDL_LASTEVENT))
		{			
			if (e.type == SDL_QUIT)
			{
				goto GOTO_LEAVE_LOOP;
			}
			if(e.type == SDL_KEYDOWN)
			{
				g_Game->Keyboard(e.key.keysym.scancode);
			}
		}
		SDL_SetRenderDrawColor(g_Renderer, 0x00, 0x00, 0x00, 0x00);
		SDL_RenderClear(g_Renderer);
		g_Game->Draw(g_Renderer);
		SDL_RenderPresent(g_Renderer);
    }
GOTO_LEAVE_LOOP:
	g_Game->Release();
	delete g_Game;
	rFT_Release();

	SDL_DestroyRenderer(g_Renderer);
	SDL_DestroyWindow(g_Window);
	SDL_Quit();
	//system("pause");
	return 0;
}
#include "cTetris.h"
#include <Windows.h>

SDL_Window      *g_Window        = 0;
SDL_Renderer    *g_Renderer      = 0;

cGameTetris g_Tetris;


INT APIENTRY WinMain(HINSTANCE hInst,HINSTANCE hPrev,LPSTR cmd,INT showcmd)
{
	AllocConsole();
	if(SDL_Init(SDL_INIT_VIDEO))
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
		abort();
	}
	if (SDL_CreateWindowAndRenderer(800, 600, SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE, &g_Window, &g_Renderer))
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window and renderer: %s", SDL_GetError());
		abort();
    }

	g_Tetris.StartNew(g_Renderer);

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
				switch(e.key.keysym.scancode)
				{
				case SDL_SCANCODE_LEFT:
					g_Tetris.StepMove(-1);
					break;
				case SDL_SCANCODE_RIGHT:
					g_Tetris.StepMove(1);
					break;
				case SDL_SCANCODE_UP:
					g_Tetris.StepMove(0,-1);
					break;
				case SDL_SCANCODE_DOWN:
					g_Tetris.StepMove(0,1);
					break;
				case SDL_SCANCODE_F2:
					g_Tetris.StartNew(g_Renderer);
					break;
				};
			}
		}

		g_Tetris.StepMove();
		SDL_SetRenderDrawColor(g_Renderer, 0x00, 0x00, 0x00, 0x00);
		SDL_RenderClear(g_Renderer);
		g_Tetris.Draw(g_Renderer);
		SDL_RenderPresent(g_Renderer);
    }
	GOTO_LEAVE_LOOP:
	g_Tetris.Close();

	SDL_DestroyRenderer(g_Renderer);
	SDL_DestroyWindow(g_Window);
	SDL_Quit();
	//system("pause");
	return 0;
}
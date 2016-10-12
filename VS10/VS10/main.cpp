#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")

#include <SDL.h>
#include <Windows.h>
#include <cstdio>

SDL_Window      *g_Window        = 0;
SDL_Renderer    *g_Renderer      = 0;
SDL_Surface     *g_BlockSurface  = 0;
SDL_Texture     *g_BlockTexture  = 0;
const char      *g_BlockFileName = "..\\block.bmp";
const char      *g_FigureFileName= "..\\blocks.txt";
int              g_BlockSize     = 32;
int              g_TetrisW       = 10;
int              g_TetrisH       = 16;

struct cBlockFigure
{
	int Id, IdL, IdR;
	char name[64];
	int cR, cG, cB;
	int xL, yL; // смещение при вращении
	int xR, yR; // смещение при вращении
	int size; //  оличество блоков
	int *x, *y; // ѕозиции блоков
	cBlockFigure()
	{
		size = 0;
		x = 0;
		y = 0;
		cR=255;cG=255;cB=255;
	}
	~cBlockFigure()
	{
		size = 0;
		if(x) delete[] x; x=0;
		if(y) delete[] y; y=0;
	}
	void Draw(SDL_Renderer *r, int x, int y, float f=0.f)
	{
		SDL_Rect rc={x,y-int(f*float(g_BlockSize)),g_BlockSize,g_BlockSize};
		SDL_SetTextureColorMod(g_BlockTexture,cR,cG,cB);
		SDL_RenderCopy(r, g_BlockTexture, 0, &rc);
	}
};


struct cGameTetris
{
	int FiguresN;
	cBlockFigure *pFigures;
	int Fx, Fy;
	int w, h;
	int *Map;
	char *GetFirstNumber(char *str)
	{
		while(*str)
		{
			if((*str>='0' && *str<='9') || *str=='-') return str;
			if(*str=='#')
			{
				for(;*str!='\n';str++)
					if(!(*str)) return 0;
			}
			str++;
		}
		return 0;
	}
	int GetNumber(char **s)
	{
		char *str=*s;
		if(!(str = GetFirstNumber(str))) return 0;
		bool minus = (*str == '-');
		if(minus) str++;
		int n=0;
		while((*str>='0' && *str<='9'))
		{
			n*=10;
			n+=*str-'0';
			str++;
		}
		*s=str;
		if(minus) return -n;
		return n;
	}
	cGameTetris()
	{
		Map=0;
		char fs[16384];
		{
			FILE * pFile;
			pFile = fopen (g_FigureFileName , "rb");
			if (pFile == NULL) {perror ("Error opening file");abort();}
			int i=0;
			if ( (i=fread  (fs , 1, 16384 , pFile)) != 0 )
			fs[i]=0;
			fclose (pFile);
		}
		char *sn=GetFirstNumber(fs);
		FiguresN=GetNumber(&sn);
		pFigures=new cBlockFigure[FiguresN];
		for(int i=0; i<FiguresN; i++)
		{
			pFigures[i].Id = GetNumber(&sn);
			pFigures[i].IdL = GetNumber(&sn);
			pFigures[i].IdR = GetNumber(&sn);
			pFigures[i].xL = GetNumber(&sn);
			pFigures[i].yL = GetNumber(&sn);
			pFigures[i].xR = GetNumber(&sn);
			pFigures[i].yR = GetNumber(&sn);

			pFigures[i].cR = GetNumber(&sn);
			pFigures[i].cG = GetNumber(&sn);
			pFigures[i].cB = GetNumber(&sn);

			pFigures[i].size = GetNumber(&sn);
			pFigures[i].x = new int[pFigures[i].size];
			pFigures[i].y = new int[pFigures[i].size];
			for(int j=0; j<pFigures[i].size; j++)
			{
				pFigures[i].x[j] = GetNumber(&sn);
				pFigures[i].y[j] = GetNumber(&sn);
			}
		}
	}
	~cGameTetris()
	{
		if(pFigures) delete[] pFigures; pFigures=0;
		if(Map) delete[] Map;Map=0;
	}
	int ReSize(int W, int H)
	{
		if(Map) delete[] Map;Map=0;
		w=W;h=H;
		Map=new int[w*h];
		for(int i=0; i<w*h;i++)
			Map[i]=0x8000;
	}
	cBlockFigure *GetFigure(int ID)
	{
		for(int i=0; i<FiguresN; i++)
			if(pFigures[i].Id==ID) 
				return pFigures+i;
		return 0;
	}
	void Draw(SDL_Renderer *r)
	{
		SDL_Rect rc={0,0,g_BlockSize,g_BlockSize};
		SDL_SetRenderDrawColor(r, 0x00, 0x00, 0x00, 0x00);
		SDL_RenderClear(r);
		SDL_SetTextureColorMod(g_BlockTexture,0xff,0xff,0xff);
		for(int i=0;i<w+2;i++)
		{
			rc.x=i*g_BlockSize;
			rc.y=0;
			SDL_RenderCopy(r, g_BlockTexture, 0, &rc);
			rc.x=i*g_BlockSize;
			rc.y=(h+1)*g_BlockSize;
			SDL_RenderCopy(r, g_BlockTexture, 0, &rc);
		}	
		for(int i=0;i<h;i++)
		{
			rc.x=0;
			rc.y=(i+1)*g_BlockSize;
			SDL_RenderCopy(r, g_BlockTexture, 0, &rc);
			rc.x=(w+1)*g_BlockSize;
			rc.y=(i+1)*g_BlockSize;
			SDL_RenderCopy(r, g_BlockTexture, 0, &rc);
		}
		if(Map)
		{
			for(int y=0;y<h;y++)
			for(int x=0;x<w;x++)
			{
				if(!(Map[y*w+x]&0x8000))
				{
					cBlockFigure *f=GetFigure(Map[y*w+x]);
					if(f)
						SDL_SetTextureColorMod(g_BlockTexture,f->cR,f->cG,f->cB);
					else
						SDL_SetTextureColorMod(g_BlockTexture,0xff,0xff,0xff);
						
					rc.x=(x+1)*g_BlockSize;
					rc.y=(y+1)*g_BlockSize;
					SDL_RenderCopy(r, g_BlockTexture, 0, &rc);
				}
			}
		}
	}
};

cGameTetris g_Tetris;

void r_Draw(SDL_Renderer *r)
{	
	
    SDL_RenderPresent(r);
}

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
	if (!(g_BlockSurface = SDL_LoadBMP(g_BlockFileName))) 
	{
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create surface from image \"%s\": %s", g_BlockFileName, SDL_GetError());
		abort();
    }	
    if (!(g_BlockTexture = SDL_CreateTextureFromSurface(g_Renderer, g_BlockSurface))) 
	{
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture from surface: %s", SDL_GetError());
        abort();
    }
    SDL_FreeSurface(g_BlockSurface);

    SDL_Event e;
	while (1) {
        SDL_PollEvent(&e);
        if (e.type == SDL_QUIT) 
		{
            break;
        }
		r_Draw(g_Renderer);
    }
	


	SDL_DestroyTexture(g_BlockTexture);
	SDL_DestroyRenderer(g_Renderer);
	SDL_DestroyWindow(g_Window);
	SDL_Quit();
	system("pause");
	return 0;
}
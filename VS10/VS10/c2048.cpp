#include "c2048.h"

const char      *g_2048_BlockFileName = "2048_0.bmp";
unsigned int     g_2048_BlockSize = 8;
unsigned int     g_2048_BlockSizes = 8;
unsigned int     g_2048_TileSize = 108;
unsigned int     g_2048_BarSize = 14;
unsigned int     g_2048_Colors[] = {
0xFAF8EF, // 0-BG
0xBBADA0, // 1-Grid
0x776E65, // 2-TextDark
0xF9F6F2, // 3-TextLight
0xCDC1B4, // 0-NULL Tile
0xEEE4DA, // 1-[2]
0xEDE0C8, // 2-[4]
0xF2B179, // 3-[8]
};
unsigned int *g_2048_TileColors = g_2048_Colors+4;

struct cG2048_Tile : public ZeroedMemoryAllocator
{
	int type;
	int pos;
	float x, y, s;
	cG2048_Tile(int Pos, int Type)
	{
		pos=Pos;
		type=Type;
	}
};


// Spawn, Move, Merge
struct cG2048_Animation : public ZeroedMemoryAllocator
{
	int type;
#define G2048_A_SPAWN 0x01
	cG2048_Tile *pTile;
	float x1,y1,s1,x2,y2,s2;
	int start;
	int time;
	cG2048_Animation(int itype, cG2048_Tile *pt, 
		float X1, float Y1, float S1, 
		float X2, float Y2, float S2,
		int istart, int itime)
	{
		type=itype;
		pTile=pt;
		x1=X1;
		y1=Y1;
		s1=S1;
		x2=X2;
		y2=Y2;
		s2=S2;
		start=istart;
		time=itime;
	}
	void Step(int tick)
	{
		if(!pTile) return;
		float a= float(tick-start)/float(time);
		if(a<0.f) a=0.f; if(a>1.f) a=1.f;
		float b=1.f-a;

		pTile->x=x1*b+x2*a;
		pTile->y=y1*b+y2*a;
		pTile->s=s1*b+s2*a;
	}
	int End(int tick)
	{return tick-start>time;	}
};


void cGame2048::Init( SDL_Renderer *r )
{
	if(TextureBlock) SDL_DestroyTexture(TextureBlock);

	SDL_Surface *Surf = 0;
	if (!(Surf = SDL_LoadBMP(g_2048_BlockFileName))) 
	{
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create surface from image \"%s\": %s", g_2048_BlockFileName, SDL_GetError());
		abort();
    }	

	SDL_Surface *surface = SDL_CreateRGBSurface(0, Surf->w, Surf->h, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
	
	SDL_LockSurface(Surf);
	SDL_LockSurface(surface);

	unsigned char *s=(unsigned char*)(Surf->pixels);
	int *d=(int*)(surface->pixels);

	for(int i=0;i<Surf->w*Surf->h;i++)
		d[i]=s[i*3]|0xffffff00;

	SDL_UnlockSurface(Surf);
	SDL_UnlockSurface(surface);
    SDL_FreeSurface(Surf);

    if (!(TextureBlock = SDL_CreateTextureFromSurface(r, surface))) 
	{
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture from surface: %s", SDL_GetError());
        abort();
    }
    SDL_FreeSurface(surface);

	srand(SDL_GetTicks());

	x=y=16;
	W = 4;
	if(Map) delete[] Map;
	Map=0;
	Map = new cG2048_Tile*[W*W];
	memset(Map,0,W*W*sizeof(cG2048_Tile*));

	SpawnNew();
	SpawnNew();
}

void cGame2048::GameOver()
{
	abort();
}

void cGame2048::SpawnNew()
{
	int type=1;
	srand(rand());
	if((rand()%100) < 13) type=2;
	srand(rand());
	int sz=W*W;
	for(int i=0;i<W*W;i++)
		if(Map[i]) sz--;
	if(sz <= 0) GameOver();
	if(sz == 1)
		for(int i=0; i<W*W; i++)
			if(!Map[i]) {abort();return;}
	int j=rand()%sz+1;
	for(int i=0; i<W*W; i++)
		if(!Map[i]) {j--;if(j<=0) {AnimAddSpawn(i, type);return;}}
}

void cGame2048::AnimAddSpawn(int pos, int type)
{
	cG2048_Tile  *pt=new cG2048_Tile(pos, type);
	float X=float((g_2048_BarSize+g_2048_TileSize)*(pos%W));
	float Y=float((g_2048_BarSize+g_2048_TileSize)*(pos/W));

	cG2048_Animation *pa=new cG2048_Animation(G2048_A_SPAWN, pt, 
		X+float(g_2048_TileSize)*0.5f, Y+float(g_2048_TileSize)*0.5f, 0.f, 
		X, Y, 1.f,
		SDL_GetTicks(), 300);

	v.push_back(pt);
	Av.push_back(pa);
	Map[pos]=pt;
	pt=0;pa=0;
}


void cGame2048::Release()	
{
	if(Map) delete[] Map;
	Map=0;
	if(TextureBlock) SDL_DestroyTexture(TextureBlock);
	TextureBlock=0;
}

void cGame2048::DrawBlock( SDL_Rect rc, int Cid, unsigned int color)
{
	if(rc.w < g_2048_BlockSizes*2) return;
	if(Cid >= 0) color=g_2048_Colors[Cid];
	SDL_SetTextureColorMod(TextureBlock,(color>>16)&0xff,(color>>8)&0xff,(color)&0xff);

	for(int i=0; i<4; i++)
	{
		SDL_Rect rcs={
			g_2048_BlockSize*(i%2), g_2048_BlockSize*(i/2),
			g_2048_BlockSize,g_2048_BlockSize};
		SDL_Rect rcd={
			rc.x+(rc.w-g_2048_BlockSizes)*(i%2), rc.y+(rc.h-g_2048_BlockSizes)*(i/2),
			g_2048_BlockSizes,g_2048_BlockSizes};
		SDL_RenderCopy(R, TextureBlock, &rcs, &rcd);

		if(i%2)
		{
			SDL_Rect rcs2={
					g_2048_BlockSize-1, g_2048_BlockSize*(i/2),
					2 ,g_2048_BlockSize};
			SDL_Rect rcd2={
				rc.x+g_2048_BlockSizes, rc.y+(rc.h-g_2048_BlockSizes)*(i/2),
				rc.w-2*g_2048_BlockSizes,g_2048_BlockSizes};
			SDL_RenderCopy(R, TextureBlock, &rcs2, &rcd2);
		}
		else
		{
			SDL_Rect rcs2={
					g_2048_BlockSize*(i/2), g_2048_BlockSize-1,
					g_2048_BlockSize, 2};
			SDL_Rect rcd2={
				rc.x+(rc.w-g_2048_BlockSizes)*(i/2), rc.y+g_2048_BlockSizes,
				g_2048_BlockSizes,rc.h-2*g_2048_BlockSizes};
			SDL_RenderCopy(R, TextureBlock, &rcs2, &rcd2);
		}
	}
	{
		SDL_Rect rcs={
				g_2048_BlockSize-1, g_2048_BlockSize-1,
				2 ,2};
		SDL_Rect rcd={
			rc.x+g_2048_BlockSizes, rc.y+g_2048_BlockSizes,
			rc.w-2*g_2048_BlockSizes,rc.h-2*g_2048_BlockSizes};
		SDL_RenderCopy(R, TextureBlock, &rcs, &rcd);
	}
}
void cGame2048::Draw( SDL_Renderer *r )	
{
	if(!TextureBlock) return;
	if(r) R=r;
	r=R;

	int tick = SDL_GetTicks();
	for(int i=0; i<Av.size(); i++)
		if(Av[i]) 
		{
			Av[i]->Step(tick);
			if(Av[i]->End(tick))
			{
				delete Av[i];
				Av[i]=0;
			}
		}

	SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(TextureBlock, SDL_BLENDMODE_BLEND);

	{
		unsigned int color=g_2048_Colors[0];
		SDL_SetRenderDrawColor(r, (color>>16)&0xff,(color>>8)&0xff,(color)&0xff, 0x00);
		SDL_RenderClear(r);
	}
	{
		g_2048_BlockSizes=8;
		SDL_Rect rcs={x,y,g_2048_TileSize*4+g_2048_BarSize*5,g_2048_TileSize*4+g_2048_BarSize*5};
		DrawBlock(rcs, 1);
		g_2048_BlockSizes=4;
	}
	for(int iy=0;iy<4;iy++)
	for(int ix=0;ix<4;ix++)
	{
		SDL_Rect rcs={x+g_2048_BarSize+(g_2048_BarSize+g_2048_TileSize)*ix,y+g_2048_BarSize+(g_2048_BarSize+g_2048_TileSize)*iy,
			g_2048_TileSize,g_2048_TileSize};
		DrawBlock(rcs, -1, g_2048_TileColors[0]);
	}

	if(v.empty()) return;
	for(int i=0; i<v.size(); i++)
	{
		int sz=int(float(g_2048_TileSize)*v[i]->s);
		SDL_Rect rcs={
			x+g_2048_BarSize+int(v[i]->x),
			y+g_2048_BarSize+int(v[i]->y),
			sz,sz};
		DrawBlock(rcs, -1, g_2048_TileColors[v[i]->type]);
	}
}
void cGame2048::Keyboard( SDL_Scancode sc )
{
}
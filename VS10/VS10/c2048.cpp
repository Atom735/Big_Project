#include "c2048.h"
#

const char      *g_2048_BlockFileName = "2048_0.bmp";
int              g_2048_BlockSize = 8;
int              g_2048_BlockSizes = 8;
int              g_2048_TileSize = 108;
int              g_2048_BarSize = 14;

unsigned int     g_2048_Colors[] = {
0xFAF8EF, // 0-BG
0xBBADA0, // 1-Grid
0x776E65, // 2-TextDark
0xF9F6F2, // 3-TextLight
0xCDC1B4, // 0-NULL Tile
0xEEE4DA, // 1-[2]
0xEDE0C8, // 2-[4]
0xF2B179, // 3-[8]
0xF59563, // 4-[16]
0xF77C61, // 5-[32]
0xF45E3C, // 6-[64]
0xEDCE72, // 7-[128]
0xEDCB61, // 8-[256]
0xECC750, // 9-[512]
0xECC440, // 10-[1024]
0xEDC12C, // 11-[2048]
0xFF3D3E, // 12-[4096]
0xFF1E1F, // 13-[8192]
0xFF1E1F, // 14-[16384]
0xFF1E1F, // 15-[32768]
0xFF1E1F, // 16-[32768]
0xFF1E1F, // 17-[32768]
0xFF1E1F, // 18-[32768]
0xFF1E1F, // 19-[32768]
};

wchar_t         *g_2048_Texts[] = {
L"Empty Text", // EmptyText
L" ",     // 0-NULL Tile
L"2",     // 1-[2]
L"4",     // 2-[4]
L"8",     // 3-[8]
L"16",    // 4-[16]
L"32",    // 5-[32]
L"64",    // 6-[64]
L"128",   // 7-[128]
L"256",   // 8-[256]
L"512",   // 9-[512]
L"1024",  // 10-[1024]
L"2048",  // 11-[2048]
L"4096",  // 12-[4096]
L"8192",  // 13-[8192]
L"16.4k", // 14-[16384]
L"32.8k", // 15-[32768]
L"65.6k", // 16-[32768]
L"131k",  // 17-[32768]
L"262k",  // 18-[32768]
L"524k",  // 19-[32768]
L"1kk",   // 20-[32768]
};

int              g_2048_TextsSizes[] = {
12, // EmptyText
12, // 0-NULL Tile
64, // 1-[2]
64, // 2-[4]
64, // 3-[8]
64, // 4-[16]
64, // 5-[32]
64, // 6-[64]
48, // 7-[128]
48, // 8-[256]
48, // 9-[512]
48, // 10-[1024]
48, // 11-[2048]
48, // 12-[4096]
48, // 13-[8192]
48, // 14-[16384]
48, // 15-[32768]
48, // 16-[32768]
48, // 17-[32768]
48, // 18-[32768]
48, // 19-[32768]
};
unsigned int     g_2048_TextColors[] = {
0x000000, // 0-EmptyText
0x000000, // 0-NULL Tile
0x776E65, // 1-[2]
0x776E65, // 2-[4]
0xF9F6F2, // 3-[8]
0xF9F6F2, // 4-[16]
0xF9F6F2, // 5-[32]
0xF9F6F2, // 6-[64]
0xF9F6F2, // 7-[128]
0xF9F6F2, // 8-[256]
0xF9F6F2, // 9-[512]
0xF9F6F2, // 10-[1024]
0xF9F6F2, // 11-[2048]
0xF9F6F2, // 12-[4096]
0xF9F6F2, // 13-[8192]
0xF9F6F2, // 14-[16384]
0xF9F6F2, // 15-[32768]
0xF9F6F2, // 16-[32768]
0xF9F6F2, // 17-[32768]
0xF9F6F2, // 18-[32768]
0xF9F6F2, // 19-[32768]
};

unsigned int    *g_2048_TileColors = g_2048_Colors+4;
wchar_t        **g_2048_TileTexts = g_2048_Texts+1;
int             *g_2048_TileTextsSizes = g_2048_TextsSizes+1;
unsigned int    *g_2048_TextTileColors = g_2048_TextColors+1;

#define G2048_TILE_MERGED 0xF0000000
#define G2048_TILE_MASK   0x0FFFFFFF

void cGame2048_Tile::DelAnimation()
{if(Animation) delete Animation; Animation = 0;}
cGame2048_Tile::~cGame2048_Tile()
{DelAnimation();}

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

	Font.Open("Font.ttf");

	srand(tick = SDL_GetTicks());

	x=(1024-(g_2048_BarSize+(g_2048_BarSize+g_2048_TileSize)*4))/2;
	y=(768-(g_2048_BarSize+(g_2048_BarSize+g_2048_TileSize)*4))/2;

	W = 4;
	PPluseLen = 555;
	GameOverLen = 3550;

	if(Load())
		NewGame();
}

void cGame2048::NewGame()
{
	srand(tick = SDL_GetTicks());

	if(Map) delete[] Map;
	Map=0;
	Map = new cGame2048_Tile[W*W];

	PointsAdd(-Points);
	GameOverTick=-1;

	HistoryStep=0;
	History.clear();
	History.push_back(G2048_HIC_BLOCK_START(HistoryStep));
	SpawnNew();
	SpawnNew();
	History.push_back(G2048_HIC_BLOCK_END(HistoryStep));
	HistoryStep++;
	History.push_back(G2048_HIC_BLOCK_START(HistoryStep));
}


int cGame2048::Load()
{
	FILE * pFile;
	pFile = fopen ("2048.bin" , "rb");
	if (pFile == NULL) {return 1;}
	int size;
	fread (&BestPoints, 1, 4, pFile);
	fread (&Points, 1, 4, pFile);
	fread (&size, 1, 4, pFile);
	fread (&HistoryStep, 1, 4, pFile);
	
	srand(tick = SDL_GetTicks());

	if(Map) delete[] Map;
	Map=0;
	Map = new cGame2048_Tile[W*W];

	for(int i=0;i<W*W;i++)
	{
		fread (&Map[i].type, 1, 4, pFile);
		fread (&Map[i].oldp, 1, 4, pFile);
	}
	
	GameOverTick=-1;	
	History.clear();	
	
	int b;
	for(int i=0;i<size;i++)
	{
		fread (&b, 1, 4, pFile);
		History.push_back(b);
	}	

	fclose (pFile);
	return 0;
}
int cGame2048::Save()
{
	FILE * pFile;
	pFile = fopen ("2048.bin" , "wb");
	if (pFile == NULL) {return 1;}
	int size=History.size();
	fwrite (&BestPoints, 1, 4, pFile);
	fwrite (&Points, 1, 4, pFile);
	fwrite (&size, 1, 4, pFile);
	fwrite (&HistoryStep, 1, 4, pFile);
	
	for(int i=0;i<W*W;i++)
	{
		fwrite (&Map[i].type, 1, 4, pFile);
		fwrite (&Map[i].oldp, 1, 4, pFile);
	}	
	
	int b;
	for(int i=0;i<size;i++)
	{
		b=History[i];
		fwrite (&b, 1, 4, pFile);		
	}	

	fclose (pFile);
	return 0;
}

void cGame2048::GameOver()
{
	GameOverTick=tick=SDL_GetTicks();
	Save();
}

void cGame2048::SpawnNew()
{
	int type=1;
	srand(rand());
	if((rand()%100) < 13) type=2;
	srand(rand());
	int sz=W*W;
	for(int i=0;i<W*W;i++)
		if(Map[i].type>0) sz--;
	if(sz <= 0) GameOver();

	int j=rand()%sz+1;
	for(int i=0; i<W*W; i++)
		if(Map[i].type==0) {j--;if(j<=0) {
			// TODO AnimationSpawns
			Map[i].type = type;
			Map[i].DelAnimation();
			Map[i].Animation = new cGame2048_AnimationSpawn(
				tick, i%W, i/W, Map[i].type);
			History.push_back(G2048_HIC_TILE_SPAWN(Map[i].type, i));
			return;}}
}

void cGame2048::Release()	
{
	Save();
	Font.Close();
	if(Map) delete[] Map;
	Map=0;
	if(TextureBlock) SDL_DestroyTexture(TextureBlock);
	TextureBlock=0;
}
void cGame2048::DrawTile( float fx, float fy, float s, int type,  unsigned int color)
{
	if(type>=0)
		color=g_2048_TileColors[type];
	SDL_Rect rc;
	s*=0.5f;
	rc.x = (x+g_2048_BarSize)+int(float(g_2048_BarSize+g_2048_TileSize)*fx+float(g_2048_TileSize)*(0.5f-s));
	rc.y = (y+g_2048_BarSize)+int(float(g_2048_BarSize+g_2048_TileSize)*fy+float(g_2048_TileSize)*(0.5f-s));
	rc.h = rc.w = int(float(g_2048_TileSize)*(s*2.f));
	DrawBlock(rc, color);
}
void cGame2048::DrawTileText(float fx, float fy, float s, int type, unsigned int color, int alpha)
{	
	if(type>=0)
		color=g_2048_TextTileColors[type];
	int sz = int(s*g_2048_TileTextsSizes[type]);
	if(sz < 8) sz=8;
	Font.SetSize(sz);
	Font.Draw(R, g_2048_TileTexts[type], 
		(x+g_2048_BarSize)+int(float(g_2048_BarSize+g_2048_TileSize)*fx+float(g_2048_TileSize-(Font.GetStrSize(g_2048_TileTexts[type])>>6))*(0.5f)), 
		(y+g_2048_BarSize)+int(float(g_2048_BarSize+g_2048_TileSize)*fy+float(g_2048_TileSize+sz/2)*(0.5f)),
		color, alpha);
}
	
	
void cGame2048::DrawBlock(SDL_Rect rc, unsigned int color)
{
	if(rc.w < g_2048_BlockSizes*2) return;

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

	tick = SDL_GetTicks();

	SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(TextureBlock, SDL_BLENDMODE_BLEND);

	{
		unsigned int color=g_2048_Colors[0];
		SDL_SetRenderDrawColor(r, 
			(color>>16)&0xff,
			(color>>8)&0xff,
			(color)&0xff, 0x00);
		SDL_RenderClear(r);
	}
	{
		g_2048_BlockSizes=8;
		int i=g_2048_TileSize*4+g_2048_BarSize*5;
		SDL_Rect rcs={x,y,i,i};
		DrawBlock(rcs, g_2048_Colors[1]);
		g_2048_BlockSizes=4;
	}
	for(int iy=0;iy<4;iy++)
	for(int ix=0;ix<4;ix++)
		DrawTile(float(ix), float(iy), 1.f);

	for(int i=0;i<W*W;i++)
		if(Map[i].type>0)
			if(Map[i].Animation)
			{
				if(Map[i].Animation->Draw(this, tick))
					Map[i].DelAnimation();
			}
			else
				DrawTile(float(i%W), float(i/W), 1.f, Map[i].type);
		
	for(int i=0;i<W*W;i++)
		if(Map[i].type>0)
			if(Map[i].Animation)
			{
				if(Map[i].Animation->DrawTxt(this, tick))
					Map[i].DelAnimation();
			}
			else
				DrawTileText(float(i%W), float(i/W), 1.f, Map[i].type);
	
	{
		int w=g_2048_TileSize*2+g_2048_BarSize*2;
		int h=g_2048_TileSize/2+g_2048_BarSize*2;
		SDL_Rect rcs={x,y-h-g_2048_BarSize,w,h};
		DrawBlock(rcs, g_2048_Colors[1]);
		rcs.x += w+g_2048_BarSize;
		DrawBlock(rcs, g_2048_Colors[1]);
		wchar_t txt_score[]=L"SCORE";
		wchar_t txt_best[]=L"BEST";

		Font.SetSize(g_2048_BarSize*2);

		Font.Draw(R, txt_score, 
			x+((w-(Font.GetStrSize(txt_score)>>6))/2), 
			rcs.y+g_2048_BarSize*2,
			g_2048_Colors[0]);

		Font.Draw(R, txt_best, 
			rcs.x+((w-(Font.GetStrSize(txt_best)>>6))/2), 
			rcs.y+g_2048_BarSize*2,
			g_2048_Colors[0]);

		wchar_t txt[256];

		Font.SetSize(g_2048_TileSize/2-g_2048_BarSize);

		swprintf(txt, L"%i", Points);

		int len = (Font.GetStrSize(txt)>>6);
		Font.Draw(R, txt, 
			x+((w-len)/2), 
			y-g_2048_BarSize*2,
			g_2048_Colors[3]);
		float f=((tick-PPluseStart))/float(PPluseLen);
		if(f<1.f && PPluse)
		{
			if(PPluse>0)
				swprintf(txt, L"+%i", PPluse);
			else
				swprintf(txt, L"%i", PPluse);

			Font.Draw(R, txt, 
				x+((w+len)/2)-(Font.GetStrSize(txt)>>6), 
				y-g_2048_BarSize*2-int(f*float(g_2048_BarSize*2)),
				g_2048_Colors[2], 255-int(f*255.f));
		}
		
		swprintf(txt, L"%i", BestPoints);

		Font.Draw(R, txt, 
			rcs.x+((w-(Font.GetStrSize(txt)>>6))/2), 
			y-g_2048_BarSize*2,
			g_2048_Colors[3]);
	}

	{
		wchar_t txt1[] = L"HOW TO PLAY:";
		wchar_t txt2[] = L"Use your arrow keys to move the tiles. F2 to start new game.";
		wchar_t txt3[] = L"When two tiles with the same number touch, they merge into one!";
		wchar_t txt4[] = L"Press Z to back!";
		

		int _y=y+(g_2048_BarSize+g_2048_TileSize)*W+g_2048_BarSize*2+(g_2048_BarSize*5/2);
		Font.SetSize(g_2048_BarSize*5/2);
		int len=Font.GetStrSize(txt1)>>6, len2;
		Font.SetSize(g_2048_BarSize*3/2);
		if((len2=(Font.GetStrSize(txt2)>>6))>len) len=len2;
		if((len2=(Font.GetStrSize(txt3)>>6))>len) len=len2;
		if((len2=(Font.GetStrSize(txt4)>>6))>len) len=len2;
		int _x=x+(g_2048_BarSize*5+g_2048_TileSize*4-len)/2; // 
		Font.SetSize(g_2048_BarSize*5/2);
		Font.Draw(R, txt1, _x, _y, g_2048_Colors[2]);
		Font.SetSize(g_2048_BarSize*3/2);
		_y+=g_2048_BarSize*3/2;
		Font.Draw(R, txt2, _x, _y, g_2048_Colors[2]);
		_y+=g_2048_BarSize*3/2;
		Font.Draw(R, txt3, _x, _y, g_2048_Colors[2]);
		_y+=g_2048_BarSize*3/2;
		Font.Draw(R, txt4, _x, _y, g_2048_Colors[2]);
	}
	if(GameOverTick>0)
	{
		int color = g_2048_Colors[0];
		float f = f=((tick-GameOverTick))/float(GameOverLen);
		if(f>1.f) f=1.f;
		SDL_SetRenderDrawColor(r, 
			(color>>16)&0xff,
			(color>>8)&0xff,
			(color)&0xff, int(175.f*f));
		int wid=g_2048_BarSize*5+g_2048_TileSize*4;
		SDL_Rect rc={x,y,wid,wid};
		SDL_RenderFillRect(r,&rc);


		wchar_t txt1[] = L"GAME OVER!";
		wchar_t txt2[] = L"Your score:";
		wchar_t txt[256];
		swprintf(txt, L"%i", Points);
		
		int _y=y+wid/2-g_2048_BarSize*8+int(g_2048_BarSize*f*10.f);
		Font.SetSize(g_2048_BarSize*9);
		Font.Draw(R, txt1, x+(wid-(Font.GetStrSize(txt1)>>6))/2, _y, g_2048_Colors[1], int(255.f*f));
		Font.SetSize(g_2048_BarSize*4);
		_y+=g_2048_BarSize*6;
		Font.Draw(R, txt2, x+(wid-(Font.GetStrSize(txt2)>>6))/2, _y, g_2048_Colors[2], int(255.f*f));
		_y+=g_2048_BarSize*4;
		Font.Draw(R, txt, x+(wid-(Font.GetStrSize(txt)>>6))/2, _y, g_2048_Colors[2], int(255.f*f));
	
	}
}


int cGame2048::CanMove(cGame2048_Tile  **Array)
{
	if(!Array) return 0;
	int leftn=0;
	for(int x=1;x<W;x++)
		if(Array[x]->type > 0)
		{
			if(Array[x]->type==Array[leftn]->type && 
				!(Array[leftn]->type&G2048_TILE_MERGED))
				return 1;
			else
				leftn++;
		}
		else
		if(Array[x]->type == 0) return 1;
	return 0;
}
int cGame2048::CanMove()
{
	cGame2048_Tile  **Ar=0;
	Ar=new cGame2048_Tile*[W];

	for(int y=0;y<W;y++)
	{
		for(int x=0;x<W;x++)
			Ar[x]=Map+(y*W+x);
		if(CanMove(Ar)) {delete[] Ar;return 1;}

		for(int x=0;x<W;x++)
			Ar[x]=Map+(y*W+(W-x-1));
		if(CanMove(Ar)) {delete[] Ar;return 1;}
		
		for(int x=0;x<W;x++)
			Ar[x]=Map+(x*W+y);
		if(CanMove(Ar)) {delete[] Ar;return 1;}
		
		for(int x=0;x<W;x++)
			Ar[x]=Map+((W-x-1)*W+y);
		if(CanMove(Ar)) {delete[] Ar;return 1;}
	}


	delete[] Ar;return 0;
}

void cGame2048::PointsAdd(int plus)
{
	Points+=plus;
	PPluse=plus;
	PPluseStart=tick;
	if(Points > BestPoints) BestPoints=Points;
}

void cGame2048::EndMove(int plus, int can)
{
	for(int i=0;i<W*W;i++)
	{
		Map[i].type &= G2048_TILE_MASK;
		Map[i].oldp = i;
	}
	if(plus)
	{
		History.push_back(G2048_HIC_POINTS_ADD(plus));
		PointsAdd(plus);
	}
	if(can)
	{
		SpawnNew();
		History.push_back(G2048_HIC_POINTS_ALL(Points));
		History.push_back(G2048_HIC_BLOCK_END(HistoryStep));
		HistoryStep++;
		History.push_back(G2048_HIC_BLOCK_START(HistoryStep));
	}
	else
	if(!CanMove())
		GameOver();
}

int cGame2048::GetType(int x1, int y1)
{return Map[y1*W+x1].type;}
int cGame2048::rCanMerge(int x1, int y1, int x2, int y2)
{return GetType(x1,y1)==GetType(x2,y2) && (!(GetType(x1,y1)&G2048_TILE_MERGED));}
void cGame2048::rMerge(int x1, int y1, int x, int y)
{
	int leftn = y1*W+x1;
	Map[leftn].type=(Map[y*W+x].type+1)|G2048_TILE_MERGED;
	//Map[leftn].oldp=leftn;
	Map[y*W+x].type=0;
	// TODO AnimationMerge
	History.push_back(G2048_HIC_TILE_MOVE(y*W+x, leftn));
	History.push_back(G2048_HIC_TILE_MERGE(Map[leftn].oldp, y*W+x, leftn));
	Map[leftn].DelAnimation();
	Map[leftn].Animation = new cGame2048_AnimationMerge(
		tick, x1, y1, Map[leftn].oldp%W, Map[leftn].oldp/W, 
		x, y, Map[leftn].type&G2048_TILE_MASK);
}
int cGame2048::rCanMove(int x1, int y1, int x, int y)
{return Map[y1*W+x1].type==0;}
void cGame2048::rMove(int x1, int y1, int x, int y)
{	
	int leftn = y1*W+x1;
	Map[leftn].type=Map[y*W+x].type;
	Map[leftn].oldp=y*W+x;
	Map[y*W+x].type=0;
	// TODO AnimationMove
	Map[leftn].DelAnimation();
	Map[leftn].Animation = new cGame2048_AnimationMove(
		tick, x1, y1, x, y, Map[leftn].type);
	History.push_back(G2048_HIC_TILE_MOVE(y*W+x, leftn));
}

void cGame2048::MoveLeft()
{
	int can=0;
	int plus=0;
	for(int y=0;y<W;y++)
	{
		int leftn=0;
		for(int x=1;x<W;x++) if(GetType(x,y) > 0) {
				if(rCanMerge(leftn,y,x,y))
				{
					rMerge(leftn,y,x,y);
					plus+=int(pow(2.f, GetType(leftn,y)&G2048_TILE_MASK));
					leftn++;
					can++;
				}
				else
				if(rCanMove(leftn,y,x,y))
				{
					rMove(leftn,y,x,y);
					can++;
				}
				else
				if(rCanMove(leftn+1,y,x,y))
				{
					leftn++;
					rMove(leftn,y,x,y);
					can++;
				}
				else
				{
					leftn++;
				}
			}
	}
	EndMove(plus, can);
}
void cGame2048::MoveRight()
{
	int can=0;
	int plus=0;
	int w=W-1;
	for(int y=0;y<W;y++)
	{
		int leftn=0;
		for(int x=1;x<W;x++) if(GetType(w-x,y) > 0) {
				if(rCanMerge(w-leftn,y,w-x,y))
				{
					rMerge(w-leftn,y,w-x,y);
					plus+=int(pow(2.f, GetType(w-leftn,y)&G2048_TILE_MASK));
					leftn++;
					can++;
				}
				else
				if(rCanMove(w-leftn,y,w-x,y))
				{
					rMove(w-leftn,y,w-x,y);
					can++;
				}
				else
				if(rCanMove(w-leftn-1,y,w-x,y))
				{
					leftn++;
					rMove(w-leftn,y,w-x,y);
					can++;
				}
				else
				{
					leftn++;
				}
			}
	}
	EndMove(plus, can);
}
void cGame2048::MoveUp()
{
	int can=0;
	int plus=0;
	for(int y=0;y<W;y++)
	{
		int leftn=0;
		for(int x=1;x<W;x++) if(GetType(y,x) > 0) {
				if(rCanMerge(y,leftn,y,x))
				{
					rMerge(y,leftn,y,x);
					plus+=int(pow(2.f, GetType(y,leftn)&G2048_TILE_MASK));
					leftn++;
					can++;
				}
				else
				if(rCanMove(y,leftn,y,x))
				{
					rMove(y,leftn,y,x);
					can++;
				}
				else
				if(rCanMove(y,leftn+1,y,x))
				{
					leftn++;
					rMove(y,leftn,y,x);
					can++;
				}
				else
				{
					leftn++;
				}
			}
	}
	EndMove(plus, can);
}
void cGame2048::MoveDown()
{
	int can=0;
	int plus=0;
	int w=W-1;
	for(int y=0;y<W;y++)
	{
		int leftn=0;
		for(int x=1;x<W;x++) if(GetType(y,w-x) > 0) {
				if(rCanMerge(y,w-leftn,y,w-x))
				{
					rMerge(y,w-leftn,y,w-x);
					plus+=int(pow(2.f, GetType(y,w-leftn)&G2048_TILE_MASK));
					leftn++;
					can++;
				}
				else
				if(rCanMove(y,w-leftn,y,w-x))
				{
					rMove(y,w-leftn,y,w-x);
					can++;
				}
				else
				if(rCanMove(y,w-leftn-1,y,w-x))
				{
					leftn++;
					rMove(y,w-leftn,y,w-x);
					can++;
				}
				else
				{
					leftn++;
				}
			}
	}
	EndMove(plus, can);
}

void cGame2048::MoveBack()
{
	int b=History.back();
	if(G2048_HI_TYPE(b)==G2048_HI_BLOCK_START && G2048_HI_NUM(b) <= 1) {return;}
	History.pop_back();
	tick = SDL_GetTicks();
	GameOverTick=-1; 
	int type,num,p0,p1,p2;
pp_start:

	b=History.back();
	if(G2048_HI_TYPE(b)==G2048_HI_BLOCK_START) 
	{
		for(int i=0;i<W*W;i++)
		{
			if(Map[i].oldp != i)
				Map[i].type = 0;
			else
				Map[i].type &= G2048_TILE_MASK;
		}
		return;
	}
	History.pop_back();
	type=G2048_HI_TYPE(b);
	num=G2048_HI_NUM(b);
	p0=G2048_HI_P0(b);
	p1=G2048_HI_P1(b);
	p2=G2048_HI_P2(b);

	switch(type)
	{
	case G2048_HI_BLOCK_END:
			HistoryStep=num;
		break;
	case G2048_HI_TILE_SPAWN:
			Map[p1].type=0;
		break;			
	case G2048_HI_TILE_MOVE:
			Map[p0].type=(Map[p1].type & G2048_TILE_MASK);
			if(!(Map[p1].type&G2048_TILE_MERGED))
				Map[p1].type=0;
			Map[p0].oldp=p0;
			Map[p0].DelAnimation();
			Map[p0].Animation = new cGame2048_AnimationMove(
				tick, p0%W, p0/W, p1%W, p1/W, Map[p0].type);
		break;			
	case G2048_HI_TILE_MERGE:
		{
			/*int pos01 = p0;
			int pos02 = p1;
			int pos1  = p2;*/
			Map[p2].type--;
			Map[p2].oldp=p0;
			Map[p2].type|=G2048_TILE_MERGED;
		}
		break;
	case G2048_HI_POINTS_ADD:
			PointsAdd(-num);
		break;
	case G2048_HI_POINTS_ALL:
			Points = num;
		break;
	};

	goto pp_start;
	
	while((G2048_HI_TYPE((b=History.back()))) != G2048_HI_BLOCK_START)
	{
		History.pop_back();
		
	}
	History.pop_back();
}


void cGame2048::Keyboard( SDL_Scancode sc )
{
	switch(sc)
	{
	case SDL_SCANCODE_LEFT:
		if(GameOverTick < 0) MoveLeft();
		break;
	case SDL_SCANCODE_RIGHT:
		if(GameOverTick < 0) MoveRight();
		break;
	case SDL_SCANCODE_UP:
		if(GameOverTick < 0) MoveUp();
		break;
	case SDL_SCANCODE_DOWN:
		if(GameOverTick < 0) MoveDown();
		break;
	case SDL_SCANCODE_F2:
		NewGame();
		break;
	case SDL_SCANCODE_Z:
		MoveBack();
		break;
#ifdef _DEBUG
	case SDL_SCANCODE_L:
		GameOver();
		break;
#endif
	};
}

cGame2048_AnimationSpawn::cGame2048_AnimationSpawn(int tick,  int ipx, int ipy, int itype)
{
	start = tick;
	len = 130;
	px = ipx;
	py = ipy;
	type = itype;
}
int cGame2048_AnimationSpawn::Draw(cGame2048* Game, int tick)
{
	if(!Game) return -1;
	float t=0.f;
	t=float(tick-start)/float(len);
	int o=0;
	if(o=(t>1.f)) t=1.f;
	Game->DrawTile(float(px), float(py), t, type);
	return o;
}
int cGame2048_AnimationSpawn::DrawTxt(cGame2048* Game, int tick)
{
	if(!Game) return -1;
	float t=0.f;
	t=float(tick-start)/float(len);
	int o=0;
	if(o=(t>1.f)) t=1.f;
	Game->DrawTileText(float(px), float(py), t, type, 0, int(t*255.f));
	return o;
}

cGame2048_AnimationMove::cGame2048_AnimationMove
	(int tick, int ipx, int ipy, int ipx0, int ipy0, int itype)
{
	start = tick;
	len = 230;
	px = float(ipx);
	py = float(ipy);
	px0 = float(ipx0);
	py0 = float(ipy0);
	type = itype;
}
int cGame2048_AnimationMove::Draw(cGame2048* Game, int tick)
{
	if(!Game) return -1;
	float t=0.f;
	t=float(tick-start)/float(len);
	int o=0;
	if(o=(t>1.f)) t=1.f;
	float t2=1.f-t;
	Game->DrawTile(px0*t2+px*t, py0*t2+py*t, 1.f, type);
	return o;
}
int cGame2048_AnimationMove::DrawTxt(cGame2048* Game, int tick)
{
	if(!Game) return -1;
	float t=0.f;
	t=float(tick-start)/float(len);
	int o=0;
	if(o=(t>1.f)) t=1.f;
	float t2=1.f-t;
	Game->DrawTileText(px0*t2+px*t, py0*t2+py*t, 1.f, type);
	return o;
}

cGame2048_AnimationMerge::cGame2048_AnimationMerge
	(int tick, int ipx, int ipy, int ipx01, int ipy01,
	int ipx02, int ipy02, int itype)
{
	start = tick;
	len = 230;
	px = float(ipx);
	py = float(ipy);
	px01 = float(ipx01);
	py01 = float(ipy01);
	px02 = float(ipx02);
	py02 = float(ipy02);
	type = itype;
}
int cGame2048_AnimationMerge::Draw(cGame2048* Game, int tick)
{
	if(!Game) return -1;
	float t=0.f;
	t=float(tick-start)/float(len);
	int o=0;
	if(o=(t>1.4f)) t=1.4f;
	if(t<=1.f)
	{
		float t2=1.f-t;
		Game->DrawTile(px01*t2+px*t, py01*t2+py*t, 1.f, type-1);
		Game->DrawTile(px02*t2+px*t, py02*t2+py*t, 1.f, type-1);
	}
	else		
	if(t<=1.2f)
	{
		t=(t-1.0f)*5.f;
		float t2=1.f-t;
		int r=int(float((g_2048_TileColors[type-1]&0xff0000)>>16)*t2 + float((g_2048_TileColors[type]&0xff0000)>>16)*t);
		int g=int(float((g_2048_TileColors[type-1]&0xff00)>>8)*t2 + float((g_2048_TileColors[type]&0xff00)>>8)*t);
		int b=int(float((g_2048_TileColors[type-1]&0xff))*t2 + float((g_2048_TileColors[type]&0xff))*t);
		if(r>255) r=255;if(g>255) g=255;if(b>255) b=255;
		Game->DrawTile(px, py, 1.f+t*0.1f, -1, (r<<16)+(g<<8)+b);
	}
	else	
	{
		float t2=1.f-((t-1.2f)*5.f);
		Game->DrawTile(px, py, 1.f+t2*0.1f, type);
	}
	return o;
}
int cGame2048_AnimationMerge::DrawTxt(cGame2048* Game, int tick)
{
	if(!Game) return -1;
	float t=0.f;
	t=float(tick-start)/float(len);
	int o=0;
	if(o=(t>1.4f)) t=1.4f;
	if(t<=1.f)
	{
		float t2=1.f-t;
		Game->DrawTileText(px01*t2+px*t, py01*t2+py*t, 1.f, type-1);
		Game->DrawTileText(px02*t2+px*t, py02*t2+py*t, 1.f, type-1);
	}
	else		
	if(t<=1.2f)
	{
		t=(t-1.0f)*5.f;
		float t2=1.f-t;
		int r=int(float((g_2048_TileColors[type-1]&0xff0000)>>16)*t2 + float((g_2048_TileColors[type]&0xff0000)>>16)*t);
		int g=int(float((g_2048_TileColors[type-1]&0xff00)>>8)*t2 + float((g_2048_TileColors[type]&0xff00)>>8)*t);
		int b=int(float((g_2048_TileColors[type-1]&0xff))*t2 + float((g_2048_TileColors[type]&0xff))*t);
		if(r>255) r=255;if(g>255) g=255;if(b>255) b=255;
		Game->DrawTileText(px, py, 1.f+t*0.1f, -1, (r<<16)+(g<<8)+b, int((1.f+t*0.1f)*255.f));
	}
	else	
	{
		float t2=1.f-((t-1.2f)*5.f);
		Game->DrawTileText(px, py, 1.f+t2*0.1f, type,  0, int((1.f+t2*0.1f)*255.f));
	}
	return o;
}



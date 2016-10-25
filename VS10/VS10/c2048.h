#ifndef H_C_2048
#define H_C_2048

#include "cGame.h"
#include "cFT.h"
#include <vector>



struct cGame2048_Tile;

/*
История действий:
1 - BlockStart
	[Num] Номер блока
2 - BlockEnd
	[Num] Номер блока
3 - TileSpawn
	[P0] Тип тайла
	[P1] Позиция
4 - TileMove
	[P0] Начальная позиция
	[P1] Конечная позиция
4 - TileMerge
	[P0] Начальная позиция [1]
	[P1] Начальная позиция [2]
	[P2] Конечная позиция
5 - PointsAdd
	[Num] Количество очков
6 - PointsFull
	[Num] Общее количество очков
*/

#define G2048_HI_BLOCK_START  0x1f
#define G2048_HI_BLOCK_END    0x8f
#define G2048_HI_TILE_SPAWN   0xf2
#define G2048_HI_TILE_MOVE    0xf3
#define G2048_HI_TILE_MERGE   0xf4
#define G2048_HI_POINTS_ADD   0xf8
#define G2048_HI_POINTS_ALL   0xf9

#define G2048_HI_NUM(data)    (data&0x00ffffff)
#define G2048_HI_TYPE(data)   ((data>>24)&0xff)
#define G2048_HI_P0(data)     ((data>>16)&0xff)
#define G2048_HI_P1(data)     ((data>>8)&0xff)
#define G2048_HI_P2(data)     (data&0xff)

#define G2048_HIC_NUM(data)   (data&0x00ffffff)
#define G2048_HIC_TYPE(data)  ((data&0xff)<<24)
#define G2048_HIC_P0(data)    ((data&0xff)<<16)
#define G2048_HIC_P1(data)    ((data&0xff)<<8)
#define G2048_HIC_P2(data)    (data&0xff)

#define G2048_HIC_BLOCK_START(b)\
	G2048_HIC_TYPE(G2048_HI_BLOCK_START)|G2048_HIC_NUM(b)
#define G2048_HIC_BLOCK_END(b)\
	G2048_HIC_TYPE(G2048_HI_BLOCK_END)|G2048_HIC_NUM(b)
#define G2048_HIC_TILE_SPAWN(a,b)\
	G2048_HIC_TYPE(G2048_HI_TILE_SPAWN)|G2048_HIC_P0(a)|G2048_HIC_P1(b)
#define G2048_HIC_TILE_MOVE(a,b)\
	G2048_HIC_TYPE(G2048_HI_TILE_MOVE)|G2048_HIC_P0(a)|G2048_HIC_P1(b)
#define G2048_HIC_TILE_MERGE(a,b,c)\
	G2048_HIC_TYPE(G2048_HI_TILE_MERGE)|G2048_HIC_P0(a)|G2048_HIC_P1(b)|G2048_HIC_P2(c)
#define G2048_HIC_POINTS_ADD(b)\
	G2048_HIC_TYPE(G2048_HI_POINTS_ADD)|G2048_HIC_NUM(b)
#define G2048_HIC_POINTS_ALL(b)\
	G2048_HIC_TYPE(G2048_HI_POINTS_ALL)|G2048_HIC_NUM(b)

/*
Анимация:
0 - A_Null
	Параметры без изменения
1 - A_Spawn
	Линейное увеличение
	и проявление текста
2 - A_Move
	Линейный сдвиг
3 - A_Merge
	Переход цвета
	1. увеличение на 10% и затухание текста
	2. уменьшение до 100% и проявление текста
*/

class cGame2048 : public cGameBase
{
private:
	cGame2048_Tile  *Map;
	int x,y,W, tick;
	SDL_Texture  *TextureBlock;
	SDL_Renderer *R;
	int Points;
	int BestPoints;

	int PPluse;
	int PPluseStart;
	int PPluseLen;

	int GameOverTick;
	int GameOverLen;

	int HistoryStep;
	std::vector<int> History;
	cFont Font;

	void DrawBlock( SDL_Rect rc, unsigned int color=0);
	void SpawnNew();
	void EndMove(int plus, int can);
	int  CanMove(cGame2048_Tile  **Array);
	int  CanMove();

	int GetType(int x1, int y1);
	int rCanMerge(int x1, int y1, int x2, int y2);
	int rCanMove(int x1, int y1, int x0, int y0);
	void rMerge(int x1, int y1, int x2, int y2);
	void rMove(int x1, int y1, int x0, int y0);

	void MoveLeft();
	void MoveRight();
	void MoveUp();
	void MoveDown();
	void MoveBack();
	void NewGame();

	void PointsAdd(int plus);

	// todo
	void GameOver();
	int Load();
	int Save();
public:
	void DrawTile( float fx, float fy, float s, int type=0, unsigned int color=0);
	void DrawTileText(float fx, float fy, float s, int type=0, unsigned int color=0, int alpha=255);
	virtual void Init( SDL_Renderer *r );
	virtual void Release();
	virtual void Draw( SDL_Renderer *r );
	virtual void Keyboard( SDL_Scancode sc=SDL_SCANCODE_UNKNOWN );
};

struct cGame2048_Animation 
	: public ZeroedMemoryAllocator
{
	int start;
	int len;
	int type;

	virtual int Draw(cGame2048* Game, int tick) = 0;
	virtual int DrawTxt(cGame2048* Game, int tick) = 0;
};

struct cGame2048_AnimationSpawn 
	: public cGame2048_Animation
{
	int px, py;
	cGame2048_AnimationSpawn(int tick, int ipx, int ipy, int itype);
	virtual int Draw(cGame2048* Game, int tick);
	virtual int DrawTxt(cGame2048* Game, int tick);
};

struct cGame2048_AnimationMove 
	: public cGame2048_Animation
{
	float px, py, px0, py0;
	cGame2048_AnimationMove(int tick, int ipx, int ipy, int ipx0, int ipy0, int itype);
	virtual int Draw(cGame2048* Game, int tick);
	virtual int DrawTxt(cGame2048* Game, int tick);
};

struct cGame2048_AnimationMerge 
	: public cGame2048_Animation
{
	float px, py, px01, py01, px02, py02;
	cGame2048_AnimationMerge(int tick, int ipx, int ipy, int ipx01, int ipy01, int ipx02, int ipy02, int itype);
	virtual int Draw(cGame2048* Game, int tick);
	virtual int DrawTxt(cGame2048* Game, int tick);
};


struct cGame2048_Tile 
	: public ZeroedMemoryAllocator
{
	int type;
	int oldp;
	cGame2048_Animation *Animation;
	void Draw(cGame2048* Game);
	void DelAnimation();
	~cGame2048_Tile();
};




#endif

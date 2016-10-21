#ifndef H_C_2048
#define H_C_2048

#include "cGame.h"
#include <vector>
struct cG2048_Tile;
struct cG2048_Animation;

class cGame2048 : public cGameBase
{
private:
	cG2048_Tile  **Map;
	int x,y,W;
	SDL_Texture  *TextureBlock;
	SDL_Renderer *R;
	std::vector<cG2048_Tile*> v;
	std::vector<cG2048_Animation*> Av;

	void DrawBlock( SDL_Rect rc, int Cid, unsigned int color=0);
	void SpawnNew();
	void GameOver();
	void AnimAddSpawn(int pos, int type);
public:
	virtual void Init( SDL_Renderer *r );
	virtual void Release();
	virtual void Draw( SDL_Renderer *r );
	virtual void Keyboard( SDL_Scancode sc=SDL_SCANCODE_UNKNOWN );
};

#endif

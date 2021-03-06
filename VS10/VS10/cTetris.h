#ifndef H_C_TETRIS
#define H_C_TETRIS

#include "cGame.h"

#include <cstdio>

class cGameTetris : public cGameBase
{	
private:
	SDL_Renderer *R;

	struct cBlockFigure;
	class cFigureAtlas;

	int points;
	int pointsLine;

	int *Map;
	int X, Y;
	int w, h;
	cFigureAtlas *FA;
	int tickStart;
	int tickPrevStep;
	int tickReal;
	int tickPerStep;
	int blockSize;
	int state;
	SDL_Texture  *blockTexture;
	cBlockFigure *pf, *pf2;
	void Step();
	void StepNext();
	int CanStepNext();
	int CanStepMove(int x);
	int CanStepUp(cBlockFigure *pf);
	void FigureNext();
	void OnLose();
public:	
	void Init( SDL_Renderer *r );
	void Release();
	void Draw( SDL_Renderer *r );
	void Keyboard( SDL_Scancode sc=SDL_SCANCODE_UNKNOWN );

	int SetTickStep(int tick=0);
	void StartNew(SDL_Renderer *r=0, int Width=0, int Height=0);
	void Close();
	//void Draw(SDL_Renderer *r);
	void Resize(int W=0, int H=0);
	void StepMove(int x=0, int y=0);
	void PauseSwitch();
	cGameTetris();
	~cGameTetris();
};

#endif

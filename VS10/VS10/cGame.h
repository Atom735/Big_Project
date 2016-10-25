#ifndef H_C_GAME
#define H_C_GAME

#include "ÒAllocator.h"
#include <SDL.h>
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")

class cGameBase : public ZeroedMemoryAllocator
{
private:
protected:
public:
	virtual void Init( SDL_Renderer *r ) {};
	virtual void Release() {};
	virtual void Draw( SDL_Renderer *r ) {};
	virtual void Keyboard( SDL_Scancode sc=SDL_SCANCODE_UNKNOWN ) {};
};

#endif

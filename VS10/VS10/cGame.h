#ifndef H_C_GAME
#define H_C_GAME

#include <SDL.h>
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")

#include <cstdio>
#include <cstdlib>

class ZeroedMemoryAllocator
{
public:
	ZeroedMemoryAllocator() {}
	virtual ~ZeroedMemoryAllocator() {}
	inline void *operator new(size_t size) { return calloc(size, 1); }
	inline void *operator new[](size_t size) { return calloc(size, 1); }
	inline void operator delete(void *ptr) { free(ptr); }
	inline void operator delete[](void *ptr) { free(ptr); }
};

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

#ifndef H_C_FONTS
#define H_C_FONTS

#include <SDL.h>
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")

class cFontCache;
class cFont
{
private:
	cFontCache *f;
public:
	cFont();
	~cFont();
	void SetSize(unsigned int pt=16);
	void Open(const char *fname = "Roboto-Regular.ttf", int Hint=0);
	void Close();
	int DrawTo(SDL_Renderer *r, wchar_t *stri = L"%NO_STRING%", int strsz=0, SDL_Rect *rci=0, int Mono=0, int color=0);
	void SwitchSubPixelLine();
	void GammaSet(float gamma=1.f);
};

void rFT_Init(SDL_Renderer *r);
void rFT_Release();

#endif
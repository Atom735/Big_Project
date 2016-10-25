#ifndef H_C_FONTS
#define H_C_FONTS


#include "сAllocator.h"
#include <SDL.h>
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")

/*
wchar_t
Представляется в виде
[ff][ff]
будем хранить блоки
для каждого фонта, каждого размера

ГЛИФ
1. размеры
2. биткарту

БЛОК ГЛИФОВ
1. 256 глифов

КЕШ ГЛИФОВ
1. размер шрифта
2. 256 указателей на блоки глифов

ФОНТ
1. Наименование

*/

struct cFontGlyph
{
	int Advance; // Ширина глифа
	int offX, offY; // Позиция битмапы
	int bmpH, bmpW; // Размеры битмапы
	int DataType;
	// 0х80 - Сглаживание
	// 0х40 - Хинты
	// 0х00-0х3F - Сдвиг по горизонтали
	unsigned char *data; // указатель на битмапу 
	
	void *ptr; // укзатель на дополнительную информацию
};

struct cFontGlyphBlock : public ZeroedMemoryAllocator
{
	cFontGlyph Glyph[256];
};

struct cFontGlyphCache : public ZeroedMemoryAllocator
{
	int FontSize;
	cFontGlyphCache *pNext;
	cFontGlyphBlock *GlyphBlock[256];

	cFontGlyphCache(int font_size);
	~cFontGlyphCache();
	cFontGlyph *GetGlyph(wchar_t glyph_index);
};

class cFont : public ZeroedMemoryAllocator
{
private:
	char *FontName;
	int FontId;
	int hint;
	int offset;
	int aa;
	cFontGlyphCache *ptr_FGC_Begin;
	cFontGlyphCache *ptr_FGC;
public:
	cFont();
	~cFont();
	void Open(const char *fname);
	void Close();
	void SetSize(unsigned int pt=16, int h=1, int a=1, int off=0);
	int  GetStrSize(wchar_t *txt);
	int  Draw(SDL_Renderer *r, wchar_t *txt, int x, int y, int color=0, int alpha=255);
	void CreateGlyph(wchar_t index);
};

void rFT_Init(SDL_Renderer *r);
void rFT_Release();

#endif
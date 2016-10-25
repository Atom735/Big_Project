#ifndef H_C_FONTS
#define H_C_FONTS


#include "�Allocator.h"
#include <SDL.h>
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")

/*
wchar_t
�������������� � ����
[ff][ff]
����� ������� �����
��� ������� �����, ������� �������

����
1. �������
2. ��������

���� ������
1. 256 ������

��� ������
1. ������ ������
2. 256 ���������� �� ����� ������

����
1. ������������

*/

struct cFontGlyph
{
	int Advance; // ������ �����
	int offX, offY; // ������� �������
	int bmpH, bmpW; // ������� �������
	int DataType;
	// 0�80 - �����������
	// 0�40 - �����
	// 0�00-0�3F - ����� �� �����������
	unsigned char *data; // ��������� �� ������� 
	
	void *ptr; // �������� �� �������������� ����������
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
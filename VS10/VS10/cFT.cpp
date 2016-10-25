#include "cFT.h"

#include <vector>
#include <cstdio>
#include <cstdlib>
#include <ft2build.h>
#ifdef _DEBUG
#pragma comment(lib, "freetype27d.lib")
#else
#pragma comment(lib, "freetype27.lib")
#endif
#include FT_FREETYPE_H

FT_Library    ft_lib=0;
int           ft_error=0;
SDL_Texture  *ft_tex=0;
int           ft_faces_n=0;
FT_Face       ft_face[16];

void rFT_Init(SDL_Renderer *r)
{
	if( ft_error = FT_Init_FreeType( &ft_lib ) ) abort();
	ft_tex=SDL_CreateTexture(r, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 256, 256);
	SDL_SetTextureBlendMode(ft_tex,SDL_BLENDMODE_BLEND);
}
void rFT_Release()
{
	SDL_DestroyTexture(ft_tex);
	if ( ft_error = FT_Done_FreeType(ft_lib) ) abort();
}

cFont::cFont(){}
cFont::~cFont(){Close();}
void cFont::Open(const char *fname)
{
	FontId = ft_faces_n;
	ft_faces_n++;

	if ( ft_error = FT_New_Face( ft_lib, fname, 0, &ft_face[FontId] )) abort();
	SetSize();
}
void cFont::Close()
{
	if(ft_face[FontId])
		if ( ft_error = FT_Done_Face(ft_face[FontId])) abort();
	ft_face[FontId] = 0;
	if(ptr_FGC_Begin)
	{
		while(ptr_FGC=ptr_FGC_Begin->pNext)
		{
			delete ptr_FGC_Begin;
			ptr_FGC_Begin=ptr_FGC;
		}
		delete ptr_FGC_Begin;
	}
	ptr_FGC_Begin=0;
}
void cFont::SetSize(unsigned int pt, int h, int a, int off)
{
	hint=h;
	aa=a;
	offset=off;
	FT_Set_Pixel_Sizes(ft_face[FontId], 0, pt);
	if(ptr_FGC_Begin)
	{
		ptr_FGC=ptr_FGC_Begin;
		while(ptr_FGC->FontSize != pt)
			if(!(ptr_FGC->pNext))
				ptr_FGC = (ptr_FGC->pNext = new cFontGlyphCache(pt));
			else
				ptr_FGC = ptr_FGC->pNext;
	}
	else
	{
		ptr_FGC=ptr_FGC_Begin=new cFontGlyphCache(pt);
	}
}
int cFont::GetStrSize(wchar_t *txt)
{
	int len=0;
	for(;*txt;txt++)
	{
		cFontGlyph *pFG = ptr_FGC->GetGlyph(*txt);
		if((!pFG->DataType) && (!pFG->Advance))
			CreateGlyph(*txt);
		len+=pFG->Advance;
	}
	return len;
}
int  cFont::Draw(SDL_Renderer *r, wchar_t *txt, int x, int y, int color, int alpha)
{
	int o=0;
	x<<=6;
	SDL_SetTextureColorMod(ft_tex,(color>>16)&0xff,(color>>8)&0xff,(color>>0)&0xff);
	SDL_SetTextureAlphaMod(ft_tex, alpha);

	for (;*txt;txt++)
	{
		cFontGlyph *pFG = ptr_FGC->GetGlyph(*txt);
		if((pFG->DataType != ((aa<<7)|(hint<<6)|(offset))))
		{
			CreateGlyph(*txt);
			o++;
		}

		unsigned char* pData=pFG->data;
		if(!pData) {x+=pFG->Advance;continue;}
		
		int sz=pFG->bmpH*pFG->bmpW;
		int *p = new int[sz];

		if(pFG->DataType & 0x80)
			for(int i=0; i<sz; i++)
				p[i] = 0xffffff00 | pData[i];
		else
		{
			int pitch = (pFG->bmpW+(pFG->bmpW%8))/8;
			for(int y=0; y<pFG->bmpH; y++)
			for(int x=0; x<pFG->bmpW; x++)
				p[y*pFG->bmpW+x] = 0xffffff00 | (255 * !!(pData[(y*pitch) + (x/8)]&(0x80>>(x%8))));
		}

		SDL_Rect rc={0,0,pFG->bmpW,pFG->bmpH};

		SDL_UpdateTexture(ft_tex, &rc,p,rc.w*4);
		delete[] p;
		SDL_Rect rc2={
			(x>>6)+pFG->offX,
			y-pFG->offY,
			rc.w,rc.h};
		SDL_RenderCopy(r, ft_tex, &rc, &rc2);

		x+=pFG->Advance;
	}
	return o;
}


cFontGlyphCache::cFontGlyphCache(int font_size)
{FontSize=font_size;memset(GlyphBlock, 0, sizeof(void*)*256);}
cFontGlyphCache::~cFontGlyphCache() {for(int i=0;i<256;i++) 
	if(GlyphBlock[i]) {delete GlyphBlock[i]; GlyphBlock[i]=0;}}
cFontGlyph *cFontGlyphCache::GetGlyph(wchar_t glyph_index)
{
	int i=glyph_index>>8;
	if(!GlyphBlock[i]) GlyphBlock[i]=new cFontGlyphBlock();
	return GlyphBlock[i]->Glyph+(glyph_index&0xff);
}

void cFont::CreateGlyph(wchar_t index)
{
	cFontGlyph *pFG = ptr_FGC->GetGlyph(index);
	if(pFG->data) delete[] pFG->data;
	pFG->data=0;
	pFG->DataType=((aa<<7)|(hint<<6)|(offset));
	
	FT_UInt glyph_index = FT_Get_Char_Index( ft_face[FontId], index);
	if (!glyph_index) return;
	if ( ft_error = FT_Load_Glyph( ft_face[FontId], glyph_index, 
		FT_LOAD_DEFAULT+FT_LOAD_FORCE_AUTOHINT*hint) ) return;
	
	pFG->Advance=ft_face[FontId]->glyph->advance.x;

	if (aa) if ( ft_error = FT_Render_Glyph( ft_face[FontId]->glyph, FT_RENDER_MODE_NORMAL )) return;
	if (!aa) if ( ft_error = FT_Render_Glyph( ft_face[FontId]->glyph, FT_RENDER_MODE_MONO )) return;

	pFG->offX=ft_face[FontId]->glyph->bitmap_left;
	pFG->offY=ft_face[FontId]->glyph->bitmap_top;	
	pFG->bmpH=ft_face[FontId]->glyph->bitmap.rows;
	pFG->bmpW=ft_face[FontId]->glyph->bitmap.width;
	int pitch=ft_face[FontId]->glyph->bitmap.pitch;
	if(!aa && pitch != (pFG->bmpW+pFG->bmpW%8)) return;
	if(aa && pitch != (pFG->bmpW)) return;
	pFG->data=new unsigned char[pFG->bmpH*pitch];
	memcpy(pFG->data, ft_face[FontId]->glyph->bitmap.buffer, pFG->bmpH*pitch);
}
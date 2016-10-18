#include "cTest.h"

#include <vector>

#include <ft2build.h>
#ifdef _DEBUG
#pragma comment(lib, "freetype27d.lib")
#else
#pragma comment(lib, "freetype27.lib")
#endif
#include <freetype/freetype.h>
#include FT_FREETYPE_H
#include <freetype/ftglyph.h>
#include FT_GLYPH_H

FT_Library  ft_lib=0;
int error=0;
SDL_Texture *tex=0;

wchar_t *teststring = 
	L"Друг мой эльф! Яшке б свёз птиц южных чащ!\n"
	L"В чащах юга жил бы цитрус? Да, но фальшивый экземпляр!\n"
	L"Любя, съешь щипцы, — вздохнёт мэр, — кайф жгуч\n"
	L"Шеф взъярён тчк щипцы с эхом гудбай Жюль.\n"
	L"Эй, жлоб! Где туз? Прячь юных съёмщиц в шкаф.\n"
	L"Экс-граф? Плюш изъят. Бьём чуждый цен хвощ!\n"
	L"Эх, чужак! Общий съём цен шляп (юфть) — вдрызг!\n"
	L"Эх, чужд кайф, сплющь объём вши, грызя цент.\n"
	L"Чушь: гид вёз кэб цапф, юный жмот съел хрящ.\n"
	L"Широкая электрификация южных губерний даст мощный толчок подъёму сельского хозяйства.\n"
	L"Съешь [же] ещё этих мягких французских булок да выпей чаю.\n"
	L"Բել դղյակի ձախ ժամն օֆ ազգությանը ցպահանջ չճշտած վնաս էր եւ փառք։\n"
;

SDL_Rect defaultrect = {0,0,256,1024};

class cFont;

struct cFontGlyph
{
	FT_UInt Index;
	FT_UInt Advance;
	FT_Glyph Glyph;
};

using namespace std;

typedef vector<cFontGlyph>::const_iterator VITER;

class cFontGlyphRow
{
private:
	FT_UInt size;
	vector<cFontGlyph> v;
public:
	cFontGlyphRow(int sz) {size=sz;}
	cFontGlyphRow(cFontGlyphRow *a) {memcpy(this, a, sizeof(cFontGlyphRow));}
	~cFontGlyphRow()
	{
		Delete();
	}
	void Delete() 
	{
		while(!v.empty())
		{
			FT_Done_Glyph(v.back().Glyph);
			v.pop_back();
		}
	}
	VITER Search(FT_UInt index, int *n=0)
	{
		const VITER endIt = v.end();

		VITER left = v.begin();
		VITER right = endIt;

		if (v.empty() || v.front().Index > index) {if(n) *n=1; return left;}
		if (v.back().Index < index) {if(n) *n=1; return right;}

		while (distance(left, right)) 
		{
			const VITER mid = left + distance(left, right) / 2;
			if (index == (*mid).Index) return mid;
			if (index < (*mid).Index) right = mid;
			else left = mid + 1;
		}

		if ((*right).Index == index)
			return right;
		if(n) *n=1;
		return right;
	}
	FT_UInt GetSize() {return size;}
	void Add(cFontGlyph &fg, VITER vv)
	{
		v.insert(vv, fg);
	}
};

class cFontCache
{
	friend class cFont;
private:
	FT_UInt LoadMode;
	FT_Face face;
	FT_UInt size;
	vector<int> v;
	cFontGlyphRow* p;
	cFontGlyphRow* Search()
	{
		for(unsigned int i=0;i<v.size();i++)
			if(((cFontGlyphRow*)(v[i]))->GetSize()==size) return ((cFontGlyphRow*)(v[i]));
		return 0;
	}
public:	

	cFontCache(const char *fname, int Hint=0)
	{
		memset(this, 0, sizeof(cFontCache));
		if ( error = FT_New_Face( ft_lib, fname, 0, &face )) abort();
		SetSize();
		LoadMode = FT_LOAD_DEFAULT;
		if(Hint) LoadMode|=FT_LOAD_FORCE_AUTOHINT;
	}
	~cFontCache() {Delete();}
	void Delete()
	{
		if(face)
			if ( error = FT_Done_Face(face )) abort();
		face = 0;
		while(!v.empty())
		{
			((cFontGlyphRow*)(v[v.size()-1]))->Delete();
			v.pop_back();
		}
	}
	void SetSize(FT_UInt pt=16)
	{
		FT_Set_Pixel_Sizes(face, 0, size=pt);
		if(v.empty()) v.push_back((int)(p = new cFontGlyphRow(size)));
		else
		if(!(p=Search()))
		{
			v.push_back((int)(p = new cFontGlyphRow(size)));
			int n=0;
			for(int i=0; i<128; i++)
			{
				FT_UInt glyph_index = FT_Get_Char_Index( face, i);
				if (!glyph_index) continue;
				n=0;
				VITER g=p->Search(glyph_index, &n);
				if(n)
				{
					error = FT_Load_Glyph( face, glyph_index, LoadMode);
					if ( error ) continue;
					cFontGlyph fg;
					fg.Index=glyph_index;				
					fg.Advance=face->glyph->advance.x;
					error = FT_Get_Glyph( face->glyph, &fg.Glyph );
					if ( error ) abort();
					p->Add(fg, g);
				}
			}
		}
	}
	FT_UInt GetStrSize(wchar_t *str)
	{
		FT_UInt len=0;
		int n=0;
		for(;*str&&*str!=L'\n'&&*str!=L'\t'&&*str!=L' ';str++)
		{
			FT_UInt glyph_index = FT_Get_Char_Index( face, *str);
			if (!glyph_index) continue;
			n=0;
			VITER g=p->Search(glyph_index, &n);
			if(n)
			{
				error = FT_Load_Glyph( face, glyph_index, LoadMode);
				if ( error ) continue;
				cFontGlyph fg;
				fg.Index=glyph_index;				
				fg.Advance=face->glyph->advance.x;
				error = FT_Get_Glyph( face->glyph, &fg.Glyph );
				if ( error ) abort();
				p->Add(fg, g);
				g=p->Search(glyph_index, &n);
			}
			len+=g->Advance;
		}
		return len;
	}
	cFontGlyphRow* Get()
	{
		if(p->GetSize()==size) return p;
		return Search();
	}
	const cFontGlyph* Get(FT_UInt index)
	{
		return &(*(p->Search(index)));
	}

	FT_UInt Draw(SDL_Renderer *r,wchar_t **stri, FT_UInt pen_x, FT_UInt pen_y, FT_Render_Mode render_mode)
	{
		wchar_t *str=*stri;
		for (;*str&&*str!=L'\n';*str++)
		{
			FT_UInt pen_X = pen_x >> 6;
			FT_UInt glyph_index = FT_Get_Char_Index( face, *str);
			if (!glyph_index) continue;

			FT_Glyph glyph;
			const cFontGlyph *fg;
			FT_Glyph_Copy((fg=Get(glyph_index))->Glyph, &glyph);

			if(*str==L'\t'||*str==L' ')
			{
				FT_Done_Glyph(glyph);
				pen_x += fg->Advance;
				*stri=str;
				return pen_x;
			}

			FT_Vector origin={pen_x-(pen_X<<6), 0};
			FT_Glyph_To_Bitmap(&glyph, render_mode, &origin, 1 ); 

			FT_BitmapGlyph glyph_bitmap = (FT_BitmapGlyph)glyph;

			if(glyph_bitmap->bitmap.buffer)
			{
				SDL_Rect rc={0,0,
					glyph_bitmap->bitmap.width,
					glyph_bitmap->bitmap.rows};

				int *p = new int[rc.w*rc.h];
				if(render_mode==FT_RENDER_MODE_MONO)
					for(int y=0; y<rc.h; y++)
					for(int x=0; x<rc.w; x++)
						p[y*rc.w+x] = 0xffffff00 | (255 * !!(glyph_bitmap->bitmap.buffer[y*glyph_bitmap->bitmap.pitch + (x/8)]&(0x80>>(x%8))));
				else
					for(int i=0; i<rc.w*rc.h; i++)
						p[i] = 0xffffff00 | (glyph_bitmap->bitmap.buffer[i]);

				SDL_UpdateTexture(tex, &rc,p,rc.w*4);
				SDL_Rect rc2={
					(pen_X)+glyph_bitmap->left,
					pen_y-glyph_bitmap->top,
					rc.w,rc.h};
				SDL_RenderCopy(r, tex, &rc, &rc2);

				pen_x += fg->Advance;
			}
			FT_Done_Glyph(glyph);

			
		}
		*stri=str;
		return pen_x;
	}
};

int clock=0;

class cFont
{
private: /* handle to face object */
	cFontCache *f;
public:
	cFont()
	{
		memset(this, 0, sizeof(cFont));
	}
	~cFont()
	{
		Close();
	}
	void SetSize(FT_UInt pt=16)
	{
		if(!f) return;
		f->SetSize(pt);
	}
	void Open(const char *fname = "Roboto-Regular.ttf", int Hint=0)
	{
		Close();
		f = new cFontCache(fname, Hint);
	}
	void Close()
	{
		if(f) {delete f;}
		f = 0;
	}
	int DrawTo(SDL_Renderer *r, wchar_t *stri = L"%NO_STRING%", int strsz=0, SDL_Rect *rci=0, int Mono=0)
	{
		if(!f) return 0;
		if(!rci) rci = &defaultrect;
		FT_Render_Mode render_mode = FT_RENDER_MODE_NORMAL;
		if(Mono) render_mode = FT_RENDER_MODE_MONO;
		
		if(strsz<=0)
		{
			strsz=1;
			for (wchar_t *str=stri;*str;str++) strsz++;
		}
		
		FT_UInt w = rci->w<<6;
		FT_UInt pen_x = 0;
		FT_UInt pen_y = 0;
		FT_UInt len = 0;

		for(wchar_t *str=stri;*str;)
		{
			len = f->GetStrSize(str);
			pen_x = clock/(5000/64);
			pen_y += f->size;
			if(len >= w)
			{
				f->Draw(r,&str,pen_x+(rci->x<<6),pen_y+rci->y, render_mode);
				str++;
			}
			else
			while(len < w && *str)
			{
				len = (pen_x = f->Draw(r, &str, pen_x+(rci->x<<6), pen_y+rci->y, render_mode)-(rci->x<<6));
				if(*str == L'\n')
				{
					str++;
					break;
				}
				str++;
				len += f->GetStrSize(str);
			}
		}
		return pen_y+rci->y;
	}
} Arial, AA;

void rTestInit(SDL_Renderer *r)
{
	if( error = FT_Init_FreeType( &ft_lib ) ) abort();
	tex=SDL_CreateTexture(r, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 256, 256);
	SDL_SetTextureBlendMode(tex,SDL_BLENDMODE_BLEND);

	Arial.Open();
	AA.Open("Roboto-Regular.ttf", 1);
	Arial.SetSize(16);
	AA.SetSize(16);
}
void rTestRelease()
{
	Arial.Close();
	AA.Close();
	SDL_DestroyTexture(tex);
	
	if ( error = FT_Done_FreeType(ft_lib) ) abort();
}


void rTest(SDL_Renderer *r)
{
	SDL_SetRenderDrawColor(r, 0, 0, 0, 0);
	SDL_RenderClear(r);
	clock = SDL_GetTicks();
	int h=((clock/5000)%7)*((clock/5000)%7)+10;
	Arial.SetSize(h);
	AA.SetSize(h);
	defaultrect.w=128;

	defaultrect.x=0;
	defaultrect.y=0;
	Arial.DrawTo(r, teststring, -1, &defaultrect, 0);
	defaultrect.x+=defaultrect.w;
	Arial.DrawTo(r, teststring, -1, &defaultrect, 0);
	defaultrect.x+=defaultrect.w;
	Arial.DrawTo(r, teststring, -1,  &defaultrect, 1);
	defaultrect.x+=defaultrect.w;
	AA.DrawTo(r, teststring, -1, &defaultrect, 1);
}
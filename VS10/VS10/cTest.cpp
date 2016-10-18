#include "cFT.h"
#include "cTest.h"

cFont Arial, AA;
wchar_t teststring[2048];

void rTestInit(SDL_Renderer *r)
{
	rFT_Init(r);

	Arial.Open("times.ttf");
	AA.Open("times.ttf", 1);
	Arial.SetSize(16);
	AA.SetSize(16);
}
void rTestRelease()
{
	Arial.Close();
	AA.Close();
	
	rFT_Release();
}

int x=0, y=0;
float gamma = 1.f;
void rTest(SDL_Renderer *r, SDL_Scancode sc)
{
	SDL_SetRenderDrawColor(r, 0xff, 0xff, 0xff, 0xff);
	SDL_RenderClear(r);
	int h=16;
	Arial.SetSize(h);
	AA.SetSize(h);
	
	SDL_Rect defaultrect = {0,0,256,1024};
	defaultrect.w=0;

	switch(sc)
	{
	case SDL_SCANCODE_F2:
		Arial.SwitchSubPixelLine();
		AA.SwitchSubPixelLine();
		break;
	case SDL_SCANCODE_UP:
		y--;
		break;
	case SDL_SCANCODE_DOWN:
		y++;
		break;		
	case SDL_SCANCODE_LEFT:
		x-= x/2 + 1;
		break;
	case SDL_SCANCODE_RIGHT:
		x+= x/2 + 1;
		break;
	case SDL_SCANCODE_A:
		gamma+=0.05f;
		Arial.GammaSet(gamma);
		AA.GammaSet(gamma);

		break;
	case SDL_SCANCODE_Z:
		gamma-=0.05f;
		Arial.GammaSet(gamma);
		AA.GammaSet(gamma);
		break;
	};
	
	for(int i=0; i<4;i++)
	{
		defaultrect.x=x+(i<<4);
		defaultrect.y=y+h*i+h;
		swprintf(teststring, L"Write formatted data to wide string... Pos: \t\t%i\t\t%i", defaultrect.x, defaultrect.y);
		Arial.DrawTo(r, teststring, -1, &defaultrect, 0);
		defaultrect.y+=h*4;
		swprintf(teststring, L"Write formatted data to wide string... Pos: \t\t%i\t\t%i", defaultrect.x, defaultrect.y);
		AA.DrawTo(r, teststring, -1, &defaultrect, 0);
		defaultrect.y+=h*4;
		swprintf(teststring, L"Write formatted data to wide string... Pos: \t\t%i\t\t%i", defaultrect.x, defaultrect.y);
		Arial.DrawTo(r, teststring, -1, &defaultrect, 1);
		defaultrect.y+=h*4;
		swprintf(teststring, L"Write formatted data to wide string... Pos: \t\t%i\t\t%i", defaultrect.x, defaultrect.y);
		AA.DrawTo(r, teststring, -1, &defaultrect, 1);
	}

}
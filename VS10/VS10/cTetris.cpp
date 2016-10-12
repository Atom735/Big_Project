#include "cTetris.h"

const char      *g_BlockFileName = "block.bmp";
const char      *g_FigureFileName= "blocks.txt";


char *GetFirstNumber(char *str)
{
	while(*str)
	{
		if(*str>='0' && *str<='9') return str;
		if(*str=='#')
		{
			for(;*str!='\n';str++)
				if(!(*str)) return 0;
		}
		str++;
	}
	return 0;
}
int GetNumber(char **s)
{
	char *str=*s;
	if(!(str = GetFirstNumber(str))) return 0;
	bool minus = (*(str-1) == '-');
	if(minus) str++;
	int n=0;
	while((*str>='0' && *str<='9'))
	{
		n*=10;
		n+=*str-'0';
		str++;
	}
	*s=str;
	if(minus) return -n;
	return n;
}

struct cGameTetris::cBlockFigure
{
	int Id, IdL, IdR, IdS;
	int dx, dy;
	int r, g, b;
	int bsize;
	int *bx, *by;

	cBlockFigure()
	{
		memset(this, 0, sizeof(cBlockFigure));
	}
	~cBlockFigure()
	{
		bsize=0;
		if(bx) delete[] bx; bx=0;
		if(by) delete[] by; by=0;
	}
};

class cGameTetris::cFigureAtlas
{
private:
	int N;
	cBlockFigure *pF;
public:
	void Refresh()
	{
		char fs[16384];
		{
			FILE * pFile;
			pFile = fopen (g_FigureFileName , "rb");
			if (pFile == NULL) {perror ("Error opening file");abort();}
			int i=0;
			if ( (i=fread  (fs , 1, 16384 , pFile)) != 0 )
			fs[i]=0;
			fclose (pFile);
		}
		char *sn=GetFirstNumber(fs);
		N=GetNumber(&sn);
		if(pF) delete[] pF;
		pF=new cBlockFigure[N];
		for(int i=0; i<N; i++)
		{
			pF[i].Id = GetNumber(&sn);

			pF[i].IdL = GetNumber(&sn);
			pF[i].IdR = GetNumber(&sn);
			pF[i].IdS = GetNumber(&sn);

			pF[i].dx = GetNumber(&sn);
			pF[i].dy = GetNumber(&sn);

			pF[i].r = GetNumber(&sn);
			pF[i].g = GetNumber(&sn);
			pF[i].b = GetNumber(&sn);

			pF[i].bsize = GetNumber(&sn);
			if(pF[i].bsize)
			{
				pF[i].bx = new int[pF[i].bsize];
				pF[i].by = new int[pF[i].bsize];
				for(int j=0; j<pF[i].bsize; j++)
				{
					pF[i].bx[j] = GetNumber(&sn);
					pF[i].by[j] = GetNumber(&sn);
				}
			}
		}
	}
	cBlockFigure *GetFigure(int Id)
	{
		if(Id<0)
		{
			srand(rand());
			cBlockFigure *o=pF+(rand()%N);
			while(!o->bsize)
			{
				srand(rand());
				o=pF+(rand()%N);
			}
			return o;
		}
		if(!pF) return 0;
		for(int i=0;i<N;i++)
			if(pF[i].Id==Id) return pF+i;
		return 0;
	}
	cFigureAtlas()
	{
		memset(this, 0, sizeof(cFigureAtlas));
	}
	~cFigureAtlas()
	{
		N=0;
		if(pF) delete[] pF; pF=0;
	}
};

int cGameTetris::CanStepNext()
{
	if(pf)
	for(int i=0;i<pf->bsize;i++)
	{
		int x=(X-pf->dx+pf->bx[i]);
		int y=(Y-pf->dy+1+pf->by[i]);
		if(y>=h || Map[y*w+x]) return 0;
	}
	return 1;
}

int cGameTetris::CanStepMove(int ix)
{
	if(pf)
	for(int i=0;i<pf->bsize;i++)
	{
		int x=(X-pf->dx+ix+pf->bx[i]);
		int y=(Y-pf->dy+pf->by[i]);
		if(x<0 || x>=w || Map[y*w+x]) return 0;
	}
	return 1;
}
int cGameTetris::CanStepUp(cBlockFigure *pf)
{
	if((!pf) || (!FA)) return 1;
	for(int i=0;i<pf->bsize;i++)
	{
		int x=(X-pf->dx+pf->bx[i]);
		int y=(Y-pf->dy+pf->by[i]);
		if(x<0 || x>=w || Map[y*w+x]) return 0;
	}
	return 1;
}

void cGameTetris::FigureNext()
{
	for(int i=0;i<pf->bsize;i++)
	{
		int x=(X-pf->dx+pf->bx[i]);
		int y=(Y-pf->dy+pf->by[i]);
		Map[y*w+x]=pf->IdS;
		if(y<=1) state = -1;
	}

	{
		int line=0;
		int pl=100;
		for(int y=0;y<h;y++)
		{
			int xln=0;
			for(int x=0;x<w;x++)
				if(Map[y*w+x])
					xln++;
			if(xln==w)
			{
				pointsLine++;
				points+=pl;
				pl+=75;

				for(int y2=y;y2>0;y2--)
					for(int x=0;x<w;x++)
						Map[y2*w+x]=Map[(y2-1)*w+x];
			}
		}
	}

	X=w/2;Y=0;
	pf = pf2;
	pf2 = FA->GetFigure(-1);
}

void cGameTetris::StepMove(int x, int y)
{
	if(!x && !y) {Step();return;}
	if(y<0)
	{
		if(y==-1)
		{
			cBlockFigure *pfu=FA->GetFigure(pf->IdR);
			if(CanStepUp(pfu)) pf=pfu;
			return;
		}
		cBlockFigure *pfu=FA->GetFigure(pf->IdL);
		if(CanStepUp(pfu)) pf=pfu;
		return;
	}
	if(y>0)
	{
		tickPrevStep=tickReal;
		if(CanStepNext())
		{Y++;return;}
		FigureNext();
		return;
	}

	if(x>0) x= 1;
	else    x=-1;
	if(CanStepMove(x))
		X+=x;
}

void cGameTetris::StepNext()
{
	tickPrevStep+=tickPerStep;
	if(CanStepNext())
	{Y++;return;}
	
	FigureNext();
}


void cGameTetris::Step()
{
	if(state == 0 || state == -1)
	{
		int i= SDL_GetTicks();
		tickPrevStep+=i-tickReal;
		tickReal = i;
		return;
	}
	tickReal = SDL_GetTicks();
	if(tickReal>=tickPrevStep+tickPerStep) StepNext();
}

void cGameTetris::Draw(SDL_Renderer *r)
{
	if(!r) return;
	if(!blockTexture) return;
	SDL_Rect rc={0,0,blockSize,blockSize};	
	SDL_SetTextureColorMod(blockTexture,0xff,0xff,0xff);
	for(int i=0;i<w+2;i++)
	{
		rc.x=i*blockSize;
		rc.y=0;
		SDL_RenderCopy(r, blockTexture, 0, &rc);
		rc.x=i*blockSize;
		rc.y=(h+1)*blockSize;
		SDL_RenderCopy(r, blockTexture, 0, &rc);
	}
	for(int i=0;i<h;i++)
	{
		rc.x=0;
		rc.y=(i+1)*blockSize;
		SDL_RenderCopy(r, blockTexture, 0, &rc);
		rc.x=(w+1)*blockSize;
		rc.y=(i+1)*blockSize;
		SDL_RenderCopy(r, blockTexture, 0, &rc);
	}
	if(state == 0)
	{
		int i=tickReal%999;
		if(i>499) i=999-i;
		i=(i*255)/499;
		for(int y=0;y<h;y++)
		for(int x=0;x<w;x++)
		{
			SDL_SetTextureColorMod(blockTexture,(i&0xf0)^((x==((tickReal/100)%w))*0xff),(i&0xf0)^((y==((tickReal/100)%h))*0xff),(i&0xf0));
			rc.x=(x+1)*blockSize;
			rc.y=(y+1)*blockSize;
			SDL_RenderCopy(r, blockTexture, 0, &rc);
		}
		return;
	}
	if(state == -1)
	{
		for(int y=0;y<h;y++)
		for(int x=0;x<w;x++)
		{
			int i=(x-y)*0x7f/w+(tickReal/7)&0xf0;
			SDL_SetTextureColorMod(blockTexture,i,i^0x55,i^0x30);
			rc.x=(x+1)*blockSize;
			rc.y=(y+1)*blockSize;
			SDL_RenderCopy(r, blockTexture, 0, &rc);
		}
	}

	if(!Map) {state = -1; return;}
	if(!FA) {state = -1; return;}

	{
		int m;
		cBlockFigure *pf=0;
		for(int y=0;y<h;y++)
			for(int x=0;x<w;x++)
				if(m=Map[y*w+x])
				{
					if(pf=FA->GetFigure(m))
						SDL_SetTextureColorMod(blockTexture,pf->r,pf->g,pf->b);
					else
						SDL_SetTextureColorMod(blockTexture,0xff,0xff,0xff);
					rc.x=(x+1)*blockSize;
					rc.y=(y+1)*blockSize;
					SDL_RenderCopy(r, blockTexture, 0, &rc);
				}
	}
	{
		int yAnim=((blockSize*1000*(tickPerStep+tickPrevStep-tickReal))/tickPerStep)/1000;
		SDL_SetTextureColorMod(blockTexture,pf->r,pf->g,pf->b);
		for(int i=0;i<pf->bsize;i++)
		{
			rc.x=(X-pf->dx+1+pf->bx[i])*blockSize;
			rc.y=(Y-pf->dy+1+pf->by[i])*blockSize-yAnim;
			SDL_RenderCopy(r, blockTexture, 0, &rc);
		}

		SDL_SetTextureColorMod(blockTexture,pf2->r,pf2->g,pf2->b);
		for(int i=0;i<pf2->bsize;i++)
		{
			rc.x=(w+3-pf2->dx+1+pf2->bx[i])*blockSize;
			rc.y=(h/2-pf2->dy+1+pf2->by[i])*blockSize;
			SDL_RenderCopy(r, blockTexture, 0, &rc);
		}
	}
}
void cGameTetris::Resize(int W, int H)
{
	blockSize = 32;
}

void cGameTetris::StartNew(SDL_Renderer *r, int Width, int Height)
{
	w=Width;h=Height;
	if(w<=10) w=10;
	if(h<=20) h=20;
	if(Map) delete[] Map; Map=new int[w*h];
	memset(Map, 0, w*h*sizeof(int));
	if(!FA) FA = new cFigureAtlas;
	FA->Refresh();
	
	srand(SDL_GetTicks());

	if(blockTexture) SDL_DestroyTexture(blockTexture);
	SDL_Surface *blockSurface = 0;

	if (!(blockSurface = SDL_LoadBMP(g_BlockFileName))) 
	{
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create surface from image \"%s\": %s", g_BlockFileName, SDL_GetError());
		abort();
    }	
    if (!(blockTexture = SDL_CreateTextureFromSurface(r, blockSurface))) 
	{
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture from surface: %s", SDL_GetError());
        abort();
    }
    SDL_FreeSurface(blockSurface);

	state=1;
	X=w/2;Y=0;
	pf = FA->GetFigure(-1);
	pf2 = FA->GetFigure(-1);
	points=0;
	pointsLine=0;
	tickPrevStep=tickStart=SDL_GetTicks();
}
void cGameTetris::Close()
{
	pf=pf2=0;
	if(FA) delete FA; FA=0;
	if(Map) delete[] Map; Map=0;		
	if(blockTexture) SDL_DestroyTexture(blockTexture); blockTexture=0;
}

cGameTetris::cGameTetris()
{
	memset(this, 0, sizeof(cGameTetris));
	Resize();
	SetTickStep(333);
}
cGameTetris::~cGameTetris()
{
	pf=pf2=0;
	if(FA) delete FA; FA=0;
	if(Map) delete[] Map; Map=0;		
	if(blockTexture) SDL_DestroyTexture(blockTexture); blockTexture=0;
}

int cGameTetris::SetTickStep(int tick)
{if(tick > 10) tickPerStep = tick;return tickPerStep;}

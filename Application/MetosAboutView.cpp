/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
/***************************/
/* vue de la fenetre about */
/***************************/
#include "MetosAboutView.h"
#include "MetosAppDefs.h"
#include "MetosGlobalDefs.h"
#include "MetosApp.h"
#include <interface/Bitmap.h>
#include <support/String.h>
#include <time.h>

const	int32	ABOUT_LINES_NUMBER = 30;

// texte de l'about
const char *aboutText[ABOUT_LINES_NUMBER] =
{
	"  MeTOS GUI Creator",
	"",
	" Created by :",
	"   Vincent Cedric - CKJ",
	"",
	" Thanks to :",
	"   Sylvain Tertois for the libCS",
	"   Guy Haviv for documentation",
	"   Brent Miszalski for smooth effect",
	"   Brian Francis for PPC version",
	" and...",
	"   Marco Nelissen for upgrader code.",
	"",
	"",
	"",
	"",
	" For help, testing and ideas :",
	" Bernd, Konrad, Cube-ness, m7m, ",
	" Baldur, mmu_man, loll, ",
	" aliensoldier, and more ...",
	"The project start in 2001, a first",
	"code release have been made,",
	"but i see lot of problem, and restart",
	"all in May 2001. You can get recent",
	"code or binary on my web site",
	"To contact me :",
	"ckj.beos@wanadoo.fr",
	"http://perso.wanadoo.fr/cvincent/Home_page.html",
	"",
	"Note : feedback is very important !"
};

/**** constructeur ****/
MetosAboutView::MetosAboutView(BRect frame)
: BView(frame,"about-view",B_FOLLOW_NONE, B_WILL_DRAW | B_PULSE_NEEDED)
{
	// initialiser
	_animCounter = ABOUT_LINES_NUMBER - 5;
	_indexText = 0;
	_mode = B_OP_COPY;
	
	// recuperer l'image de fond
	_background = g_MetosApp->GetBitmap("SplashScreen");
	_logo[0] = g_MetosApp->GetBitmap("BeOSLogo");
	_logo[1] = g_MetosApp->GetBitmap("ZetaLogo");
	
	// pour l'animation
	_smoothBitmap = new BBitmap(BRect(0,0,220,75),B_RGB32,true);
	_smoothView = new BView(BRect(0,0,220,75),"smooth-view",B_FOLLOW_NONE, B_WILL_DRAW);
	if(_smoothBitmap->Lock())
	{
		_smoothBitmap->AddChild(_smoothView);
		_smoothBitmap->Unlock();
	}
	else
		g_MetosApp->SendDebbug("Error : LockLooper() in MetosAboutView::MetosAboutView()",VDEBUG_ERROR);
	
	// fixer la taille des fontes
	SetFontSize(10);
}

/**** destructeur ****/
MetosAboutView::~MetosAboutView()
{
	// image de fond
	if(_background!=NULL)
		delete _background;
	
	// logo
	if(_logo[0]!=NULL)
		delete _logo[0];
	if(_logo[1]!=NULL)
		delete _logo[1];

	// image de l'animation	
	delete _smoothBitmap;
}

/**** dessin ****/
void MetosAboutView::Draw(BRect updateRect)
{
	// dessin de l'image
	DrawBitmap(_background,BPoint(-5,-5));
	DrawBitmap(_logo[0],BPoint(15,257));
	DrawBitmap(_logo[1],BPoint(70,256));

	// dessiner l'animation
	DrawAnimation();

	// version
	SetDrawingMode(B_OP_COPY);
	SetHighColor(0,0,0);
	SetLowColor(255,207,74);
	DrawString("Version : ",BPoint(135,274));	
	DrawString(APPLICATION_VERSION,BPoint(180,274));	
	DrawString("Build : ",BPoint(239,274));	
	DrawString(UPGRADER_VERSION,BPoint(268,274));	
}

/**** dessiner l'animation ****/
void MetosAboutView::DrawAnimation()
{
	// bloquer l'image
	if(_smoothBitmap->Lock())
	{
		_smoothView->SetDrawingMode(_mode);
		_smoothView->SetHighColor(255,207,74,96);
		_smoothView->SetLowColor(255,207,74,96);
		_smoothView->FillRect(_smoothView->Bounds());
	
		// texte
		_smoothView->SetDrawingMode(B_OP_COPY);
		_smoothView->SetHighColor(0,0,0,128);
		for(int32 i=0;i<5;i++)
			_smoothView->DrawString(aboutText[i+_indexText],BPoint(5,10+i*12));

		// synchronisation
		_smoothView->Sync();
		DrawBitmapAsync(_smoothBitmap,BPoint(12,180));
	
		// afficher
		Flush();
		Sync();	
	
		// debloquer
		_smoothBitmap->Unlock();	
	}
	else
		g_MetosApp->SendDebbug("Error : Lock() in MetosAboutView::DrawAnimation()",VDEBUG_ERROR);
}

/**** evenement timer ****/
void MetosAboutView::Pulse()
{
	_animCounter++;
	if(_animCounter==34)
	{
		// decalage du texte a afficher
		_indexText += 5;
		if(_indexText>(ABOUT_LINES_NUMBER-1))
			_indexText = 0;
	
		// reinitialiser
		_animCounter = 0;
		_mode = B_OP_ALPHA;
	}

	// dessiner l'animation
	DrawAnimation();
}

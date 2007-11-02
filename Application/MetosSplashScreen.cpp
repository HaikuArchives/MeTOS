/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
/*************************************/
/* fenetre de progression du parsing */
/*************************************/
#include "MetosSplashScreen.h"
#include "MetosApp.h"
#include <interface/View.h>
#include <translation/TranslationUtils.h>
#include <interface/Bitmap.h>
#include <app/Message.h>

//=====================================
// la vue
//=====================================
DisplayView::DisplayView(BRect frame)
: BView(frame,"support-view",B_FOLLOW_NONE,B_WILL_DRAW)
{
	// initialiser les variables
	_nbFiles = 0;
	_counter = 0;
	_counterStep = 0;
	_parsingRect.Set(10,15,140,25);
	_displayText = "Parsing...";
	
	// fixer les couleurs
	SetLowColor(255,209,74);
	SetViewColor(255,209,74);
	
	// taille des fontes
	SetFontSize(10);
	
	// charger l'images du splash-screen
	_screenBitmap = g_MetosApp->GetBitmap("SplashScreen");
	_logo[0] = g_MetosApp->GetBitmap("BeOSLogo");
	_logo[1] = g_MetosApp->GetBitmap("ZetaLogo");
	_parsingRect.Set(40,240,170,250);
}

DisplayView::~DisplayView()	
{
	// image de fond
	if(_screenBitmap!=NULL)
		delete _screenBitmap;

	// logo
	if(_logo[0]!=NULL)
		delete _logo[0];
	if(_logo[1]!=NULL)
		delete _logo[1];
}

void DisplayView::Draw(BRect updateRect)
{
	BRect	statusRect = _parsingRect;

	BView::Draw(updateRect);
	
	// afficher l'image
	DrawBitmap(_screenBitmap,Bounds());
	DrawBitmap(_logo[0],BPoint(20,259));
	DrawBitmap(_logo[1],BPoint(75,258));

	// a propos
	SetFontSize(10);
	DrawString("Created by :",BPoint(160,265));
	DrawString("Vincent Cedric",BPoint(160,276));

	// info
	SetHighColor(55,55,55);
	DrawString(_displayText.String(),BPoint(statusRect.left,statusRect.top-5));
	
	// dessiner l'evolution
	SetHighColor(25,25,25);
	StrokeRect(statusRect);
	statusRect.InsetBy(1,1);
	statusRect.right = statusRect.left + _counter;
	SetHighColor(255,169,34);
	StrokeLine(BPoint(statusRect.right,statusRect.top),BPoint(statusRect.right,statusRect.bottom));
	StrokeLine(BPoint(statusRect.left,statusRect.bottom),BPoint(statusRect.right,statusRect.bottom));
	SetHighColor(255,255,144);
	StrokeLine(BPoint(statusRect.left,statusRect.top),BPoint(statusRect.left,statusRect.bottom));
	StrokeLine(BPoint(statusRect.left,statusRect.top),BPoint(statusRect.right,statusRect.top));
	SetHighColor(255,239,104);
	statusRect.InsetBy(1,1);
	FillRect(statusRect);
}

/**** on defini le nombre de fichiser ****/
void DisplayView::SetNbFiles(float nbFiles)
{
	_counter = 0;
	_nbFiles = nbFiles;
	_counterStep = 128 / nbFiles;
}

/**** redessiner le text d'info ****/
void DisplayView::InvalidateText()
{
	BRect	textZone;
	
	textZone = _parsingRect;
	textZone.OffsetBy(0,-13);
	Invalidate(textZone);
}

//=====================================
// la fenetre
//=====================================
MetosSplashScreen::MetosSplashScreen(BRect frame)
: BWindow(frame,"InfoParsing",B_NO_BORDER_WINDOW_LOOK,B_MODAL_APP_WINDOW_FEEL,B_AVOID_FOCUS)
{
	// creer la vue support
	_view = new DisplayView(Bounds());

	AddChild(_view);
	_view->Invalidate();
}

MetosSplashScreen::~MetosSplashScreen()
{
}

void MetosSplashScreen::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
	case M_PROGRESSSTART_MSG:
		{
			int32		nbFiles;
			BString		newText;
			
			// recuperer le nombre de fichier
			if(message->FindInt32(M_TOTALPROGRESS,&nbFiles)==B_OK)
				_view->SetNbFiles(nbFiles);

			// recuperer le text a definir
			if(message->FindString(M_TEXTDISPLAY,&newText)==B_OK)
				_view->SetTextDisplay(newText.String());
				
			// rafraichir l'affichage
			_view->InvalidateText();
		}
		break;
	case M_PROGRESSINCREASE_MSG:
		_view->IncreaseCounter();
		_view->InvalidateStatus();
		break;
	default:
		BWindow::MessageReceived(message);
	}
}

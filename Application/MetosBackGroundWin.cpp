/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
/***************************/
/* fenetre du fond d'ecran */
/***************************/
#include "MetosBackGroundWin.h"
#include "MetosMessageDefs.h"
#include "MetosGlobalDefs.h"
#include "VPreferenceFile.h"
#include "MetosApp.h"
#include <interface/View.h>
#include <app/Message.h>
#include <interface/Screen.h>
#include <translation/TranslationUtils.h>
#include <interface/Bitmap.h>
#include <storage/Entry.h>
#include <storage/Path.h>

/**** constructeur ****/
MetosBackGroundWin::MetosBackGroundWin()
: BWindow(ScreenFrame(),"MeTOS", B_NO_BORDER_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL , B_NOT_RESIZABLE | B_NOT_ZOOMABLE )
{
	// creer la vue support est l'attacher
	_supportView = new BView(Bounds(),"support-background-view",B_FOLLOW_ALL_SIDES, B_WILL_DRAW);
	_supportView->SetViewColor(VIEW_COLOR);
	
	AddChild(_supportView);
	
	// preferences
	RefreshPreferences();
}

/**** destructeur ****/
MetosBackGroundWin::~MetosBackGroundWin()
{
}

/**** gestion des messages ****/
void MetosBackGroundWin::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
	// drag & drop, image ou couleur
	case B_SIMPLE_DATA:
	case 'PSTE':	// ArtPaint DropColor
		ManageDragAndDrop(message);
	// preferences
	case M_PREFERENCE_MSG:
		RefreshPreferences();
		break;
	default:
		BWindow::MessageReceived(message);	
	}
}

/**** determiner la taille de l'ecran ****/
BRect MetosBackGroundWin::ScreenFrame()
{
	// determiner la taille de l'ecran
	BScreen		screen;
	
	// taille de l'ecran
	return screen.Frame();
}

/**** gerer le drag&drop ****/
void MetosBackGroundWin::ManageDragAndDrop(BMessage *message)
{
	rgb_color			*color;
	ssize_t 			size;
	entry_ref			ref;
	
	// verifier ce qu'on a recu
	// c'est peux-etre une image
	if(message->FindRef("refs",&ref)==B_OK)
	{
		// charger l'image
		_background = BTranslationUtils::GetBitmap(&ref);
		if(_background!=NULL)
		{
			BPath	path(&ref);
			
			// stretcher l'image
			if(Lock())
			{
				ChildAt(0)->SetViewBitmap(_background,_background->Bounds(),Bounds());
				ChildAt(0)->Invalidate();			
				delete _background;
				Unlock();
			
				// sauver les prefs
				g_MeTOSPrefs->Load();
				g_MeTOSPrefs->SetString(path.Path(),"background-bitmap-path");
				g_MeTOSPrefs->Save();
			}
			else
				g_MetosApp->SendDebbug("Error : Lock() in MetosApp::ManageDragAndDrop()",VDEBUG_ERROR);
		}
	}
	
	// handle roColour drag&drop
	if(message->FindData("RGBColor",'RGBC',(const void **)(&color),&size)==B_OK)
	{
		_backgroundColor = *color;
		
		// fixer la couleur de la vue et sauver les prefs
		if(Lock())
		{
			ChildAt(0)->SetViewColor(_backgroundColor);		
			ChildAt(0)->Invalidate();
			Unlock();
		
			// sauver les prefs
			g_MeTOSPrefs->Load();
			g_MeTOSPrefs->SetRGBColor(_backgroundColor,"background-color");
			g_MeTOSPrefs->Save();
		}
		else
			g_MetosApp->SendDebbug("Error : Lock() in MetosApp::ManageDragAndDrop()",VDEBUG_ERROR);
	}	
}

/**** rafraichir les preferences ****/
void MetosBackGroundWin::RefreshPreferences()
{
	// recuperer le niveau de debug
	_bitmapPath = g_MeTOSPrefs->GetString("","background-bitmap-path");
	_backgroundColor = g_ColorsPrefs->GetRGBColor(VIEW_COLOR,"background-color");

	// effacer l'image actuellement en fond
	if(Lock())
	{
		ChildAt(0)->ClearViewBitmap();

		// fixer la couleur de fond
		ChildAt(0)->SetViewColor(_backgroundColor);		
	
		// fixer le fond
		if(_bitmapPath.Length()>0)
		{
			// charger l'image
			_background = BTranslationUtils::GetBitmapFile(_bitmapPath.String());
			if(_background!=NULL)
			{
				// stretcher l'image
				ChildAt(0)->SetViewBitmap(_background,_background->Bounds(),Bounds());
				delete _background;
			}
		}
	
		// rafraichir
		ChildAt(0)->Invalidate();
		Unlock();
	}
	else
		g_MetosApp->SendDebbug("Error : Lock() in MetosApp::RefreshPreferences()",VDEBUG_ERROR);
}

// =======================
// = Gestion des l'ecran =
// =======================
#include "ScreenPrefs.h"
#include "MetosMessageDefs.h"
#include "MetosGlobalDefs.h"
#include "VPreferenceFile.h"
#include "VLanguage.h"

#include <PopUpMenu.h>
#include <MenuItem.h>
#include <MenuField.h>
#include <String.h>

// messages
const uint32		SCREEN_SIZE_MSG		= 'Sscs';

/**** constructeur ****/
ScreenPrefs::ScreenPrefs(BRect frame)
: VPreference(frame,"Screen",METOS_GROUP)
{
	BRect				bounds = Bounds();
	ScreenBox			*box = NULL;
	BPopUpMenu			*menu = NULL;
	BString				buffer;

	// decallage a cause du titre
	bounds.top += VPREFERENCE_TITLE_HIGH;
	bounds.InsetBy(7,7);

	// le bloc Debug
	bounds.bottom = bounds.top + 100;
	box = new ScreenBox(bounds,"screen-settings");
	box->SetLabel(_language->GetTranslationFor("SCREEN_SETTINGS"));	

	// construire le menu du niveau de debug
	menu = new BPopUpMenu("screen-size");
	menu->AddItem(new BMenuItem("640*480",new BMessage(SCREEN_SIZE_MSG)));
	menu->AddItem(new BMenuItem("800*600",new BMessage(SCREEN_SIZE_MSG)));
	menu->AddItem(new BMenuItem("1024*768",new BMessage(SCREEN_SIZE_MSG)));
	menu->AddItem(new BMenuItem("1152*864",new BMessage(SCREEN_SIZE_MSG)));
	menu->AddItem(new BMenuItem("1280*1024",new BMessage(SCREEN_SIZE_MSG)));
	menu->AddItem(new BMenuItem("1600*1200",new BMessage(SCREEN_SIZE_MSG)));

	// fixer la taille utilisé
	_screenSize = g_MeTOSPrefs->GetInt32(SCREEN_1152_864,"screen-size");
	menu->ItemAt(_screenSize)->SetMarked(true);
	
	// le choix de niveau de debug
	_screenField = new BMenuField(BRect(140,45,290,59),"screen-size",_language->GetTranslationFor("SCREEN_SIZE"),menu);
	_screenField->SetDivider(65);
	
	// ajouter
	box->AddChild(_screenField);
	AddChild(box);
}

/**** attachement a la fenetre ****/
void ScreenPrefs::AttachedToWindow()
{
	// fonction parente
	VPreference::AttachedToWindow();

	// donner la destination du BMenuField
	_screenField->Menu()->SetTargetForItems(this);
}

/**** gestion des messages ****/
void ScreenPrefs::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
	case SCREEN_SIZE_MSG:
		SetScreenSize(message);
		break;
	default:
		VPreference::MessageReceived(message);
	}
}

/**** fixer la taille ****/
void ScreenPrefs::SetScreenSize(BMessage *message)
{
	int32	index = -1;
	
	// trouver l'index du menu
	if(message->FindInt32("index",&index)!=B_OK)
		return;

	// ok on prend l'index du menu selectionné
	_screenSize = index;
}

/**** sauver les preferences ****/
void ScreenPrefs::SavePrefs()
{
	// relire avant
	g_MeTOSPrefs->Load();

	// recuperer la taille demandé
	g_MeTOSPrefs->SetInt32(_screenSize,"screen-size");

	// sauver
	g_MeTOSPrefs->Save();
}

/**** constructeur ****/
ScreenBox::ScreenBox(BRect frame,const char *name)
: BBox(frame,name)
{
	_screenRect.Set(25,25,100,80);
}

/**** dessin de la vue ****/
void ScreenBox::Draw(BRect updateRect)
{
	BRect	littleScreen = _screenRect;
	BRect	socle = _screenRect;
	
	// appel parent
	BBox::Draw(updateRect);
	
	// fixer la position du socle
	socle.top = socle.bottom;
	socle.bottom += 5;
	socle.InsetBy(10,0);
	
	SetHighColor(162,162,162);
	FillRoundRect(_screenRect,4,4);
	FillRect(socle);
	SetHighColor(0,133,189);
	littleScreen.InsetBy(3,3);
	FillRoundRect(littleScreen,4,4);	
	SetHighColor(0,0,0);
	StrokeRoundRect(littleScreen,4,4);
	StrokeRoundRect(_screenRect,4,4);	
	StrokeRect(socle);
	
	// led
	SetHighColor(0,255,0);
	StrokeLine(BPoint(socle.right-6,socle.top+3),BPoint(socle.right-4,socle.top+3));
}

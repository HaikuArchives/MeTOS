// ================================
// = Preferences de l'application =
// ================================
#include "BackgroundPrefs.h"
#include "MetosMessageDefs.h"
#include "MetosGlobalDefs.h"
#include "VPreferenceFile.h"
#include "VLanguage.h"

#include <Box.h>
#include <CheckBox.h>
#include <TextControl.h>
#include <Button.h>
#include <File.h>
#include <Path.h>
#include <FindDirectory.h>
#include <FilePanel.h>
#include <Window.h>
#include <String.h>

/**** constructeur ****/
BackgroundPrefs::BackgroundPrefs(BRect frame)
: VPreference(frame,"BackGround",METOS_GROUP)
{
	BRect				bounds = Bounds();
	BString				buffer;
	BBox				*box = NULL;

	// decallage a cause du titre
	bounds.top += VPREFERENCE_TITLE_HIGH;
	bounds.InsetBy(7,7);

	// le bloc principale
	bounds.bottom = bounds.top + 98;
	box = new BBox(bounds,"background");
	box->SetLabel(_language->GetTranslationFor("PREFERENCE_NAME"));

	_useBackGround = new BCheckBox(BRect(20,20,150,34),"use-background",_language->GetTranslationFor("BACKGROUND_USE"),NULL);
	_bitmapPath = new BTextControl(BRect(20,40,305,54),"bitmap-path",_language->GetTranslationFor("BACKGROUND_BITMAP_PATH"),"",NULL);
	_bitmapPath->SetDivider(70);

	_selector = new BButton(BRect(306,37,324,52),"selector","...",new BMessage(OPEN_SELECTOR));
	_pathSelector = new BFilePanel(B_OPEN_PANEL,NULL,NULL,B_DIRECTORY_NODE,false,new BMessage(PATH_SELECTION),NULL,true);
	_pathSelector->Window()->SetType(B_FLOATING_WINDOW);
	_pathSelector->SetPanelDirectory(buffer.String());

	// fixer les parametres
	_useBackGround->SetValue(g_MeTOSPrefs->GetInt32(B_CONTROL_OFF,"use-background"));
	_bitmapPath->SetText((g_MeTOSPrefs->GetString("","background-bitmap-path")).String());

	box->AddChild(_useBackGround);
	box->AddChild(_bitmapPath);
	box->AddChild(_selector);
	AddChild(box);
}

/**** attachement a la fenetre ****/
void BackgroundPrefs::AttachedToWindow()
{
	// attachement standard
	VPreference::AttachedToWindow();

	// les controles
	_selector->SetTarget(this);
	_pathSelector->SetTarget(BMessenger(this));
}

/**** gestion des messages ****/
void BackgroundPrefs::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
	// ouvrir le selecteur de fichier
	case OPEN_SELECTOR:
		_pathSelector->Show();
		break;
	// recuperer le chemin du selecteur de fichier
	case PATH_SELECTION:
		{
			entry_ref	selectedPath;
			BPath		path;

			// le fichier doit exister
			if(message->FindRef("refs",&selectedPath)!=B_OK)
				break;
		
			path.SetTo(&selectedPath);
			if(path.InitCheck()!=B_OK)
				break;
				
			// afficher le nouveau chemin
			_bitmapPath->SetText(path.Path());
		}		
		break;
	// c'est envoyer par l'application
	// pour dire que l'on a cliqué sur le bouton "save"
	case M_SAVE_PREFERENCE_MSG:
		SavePrefs();
		break;
	default:
		VPreference::MessageReceived(message);
	}
}

/**** sauver les preferences ****/
void BackgroundPrefs::SavePrefs()
{
	// relire avant
	g_MeTOSPrefs->Load();
	
	// sauver les parametres
	// gui-manager
	g_MeTOSPrefs->SetInt32(_useBackGround->Value(),"use-background");
	g_MeTOSPrefs->SetString(_bitmapPath->Text(),"background-bitmap-path");

	// sauver
	g_MeTOSPrefs->Save();
}

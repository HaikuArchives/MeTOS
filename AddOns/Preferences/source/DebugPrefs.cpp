// ====================
// = Gestion du debug =
// ====================
#include "DebugPrefs.h"
#include "MetosMessageDefs.h"
#include "MetosGlobalDefs.h"
#include "VPreferenceFile.h"
#include "VLanguage.h"

#include <Box.h>
#include <PopUpMenu.h>
#include <MenuItem.h>
#include <MenuField.h>
#include <TextControl.h>
#include <String.h>
#include <stdio.h>

// messages
const uint32		DEBUG_LEVEL_MSG		= 'Dlvm';
const uint32		MDEBUG_INFO			= 0;

/**** constructeur ****/
DebugPrefs::DebugPrefs(BRect frame)
: VPreference(frame,"Debug",METOS_GROUP)
{
	BRect				bounds = Bounds();
	BBox				*box = NULL;
	BPopUpMenu			*menu = NULL;
	BString				buffer;

	// decallage a cause du titre
	bounds.top += VPREFERENCE_TITLE_HIGH;
	bounds.InsetBy(7,7);

	// le bloc Debug
	bounds.bottom = bounds.top + 100;
	box = new BBox(bounds,"debug-settings");
	box->SetLabel(_language->GetTranslationFor("DEBUG_SETTINGS"));	

	// construire le menu du niveau de debug
	menu = new BPopUpMenu("debug-level");
	menu->AddItem(new BMenuItem(_language->GetTranslationFor("DEBUG_LEVEL_INFO"),new BMessage(DEBUG_LEVEL_MSG)));
	menu->AddItem(new BMenuItem(_language->GetTranslationFor("DEBUG_LEVEL_WARN"),new BMessage(DEBUG_LEVEL_MSG)));
	menu->AddItem(new BMenuItem(_language->GetTranslationFor("DEBUG_LEVEL_ERR"),new BMessage(DEBUG_LEVEL_MSG)));
	menu->AddItem(new BMenuItem(_language->GetTranslationFor("DEBUG_LEVEL_DEBUG"),new BMessage(DEBUG_LEVEL_MSG)));

	// fixer le level utilisé
	_debugLevel = g_MeTOSPrefs->GetInt32(MDEBUG_INFO,"debug-level");
	menu->ItemAt(_debugLevel)->SetMarked(true);
	
	// le choix de niveau de debug
	_debugLevelMenu = new BMenuField(BRect(20,20,110,34),"debug-level",_language->GetTranslationFor("DEBUG_LEVEL"),menu);
	_debugLevelMenu->SetDivider(75);

	// le nombre de message
	_nbDebugMessage = new BTextControl(BRect(20,45,172,60),"nb-debug-message",_language->GetTranslationFor("DEBUG_NB_MESSAGES"),"",NULL);
	_nbDebugMessage->SetDivider(120);

	// fixer les parametres
	buffer = "";
	buffer << g_MeTOSPrefs->GetInt32(50,"max-debug-message");
	_nbDebugMessage->SetText(buffer.String());

	box->AddChild(_debugLevelMenu);
	box->AddChild(_nbDebugMessage);
	AddChild(box);
}

/**** attachement a la fenetre ****/
void DebugPrefs::AttachedToWindow()
{
	// attachement standard
	VPreference::AttachedToWindow();
	
	// donner la destination du BMenuField
	_debugLevelMenu->Menu()->SetTargetForItems(this);
	_nbDebugMessage->SetTarget(this);
}

/**** gestion des messages ****/
void DebugPrefs::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
	case DEBUG_LEVEL_MSG:
		SetDebugLevel(message);
		break;
	case M_SAVE_PREFERENCE_MSG:
		SavePrefs();
		break;
	default:
		VPreference::MessageReceived(message);
	}
}

/**** fixer le niveau de debbugage ****/
void DebugPrefs::SetDebugLevel(BMessage *message)
{
	int32	index = -1;
	
	// trouver l'index du menu
	if(message->FindInt32("index",&index)!=B_OK)
		return;

	// ok on prend l'index du menu selectionné
	_debugLevel = index;
}

/**** sauver les preferences ****/
void DebugPrefs::SavePrefs()
{
	int32				nbDebugMessage;

	// relire avant
	g_MeTOSPrefs->Load();

	// renseigner le niveau de debug
	g_MeTOSPrefs->SetInt32(_debugLevel,"debug-level");
	sscanf(_nbDebugMessage->Text(),"%ld",&nbDebugMessage);
	if(nbDebugMessage>150 || nbDebugMessage<20)
	{
		nbDebugMessage = 50;		
		_nbDebugMessage->SetText("50");
	}	
	g_MeTOSPrefs->SetInt32(nbDebugMessage,"max-debug-message");
		
	// sauver
	g_MeTOSPrefs->Save();
}

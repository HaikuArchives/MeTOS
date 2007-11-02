// ================================
// = Preferences de l'application =
// ================================
#include "ApplicationPrefs.h"
#include "MetosMessageDefs.h"
#include "MetosGlobalDefs.h"
#include "VPreferenceFile.h"
#include "VLanguage.h"

#include <Box.h>
#include <CheckBox.h>
#include <TextControl.h>
#include <ColorControl.h>
#include <String.h>
#include <stdio.h>

// messages
const uint32		GRIDUSED_CHECK_MSG		= 'Gusm';

/**** constructeur ****/
ApplicationPrefs::ApplicationPrefs(BRect frame)
: VPreference(frame,"Interface",METOS_GROUP)
{
	BRect				bounds = Bounds();
	BString				buffer;
	BBox				*box = NULL;

	// decallage a cause du titre
	bounds.top += VPREFERENCE_TITLE_HIGH;
	bounds.InsetBy(7,7);

	// le bloc GUIManager
	bounds.bottom = bounds.top + 98;
	box = new BBox(bounds,"gui-manager");
	box->SetLabel(_language->GetTranslationFor("APP_GUI_MANAGER"));

	_useGrid = new BCheckBox(BRect(20,20,190,34),"use-grid",_language->GetTranslationFor("APP_USE_GRID"),new BMessage(GRIDUSED_CHECK_MSG));
	_snapToGrid = new BCheckBox(BRect(20,36,110,50),"snap-grid",_language->GetTranslationFor("APP_SNAP_TO_GRID"),NULL);
	_resizeToGrid = new BCheckBox(BRect(20,52,210,66),"resize-grid",_language->GetTranslationFor("APP_RESIZE_TO_GRID"),NULL);
	_gridStep = new BTextControl(BRect(20,70,110,75),"step-grid",_language->GetTranslationFor("APP_STEP_GRID"),"",NULL);
	_gridStep->SetDivider(60);
	
	// fixer les parametres
	_useGrid->SetValue(g_MeTOSPrefs->GetInt32(B_CONTROL_OFF,"grid-used"));
	_snapToGrid->SetValue(g_MeTOSPrefs->GetInt32(B_CONTROL_OFF,"grid-snap"));
	_resizeToGrid->SetValue(g_MeTOSPrefs->GetInt32(B_CONTROL_OFF,"grid-resize"));
	buffer << g_MeTOSPrefs->GetInt32(10,"grid-step");
	_gridStep->SetText(buffer.String());
	_gridStep->SetEnabled((_useGrid->Value() == B_CONTROL_ON));

	// on desactive cetrain controles
	_gridStep->SetEnabled((_useGrid->Value() == B_CONTROL_ON));
	_snapToGrid->SetEnabled((_useGrid->Value() == B_CONTROL_ON));
	_resizeToGrid->SetEnabled((_useGrid->Value() == B_CONTROL_ON));
	
	// ajouter les controles
	box->AddChild(_useGrid);
	box->AddChild(_snapToGrid);
	box->AddChild(_resizeToGrid);
	box->AddChild(_gridStep);
	AddChild(box);

	// le bloc tools
	bounds.top += 103;
	bounds.bottom = bounds.top + 68;
	box = new BBox(bounds,"tools");
	box->SetLabel(_language->GetTranslationFor("APP_TOOLS"));

	// creer les controles
	_useToolsPopup = new BCheckBox(BRect(20,20,180,34),"use-popup-tools",_language->GetTranslationFor("APP_TOOLS_POPUP"),NULL);
	_nbToolsPerLine = new BTextControl(BRect(20,38,160,53),"tools-per-line",_language->GetTranslationFor("APP_TOOLS_BUTTONS"),"",NULL);
	_nbToolsPerLine->SetDivider(110);

	// fixer les parametres
	_useToolsPopup->SetValue(g_MeTOSPrefs->GetInt32(B_CONTROL_OFF,"use-popup-tools"));
	buffer = "";
	buffer << g_MeTOSPrefs->GetInt32(2,"tools-per-line");
	_nbToolsPerLine->SetText(buffer.String());
	
	// ajouter les controles
	box->AddChild(_useToolsPopup);
	box->AddChild(_nbToolsPerLine);
	AddChild(box);


	// le bloc Undo
	bounds.top += 73;
	bounds.bottom = bounds.top + 48;
	box = new BBox(bounds,"undos");
	box->SetLabel(_language->GetTranslationFor("APP_UNDOS"));

	// nombres de undos possible
	_nbUndos = new BTextControl(BRect(20,20,160,35),"max-undo",_language->GetTranslationFor("APP_UNDOS_MAX"),"",NULL);
	_nbUndos->SetDivider(110);

	// fixer les parametres
	buffer = "";
	buffer << g_MeTOSPrefs->GetInt32(40,"max-undo");
	_nbUndos->SetText(buffer.String());

	// ajouter les controles
	box->AddChild(_nbUndos);
	AddChild(box);

	// le bloc recents
	bounds.top += 53;
	bounds.bottom = bounds.top + 48;
	box = new BBox(bounds,"recents");
	box->SetLabel(_language->GetTranslationFor("APP_RECENT"));

	// nombres fichiers dans les recents ouvert
	_nbRecent = new BTextControl(BRect(20,20,170,35),"max-recent",_language->GetTranslationFor("APP_RECENT_MAX"),"",NULL);
	_nbRecent->SetDivider(120);

	// fixer les parametres
	buffer = "";
	buffer << (int32)(g_MeTOSPrefs->GetInt8(5,"max-nb-recent"));
	_nbRecent->SetText(buffer.String());

	// ajouter les controles
	box->AddChild(_nbRecent);
	AddChild(box);
}

/**** attachement a la fenetre ****/
void ApplicationPrefs::AttachedToWindow()
{
	// attachement standard
	VPreference::AttachedToWindow();
	
	// fixer la cible des messages de nos controles
	_useGrid->SetTarget(this);
	_snapToGrid->SetTarget(this);
	_resizeToGrid->SetTarget(this);
	_gridStep->SetTarget(this);
	_useToolsPopup->SetTarget(this);
	_nbToolsPerLine->SetTarget(this);
}

/**** gestion des messages ****/
void ApplicationPrefs::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
	// gerer la checkbox "use grid"
	case GRIDUSED_CHECK_MSG:
		_gridStep->SetEnabled((_useGrid->Value() == B_CONTROL_ON));
		_snapToGrid->SetEnabled((_useGrid->Value() == B_CONTROL_ON));
		_resizeToGrid->SetEnabled((_useGrid->Value() == B_CONTROL_ON));
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
void ApplicationPrefs::SavePrefs()
{
	int32				gridStep;
	int32				nbToolsPerLine;
	int32				nbUndos;
	int32				nbRecents;

	// relire avant
	g_MeTOSPrefs->Load();

	// sauver les parametres
	// gui-manager
	g_MeTOSPrefs->SetInt32(_useGrid->Value(),"grid-used");
	g_MeTOSPrefs->SetInt32(_snapToGrid->Value(),"grid-snap");
	g_MeTOSPrefs->SetInt32(_resizeToGrid->Value(),"grid-resize");
	sscanf(_gridStep->Text(),"%ld",&gridStep);
	if(_useGrid->Value()==B_CONTROL_ON)
	{
		if(gridStep>15)
		{
			gridStep = 15;
			_gridStep->SetText("15");
		}
		if(gridStep<5)
		{
			gridStep = 5;
			_gridStep->SetText("5");
		}
	}
	else	
	{
		gridStep = 1;
		_gridStep->SetText("1");
	}
	g_MeTOSPrefs->SetInt32(gridStep,"grid-step");
	
	// popup-tools
	g_MeTOSPrefs->SetInt32(_useToolsPopup->Value(),"use-popup-tools");
	sscanf(_nbToolsPerLine->Text(),"%ld",&nbToolsPerLine);
	if(nbToolsPerLine>4 || nbToolsPerLine<1)
	{
		nbToolsPerLine = 2;		
		_nbToolsPerLine->SetText("2");
	}	
	g_MeTOSPrefs->SetInt32(nbToolsPerLine,"tools-per-line");
	
	// undos
	sscanf(_nbUndos->Text(),"%ld",&nbUndos);
	if(nbUndos>100 || nbUndos<15)
	{
		nbToolsPerLine = 40;		
		_nbUndos->SetText("40");
	}	
	g_MeTOSPrefs->SetInt32(nbUndos,"max-undo");
	
	// fichiers recents
	sscanf(_nbRecent->Text(),"%ld",&nbRecents);
	if(nbRecents>20 || nbRecents<5)
	{
		nbRecents = 5;		
		_nbRecent->SetText("5");
	}	
	g_MeTOSPrefs->SetInt8(nbRecents,"max-nb-recent");
	
	// sauver
	g_MeTOSPrefs->Save();
}

/**************************/
/* MeTOS preference class */
/* pour l'application     */
/**************************/
#include "CPPGeneratorPrefs.h"
#include "MetosMessageDefs.h"
#include "MetosGlobalDefs.h"
#include "VPreferenceFile.h"
#include "VLanguage.h"

#include <TextControl.h>
#include <CheckBox.h>
#include <Box.h>
#include <Button.h>
#include <File.h>
#include <Path.h>
#include <FindDirectory.h>
#include <FilePanel.h>
#include <Window.h>
#include <String.h>

/**** Constructeur ****/
CPPGeneratorPrefs::CPPGeneratorPrefs(BRect frame)
: VPreference(frame,"C++",CODE_GENERATION_GROUP)
{
	BRect				bounds = Bounds();
	BBox				*box = NULL;
	BString				buffer;
	BPath				sysPath;

	// preferences
	_prefFile = new VPreferenceFile("CPPGenerator",CODE_GENERATION_GROUP_PATH);
	_prefFile->Load();

	// decallage a cause du titre
	bounds.top += VPREFERENCE_TITLE_HIGH;
	bounds.InsetBy(7,7);

	// le bloc generator
	bounds.bottom = bounds.top + 140;
	box = new BBox(bounds,"generator-settings");
	box->SetLabel(_language->GetTranslationFor("CPP_SETTINGS"));

	// la chaine en entete des variables membres
	_prependMemberString = new BTextControl(BRect(20,25,172,40),"member-prepend-string",_language->GetTranslationFor("CPP_MEMBERS"),"",NULL);
	_prependMemberString->SetDivider(120);

	// style d'ecriture	
	_indentStyle = new BCheckBox(BRect(20,45,180,59),"indent-style","",new BMessage(IDENTSTYLE_CHECK_MSG));
	_indentStyle->SetValue(_prefFile->GetInt32(B_CONTROL_ON,"indent-style"));
	SetIndentLabel();
	
	// ecriture du makefile
	_writeMakeFile = new BCheckBox(BRect(20,65,230,79),"write-makefile",_language->GetTranslationFor("CPP_MAKEFILE"),NULL);
	_writeMakeFile->SetValue(_prefFile->GetInt32(B_CONTROL_OFF,"write-makefile"));
	
	// utiliser le = dans le makefile
	_useEqualOnly = new BCheckBox(BRect(20,85,240,99),"use-equal-in-makefile",_language->GetTranslationFor("CPP_EQUAL"),NULL);
	_useEqualOnly->SetValue(_prefFile->GetInt32(B_CONTROL_ON,"use-equal-in-makefile"));

	// fixer les parametres
	buffer = _prefFile->GetString("_","prepend-string-member");
	_prependMemberString->SetText(buffer.String());

	// fichier CS
	_systemCSLibPath = new BTextControl(BRect(20,105,305,120),"CS-path",_language->GetTranslationFor("CPP_CS_FILE"),"",NULL);
	_systemCSLibPath->SetDivider(95);

	// si le repertoire n'existe pas on va le creer
	find_directory(B_USER_SETTINGS_DIRECTORY,&sysPath);
	buffer = sysPath.Path();
	buffer << "/" << METOS_SETTINGS_FOLDER;
	buffer = _prefFile->GetString(buffer,"CS-system-path");
	_systemCSLibPath->SetText(buffer.String());

	_selector = new BButton(BRect(306,102,324,117),"selector","...",new BMessage(OPEN_SELECTOR));
	_pathSelector = new BFilePanel(B_OPEN_PANEL,NULL,NULL,B_DIRECTORY_NODE,false,new BMessage(PATH_SELECTION),NULL,true);
	_pathSelector->Window()->SetType(B_FLOATING_WINDOW);
	_pathSelector->SetPanelDirectory(buffer.String());

	box->AddChild(_prependMemberString);
	box->AddChild(_indentStyle);
	box->AddChild(_writeMakeFile);
	box->AddChild(_useEqualOnly);
	box->AddChild(_systemCSLibPath);
	box->AddChild(_selector);
	AddChild(box);
}

/**** destructeur ****/
CPPGeneratorPrefs::~CPPGeneratorPrefs()
{
	// detruire le seecteur de fichier
	delete _pathSelector;
}

/**** attachement a la fenetre ****/
void CPPGeneratorPrefs::AttachedToWindow()
{
	// attachement standard
	VPreference::AttachedToWindow();

	// les controles
	_indentStyle->SetTarget(this);
	_selector->SetTarget(this);
	_pathSelector->SetTarget(BMessenger(this));
}

/**** gestion des messages ****/
void CPPGeneratorPrefs::MessageReceived(BMessage *message)
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
			_systemCSLibPath->SetText(path.Path());
		}		
		break;
	// texte de l'indentation
	case IDENTSTYLE_CHECK_MSG:
		SetIndentLabel();
		break;
	// sauver les prefs
	case M_SAVE_PREFERENCE_MSG:
		SavePrefs();
		break;
	default:
		VPreference::MessageReceived(message);
	}
}

/**** fixer le label du style d'indentation ****/
void CPPGeneratorPrefs::SetIndentLabel()
{
	if(_indentStyle->Value()==B_CONTROL_OFF)
		_indentStyle->SetLabel(_language->GetTranslationFor("CPP_INDENT_1"));
	else
		_indentStyle->SetLabel(_language->GetTranslationFor("CPP_INDENT_2"));
}

/**** sauver les preferences ****/
void CPPGeneratorPrefs::SavePrefs()
{
	BString				buffer;
	
	// relire avant
	_prefFile->Load();

	// renseigner le caractere de "prepend"
	buffer = _prependMemberString->Text();
	if(buffer.Length()==0)
	{
		buffer = "_";
		_prependMemberString->SetText("_");
	}		
	_prefFile->SetString(buffer,"prepend-string-member");
	_prefFile->SetInt32(_indentStyle->Value(),"indent-style");
	_prefFile->SetInt32(_writeMakeFile->Value(),"write-makefile");
	_prefFile->SetInt32(_useEqualOnly->Value(),"use-equal-in-makefile");
	_prefFile->SetString(_systemCSLibPath->Text(),"CS-system-path");
		
	// sauver
	_prefFile->Save();
}
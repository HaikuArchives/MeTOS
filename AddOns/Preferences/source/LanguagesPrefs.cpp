// ==================================
// = Preferences de la localisation =
// ==================================
#include "LanguagesPrefs.h"
#include "MetosMessageDefs.h"
#include "MetosGlobalDefs.h"
#include "VPreferenceFile.h"
#include "VLanguage.h"

#include <app/Application.h>
#include <app/Roster.h>
#include <interface/PopUpMenu.h>
#include <interface/MenuItem.h>
#include <interface/MenuField.h>
#include <interface/StringView.h>
#include <support/String.h>
#include <interface/Box.h>
#include <storage/Path.h>
#include <storage/Entry.h>
#include <storage/Directory.h>

// messages
const uint32		LANGUAGE_CHANGE_MSG		= 'Lchg';

/**** constructeur ****/
LanguagesPrefs::LanguagesPrefs(BRect frame)
: VPreference(frame,"Languages",METOS_GROUP)
{
	BRect				bounds = Bounds();
	BString				buffer;
	BBox				*box = NULL;
	BStringView			*stingView = NULL;

	// decallage a cause du titre
	bounds.top += VPREFERENCE_TITLE_HIGH;
	bounds.InsetBy(7,7);

	// le bloc principale
	bounds.bottom = bounds.top + 70;
	box = new BBox(bounds,"languages");
	box->SetLabel(_language->GetTranslationFor("PREFERENCE_NAME"));

	// le texte d'info
	stingView = new BStringView(BRect(20,20,320,34),"info-text",_language->GetTranslationFor("LANGUAGES_ALERT"));

	// recuperer le language courant
	_currentLanguage = g_MeTOSPrefs->GetString("English",METOS_LANGUAGES_TYPE);

	// construire le menu
	_build_menu();

	// ajouter le menu de choix
	box->AddChild(stingView);
	box->AddChild(_languages);
	AddChild(box);
}

/**** attachement a la fenetre ****/
void LanguagesPrefs::AttachedToWindow()
{
	// fonction parente
	VPreference::AttachedToWindow();

	// donner la destination du BMenuField
	_languages->Menu()->SetTargetForItems(this);
}

/**** gestion des messages ****/
void LanguagesPrefs::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
	case LANGUAGE_CHANGE_MSG:
		ChangeLanguage(message);
		break;
	default:
		VPreference::MessageReceived(message);
	}
}

/**** changer la langue ****/
void LanguagesPrefs::ChangeLanguage(BMessage *message)
{
	int32	index = -1;
	
	// trouver l'index du menu
	if(message->FindInt32("index",&index)!=B_OK)
		return;
printf("ok message\n");
	// ok on prend l'index du menu selectionné
	_currentLanguage = _languages->Menu()->ItemAt(index)->Label();
printf("%s\n",_currentLanguage.String());
}

/**** sauver les preferences ****/
void LanguagesPrefs::SavePrefs()
{
	// relire avant
	g_MeTOSPrefs->Load();
	
	// sauver les parametres
	g_MeTOSPrefs->SetString(_currentLanguage,METOS_LANGUAGES_TYPE);

	// sauver
	g_MeTOSPrefs->Save();
}

/**** construire le menu avec les languages dispo ****/
void LanguagesPrefs::_build_menu()
{
	// on va regarder les languages dispo pour les fichiers de l'application et construire alors
	// le choix avec ces derniers
	// lors de la creation des addons, l'objet application existe deja, donc on va pouvoir acceder aux
	// elements de chemins et de language
	BPopUpMenu			*menu = NULL;
	BMenuItem			*menuItem = NULL;
	BString				addonsPath;
	BPath				appPath;
	app_info			info;

	// creer le BMenuField
	menu = new BPopUpMenu("langages-menu");
	_languages = new BMenuField(BRect(20,40,210,54),"language-choice",_language->GetTranslationFor("LANGUAGES_CHOOSE"),menu);

	// initialiser le fichier ressource
	// et le chemin de l'appli
	be_app->GetAppInfo(&info);
	appPath.SetTo(&(info.ref)); 
    appPath.GetParent(&appPath);

	// ajouter le chemin des addons
	addonsPath = ADDON_FOLDER;
	addonsPath << "/";
	addonsPath << METOS_LANGUAGES_FOLDER;
	addonsPath << "/";
	appPath.Append(addonsPath.String());

	// verifier la validité
	if(appPath.InitCheck()!=B_OK)
		return;
		
	BDirectory	dir(appPath.Path());
   	status_t 	err = B_NO_ERROR;
	BEntry		entry;
	BPath		path;
	BString		language;

	// ok rechercher tous les languages
   	// trouver le chemin des addons
	while( err == B_NO_ERROR )
	{
		// y a t-il un fichier suivant
		err = dir.GetNextEntry((BEntry*)&entry, true);			
		if( entry.InitCheck() != B_NO_ERROR )
			break;
		
		// si oui recuperer le chemin
		if( entry.GetPath(&path) == B_NO_ERROR )
		{
			// on va retenir que les fichier avec en debut de nom "MeTOS."
			// c'est la traduction de l'application
			language = path.Leaf();
			if(language.FindFirst("MeTOS.")==0)
			{
				// enlever "MeTOS."
				language.Remove(0,6);
				if(language.Length()>0)
				{
					// puis creer un item de menu avec le language
					menuItem = new BMenuItem(language.String(),new BMessage(LANGUAGE_CHANGE_MSG));
					menu->AddItem(menuItem);
					
					// on va aussi verifier si c'est le language actuel
					if(language == _currentLanguage)
						menuItem->SetMarked(true);
				}
			}
		}
	}
}

/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
/***************************/
/* fenetre des preferences */
/***************************/
#include "PreferenceManagerWin.h"
#include "PreferenceManagerListView.h"
#include "PreferenceManagerItem.h"
#include "MetosSplashScreen.h"
#include "MetosMessageDefs.h"
#include "MetosGlobalDefs.h"
#include "VPreferenceFile.h"
#include "VPreference.h"
#include "MetosApp.h"
#include <interface/View.h>
#include <ScrollView.h>
#include <app/Messenger.h>
#include <app/Roster.h>
#include <storage/Path.h>
#include <storage/Directory.h>
#include <interface/Button.h>

/**** constructeur ****/
PreferenceManagerWin::PreferenceManagerWin(BRect frame)
: BWindow(frame,"Preferences...", B_TITLED_WINDOW_LOOK, B_FLOATING_SUBSET_WINDOW_FEEL , B_NOT_RESIZABLE | B_NOT_ZOOMABLE )
{
	BScrollView		*supportScrollView = NULL;
	BRect			listFrame = Bounds();

	// ajouter la vue de support
	_supportView = new BView(listFrame,"support-view",B_FOLLOW_NONE, B_WILL_DRAW);
	_supportView->SetViewColor(VIEW_COLOR);

	// ajouter le bouton de sauvegarde
	_saveButton = new BButton(BRect(listFrame.right-67,listFrame.bottom-25,listFrame.right-3,listFrame.bottom-2),"save-button","Save",new BMessage(M_SAVE_PREFERENCE_MSG));
	_supportView->AddChild(_saveButton);

	// determiner l'emplacement
	listFrame.InsetBy(5,5);
	listFrame.right = listFrame.left + 100;
	listFrame.bottom -= 24;
	// ajouter la liste des groupes d'options
	_groupListView = new PreferenceManagerListView(listFrame,"group-list");
	supportScrollView = new BScrollView("scroll-navigation",_groupListView,B_FOLLOW_ALL_SIDES,B_WILL_DRAW | B_FRAME_EVENTS,false,true);
	_supportView->AddChild(supportScrollView);

	AddChild(_supportView);
}

/**** destructeur ****/
PreferenceManagerWin::~PreferenceManagerWin()
{
	// sauver les preferences
	SavePreferences();
	
	// decharger les addons
	UnloadAddOns();	
}

/**** gestion des messages ****/
void PreferenceManagerWin::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
	// appeler la sauvegarde des preferences courantes
	case M_SAVE_PREFERENCE_MSG:
		SaveCurrentPreference();
		break;
	default:
		BWindow::MessageReceived(message);
	}
}

/**** fermer la fenetre ****/
bool PreferenceManagerWin::QuitRequested()
{
	// quitter
	if(g_DoQuit)
		return (true);
	
	// doit-on quitter ou simplement cacher la fenetre
	// si non on reduit la fenetre si ca n'est pas deja fait
	if(!IsHidden())
		Hide();
	
	return	(false);
}

// ======================
// = Gestion des Addons =
// ======================

/**** charger les Addons ****/
void PreferenceManagerWin::LoadAddOns()
{
	PreferenceManagerItem	*addOnItem = NULL;
	BList					list;
	void					(*instantiate_VPreferences)(BList *,BRect);
	image_id				addonId;
   	status_t 				err = B_NO_ERROR;
	app_info 				info; 
    BPath 					path;
	BString					message;
	int32					nbFiles;
	BMessage				splashInfo(M_PROGRESSSTART_MSG);
	BRect					frame;
	BEntry					entry;
	
	// initialiser le compteur du splash-screen
	nbFiles = g_MetosApp->CountAddOnFile(ADDON_PREFERENCES_FOLDER);
	splashInfo.AddInt32(M_TOTALPROGRESS,nbFiles);
	splashInfo.AddString(M_TEXTDISPLAY,"Load Preferences AddOns...");
	BMessenger(g_MetosApp->_splashScreen).SendMessage(&splashInfo);
	
	// determiner l'emplacement des vues
	frame = Bounds();
	frame.InsetBy(3,3);
	frame.left += 122;	
	frame.bottom -= 24;
	
   	// trouver le chemin des addons
    path = g_MetosApp->GetAddonsPath();
	path.Append(ADDON_PREFERENCES_FOLDER);
	BDirectory dir(path.Path());

   	// charger les addons
	while( err == B_NO_ERROR )
	{
		err = dir.GetNextEntry((BEntry*)&entry,false);			
		if( entry.InitCheck() != B_NO_ERROR )
			break;
		
		if( entry.GetPath(&path) == B_NO_ERROR )
		{
			addonId = load_add_on(path.Path());
			if(addonId >=0)
			{
				// on regarde si on arrive a trouver la metode de creation de l'objet
				if(get_image_symbol(addonId,"instantiate_VPreferences", B_SYMBOL_TYPE_TEXT,(void **)&instantiate_VPreferences) == B_OK)
				{
					// on creer un VObject sans creer l'objet graphique qu'il contient
					// ca nous permet d'acceder a ces proprietes et un tas d'autre chose
					// lors de la creation d'un objet graphique on va creer les deux
					(*instantiate_VPreferences)(&list,frame);
					if(list.CountItems()>0)
						_groupListView->AddPreferences(&list);

					// vider la liste
					list.MakeEmpty();
					
					// creer puis stoquer l'item de la liste (liste des AddOn)
					addOnItem = new PreferenceManagerItem(addonId,&entry);
					_addOnList.AddItem(addOnItem);

					// informer le splash-screen
					BMessenger(g_MetosApp->_splashScreen).SendMessage(M_PROGRESSINCREASE_MSG);

					// informer que tout c'est bien passé
					message = "Preference : ";	
					message << path.Path() << " Loaded";
					g_MetosApp->SendDebbug(message.String(),VDEBUG_DEBUG);
				}
				else
					unload_add_on(addonId);
			}
		}
	}
	// informer que tout c'est bien passé
	g_MetosApp->SendDebbug("Preference Addons Loading finish",VDEBUG_INFO);
	
	// selectionner le premier item valide
	_groupListView->SelectFirstItem();
}

/**** decharger les addons ****/
void PreferenceManagerWin::UnloadAddOns()
{
	// liberer la memoire prise par les items
	PreferenceManagerItem	*AddOn = NULL;
	int32					nbItem;
	image_id				image;

	// tout d'abord vider la liste des prefs
	_groupListView->ClearList();

	// liberer la liste et decharger les addons
	nbItem = _addOnList.CountItems() - 1;
	for(int32 index = nbItem;index>=0;index--)
	{
		AddOn = (PreferenceManagerItem *)(_addOnList.RemoveItem(index));
		image = AddOn->AddonId();				
		delete AddOn;
		if(unload_add_on(image)!=B_OK)
			printf("Error unloading Preference Addon\n");
	}
}

// ===============
// = Preferences =
// ===============

/**** sauvegarde des prefs courantes ****/
void PreferenceManagerWin::SaveCurrentPreference()
{
	BView	*prefView = NULL;
	
	// recuperer la vue des preferences attaché
	if(Lock())
	{
		prefView = _supportView->ChildAt(2);
		Unlock();
	}
	else
		g_MetosApp->SendDebbug("Error : Lock() in PreferenceManagerWin::SaveCurrentPreference()",VDEBUG_ERROR);
	
	// on doit avoir une vue
	if(prefView==NULL)
		return;

	// on demande a la vue de sauver
	((VPreference *)prefView)->SavePrefs(); 
	
	// prevenir l'application du changement
	BMessenger(be_app).SendMessage(M_PREFERENCE_MSG);
}

/**** sauver les preferences ****/
void PreferenceManagerWin::SavePreferences()
{
	// charger les preferences
	g_MeTOSPrefs->Load();
	g_MeTOSPrefs->SetRect(Frame(),"preference-rect");
	g_MeTOSPrefs->Save();
}

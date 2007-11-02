/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/

/********************************************/
/* Fenetre principale du programme (log...) */
/********************************************/
#include "MetosApp.h"
#include "MetosWin.h"
#include "GUIManager.h"
#include "MetosMessageDefs.h"
#include "MetosGlobalDefs.h"
#include "MetosUndoDefs.h"
#include "MetosLanguagesDefs.h"
#include "VPreferenceFile.h"
#include "ObjectManagerWin.h"
#include "PropertieManagerWin.h"
#include "AddOnManagerWin.h"
#include "ToolManagerWin.h"
#include "UndoManagerWin.h"
#include "PreferenceManagerWin.h"
#include "VCodeGenerator.h"
#include "VDebugItem.h"
#include "VLanguage.h"

#include <interface/MenuBar.h>
#include <interface/Menu.h>
#include <interface/MenuItem.h>
#include <support/String.h>
#include <interface/ListView.h>
#include <interface/ScrollView.h>
#include <interface/Bitmap.h>

// messenger global
BWindow					*g_MetosBackWin = NULL;

/*** constructeur de la fenetre MeTOS Application ****/
MetosWin::MetosWin(BRect frame)
: BWindow(frame,"MeTOS", B_TITLED_WINDOW, B_NOT_ZOOMABLE )
{	
	BView			*metosView;
	BScrollView		*supportScrollView;
	BRect			listRect = Bounds();
	
	// charger les images
	if(g_MetosApp->Lock())
	{
		_infoBitmap[0] = g_MetosApp->GetBitmap("InfoInfo");
		_infoBitmap[1] = g_MetosApp->GetBitmap("WarningInfo");
		_infoBitmap[2] = g_MetosApp->GetBitmap("ErrorInfo");
		_infoBitmap[3] = g_MetosApp->GetBitmap("DebugInfo");
		g_MetosApp->Unlock();
	}
	else
		g_MetosApp->SendDebbug("Error : Lock() in MetosWin::MetosWin()",VDEBUG_ERROR);

	
	// construire le menu
	BuildMenu(listRect);

	// construire la vue principale
	metosView = new BView(listRect,"application-view",B_FOLLOW_ALL_SIDES, B_WILL_DRAW);
	metosView->SetViewColor(VIEW_COLOR);

	listRect.InsetBy(2,2);
	listRect.top += 34;
	listRect.right -= 15;
	_infoList = new BListView(listRect,"navigation-list",B_SINGLE_SELECTION_LIST,B_FOLLOW_ALL_SIDES,B_WILL_DRAW | B_FRAME_EVENTS);
	supportScrollView = new BScrollView("scroll-navigation",_infoList,B_FOLLOW_ALL_SIDES,B_WILL_DRAW | B_FRAME_EVENTS,false,true);

	metosView->AddChild(supportScrollView);
	AddChild(metosView);
	
	// charger les prefs
	RefreshPreferences();	
}

/*** destructeur de la fenetre MeTOS Application ****/
MetosWin::~MetosWin()
{
	int32	nbItems;

	// sauver les preferences
	SavePreferences();

	// liberer la liste des proprietes
	nbItems = _infoList->CountItems();
	for(int32 item = nbItems-1;item>=0;item--)
		delete (VDebugItem *)(_infoList->RemoveItem(item));	

	// liberer la memoire
	delete _infoBitmap[0];
	delete _infoBitmap[1];
	delete _infoBitmap[2];
	delete _infoBitmap[3];
}

/**** traiter les messages ****/
void MetosWin::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
	// renvoyer le mouvement de la roulette de la souris a la srcoll-view
	case B_MOUSE_WHEEL_CHANGED:
		BMessenger(_infoList).SendMessage(message);
		break;
	// drag&drop renvoyer a l'application
	case B_SIMPLE_DATA:
		be_app_messenger.SendMessage(message);
		break;
	// ouvrir un fichier recent
	case M_OPEN_RECENT_MSG:
		OpenRecent(message);
		break;	
	// ajouter un fichier a la liste des recents
	case M_ADD_RECENT_MSG:
		AddProjectToRecent(message);
		break;
	// afficher des infos sur l'execution etc...
	case METOS_DEBUG_MSG:
		AddDebugMessage(message);
		break;
	// preferences
	case M_PREFERENCE_MSG:
		RefreshPreferences();
		_infoList->Invalidate();
		break;
	default:
		BWindow::MessageReceived(message);	
	}
}

/**** Si on quitte on ferme les fenetre du projet ****/
bool MetosWin::QuitRequested()
{
	// annuler la selection du GUIManager
	g_GUIManager.SendMessage(M_SELECT_OBJECT_MSG);

	// quitter l'application
	be_app_messenger.SendMessage(B_QUIT_REQUESTED);
	
	return (true);
}

/**** ajouter un message d'info ****/
void MetosWin::AddDebugMessage(BMessage *message)
{
	status_t	state = B_OK;
	int32		nbMessages;
	const char	*info = NULL;
	int32		type = 0;

	// trouver les infos
	state &= message->FindString(VDEBUG_STRING,&info);
	state &= message->FindInt32(VDEBUG_TYPE,&type);
	if(state!=B_OK)
		return;
		
	// effacer l'ancien message
	nbMessages = _infoList->CountItems() - 1;
	if(nbMessages>=_nbMaxDebug)
		for(int32 index=nbMessages;index>=_nbMaxDebug;index--)
			delete (VDebugItem *)(_infoList->RemoveItem(index));

	// ajouter le nouveau message
	_infoList->AddItem(new VDebugItem(info,type),0);
}

/*** Construire le menu de l'application ***/
void MetosWin::BuildMenu(BRect frame)
{
	// Set the menu bar
	BMenu			*menu = NULL;
	
	// creation de la bar de menu
	_menuBar = new BMenuBar(frame,"Main menu");

	// menu File
	menu = new BMenu(g_Languages->GetTranslationFor(L_MENU_FILE));
	menu->AddItem(new BMenuItem(g_Languages->GetTranslationFor(L_MENU_FILE_NEW_PROJECT),new BMessage(M_NEW_PROJECT_MSG),M_NEW_PROJECT_SHORTCUT));
	menu->AddItem(new BMenuItem(g_Languages->GetTranslationFor(L_MENU_FILE_OPEN_PROJECT),new BMessage(M_LOAD_PROJECT_MSG),M_OPEN_PROJECT_SHORTCUT));
	
	// ajouter ici les fichiers recement ouvert
	BuildRecentMenu(menu);
	
	menu->AddItem(new BMenuItem(g_Languages->GetTranslationFor(L_MENU_FILE_SAVE_PROJECT),new BMessage(M_SAVE_PROJECT_MSG),M_SAVE_PROJECT_SHORTCUT));
	menu->AddItem(new BMenuItem(g_Languages->GetTranslationFor(L_MENU_FILE_SAVE_AS),new BMessage(M_SAVE_PROJECT_AS_MSG)));
	menu->AddSeparatorItem();
	menu->AddItem(new BMenuItem(g_Languages->GetTranslationFor(L_MENU_FILE_QUIT),new BMessage(B_QUIT_REQUESTED)));
	_menuBar->AddItem(menu);
	// definir la destination des messages du menu
	menu->SetTargetForItems(be_app);

	// menu Edition
	menu = new BMenu(g_Languages->GetTranslationFor(L_MENU_EDIT));
	menu->AddItem(new BMenuItem(g_Languages->GetTranslationFor(L_MENU_EDIT_UNDO),new BMessage(UNDO_RESTORE_MSG),M_UNDO_SHORTCUT));
	menu->AddSeparatorItem();
	menu->AddItem(new BMenuItem(g_Languages->GetTranslationFor(L_MENU_EDIT_PREFERENCES),new BMessage(SHOW_PREFERENCES_WIN_MSG)));
	_menuBar->AddItem(menu);
	// definir la destination des messages du menu
	menu->SetTargetForItems(be_app);

	// Menu View
	menu = new BMenu(g_Languages->GetTranslationFor(L_MENU_VIEW));
	menu->AddItem(new BMenuItem(g_Languages->GetTranslationFor(L_MENU_VIEW_OBJECTS),new BMessage(SHOW_PROJECT_WIN_MSG)));
	menu->AddItem(new BMenuItem(g_Languages->GetTranslationFor(L_MENU_VIEW_PROPERTIES),new BMessage(SHOW_PROPERTIES_WIN_MSG)));
	menu->AddItem(new BMenuItem(g_Languages->GetTranslationFor(L_MENU_VIEW_ADDONS),new BMessage(SHOW_OBJECT_WIN_MSG)));
	_menuBar->AddItem(menu);
	// definir la destination des messages du menu
	menu->SetTargetForItems(be_app);

	// menu MeTOS
	menu = new BMenu("MeTOS");
	menu->AddItem(new BMenuItem(g_Languages->GetTranslationFor(L_MENU_METOS_TIPS),new BMessage(SHOW_TIPS_WIN_MSG)));
	menu->AddItem(new BMenuItem(g_Languages->GetTranslationFor(L_MENU_METOS_UPDATE),new BMessage(M_CHECK_UPDATE_MSG)));
	menu->AddSeparatorItem();
	menu->AddItem(new BMenuItem(g_Languages->GetTranslationFor(L_MENU_METOS_HELP),new BMessage(M_METOS_HELP_MSG)));
	menu->AddItem(new BMenuItem(g_Languages->GetTranslationFor(L_MENU_METOS_DONATE),new BMessage(M_METOS_DONATE_MSG)));
	menu->AddSeparatorItem();
	menu->AddItem(new BMenuItem(g_Languages->GetTranslationFor(L_MENU_METOS_ABOUT),new BMessage(SHOW_ABOUT_WIN_MSG)));
	_menuBar->AddItem(menu);
	// definir la destination des messages du menu
	menu->SetTargetForItems(be_app);

	AddChild(_menuBar);
}

/**** construire le menu des elements recents ****/
void MetosWin::BuildRecentMenu(BMenu *destination)
{
	// on va sauver la taille des colonnes de la liste
	BString				filePath("-");
	BEntry				entry;
	int8				nbPath;

	// recuperer les chemins des derniers fichiers ouvert
	_recentLoad = new BMenu(g_Languages->GetTranslationFor(L_MENU_FILE_OPEN_RECENT_PROJECT));
	_maxRecent = g_MeTOSPrefs->GetInt8(5,"max-nb-recent");
	nbPath = g_MeTOSPrefs->GetInt8(_maxRecent,"nb-recent");
	for(int8 index=0;index<nbPath;index++)
	{
		filePath = g_MeTOSPrefs->GetString(BString("-"),"path-recent",index);
		if(filePath!="-")
		{
			// ajouter l'item au menu
			entry.SetTo(filePath.String());
			if(entry.Exists())
				_recentLoad->AddItem(new BMenuItem(filePath.String(),new BMessage(M_OPEN_RECENT_MSG)),0);
			else
				// si le liens n'existe pas on doit retirer au nombre de liens
				_maxRecent--;
		}
	}
	// ajouter au menu principale
	// et fixer la destination des messages
	destination->AddItem(_recentLoad);
	_recentLoad->SetTargetForItems(this);
}

/**** ajouter un fichier a la liste des fichiers recents ****/
void MetosWin::AddProjectToRecent(BMessage *message)
{
	// on doit avoir un message
	if(message==NULL)
		return;
	
	BMenuItem	*recentPath = NULL;
	BString		filePath;
	BString		recentFilePath;
	BEntry		entry;
	int32		nbRecent;

	// retrouver la chaine de caractere (chemin du fichier)
	if(message->FindString(M_RECENT_PATH,&filePath)!=B_OK)
		return;
	
	// verifier qu'il n'est pas deja dans la liste
	// sinon il faut le deplacer en tete
	nbRecent = _recentLoad->CountItems();
	for(int32 index=0;index<nbRecent;index++)
	{
		recentPath = _recentLoad->ItemAt(index);
		recentFilePath = recentPath->Label();
		if(recentFilePath==filePath)
		{
			// le replacer en tete
			if(_recentLoad->RemoveItem(recentPath))
				_recentLoad->AddItem(recentPath,0);
				
			return;
		}
	}

	// verifier la validité du fichier
	entry.SetTo(filePath.String());
	if(entry.InitCheck()!=B_OK)
		return;
	
	// verrifier que l'on ne depasse pas le nombre maximum
	// de fichiers recents
	if(_recentLoad->CountItems()==_maxRecent)
		delete _recentLoad->RemoveItem((_maxRecent - 1));
		
	// ok tout c'est bien passé on peux
	// ajouter le fichier a la liste	
	_recentLoad->AddItem(new BMenuItem(filePath.String(),new BMessage(M_OPEN_RECENT_MSG)),0);
}

/**** ouvrir un fichier recent ****/
void MetosWin::OpenRecent(BMessage *message)
{
	// on veux un message valide
	if(message==NULL)
		return;
	
	int32	selected;

	// on doit trouver l'index du menu selectionné
	if(message->FindInt32("index",&selected)!=B_OK)
		return;

	// on a trouver maintenant il faut construire
	// le message qui ouvre un fichier
	BMenuItem	*item = NULL;
	BMessage	open(B_LOAD_REQUESTED);
	BEntry		entry;
	entry_ref	ref;
	
	// recuperer l'item de menu
	item = _recentLoad->ItemAt(selected);
	if(item==NULL)
		return;
		
	// en meme temps on va tester la validité du lien
	entry.SetTo(item->Label());
	if(entry.InitCheck()!=B_OK)
	{
		// si le fichier n'est pas valide
		// on va l'enlever du menu
		delete _recentLoad->RemoveItem(selected);
		
		// on sort
		return;
	}
	
	// ok tout c'est bien passé
	entry.GetRef(&ref);
	open.AddRef("refs",&ref);
	be_app_messenger.SendMessage(&open);
}

/**** ajouter le menu des generateurs ****/
void MetosWin::AddGeneratorMenu(BList *generatorList)
{
	// recuperer le menu principal
	BMenu			*menu = NULL;

	// on doit trouver le menu principale
	menu = _menuBar->SubmenuAt(0);
	if(menu==NULL)
		return;

	// liberer la memoire prise par les items
	VCodeGenerator	*generator = NULL;
	BMenu			*generatorMenu = NULL;
	BString			display;
	int32			nbItem;

	// creer le menu
	generatorMenu = new BMenu(g_Languages->GetTranslationFor(L_MENU_GENERATE));
	
	// ajouter les items
	nbItem = generatorList->CountItems();
	for(int32 index = 0;index<nbItem;index++)
	{
		generator = (VCodeGenerator *)(generatorList->ItemAt(index));
		if(generator!=NULL)
		{
			display = generator->DisplayLabel();
			generatorMenu->AddItem(new BMenuItem(display.String(),new BMessage(SHOW_GENERATOR_MSG)));
		}
	}
	// definir la destination des messages du menu
	generatorMenu->SetTargetForItems(be_app);
	
	// ajouter le menu des generateurs
	menu->AddItem(generatorMenu,6);
	menu->AddItem(new BSeparatorItem(),7);
}

// ===============
// = Preferences =
// ===============

/**** rafraichir les preferences ****/
void MetosWin::RefreshPreferences()
{
	VDebugItem			*item = NULL;
	int32				nbItems;

	// recuperer le niveau de debug
	_debugLevel = g_MeTOSPrefs->GetInt32(VDEBUG_INFO,"debug-level");
	// on enleve 1 au nombre car on calcul avec 0 compté
	_nbMaxDebug = g_MeTOSPrefs->GetInt32(50,"max-debug-message");
	_nbMaxDebug--;

	// rafraichir tous les items de la liste
	nbItems = _infoList->CountItems();
	for(int32 index=0;index<nbItems;index++)
	{
		item = (VDebugItem *)(_infoList->ItemAt(index));
		if(item!=NULL)
			item->RefreshPreferences();
	}	
}

/**** sauver les preferences ****/
void MetosWin::SavePreferences()
{
	// on va sauver la taille des colonnes de la liste
	BMenuItem			*item = NULL;
	int8				nbRecent;

	// charger les preferences
	g_MeTOSPrefs->Load();
	g_MeTOSPrefs->SetRect(Frame(),"appWindow-rect");

	// et les derniers fichiers ouvert
	nbRecent = _recentLoad->CountItems();
	g_MeTOSPrefs->SetInt8(nbRecent,"nb-recent");
	for(int8 index=0;index<nbRecent;index++)
	{
		// ecrire dans les prefs la liste des chemins
		item = _recentLoad->ItemAt(index);
		if(item!=NULL)
			g_MeTOSPrefs->SetString(BString(item->Label()),"path-recent",index);
	}
	
	// puis les sauver
	g_MeTOSPrefs->Save();
}

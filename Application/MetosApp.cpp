/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/

/*************************************/
/* MeTOS projet RAD tool application */
/* generateur de code C++ pour       */
/* interface graphique               */
/*************************************/

#include "MetosApp.h"
#include "MetosAppDefs.h"
#include "MetosLanguagesDefs.h"
#include "VPreferenceFile.h"
#include "MetosWin.h"
#include "MetosBackGroundWin.h"
#include "MetosSplashScreen.h"
#include "ObjectManagerWin.h"
#include "PropertieManagerWin.h"
#include "AddOnManagerWin.h"
#include "TipsManagerWin.h"
#include "ToolManagerWin.h"
#include "UndoManagerWin.h"
#include "MetosMessageDefs.h"
#include "MetosGlobalDefs.h"
#include "MetosUndoDefs.h"
#include "MetosAboutWin.h"
#include "PreferenceManagerWin.h"
#include "GUIManager.h"
#include "VObject.h"
#include "VTranslator.h"
#include "VCodeGenerator.h"
#include "VLanguage.h"

#include <storage/File.h>
#include <app/Roster.h>
#include <interface/Bitmap.h>
#include <storage/Entry.h>
#include <storage/Node.h>
#include <storage/NodeInfo.h>
#include <storage/Directory.h>
#include <storage/FindDirectory.h>
#include <storage/FilePanel.h>
#include <app/Cursor.h>
#include <interface/Control.h>
#include <interface/Window.h>
#include <interface/MenuField.h>
#include <interface/PopUpMenu.h>
#include <interface/MenuItem.h>
#include <interface/Screen.h>
#include <stdio.h>
#include <stdlib.h>

// pointer globale sur be_app apres cast
MetosApp		*g_MetosApp = NULL;
VLanguage		*g_Languages = NULL;

// messengers en globale
BMessenger		g_MetosWin;
BMessenger		g_ToolManager;
BMessenger  	g_UndoManager;
BMessenger 		g_AddOnManager;
BMessenger		g_PreferenceManager;
BMessenger		g_PropertieManager;
BMessenger		g_ObjectManager;

/*** Application ***/
int main(void)
{
	MetosApp	*application = NULL;

	// fichier de preferences
	g_MeTOSPrefs = new VPreferenceFile("MeTOS");
	g_ColorsPrefs = new VPreferenceFile("Colors");

	// lancer l'application
	application = new MetosApp();
	application->Run();
	delete application;

	// fichiers de preferences
	delete g_MeTOSPrefs;
	delete g_ColorsPrefs;
	
	// quitter
	return 0;
}

/*** constructeur ***/
MetosApp::MetosApp()
: BApplication(APPLICATION_SIGNATURE)
{
	// initialiser
	BString				addonsPath;
	BFile				file;
	BEntry				entry;
	app_info			info;

	// pointer globale sur l'application castée
	g_MetosApp = (MetosApp *)be_app;
	_aboutWin = NULL;
	
	// initialiser le fichier ressource
	// et le chemin de l'appli
	GetAppInfo(&info);
	file.SetTo(&(info.ref), B_READ_ONLY);
	_ressource.SetTo(&file);
	_appPath.SetTo(&(info.ref)); 
    _appPath.GetParent(&_appPath);

	// ajouter le chemin des addons
	addonsPath = ADDON_FOLDER;
	addonsPath << "/";
	_appPath.Append(addonsPath.String());

	// charger les curseurs
	_cursorSprite[0] = NULL;
	_cursorSprite[1] = GetCursor("MoveCursor");
	_cursorSprite[2] = GetCursor("ResizeCursor");

	// charger les preferences
	// si ca marche pas on fixe les valeurs par defaut
	g_MeTOSPrefs->Load();

	// contruire le message des languages
	// et le charger du fichier texte
	g_Languages = new VLanguage("MeTOS");
	if(g_Languages->Load()!=B_OK)
		printf("Error loading Language file from application\n");

	// creer le selecteur de fichier
	_loadPanel = new BFilePanel(B_OPEN_PANEL,new BMessenger(this),NULL,B_FILE_NODE,false,new BMessage(B_LOAD_REQUESTED),NULL,true);
	_loadPanel->Window()->SetType(B_FLOATING_WINDOW);
	_savePanel = new BFilePanel(B_SAVE_PANEL,new BMessenger(this),NULL,B_FILE_NODE,false,NULL,NULL,true);
	_savePanel->Window()->SetType(B_FLOATING_WINDOW);

	// creation des fenetre
	_splashScreen = new MetosSplashScreen(GetCenterFrame(400,300));

	// afficher la fenetre de l'application
	_metosWin = new MetosWin(g_MeTOSPrefs->GetRect(BRect(50,50,400,250),"appWindow-rect"));
	g_MetosWin = BMessenger(_metosWin);

	// affichage du splash-screen
	_splashScreen->Show();

	// charger les translators de format
	// et les ajouter a la fenetre de chargement
	LoadTranslators();
	AddTranslatorToPanel();	

	// charger les generateurs de code
	LoadGenerators();
	_metosWin->AddGeneratorMenu(&_generatorList);

	// fenetre principale
	_metosWin->Show();

	// fenetre des preferences
	// on ne l'affiche pas tout de suite
	_preferenceManagerWin = new PreferenceManagerWin(g_MeTOSPrefs->GetRect(BRect(280,200,750,540),"preference-rect"));	
	g_PreferenceManager = BMessenger(_preferenceManagerWin);

	// fenetre de gestion des undo
	_undoManagerWin = new UndoManagerWin(g_MeTOSPrefs->GetRect(BRect(800,200,872,600),"undoManager-rect"));
	g_UndoManager = BMessenger(_undoManagerWin);

	// la fenetre des outils
	_toolManagerWin = new ToolManagerWin(g_MeTOSPrefs->GetRect(BRect(800,50,872,158),"toolsManager-rect"));
	g_ToolManager = BMessenger(_toolManagerWin);

	// creer puis ouvrir la fenetre des addons
	_addOnManagerWin = new AddOnManagerWin(g_MeTOSPrefs->GetRect(BRect(450,50,780,240),"addOnManager-rect"));
	g_AddOnManager = BMessenger(_addOnManagerWin);

	// creer puis ouvrir la fenetre des proprietes
	_propertieManagerWin = new PropertieManagerWin(g_MeTOSPrefs->GetRect(BRect(450,350,780,540),"propertiesManager-rect"));
	g_PropertieManager = BMessenger(_propertieManagerWin);

	// creer puis ouvrir la fenetre projet
	_objectManagerWin = new ObjectManagerWin(g_MeTOSPrefs->GetRect(BRect(50,350,400,540),"objectManager-rect"));
	g_ObjectManager = BMessenger(_objectManagerWin);

	// fenetre Tips
	_tipsManagerWin = new TipsManagerWin(GetCenterFrame(480,160));

	// afficher les fenetres
	_undoManagerWin->Show();
	_toolManagerWin->Show();
	_addOnManagerWin->Show();
	_propertieManagerWin->Show();
	_objectManagerWin->Show();

	// verifier si on utilise un fond
	_backgroundState = g_MeTOSPrefs->GetInt32(B_CONTROL_OFF,"use-background");
	DisplayBackGround(_backgroundState);

	// afficher les Tips si il faut
	if(g_MeTOSPrefs->GetInt32(B_CONTROL_ON,"show-at-startup")==B_CONTROL_ON)
		_tipsManagerWin->Show();
	
	// taille de l'ecran
	SetScreenSize();		

	// charger les addons
	_preferenceManagerWin->LoadAddOns();
	_toolManagerWin->LoadAddOns();
	_propertieManagerWin->LoadAddOns();
	_objectManagerWin->LoadAddOns();

	// lancer la creation de la liste des outils
	g_ToolManager.SendMessage(M_TOOLS_NAME_LIST_MSG,_objectManagerWin);		

	// quitter le splash
	BMessenger(_splashScreen).SendMessage(B_QUIT_REQUESTED);		

	// selection vide d'un objet
	// ceci pour griser les addons non disponible
	g_AddOnManager.SendMessage(M_SELECT_OBJECT_MSG);
}

/*** destructeur ***/
MetosApp::~MetosApp()
{
	// decharger les generateurs de code
	UnloadGenerators();

	// detruire les translators
	UnloadTranslators();

	// effacer les curseurs si on les a definis
	// mais remettre le standard
	if(_cursorSprite[1]!=NULL)
		delete _cursorSprite[1];
	if(_cursorSprite[2]!=NULL)
		delete _cursorSprite[2];

	// detruire le seecteur de fichier
	delete _loadPanel;
	delete _savePanel;
	
	// detruire le fichiers des preferences
	delete g_Languages;
}

/**** quit requested ****/
bool MetosApp::QuitRequested()
{

	// remettre le curseur par defaut
	if(Lock())
	{
		SetCursor(B_CURSOR_SYSTEM_DEFAULT,true);
		Unlock();
	}
	else
		SendDebbug("Error : Lock() in MetosApp::QuitRequested()",VDEBUG_ERROR);

	// on quitte alors les fenetre
	g_DoQuit = true;

	// quitter
	return (true);
}

/**** MessageReceived ****/
void MetosApp::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
	// drag & drop ou creation d'un objet
	// nouveau projet
	case M_NEW_PROJECT_MSG:
	case M_DROP_ADDON_MSG:
		g_ObjectManager.SendMessage(message);
		break;
	// menu de l'application
	case M_SAVE_PROJECT_AS_MSG:
		_savePanel->Show();
		break;
	// sauver un projet
	case M_SAVE_PROJECT_MSG:
		Save();
		break;
	case B_SAVE_REQUESTED:
		SaveAs(message);
		break;
	// charger un projet
	case M_LOAD_PROJECT_MSG:
		_loadPanel->Show();
		break;
	// drag&drop renvoyer a l'application
	case B_SIMPLE_DATA:
	case B_LOAD_REQUESTED:
		LoadProject(message);
		break;
	// generation de code
	case SHOW_GENERATOR_MSG:
		StartGenerators(message);
		break;
	// undo (on appel le menu)
	case UNDO_RESTORE_MSG:
		g_UndoManager.SendMessage(message);
		break;
	// voir les fenetres de l'application
	case SHOW_PROJECT_WIN_MSG:
		if(_objectManagerWin->Lock())
		{
			if(_objectManagerWin->IsHidden())
				_objectManagerWin->Show();
			_objectManagerWin->Unlock();
		}
		else
			SendDebbug("Error : _objectManagerWin->Lock() in MetosApp::MessageReceived()",VDEBUG_ERROR);
		break;
	case SHOW_PROPERTIES_WIN_MSG:
		if(_propertieManagerWin->Lock())
		{
			if(_propertieManagerWin->IsHidden())
				_propertieManagerWin->Show();
			_propertieManagerWin->Unlock();
		}
		else
			SendDebbug("Error : _propertieManagerWin->Lock() in MetosApp::MessageReceived()",VDEBUG_ERROR);
		break;
	case SHOW_OBJECT_WIN_MSG:
		if(_addOnManagerWin->Lock())
		{
			if(_addOnManagerWin->IsHidden())
				_addOnManagerWin->Show();
			_addOnManagerWin->Unlock();
		}
		else
			SendDebbug("Error : _addOnManagerWin->Lock() in MetosApp::MessageReceived()",VDEBUG_ERROR);
		break;
	case SHOW_PREFERENCES_WIN_MSG:
		if(_preferenceManagerWin->Lock())
		{
			if(_preferenceManagerWin->IsHidden())
				_preferenceManagerWin->Show();
			_preferenceManagerWin->Unlock();
		}
		else
			SendDebbug("Error : _preferenceManagerWin->Lock() in MetosApp::MessageReceived()",VDEBUG_ERROR);
		break;
	case SHOW_TIPS_WIN_MSG:
		if(_tipsManagerWin->Lock())
		{
			if(_tipsManagerWin->IsHidden())
				_tipsManagerWin->Show();
			_tipsManagerWin->Unlock();
		}
		else
			SendDebbug("Error : _tipsManagerWin->Lock() in MetosApp::MessageReceived()",VDEBUG_ERROR);
		break;
	case SHOW_ABOUT_WIN_MSG:
		ShowAboutBox();
		break;
	// action utilisateur sur les objets
	// rafraichir des proprietes
	case M_SELECT_OBJECT_MSG:
	case M_OBJECT_CHANGE_MSG:
		g_ObjectManager.SendMessage(message);
		break;
	// effacer les objets selectionnés
	case M_DELETE_OBJECT_MSG:
		g_ObjectManager.SendMessage(message);
		break;
	// definir la forme du curseur
	case M_SET_CURSOR_MSG:
		DefineCursorLook(message);
		break;
	// rafraichir l'affichage de l'objet courant
	case M_REFRESH_OBJECTS_MSG:
		g_GUIManagerView->InvalidateAll();
		g_ObjectManager.SendMessage(message);
		break;
	// demande de rafraichir l'affichage d'une proprietes
	case M_REFRESH_PROPERTIES_MSG:
		g_PropertieManager.SendMessage(message);
		break;
	// demande de recup de la liste des generateurs
	case M_GENERATOR_NAME_LIST_MSG:
		GetGeneratorList(message);
		break;
	// rafraichir les preferences
	case M_PREFERENCE_MSG:
		RefreshPreferences();
		break;
	// demande de verification d'update
	case M_CHECK_UPDATE_MSG:
		CheckUpdate(message);
		break;
	// afficher l'aide
	case M_METOS_HELP_MSG:
		LaunchHelp();
		break;
	// supporter MeTOS en faisant un don
	case M_METOS_DONATE_MSG:
		SupportMeTOS();
		break;
	default:
		BApplication::MessageReceived(message);	
	}
}

// ==================================
// construire un message et l'envoyer
// ==================================

/**** fonction globale d'envoi de message ****/
status_t MetosApp::SendDebbug(const char *message,debug_type type = VDEBUG_DEBUG)
{
	// verifier le niveau de debug
	if(type > _metosWin->DebugLevel())
		return B_ERROR;
	
	BMessage	msg(METOS_DEBUG_MSG);
	
	// ajouter les donnees
	msg.AddString(VDEBUG_STRING,message);
	msg.AddInt32(VDEBUG_TYPE,(int32)type);
	
	// envoyer le message
	return g_MetosWin.SendMessage(&msg);
}

// =============================
// = taille d'ecran utilisable =
// =============================

/**** definir la position des fenetres par rapport a la taille de l'ecran ****/
void MetosApp::SetScreenSize()
{
	BRect				frame;
	int32				newSize;
	int32				height,width;
	
	// on va comparer la valeur sauver avec la taille reel
	// si c'est different il faut modifier, on modifie aussi si
	// la valeur est differente de celle de _screenSize

	// charger les prefs
	g_MeTOSPrefs->Load();

	// taille des prefs
	newSize = g_MeTOSPrefs->GetInt32(SCREEN_1024_768,"screen-size");

	// on va deplacer les fenetres pour permettre de les voir
	switch(newSize)
	{
	case SCREEN_800_600:
		width = 799;
		height = 599;
		break;
	case SCREEN_1024_768:
		width = 1023;
		height = 767;
		break;
	case SCREEN_1152_864:
		width = 1151;
		height = 863;
		break;
	case SCREEN_1280_1024:
		width = 1279;
		height = 1023;
		break;
	case SCREEN_1600_1200:
		width = 1599;
		height = 1199;
		break;
	default:	// SCREEN_640_480
		width = 639;
		height = 479;
	}
	
	// verifier l'emplacement des fenetres !
	BWindow		*searchWindow = NULL;
	BWindow		*background = NULL;
	int32		dx,dy;
	int32		nbWindow;

	// fenetre de fond (tenir compte si elle n'existe pas)
	if(g_MetosBackWin == _metosWin)
		background = _metosWin;

	// fenetres de l'application
	// et fenetre creer par l'utilisateur
	nbWindow = CountWindows();
	for(int32 index = 0;index<nbWindow;index++)
	{
		searchWindow = WindowAt(index);	
		if(searchWindow!=NULL && searchWindow!=background)
		{
			// verifier pour chaques fenetres (sauf le background)
			if(searchWindow->Lock())
			{
				frame = searchWindow->Frame();
				searchWindow->Unlock();

				// deplacer
				dx = width - (int32)(frame.right);
				dy = height - (int32)(frame.bottom);
				if(dx>=0)
					dx = 0;
				if(dy>=0)
					dy = 0;

				// bouger la fenetre si il le faut
				if(dx<0 || dy<0)
					searchWindow->MoveBy(dx,dy);
			}
			else
				SendDebbug("Error : Lock() in MetosApp::SetScreenSize()",VDEBUG_ERROR);
		}
	}
}

// ===========================
// = gestion du fond d'ecran =
// ===========================

/**** afficher un fond si il faut ****/
void MetosApp::DisplayBackGround(int32 state)
{
	// afficher ou pas le fond
	if(state == B_CONTROL_ON)
	{	
		// verifier si le fond existe !
		// est si c'est pas la fenetre du programme
		if(g_MetosBackWin==NULL || g_MetosBackWin==_metosWin)
			g_MetosBackWin =  new MetosBackGroundWin();

		_metosWin->SetFeel(B_FLOATING_SUBSET_WINDOW_FEEL);
		_metosWin->AddToSubset(g_MetosBackWin);

		// afficher la fenetre de fond d'ecran
		g_MetosBackWin->Show();
	}
	else
	{
		// retablir l'etat de la fenetre principale
		_metosWin->SetFeel(B_NORMAL_WINDOW_FEEL);
		_metosWin->RemoveFromSubset(g_MetosBackWin);

		if(g_MetosBackWin!=NULL)
			BMessenger(g_MetosBackWin).SendMessage(B_QUIT_REQUESTED);
		
		// attendre un peu
		g_MetosBackWin = _metosWin;
	}
	
	// attacher les fenetres au bon fond !	
	ModifySubset();
}

/**** modifier la fenetre mere des autres ****/
void MetosApp::ModifySubset()
{
	BWindow				*searchWindow = NULL;
	int32				nbWindow;

	// fenetres de l'application
	// et fenetre creer par l'utilisateur
	nbWindow = CountWindows();
	for(int32 index = 0;index<nbWindow;index++)
	{
		searchWindow = WindowAt(index);	
		if(searchWindow!=NULL && searchWindow!=g_MetosBackWin)
			searchWindow->AddToSubset(g_MetosBackWin);
	}
}

// =======================================
// = extraire des données des ressources =
// =======================================

/**** trouver une image dans les ressources ****/
BBitmap *MetosApp::GetBitmap(const char *name)
{
	BBitmap 	*bitmap = NULL;
	size_t 		len = 0;
	status_t 	error;	

	// charger depuis les ressources
	const void *data = _ressource.LoadResource('BBMP', name, &len);

	BMemoryIO stream(data, len);
	
	// charge l'image archivé
	BMessage archive;
	error = archive.Unflatten(&stream);
	if (error != B_OK)
		return NULL;

	// on va essayer de la recreer
	bitmap = new BBitmap(&archive);
	if(!bitmap)
		return NULL;

	// verifier que ca a marché
	if(bitmap->InitCheck() != B_OK)
	{
		delete bitmap;
		return NULL;
	}
	
	// tout c'est bien passé
	return bitmap;
}

/**** trouver un curseur dans les ressources ****/
BCursor *MetosApp::GetCursor(const char *name)
{
	BCursor		*cursor = NULL;
	size_t 		len = 0;

	// charger depuis les ressources
	const void *data = _ressource.LoadResource('CURS', name, &len);

	cursor = new BCursor(data);

	// tout c'est bien passé
	return cursor;
}

// ================================================
// = Gestion des translators du format de fichier =
// ================================================

/**** charger les translators ****/
void MetosApp::LoadTranslators()
{
	VTranslator			*translator = NULL;
	VTranslator			*(*instantiate_VTranslator)(image_id addonId,BEntry *entry);
	image_id			addonId;
   	status_t 			err = B_NO_ERROR;
	app_info 			info; 
    BPath 				path;
	BString				message;
	int32				nbFiles;
	BMessage			splashInfo(M_PROGRESSSTART_MSG);
	BMessage			splashProgress(M_PROGRESSINCREASE_MSG);
	BEntry				entry;
	BString				buffer;
	
	if(Lock())
	{
		// initialiser le compteur du splash-screen
		nbFiles = CountAddOnFile(ADDON_TRANSLATORS_FOLDER);
		splashInfo.AddInt32(M_TOTALPROGRESS,nbFiles);
		splashInfo.AddString(M_TEXTDISPLAY,"Load Translators AddOns...");
		BMessenger(_splashScreen).SendMessage(&splashInfo);
		
 	  	// trouver le chemin des addons
	    path = GetAddonsPath();
		path.Append(ADDON_TRANSLATORS_FOLDER);
		BDirectory dir(path.Path());

	   	// charger les addons
		while( err == B_NO_ERROR )
		{
			err = dir.GetNextEntry((BEntry*)&entry, true);			
			if( entry.InitCheck() != B_NO_ERROR )
				break;
		
			if( entry.GetPath(&path) == B_NO_ERROR )
			{
				addonId = load_add_on(path.Path());
				if(addonId>=0)
				{
					// on regarde si on arrive a trouver la metode de creation de l'objet
					if(get_image_symbol(addonId,"instantiate_VTranslator", B_SYMBOL_TYPE_TEXT,(void **)&instantiate_VTranslator) == B_OK)
					{
						// on creer un VObject sans creer l'objet graphique qu'il contient
						// ca nous permet d'acceder a ces proprietes et un tas d'autre chose
						// lors de la creation d'un objet graphique on va creer les deux
						translator = (*instantiate_VTranslator)(addonId,&entry);

						// verfrifier le type MIME
						if(translator!=NULL && translator->CheckMIMEType()!=B_OK)
							SendDebbug("MIME Type not registered !",VDEBUG_ERROR);

						// creer l'addon
						// ajouter a la liste interne						
						_translatorList.AddItem(translator);

						// informer le splash-screen
						BMessenger(_splashScreen).SendMessage(&splashProgress);
	
						// informer que tout c'est bien passé
						message = "Translator : ";	
						message << path.Path() << " Loaded";
						SendDebbug(message.String(),VDEBUG_DEBUG);
					}					
					else
						unload_add_on(addonId);
				}
			}
		}
		// debloquer
		Unlock();

		// informer que tout c'est bien passé
		message = "Translators Addons Loading finish";	
		SendDebbug(message.String(),VDEBUG_INFO);
	}
	else
		SendDebbug("Error : Lock() in MetosApp::LoadTranslators()",VDEBUG_ERROR);
}

/**** decharger les translators ****/
void MetosApp::UnloadTranslators()
{
	// liberer la memoire prise par les items
	VTranslator		*translator = NULL;
	image_id		image;
	int32			nbItem;
	
	nbItem = _translatorList.CountItems();
	for(int32 index = 0;index<nbItem;index++)
	{
		translator = (VTranslator *)(_translatorList.RemoveItem((int32)0));
		image = translator->AddonId();
		delete translator;				
		if(unload_add_on(image)!=B_OK)
			printf("Error Unloading Translator image\n");
	}
}

/**** trouver un translator pour un type mime ****/
VTranslator *MetosApp::FindTranslatorFromMIME(const char *mime)
{
	// liberer la memoire prise par les items
	VTranslator		*translator = NULL;
	BString			mimeManage;
	int32			nbItem;
	
	// parcourir la liste des Translators
	nbItem = _translatorList.CountItems();
	for(int32 index = 0;index<nbItem;index++)
	{
		translator = (VTranslator *)(_translatorList.ItemAt(index));
		if(translator!=NULL)
		{
			// trouver le type mime demandé
			mimeManage = translator->MIMEType();
			if(mimeManage==mime)
				return translator;
		}
	}

	// on a pas trouvé de translator
	return NULL;
}

/**** ajouter la selection du translator ****/
void MetosApp::AddTranslatorToPanel()
{
	BView				*background = NULL;
	BPopUpMenu			*translatorList = NULL;
	BMenuField			*selection = NULL;
	float				xPosition,yPosition;
	VTranslator			*translator = NULL;
	BString				translatorName;
	int32				nbItem;
	int32				defaultTranslator;

	// charger les preferences
	// si ca marche pas on fixe les valeurs par defaut
	g_MeTOSPrefs->Load();
	
	// creer le menu et ajouter les nom des translators
	translatorList = new BPopUpMenu("translator-selection");
	nbItem = _translatorList.CountItems();
	for(int32 index = 0;index<nbItem;index++)
	{
		translator = (VTranslator *)(_translatorList.ItemAt(index));
		if(translator!=NULL)
		{
			// trouver le type mime demandé
			translatorName = translator->Name();
			if(translatorName.Length()>0 && translator->Type()!=VTRANSLATOR_READ_ONLY)
				translatorList->AddItem(new BMenuItem(translatorName.String(),NULL));
		}
	}
	
	// la liste deroulante
	selection = new BMenuField(BRect(100,10,100,20),"type-save","",translatorList,B_FOLLOW_LEFT | B_FOLLOW_BOTTOM);

	// fixer le translator par defaut
	defaultTranslator = g_MeTOSPrefs->GetInt32(0,"default-translator");
	if(translatorList->CountItems()>0)
		translatorList->ItemAt(defaultTranslator)->SetMarked(true);
	
	// ajouter une selection du translator au paneau de lecture
	if(_savePanel->Window()->Lock())
	{
		background = _savePanel->Window()->ChildAt(0);
		background->AddChild(selection);
		xPosition = (background->Bounds()).right - 270;
		yPosition = (background->Bounds()).bottom - 35;
		selection->MoveTo(xPosition,yPosition);
		_savePanel->Window()->Unlock();
	}
	else
		SendDebbug("Error : Lock() in MetosApp::AddTranslatorToPanel()",VDEBUG_ERROR);
}

// ===================================
// = Gestion des Generateurs de code =
// ===================================

/**** charger les generateurs de code ****/
void MetosApp::LoadGenerators()
{
	VCodeGenerator		*generator = NULL;
	VCodeGenerator		*(*instantiate_VCodeGenerator)(image_id addonId,BEntry *entry);
	image_id			addonId;
   	status_t 			err = B_NO_ERROR;
	app_info 			info; 
    BPath 				path;
	BString				message;
	int32				nbFiles;
	BMessage			splashInfo(M_PROGRESSSTART_MSG);
	BMessage			splashProgress(M_PROGRESSINCREASE_MSG);
	BEntry				entry;
	BString				buffer;
	
	if(Lock())
	{
		// initialiser le compteur du splash-screen
		nbFiles = CountAddOnFile(ADDON_GENERATORS_FOLDER);
		splashInfo.AddInt32(M_TOTALPROGRESS,nbFiles);
		splashInfo.AddString(M_TEXTDISPLAY,"Load Generators AddOns...");
		BMessenger(_splashScreen).SendMessage(&splashInfo);
		
   		// trouver le chemin des addons
	    path = GetAddonsPath();
		path.Append(ADDON_GENERATORS_FOLDER);
		BDirectory dir(path.Path());

	   	// charger les addons
		while( err == B_NO_ERROR )
		{
			err = dir.GetNextEntry((BEntry*)&entry, true);			
			if( entry.InitCheck() != B_NO_ERROR )
				break;
		
			if( entry.GetPath(&path) == B_NO_ERROR )
			{
				addonId = load_add_on(path.Path());
				if(addonId>=0)
				{
					// on regarde si on arrive a trouver la metode de creation de l'objet
					if(get_image_symbol(addonId,"instantiate_VCodeGenerator", B_SYMBOL_TYPE_TEXT,(void **)&instantiate_VCodeGenerator) == B_OK)
					{
						// on creer un VObject sans creer l'objet graphique qu'il contient
						// ca nous permet d'acceder a ces proprietes et un tas d'autre chose
						// lors de la creation d'un objet graphique on va creer les deux
						generator = (*instantiate_VCodeGenerator)(addonId,&entry);

						// creer l'addon
						// ajouter a la liste interne						
						_generatorList.AddItem(generator);

						// informer le splash-screen
						BMessenger(_splashScreen).SendMessage(&splashProgress);

						// informer que tout c'est bien passé
						message = "Generator : ";	
						message << path.Path() << " Loaded";
						SendDebbug(message.String(),VDEBUG_DEBUG);
					}
					else
						unload_add_on(addonId);
				}
			}
		}
		// debloquer
		Unlock();
		
		// informer que tout c'est bien passé
		message = "Generators Addons Loading finish";	
		SendDebbug(message.String(),VDEBUG_INFO);
	}
	else
		SendDebbug("Error : Lock() in MetosApp::LoadGenerators()",VDEBUG_ERROR);
}

/**** decharger de la memoire les generateurs de code ****/
void MetosApp::UnloadGenerators()
{
	// liberer la memoire prise par les items
	VCodeGenerator	*generator = NULL;
	image_id		image;
	int32			nbItem;
	
	nbItem = _generatorList.CountItems();
	for(int32 index = 0;index<nbItem;index++)
	{
		generator = (VCodeGenerator *)(_generatorList.RemoveItem((int32)0));
		image = generator->AddonId();
		delete generator;
		if(unload_add_on(image)!=B_OK)
			printf("Error unloading Generator Addon\n");
	}
}

/**** lancer le generateur selectionné ****/
void MetosApp::StartGenerators(BMessage *message)
{
	VCodeGenerator	*generator = NULL;
	int32			index = -1;
	
	// verifier si on trouve un index
	if(message->FindInt32("index",&index)!=B_OK)
		return;
		
	// trouver le generateur dans la liste
	generator = (VCodeGenerator *)(_generatorList.ItemAt(index));
	if(generator!=NULL)
	{
		BMessage	objectArchive(VOBJECT_ARCHIVE_MSG);
		BString		generatorID = generator->ID();
	
		// creer l'archive
		if(_objectManagerWin->Lock())
		{
			// pre-generer les archive des objets
			// puis debloquer la fenetre
			_objectManagerWin->PreGenerate(&objectArchive,generatorID);
			_objectManagerWin->Unlock();
	
			// lancer le generateur
			generator->StartGenerator(&objectArchive);
		}
		else
			SendDebbug("Error : Lock() in MetosApp::StartGenerators()",VDEBUG_ERROR);
	}
}

/**** pour informer la fenetre des proprietes des generateurs existants ****/
void MetosApp::GetGeneratorList(BMessage *reply)
{
	VCodeGenerator	*generator = NULL;
	BMessage		generatorList(M_GENERATOR_NAME_LIST_MSG);
	int32			nbGenerators = 0;
	
	// parcourir la liste interne des generateurs
	nbGenerators = _generatorList.CountItems();
	for(int32 i=0;i<nbGenerators;i++)
	{
		// recuperer les infos de chaque generateurs
		generator = (VCodeGenerator *)(_generatorList.ItemAt(i));
		if(generator!=NULL)
		{
			// on ajoute le nom reel et l'identifiant !
			generatorList.AddString(M_GENERATOR_ID,generator->ID());
			generatorList.AddString(M_GENERATOR_NAME,generator->Name());
		}
	}
	
	// renvoyer au demandeur
	reply->SendReply(&generatorList);
}

// ========================================
// = chargement et sauvegarde d'un projet =
// ========================================

/**** charger un projet ****/
void MetosApp::LoadProject(BMessage *message)
{
	VTranslator	*translator = NULL;
	entry_ref	selectedFile;
	BFile		loadFile;

	// le fichier doit exister
	if(message->FindRef("refs",&selectedFile)!=B_OK)
		return;

	// sinon on ouvre le fichier
	// verifier tout de meme si on peux
	loadFile.SetTo(&selectedFile,B_READ_ONLY);
	if(loadFile.InitCheck()!=B_OK)
	{
		SendDebbug("File can't be open !",VDEBUG_ERROR);
		return;
	}
		
	// on doit aussi verifier le type MIME
	BNode		fileNode(&selectedFile);
	BNodeInfo	nodeInfo(&fileNode);
	char		typeMIME[255];

	if(nodeInfo.InitCheck()!=B_OK)
		return;
	
	// comparer le type MIME du fichier par rapport a notre type	
	nodeInfo.GetType(typeMIME);
	translator = FindTranslatorFromMIME(typeMIME);
	if(translator==NULL)
		return;
	
	// le translator doit permetre la lecture
	if(translator->Type()==VTRANSLATOR_WRITE_ONLY)
		return;

	// charger les objets et les recreer
	BMessage	archive(M_PROJECT_ARCHIVE_MSG);	
	status_t	state;
	
	// chargement depuis le fichiers
	state = translator->LoadFile(&loadFile,&archive);
	if(state!=B_OK)
		return;

	// variable pour l'information
	BMessage	recent(M_ADD_RECENT_MSG);
	BString		messageInfo;
	BPath		path(&selectedFile);

	// verifier la validité de l'archive
	if(CheckProjectArchive(&archive)!=B_OK)
	{
		SendDebbug("One Addon used in the project file is not loaded !",VDEBUG_ERROR);
		return;
	}
	
	// si il existe deja un projet on va l'effacer
	// puis on va recuperer les objets de l'archive
	// et recreer le projet, les objets VObject et GUI
	// sans oublier de restorer les Undos et les reste des
	// preferences liées au projet
	state &= _objectManagerWin->DeleteAll();
	state &= _objectManagerWin->CreateNewProject();
	state &= _objectManagerWin->UnflattenAllObject(&archive);
	state &= _undoManagerWin->InstantiateUndo(&archive);
	
	// afficher un message d'alerte si il y a eu une erreur
	if(state!=B_OK)
	{
		messageInfo = "Error loading : ";
		messageInfo << path.Path();
		SendDebbug(messageInfo.String(),VDEBUG_WARNING);
		
		return;
	}
		
	// ajouter au fichiers recents
	path.SetTo(&selectedFile);
	if(path.InitCheck()==B_OK)
	{
		recent.AddString(M_RECENT_PATH,path.Path());
		g_MetosWin.SendMessage(&recent);
	}
	
	// information	
	messageInfo = "File : ";
	messageInfo << path.Path();
	messageInfo << " loaded";
	SendDebbug(messageInfo.String(),VDEBUG_INFO);
}

/**** sauver ****/
void MetosApp::Save()
{
	BString		selectedFile;

	// recuperer le chemin de la liste des objets
	if(_objectManagerWin->SavePath(selectedFile)!=B_OK)
		return ;

	// sauver
	SaveProject(selectedFile);	
}

/**** sauver sous ****/
void MetosApp::SaveAs(BMessage *message)
{
	status_t	state;
	BString		selectedFile;
	entry_ref	directory;

	// si on ne trouve pas le repertoire et le nom du fichier
	// on ouvre le panneaux de selection
	state = message->FindRef("directory",&directory);
	state &= message->FindString("name",&selectedFile);
	if(state!=B_OK)
		return;

	// sinon c'est bon on va essayer de sauvegarder
	BPath		path(&directory);

	// rafraichir l'affichage
	if(_objectManagerWin->Lock())
	{
		// rafraichir la fenetre des objets puis la debloquer
		_objectManagerWin->RefreshSavePath(path.Path(),selectedFile.String());
		_objectManagerWin->Unlock();
	
		selectedFile.Prepend("/");
		selectedFile.Prepend(path.Path());
	
		// sauver
		SaveProject(selectedFile);
	}
	else
		SendDebbug("Erreur : Lock() in MetosApp::SaveAs()",VDEBUG_ERROR);
}

/**** sauvegarder un projet ****/
status_t MetosApp::SaveProject(BString &selectedFile)
{
	BMessage		archive(M_PROJECT_ARCHIVE_MSG);
	BFile			saveFile;
	status_t		state = B_OK;
	BMenuField		*background = NULL;
	int32			translatorUsed = -1;
	VTranslator		*translator = NULL;
	BString			extension;
	
	// trouver le translator selectionné
	if(_savePanel->Window()->Lock())
	{
		// bloquer la fenetre pendant cette operation
		background = dynamic_cast<BMenuField *>(_savePanel->Window()->ChildAt(0)->FindView("type-save"));
		translatorUsed = background->Menu()->IndexOf(background->Menu()->FindMarked());
		_savePanel->Window()->Unlock();
	}
	else
		SendDebbug("Error : Lock() in MetosApp::SaveProject()",VDEBUG_ERROR);
	
	// recuperer le translator
	translator = (VTranslator *)(_translatorList.ItemAt(translatorUsed));
	if(translator==NULL)
		return B_ERROR;		
	
	// verifier si on a bien l'extension
	extension = translator->Extension();
	extension.Prepend(".");
	if(selectedFile.FindLast(extension)==B_ERROR)
		selectedFile << extension;
	
	saveFile.SetTo(selectedFile.String(),B_CREATE_FILE|B_ERASE_FILE|B_WRITE_ONLY);
	// archiver tous les objets et les sauvegarder
	if((state &= saveFile.InitCheck())!=B_OK)
		return state;

	// renseigner le MIME Type
	state &= translator->RegisterMIMEType(selectedFile.String());
	
	// archiver les objets les undo et tout le reste
	if((state &= CreateProjectArchive(&archive))!=B_OK)
		return state;
		
	// enregistrer
	state &= translator->SaveFile(&saveFile,&archive);
		
	// information
	BString		debug;

	debug = "File : ";
	debug << selectedFile;
	debug << " saved";
	SendDebbug(debug.String(),VDEBUG_INFO);

	// resultat
	return state;
}

/**** ouverture d'un projet par double clique sur le fichier ****/
void MetosApp::RefsReceived(BMessage *message)
{
	// appeler la metode de chargement d'un projet
	LoadProject(message);
}

// ===========================================
// = verification de l'integrité d'un projet =
// = et creation de l'archive				 =
// ===========================================

/**** compter les fichiers Addon dans un repertoire de l'appli ****/
int32 MetosApp::CountAddOnFile(const char *searchPath)
{
	int32		nbFind = 0;
	BPath		path;
	status_t	err = B_NO_ERROR;
	BEntry		entry;
	BNode		node;
	BNodeInfo	nodeInfo;
	char		type[255];	

	// construire le chemin a partir de l'appli
	path = _appPath;
	path.Append(searchPath);
	BDirectory dir(path.Path());

	while( err == B_NO_ERROR )
	{
		// parcourir le repertoire
		err = dir.GetNextEntry((BEntry*)&entry, true);			
		if( entry.InitCheck() != B_NO_ERROR )
			break;

		// on creer in nodeInfo pour obtenir le type
		node.SetTo(&entry);
		nodeInfo.SetTo(&node);
		if(node.InitCheck()==B_OK && nodeInfo.InitCheck()==B_OK)
		{
			// c'est les executables qui nous interesse
			if(nodeInfo.GetType(type)==B_OK)
				if(strcmp(type,"application/x-vnd.Be-elfexecutable")==0)
					nbFind++;	
		}	
	}

	// nombre d'exe trouvé
	return nbFind;
}

/**** verifier que les dependances existent (addons etc...) ****/
status_t MetosApp::CheckProjectArchive(BMessage *archive)
{	
	BMessage	objectArchive;
	BMessage	objectProperties;
	BString		addonSource;
	status_t	state = B_OK;
	type_code	type;
	int32		numberObject;
	int32		addonIndex = -1;

	// trouver le nombre d'objet
	if(archive->GetInfo(VOBJECT_ARCHIVE,&type,&numberObject)!=B_OK)
		return B_ERROR;
		
	for(int32 index=0;index<numberObject;index++)
	{
		// chercher pour chaque objet avec quelle addons on l'a creer
		state &= archive->FindMessage(VOBJECT_ARCHIVE,index,&objectArchive);
		state &= objectArchive.FindMessage(VOBJECT_PROPERTIES,&objectProperties);
		state &= objectArchive.FindString(VOBJECT_ADDON_SOURCE,&addonSource);
		if(state!=B_OK)
			return state;

		// verifier que l'ona l'addon pour creer l'objet
		if(_objectManagerWin->Lock())
		{
			addonIndex = _objectManagerWin->FindItemFromName(addonSource.String());
			_objectManagerWin->Unlock();
		}
		else
			SendDebbug("Error : Lock() in MetosApp::CheckProjectArchive()",VDEBUG_ERROR);
		
		// as-ton un index valide
		if(addonIndex==-1)
			return B_ERROR;		
	}
	
	return B_OK;
}

/**** creer l'archive du projet ****/
status_t MetosApp::CreateProjectArchive(BMessage *archive)
{
	status_t	state = B_ERROR;
	
	if(Lock())
	{
		// archiver les objets
		state = _objectManagerWin->FlattenAllObject(archive);
	
		// archiver les undo
		state &= _undoManagerWin->ArchiveUndo(archive);
		Unlock();
	}
	else
		SendDebbug("Error : Lock() in MetosApp::CreateProjectArchive()",VDEBUG_ERROR);
	
	return state;
}
// ==============
// = Divers ... =
// ==============

/**** ou placer la fenetre ****/
BRect MetosApp::GetCenterFrame(float dx,float dy)
{
	BRect		newFrame;
	BScreen		screen;
	
	if(!screen.IsValid())
		return BRect(200,210,680,390);
	
	newFrame = screen.Frame();

	// regler la largeur
	newFrame.left = (newFrame.right / 2) - (dx / 2);
	newFrame.right = newFrame.left + dx;
	// regler la hauteur
	newFrame.top = (newFrame.bottom / 2) - (dy / 2);
	newFrame.bottom = newFrame.top + dy;
	
	return newFrame;
}

// =================
// = Fenetre About =
// =================

/**** Afficher la fenetre d'information ****/
void MetosApp::ShowAboutBox()
{
	// pas deux fois
	if(_aboutWin!=NULL)
		return;
	
	// fenetre a propos
	g_MeTOSPrefs->Load();
	_aboutWin = new MetosAboutWin(g_MeTOSPrefs->GetRect(BRect(280,200,669,488),"about-rect"));
	_aboutWin->AddToSubset(_metosWin);
	_aboutWin->Show();
}
// =====================
// = Curseur de souris =
// =====================

/**** definir la forme du curseur ****/
void MetosApp::DefineCursorLook(BMessage *data)
{
	int32	cursorState = 0;
	
	if(data->FindInt32(M_CURSOR_LOOK,&cursorState)!=B_OK)
		return;
	
	if(Lock())
	{
		if(cursorState==0)
			SetCursor(B_CURSOR_SYSTEM_DEFAULT,true);
		else
			SetCursor(_cursorSprite[cursorState],true);
		Unlock();
	}
	else
		SendDebbug("Error : Lock() in MetosApp::DefineCursorLook()",VDEBUG_ERROR);
}

// ===========================
// = gestion des preferences =
// ===========================

/**** rafraichir les preferences ****/
void MetosApp::RefreshPreferences()
{
	BWindow				*searchWindow = NULL;
	int32				nbWindow;
	int32				backgroundState;

	// charger les preferences
	// si ca marche pas on fixe les valeurs par defaut
	g_MeTOSPrefs->Load();
	g_ColorsPrefs->Load();

	// verifier si il faut un fond d'ecran
	backgroundState = g_MeTOSPrefs->GetInt32(B_CONTROL_OFF,"use-background");
	if(backgroundState!=_backgroundState)
	{

		DisplayBackGround(backgroundState);
		
		// remettre en avant la fenetre des prefs
		g_MetosBackWin->Activate(true);
		_preferenceManagerWin->Activate(true);		
		_backgroundState = backgroundState;
	}

	// verifier la taille de l'ecran
	SetScreenSize();

	// envoyer a chaque fenetre
	nbWindow = CountWindows();
	for(int32 index = 0;index<nbWindow;index++)
	{
		searchWindow = WindowAt(index);	
		if(searchWindow!=NULL)
			BMessenger(searchWindow).SendMessage(M_PREFERENCE_MSG);
	}

	// et au gestionnaire graphique
	g_GUIManager.SendMessage(M_PREFERENCE_MSG);
}

// ==========================
// = gestion des Mises a jour
// ==========================

/**** verifier si il y a une mise a jour ****/
void MetosApp::CheckUpdate(BMessage *data)
{
	// verifier si le message est valide
	if(data==NULL)
		return;
	
	BEntry				entry;
	BFile				file;
	app_info			info;
	BString				upgraderFile;
	BPath				upgraderPath;
	BString				upgraderCommand;
	BString				upgraderVersion;
	BString				upgraderURL;

	// recuperer le chemin de l'application	
	GetAppInfo(&info);
	file.SetTo(&(info.ref), B_READ_ONLY);
	upgraderPath.SetTo(&(info.ref)); 
	upgraderPath.GetParent(&upgraderPath);

	upgraderFile.SetTo(upgraderPath.Path());
	upgraderFile.Append("/");
	upgraderFile.Append(METOS_UPGRADER_FOLDER);
	upgraderFile.Append("/");
	upgraderFile.Append(METOS_UPGRADER_APPLICATION);

	// verifier si l'exe d'upgrade existe
	entry.SetTo(upgraderFile.String());
	if(!entry.Exists())
		return;

	// on va construire la commande de mise a jour
	// on peux avoir dans le message le nom du fichier script et l'URL
	// sinon par defaut c'est une update du logiciel
	// on prend ces infos depuis le fichiers de preferences
	upgraderCommand << g_MeTOSPrefs->GetString(METOS_UPGRADER_URL,"upgrader-url");
	upgraderCommand << "/";
	upgraderCommand << g_MeTOSPrefs->GetString(METOS_UPGRADER_SCRIPT,"upgrader-script");

	// de meme on va verifier si on trouve une version dans le message
	if(data->FindString(M_UPDATER_VERSION,&upgraderVersion)!=B_OK)
		upgraderVersion = UPGRADER_VERSION;

	// ajout des parametres a la commande
	upgraderFile.Append(" ");
	upgraderFile.Append(upgraderVersion);
	upgraderFile.Append(" ");
	upgraderFile.Append(upgraderCommand);

	// on va utiliser la commande system()
	// pour lancer l'exe externe d'upgrade
	system(upgraderFile.String());
}

/**** afficher l'aide ****/
void MetosApp::LaunchHelp()
{
	BEntry		entry;
	BFile		file;
	app_info	info;
	BPath		urlPath;
	BString		url;
	char		*buffer;

	// recuperer le chemin de l'application	
	GetAppInfo(&info);
	file.SetTo(&(info.ref), B_READ_ONLY);
	urlPath.SetTo(&(info.ref)); 
	urlPath.GetParent(&urlPath);

	// construire l'url
	url = "file://";
	url << urlPath.Path();
	url << "/";
	url << METOS_DOCUMENTATIONS_FOLDER;
	url << "/";

	// en prenant le bon language
	url << g_MeTOSPrefs->GetString("English",METOS_LANGUAGES_TYPE);
	url << "/index.html";

	// on recupere la chaine du fichier a lancer
	buffer = url.LockBuffer(0);
	be_roster->Launch("text/html", 1, &buffer);
	url.UnlockBuffer();
}

/**** lancer la page web PayPal ****/
void MetosApp::SupportMeTOS()
{
	BString		url;
	char		*buffer;

	// url du lien paypal
	url = "https://www.paypal.com/xclick/business=cedric-achats-online%40orange.fr&item_name=Don+for+my+BeOS+developpement...&item_number=DonBeOSCKJ&no_note=1&tax=0&currency_code=EUR";

	// on recupere la chaine du fichier a lancer
	buffer = url.LockBuffer(0);
	be_roster->Launch("text/html", 1, &buffer);
	url.UnlockBuffer();
}

/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
/**********************/
/* fenetre des outils */
/**********************/
#include "MetosApp.h"
#include "GUIManager.h"
#include "ToolManagerWin.h"
#include "ToolManagerView.h"
#include "MetosSplashScreen.h"
#include "MetosMessageDefs.h"
#include "MetosGlobalDefs.h"
#include "VPreferenceFile.h"
#include "VTool.h"
#include "VObject.h"
#include "ObjectManagerWin.h"
#include "MetosLanguagesDefs.h"
#include "VLanguage.h"

#include <app/Roster.h>
#include <app/Message.h>
#include <support/String.h>
#include <storage/Directory.h>
#include <storage/Path.h>
#include <support/List.h>
#include <app/Messenger.h>

/**** constructeur ****/
ToolManagerWin::ToolManagerWin(BRect frame)
: BWindow(frame,g_Languages->GetTranslationFor(L_TOOLS_TITLE), B_FLOATING_WINDOW, B_NOT_ZOOMABLE | B_NOT_RESIZABLE | B_WILL_ACCEPT_FIRST_CLICK)
{
	BRect		bounds = Bounds();

	// la liste des tools
	// le nombre par ligne
	_toolsList = new BList();
	_nbPerLine = 2;

	// construire la vue principale
	_toolsManagerView = new ToolManagerView(bounds,"Tools-Manager-view");
	_toolsManagerView->SetViewColor(VIEW_COLOR);
	AddChild(_toolsManagerView);
}

/**** destructeur ****/
ToolManagerWin::~ToolManagerWin()
{
	// sauver les preferences
	SavePreferences();
	
	// liberer la memeoire
	UnloadAddOns();

	// liberer la memoire
	delete _toolsList;
}

/**** gerer les messages ****/
void ToolManagerWin::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
	// on veux la liste des outils
	case M_TOOLS_NAME_LIST_MSG:
		BuildToolsNameList(message);
		break;
	// appel d'un outils de l'exterieur
	case M_TOOLS_CALL_MSG:
		CallTools(message);
		break;
	// preferences
	case M_PREFERENCE_MSG:
		RefreshPreferences();
		break;
	default:
		BWindow::MessageReceived(message);
	}
}

/**** fermer la fenetre ****/
bool ToolManagerWin::QuitRequested()
{
	// quitter
	if(g_DoQuit)
		return (true);

	return	(false);
}

// ======================
// = Gestion des Addons =
// ======================

/**** charger les addons ****/
void ToolManagerWin::LoadAddOns()
{
   	VTool			*tools = NULL; 
	VTool			*(*instantiate_VTool)(image_id addonId,BEntry *entry);
	image_id		addonId;
   	status_t 		err = B_NO_ERROR;
	app_info 		info; 
    BPath 			path;
	BString			message;	
	int32			nbFiles;
	int32			toolsNumber;
	BMessage		splashInfo(M_PROGRESSSTART_MSG);
	BMessage		splashProgress(M_PROGRESSINCREASE_MSG);
	BEntry			entry;
	
	// initialiser le compteur du splash-screen
	nbFiles = g_MetosApp->CountAddOnFile(ADDON_TOOLS_FOLDER);
	splashInfo.AddInt32(M_TOTALPROGRESS,nbFiles);
	splashInfo.AddString(M_TEXTDISPLAY,"Load Tools AddOns...");
	BMessenger(g_MetosApp->_splashScreen).SendMessage(&splashInfo);

   	// trouver le chemin des addons
    path = g_MetosApp->GetAddonsPath();
	path.Append(ADDON_TOOLS_FOLDER);
	BDirectory dir(path.Path());

   	//load all effects
	toolsNumber = 0;
	while( err == B_NO_ERROR )
	{
		err = dir.GetNextEntry((BEntry*)&entry,false);			
		if( entry.InitCheck() != B_NO_ERROR )
			break;
		
		if( entry.GetPath(&path) == B_NO_ERROR )
		{
			addonId = load_add_on(path.Path());
			if(addonId>=0)
			{
				// on regarde si on arrive a trouver la metode de creation de l'objet
				if(get_image_symbol(addonId,"instantiate_VTool", B_SYMBOL_TYPE_TEXT,(void **)&instantiate_VTool) == B_OK)
				{
					// on va creer un objet et le garder de coté
					// on va ensuite l'ajouter puis l'enlever en fonction du besoin
					// et l'effacer definitivement quand on quitte l'application
					tools = (*instantiate_VTool)(addonId,&entry);
					_toolsList->AddItem(tools);

					// ajouter graphiquement l'objet
					if(Lock())
					{
						_toolsManagerView->AddChild(tools);
						Unlock();
					}
					else
						g_MetosApp->SendDebbug("Error : Lock() in ToolManagerWin::LoadAddOns()",VDEBUG_ERROR);
					
					// informer le splash-screen
					BMessenger(g_MetosApp->_splashScreen).SendMessage(&splashProgress);

					// informer que tout c'est bien passé
					message = "Tools : ";	
					message << path.Path() << " Loaded";
					g_MetosApp->SendDebbug(message.String(),VDEBUG_DEBUG);					
				}
				else
					unload_add_on(addonId);
			}
		}
	}
	// informer que tout c'est bien passé
	g_MetosApp->SendDebbug("Tools Addons Loading finish",VDEBUG_INFO);
	
	// charger les preferences
	// pour placer ensuite les outils
	RefreshPreferences();
}

/**** fonction pour decharger les addons ****/
void ToolManagerWin::UnloadAddOns()
{
	// liberer la memoire prise par les items
	VTool		*tools = NULL;
	image_id	image;
	int32		nbItem;
	
	nbItem = _toolsList->CountItems() - 1;
	for(int32 index = nbItem;index>=0;index--)
	{
		tools = (VTool *)(_toolsList->RemoveItem(index));
		image = tools->AddonId();				
		tools->RemoveSelf();
		if(unload_add_on(image)!=B_OK)
			printf("Error unloading Tools Addon\n");
	}
}

// ======================
// = Gestion des outils =
// ======================

// creer la liste des noms des outils
void ToolManagerWin::BuildToolsNameList(BMessage *reply)
{
	BMessage	toolsNameList(M_TOOLS_NAME_LIST_MSG);
	VTool		*tool = NULL;
	int32		nbTools;
	
	// on doit trouver l'outils
	nbTools = _toolsList->CountItems();
	for(int32 index=0;index<nbTools;index++)
	{
		tool = (VTool *)(_toolsList->ItemAt(index));
		if(tool!=NULL)
			toolsNameList.AddString(M_TOOLS_NAME,tool->Name());
	}

	// renvoyer la liste a son demandeur
	reply->SendReply(&toolsNameList);
}

/**** positionner l'outils ****/
status_t ToolManagerWin::PlaceTools(int32 toolIndex)
{
	// les tool (bouton) on une dimension de 38/38
	VTool	*tool = NULL;
	int32	x;
	int32	y;
	
	tool = (VTool *)(_toolsList->ItemAt(toolIndex));
	if(tool==NULL)
		return B_ERROR;
	
	// calcul de la position de l'outil
	y = (toolIndex / _nbPerLine) * TOOL_SISE + FRAME_SIZE + 1;
	x = (toolIndex % _nbPerLine) * TOOL_SISE + FRAME_SIZE + 1;
	if(tool->LockLooper())
	{
		// replacer le bouton au bon endroit
		tool->MoveTo(x,y);
		tool->UnlockLooper();
	
		return B_OK;
	}
	else
		g_MetosApp->SendDebbug("Error : LockLooper() in ToolManagerWin::PlaceTools()",VDEBUG_ERROR);

	return B_ERROR;
}

/**** appeler un outil directement ****/
void ToolManagerWin::CallTools(BMessage *message)
{
	int32	index = -1;
	VTool	*tools = NULL;
	
	// le message doit etre valide
	if(message==NULL)
		return;
	
	// trouver l'index
	if(message->FindInt32(M_TOOLS_INDEX,&index)!=B_OK)
		return;
		
	// on doit trouver l'outils
	tools = (VTool *)(_toolsList->ItemAt(index));
	if(tools==NULL)
		return;
	
	// ok tout va bien on selectionne l'outil	
	tools->DoAction(NULL);
}


// ===========================
// = Gestion des preferences =
// ===========================

/**** rafraichir les preferences ****/
void ToolManagerWin::RefreshPreferences()
{
	VTool				*tool = NULL;
	int32				toolsNumber;
	int32				x_step;
	int32				y_step;
	
	// charger les preferences
	_nbPerLine = g_MeTOSPrefs->GetInt32(2,"tools-per-line");

	// nombre d'outils
	toolsNumber = _toolsList->CountItems();

	// redimentionner la fenetre en fonction du nombre d'addons
	x_step = _nbPerLine * TOOL_SISE + FRAME_SIZE * 2;
	y_step = (((toolsNumber - 1) / _nbPerLine) + 1) * TOOL_SISE + FRAME_SIZE * 2;
	ResizeTo(x_step,y_step);
	for(int32 index = 0;index<toolsNumber;index++)
	{
		PlaceTools(index);
		tool = (VTool *)(_toolsList->ItemAt(index));
		if(tool!=NULL)
			tool->RefreshPreferences();
	}
		
	// si on a des outils on rafraichit l'affichage
	if(toolsNumber>0 && _toolsManagerView->LockLooper())
	{
		_toolsManagerView->Invalidate();
		_toolsManagerView->UnlockLooper();
	}
	else
		g_MetosApp->SendDebbug("Error : LockLooper() in ToolManagerWin::RefreshPreferences()",VDEBUG_ERROR);
}

/**** sauver les preferences ****/
void ToolManagerWin::SavePreferences()
{
	// charger les preferences
	g_MeTOSPrefs->Load();
	g_MeTOSPrefs->SetRect(Frame(),"toolsManager-rect");
	g_MeTOSPrefs->Save();
}

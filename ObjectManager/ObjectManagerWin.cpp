/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
/************************************************/
/* Fenetre qui contient le projet et les objets */
/************************************************/
#include "MetosApp.h"
#include "MetosWin.h"
#include "AddOnManagerWin.h"
#include "ObjectAddOnItem.h"
#include "ObjectManagerWin.h"
#include "ObjectManagerListView.h"
#include "ObjectManagerItem.h"
#include "PropertieManagerWin.h"
#include "ToolManagerWin.h"
#include "MetosUndoDefs.h"
#include "UndoManagerWin.h"
#include "VTool.h"
#include "VPreferenceFile.h"
#include "MetosSplashScreen.h"
#include "GUIManager.h"
#include "VObject.h"
#include "MetosMessageDefs.h"
#include "MetosGlobalDefs.h"
#include "MetosLanguagesDefs.h"
#include "CColumnView.h"
#include "CColumnItem.h"
#include "FunctionItem.h"
#include "CStatusView.h"
#include "CStatusItem.h"
#include "VLanguage.h"

#include <app/Application.h>
#include <Clipboard.h>
#include <app/Roster.h>
#include <storage/Path.h>
#include <storage/Directory.h>
#include <interface/View.h>
#include <interface/TabView.h>
#include <interface/PopUpMenu.h>
#include <interface/MenuItem.h>
#include <support/String.h>
#include <app/Messenger.h>
#include <Control.h>
#include <interface/ListView.h>
#include <interface/ScrollView.h>
#include <interface/MenuField.h>
#include <interface/MenuBar.h>

// ==============================================
// = fonctions surchargées de la classe BWindow =
// ==============================================

/**** constructeur de la fenetre MeTOS Application ****/
ObjectManagerWin::ObjectManagerWin(BRect frame)
: BWindow(frame,g_Languages->GetTranslationFor(L_MENU_VIEW_OBJECTS), B_DOCUMENT_WINDOW_LOOK, B_FLOATING_SUBSET_WINDOW_FEEL, B_WILL_ACCEPT_FIRST_CLICK | B_NOT_ZOOMABLE )
{
	BScrollView			*supportScrollView;
	BRect				listRect = Bounds();

	// initialisation
	_options = NULL;
	_toolsMenu = NULL;

	// construire la vue principale
	listRect.bottom -= B_V_SCROLL_BAR_WIDTH;	
	_objectManagerView = new BView(listRect,"ObjectManager-view",B_FOLLOW_ALL_SIDES, B_WILL_DRAW);
	_objectManagerView->SetViewColor(VIEW_COLOR);

	// status
	listRect.right -= B_H_SCROLL_BAR_HEIGHT;
	_statusView = new CStatusView(BRect(listRect.left,listRect.bottom+1,listRect.right,listRect.bottom+B_V_SCROLL_BAR_WIDTH),"Status");
	_statusView->AddItem(new CStatusItem("nb-items",g_Languages->GetTranslationFor(L_OBJECT_OBJECTS),85));
	_statusView->AddItem(new CStatusItem("nb-items-garbage",g_Languages->GetTranslationFor(L_OBJECT_GARBAGE),85));
	AddChild(_statusView);

	listRect.InsetBy(2,2);
	_objectList = new ObjectManagerListView(listRect,"navigation-list");
	supportScrollView = new BScrollView("scroll-navigation",_objectList,B_FOLLOW_ALL_SIDES,B_WILL_DRAW | B_FRAME_EVENTS,false,true);

	// ajouter les colonnes
	_columnView = new CColumnView("column-object-view",g_MetosApp->GetCursor("HeightResize"));
	_columnView->AddItem(new CColumnItem("object-name",g_Languages->GetTranslationFor(L_OBJECT_NAME)),g_MeTOSPrefs->GetInt32(180,"object-manager-column-list",0));
	_columnView->AddItem(new CColumnItem("lock-object",g_Languages->GetTranslationFor(L_OBJECT_LOCK)),g_MeTOSPrefs->GetInt32(16,"object-manager-column-list",1));
	_columnView->AddItem(new CColumnItem("derived-object",g_Languages->GetTranslationFor(L_OBJECT_DERIVED)),g_MeTOSPrefs->GetInt32(130,"object-manager-column-list",2));
	_columnView->AddItem(new CColumnItem("object-id",g_Languages->GetTranslationFor(L_OBJECT_ID)),g_MeTOSPrefs->GetInt32(150,"object-manager-column-list",3));
	supportScrollView->AddChild(_columnView);	
	
	// attacher les vues
	_objectManagerView->AddChild(supportScrollView);
	AddChild(_objectManagerView);

	// gestionnaire des objets graphiques
	g_GUIManagerView = new GUIManager();
	AddChild(g_GUIManagerView);

	// creer le messenger du GUIManager
	g_GUIManager = BMessenger(g_GUIManagerView);	
}

/**** destructeur de la fenetre MeTOS Application ****/
ObjectManagerWin::~ObjectManagerWin()
{
	// sauver les preferences
	SavePreferences();
	
	// effacer reelement les objets "effacés"
	ClearGarbage();

	// effacer la liste
	ClearList();
	
	// decharger les addons
	UnloadAddOns();

	// liberer les pointers des popups
	if(_options!=NULL)
		delete _options;
	if(_toolsMenu!=NULL)
		delete _toolsMenu;
}

/**** gestion des messages ****/
void ObjectManagerWin::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
	// renvoyer le mouvement de la roulette de la souris a la srcoll-view
	case B_MOUSE_WHEEL_CHANGED:
		BMessenger(_objectList).SendMessage(message);
		break;
	// creation d'un nouvel objet
	case M_DROP_ADDON_MSG:
		DropAddon(message);
		break;
	// creation d'un nouveau projet
	case M_NEW_PROJECT_MSG:
		CreateNewProject();
		break;
	case M_SELECT_OBJECT_MSG:
		SelectObject(message);
		break;
	// on recupere le Handler et on renvoi un pointer sur l'objet
	case M_OBJECT_CHANGE_MSG:
		ConvertHandlerToObject(message);
		g_PropertieManager.SendMessage(message);
		UpdateProperties(*message);
		break;
	// on a modifier une proprietes dans la fenetre des proprietes
	case M_PROPERTIES_CHANGE_MSG:
		UpdateProperties(*message);
		break;
	// effacer les objets selectionnés
	case M_DELETE_OBJECT_MSG:
		Delete();
		break;
	// rafraichir la liste des objets
	case M_REFRESH_OBJECTS_MSG:
		_objectList->Invalidate();
		break;
	// restoration de proprietes
	case UNDO_ACTION_MSG:
		RestoreUndo(message);
		break;
	// selection d'un item de fonction
	case M_SELECT_FUNCTION_MSG:
		CheckFunction(message);
		break;
	// construire la list des noms des outils
	// et virer les elements du message d'origine
	case M_TOOLS_NAME_LIST_MSG:
		_toolsNameList = *message;
		_toolsNameList.RemoveData("_previous_");
		break;
	// preferences
	case M_PREFERENCE_MSG:
		RefreshPreferences();
		_objectList->Invalidate();
		break;
	// mise a jour du code externe
	case M_UPDATE_EXTERNAL_CODE_MSG: 
		UpdateExternalCode(message);
		break;
	default:
		BWindow::MessageReceived(message);	
	}
}

/**** vider la liste ****/
void ObjectManagerWin::ClearList()
{
	// les colonnes
	ObjectManagerItem	*item = NULL;
	int32				nbItem;
	int32				index;
	
	nbItem = _statusView->CountItems();
	for(int32 index = 0;index<nbItem;index++)
		delete _statusView->RemoveItem((int32)0);

	nbItem = _columnView->CountItems();
	for(index = 0;index<nbItem;index++)
		delete _columnView->RemoveItem((int32)0);
	
	// liberer la memoire prise par les items
	nbItem = _objectList->CountItems();
	for(index = 0;index<nbItem;index++)
	{
		item = (ObjectManagerItem *)(_objectList->RemoveItem((int32)0));
		delete item->Object();
		delete item;
	}
}

/**** Si on quitte on ferme les fenetre du projet ****/
bool ObjectManagerWin::QuitRequested()
{
	// quitter
	if(g_DoQuit)
		return (true);
	
	// doit-on quitter ou simplement reduire la fenetre
	// si non on reduit la fenetre si ca n'est pas deja fait
	if(!IsHidden())
		Hide();
	
	return	(false);
}

// ==============================
// = Gestion de la garbage list =
// ==============================

/**** vider la liste des objets effacés ****/
void ObjectManagerWin::ClearGarbage()
{
	int32				nbItem;

	// detruire les objet gardé en reserve
	nbItem = _garbageList.CountItems();
	for(int32 index = 0;index<nbItem;index++)
		delete (VObject *)(_garbageList.RemoveItem((int32)0));
}

/**** trouver un objet dans la liste garbage ****/
VObject *ObjectManagerWin::FindObjectInGarbage(const char *addonSource)
{
	VObject				*object = NULL;
	BMessage			*settings = NULL;
	BString				source;
	int32				nbItem;
	int32				index;

	// detruire les objet gardé en reserve
	nbItem = _garbageList.CountItems();
	for(index = 0;index<nbItem;index++)
	{
		// voir la liste des objets effacés
		object = (VObject *)(_garbageList.ItemAt(index));
		if(object!=NULL)
		{
			// rechercher le nom de l'addon
			settings = object->Settings();
			settings->FindString(VOBJECT_ADDON_SOURCE,&source);
			if(source==addonSource)
				break;
		}
		// reinitialiser
		object = NULL;
	}
	
	// afficher les objets en garbage
	if(object==NULL)
		return NULL;

	// enlever l'objet de la liste
	_garbageList.RemoveItem(index);
	RefreshDisplayNumberItems();
	
	return object;
}

// ================================
// = Affichage du nombre d'objets =
// ================================

/**** mettre a jour le nombre d'items affiché ****/
void ObjectManagerWin::RefreshDisplayNumberItems()
{
	BString		buffer;

	// afficher le nombre d'items
	buffer = "Objects : ";
	buffer << _objectList->CountItems();
	(_statusView->ItemAt(0))->SetLabel(buffer.String());
	
	// afficher le nombre d'items
	buffer = "Garbage : ";
	buffer << _garbageList.CountItems();
	(_statusView->ItemAt(1))->SetLabel(buffer.String());

	// rafraichir l'affichage
	if(_statusView->LockLooper())
	{
		_statusView->Invalidate();
		_statusView->UnlockLooper();
	}
	else
		g_MetosApp->SendDebbug("Error : LockLooper() in ObjectManagerWin::RefreshDisplayNumberItems()",VDEBUG_ERROR);
}

// ==========================
// = Gestion du drag & drop =
// ==========================

/**** creer un nouvel objet a partir d'un drag & drop ****/
void ObjectManagerWin::DropAddon(BMessage *message)
{
	VObject		*newObject = NULL;
	
	// si ca vient d'un drag&drop sur une fenetre des fenetres crées
	// trouver l'objet correspondant au handler
	if(ConvertHandlerToObject(message)==B_OK)
		SelectObject(message);

	// creation du nouvel objet
	InstantiateObject(newObject,message);
}

/**** fixer la propriete de positionnement de l'objet en fonction d'un point de drag&drop ****/
void ObjectManagerWin::PlaceNewObjectIntoDropPoint(VObject *newObject,BPoint &where)
{	
	BMessage		*properties = newObject->Properties();
	char			*name;
	type_code		type; 
	int32			count;
	BRect			frame;

	// trouver un Rectangle
	if(properties->GetInfo(B_RECT_TYPE,0,&name, &type, &count)==B_OK)
	{
		// le deplacer
		properties->FindRect(name,&frame);
		frame.OffsetTo(where);
		properties->ReplaceRect(name,frame);
	}
}

// ======================
// = Gestion des objets =
// ======================

/**** trouver un objet a partir de son ID ****/
VObject *ObjectManagerWin::ObjectFromID(uint32 id)
{
	ObjectManagerItem	*current = NULL;
	int32				nbObjects;
	
	// chercher dans la liste
	nbObjects = _objectList->CountItems();
	for(int32 index=0;index<nbObjects;index++)
	{
		current = (ObjectManagerItem *)(_objectList->ItemAt(index));
		if(current!=NULL && current->Object()->Id()==id)
			return current->Object();
	}

	// on a pas trouvé
	return NULL;
}

/**** ajout de l'objet au parent ****/
status_t ObjectManagerWin::AddNewObject(VObject *newObject)
{
	// attention l'objet source ne doit pas etre NULL
	if(newObject==NULL)
	{
		g_MetosApp->SendDebbug("New Object with NULL pointer",VDEBUG_ERROR);
		return B_ERROR;
	}
	
	// recuperer le type d'action
	VObject			*destinationObject = NULL;
	BMessage		*properties = newObject->Properties();
	uint32			fatherId = 0;
	int32			level = 0;
	
	// l'objet destination est le courant
	// si on en trouve pas deja un
	destinationObject = ObjectFromID(_selectedObject);
	properties->FindInt32(VOBJECT_FATHER_ID,(int32 *)&fatherId);
	if(fatherId!=0)
		destinationObject = _objectList->FindObjectByID(fatherId);
	
	// ajouter l'objet graphiquement
	AddObjectMethode(newObject,destinationObject);
	
	// augmenter le level
	// par rapport a celui qui est selectionné dans les properties
	// sauf un projet qui est de toute maniere en level 0
	if(destinationObject!=NULL)
		level = destinationObject->Level() +1;
	
	// renseigner a notre objet qui est son parent
	newObject->SetParentId(destinationObject);

	// ajouter l'objet a la liste
	_objectList->AddVObject(newObject,destinationObject,level);

	// afficher le nombre d'objet
	RefreshDisplayNumberItems();

	return B_OK;
}

/**** metode d'ajout ****/
void ObjectManagerWin::AddObjectMethode(VObject *newObject,VObject *destObject)
{
	BView	*destination = NULL;
	bool	locked = false;

	if(destObject==NULL)
	{
		if(newObject->Type()!=PROJECT_TYPE)
			g_MetosApp->SendDebbug("Erreur no destination object !",VDEBUG_ERROR);
		
		// on quitte tout de meme
		return ;
	}

	// recuperer la vue destination
	switch(destObject->Type())
	{
	case PROJECT_TYPE:
	case APPLICATION_TYPE:
		break;
	case WINDOW_TYPE:
		{
			// si la destination est une fenetre !!!
			if(((BWindow *)(destObject->Object()))->Lock())
			{
				destination = ((BWindow *)(destObject->Object()))->ChildAt(0);
				((BWindow *)(destObject->Object()))->Unlock();
			}
			else
				g_MetosApp->SendDebbug("Error : Lock() in ObjectManagerWin::AddObjectMethode()",VDEBUG_ERROR);
		}			
		break;
	case SCROLLVIEW_TYPE:
		{
			// la destination est une scrollView
			if(((BView *)(destObject->Object()))->LockLooper())
			{
				destination = ((BView *)(destObject->Object()))->ChildAt(0);
				((BView *)(destObject->Object()))->UnlockLooper();
			}
			else
				g_MetosApp->SendDebbug("Error : LockLooper() in ObjectManagerWin::AddObjectMethode()",VDEBUG_ERROR);
		}
		break;
	default:
		destination = dynamic_cast<BView *>(destObject->Object());
		if(destination==NULL)
			g_MetosApp->SendDebbug("Destination object not finded !",VDEBUG_ERROR);		
	}


	// ajouter l'objet
	switch(newObject->Type())
	{
	case PROJECT_TYPE:
	case APPLICATION_TYPE:
		break;
	case UNDEFINED_TYPE:
		g_MetosApp->SendDebbug("Object type not define !",VDEBUG_ERROR);
		break;
	case WINDOW_TYPE:
		{
			// ok rend visible la fenetre
			// mais on la rend fille de la fenetre principale de l'application
			((BWindow *)(newObject->_object))->SetFeel(B_FLOATING_SUBSET_WINDOW_FEEL);
			((BWindow *)(newObject->_object))->AddToSubset(g_MetosBackWin);
			((BWindow *)(newObject->_object))->Show();
			g_MetosApp->SendDebbug("Show an object based on BWindow Class",VDEBUG_DEBUG);
		}
		break;
	case MENU_TYPE:
		{
			// si la destination est un objet qui accepte les menus items
			if((locked = destination->LockLooper())==true)
			{
				((BMenu *)destObject->Object())->AddItem((BMenu *)(newObject->Object()));
			
				// si le looper est bloqué on le libere
				if(locked)
					destination->UnlockLooper();
			}
			else
				g_MetosApp->SendDebbug("Error : LockLooper() in ObjectManagerWin::AddObjectMethode()",VDEBUG_ERROR);
		}
		break;
	case POPUPMENU_TYPE:
		{
			// si la destination est un objet qui accepte les menus items
			if(destination->LockLooper() && destObject->Type()==MENUFIELD_TYPE)
			{
				((BMenuField *)destObject->Object())->MenuBar()->AddItem((BPopUpMenu *)(newObject->Object()),0);
				delete ((BMenuField *)destObject->Object())->MenuBar()->RemoveItem((int32)1);
				destination->UnlockLooper();
			}
			else
				g_MetosApp->SendDebbug("Error : LockLooper() in ObjectManagerWin::AddObjectMethode()",VDEBUG_ERROR);
		}
		break;
	case MENUITEM_TYPE:
		{
			// les item de menu doivent avoir un parent qui est un menu ou un popupmenu
			if(destObject->Type()==MENU_TYPE || destObject->Type()==POPUPMENU_TYPE)
				((BMenu *)destObject->Object())->AddItem((BMenuItem *)(newObject->Object()));
		}
		break;
	case LISTITEM_TYPE:
	case BITMAP_TYPE:
	case SHELF_TYPE:
		break;
	default:
		{
			// on va s'assuerer que l'on a une vue
			if(dynamic_cast<BView *>(newObject->Object())==NULL)
				g_MetosApp->SendDebbug("Destination object not BView based !",VDEBUG_ERROR);		

			// si la destination est une TabView on doit creer un onglet
			if(destination->LockLooper())
			{
				if(destObject->Type()==TABVIEW_TYPE)
				{
					// dans le cas d'une tabview on va ajouter l'onglet avant !
					((BTabView *)destObject->Object())->AddTab((BView *)(newObject->Object()));
					((BTabView *)destObject->Object())->Select(((BTabView *)destObject->Object())->CountTabs()-1);
				}
				else
					// ok on ajoute la nouvelle vue
					destination->AddChild((BView *)(newObject->Object()));
				
				// debloquer le looper
				destination->Window()->Unlock();
			}
			else
				g_MetosApp->SendDebbug("Error : LockLooper() in ObjectManagerWin::AddObjectMethode()",VDEBUG_ERROR);
			
			// info
			g_MetosApp->SendDebbug("Added an object based on BView Class (default case used)",VDEBUG_DEBUG);
		}
	}
}

/**** effacer tous les objets ****/
status_t ObjectManagerWin::DeleteAll()
{
	// on doit avoir au moins plus qu'un projet
	if(_objectList->CountItems()<1)
		return B_ERROR;

	ObjectManagerItem	*item = NULL;
	status_t			state = B_OK;

	// selectionner le projet puis tout effacer
	item = (ObjectManagerItem *)(_objectList->ItemAt(0));
	if(item!=NULL)
	{
		// selectionner le projet
		_objectList->Select(0);
		
		// puis l'effacer
		state &= Delete();
	}
	
	// retourner le resultat
	return state;
}

/**** effacer les objets selectionnes ****/
status_t ObjectManagerWin::Delete()
{
	ObjectManagerItem	*current = NULL;
	int32				index;
	status_t			result = B_OK;
	
	// effacer les proprietes, pour ca on envoi un message de selection
	// vide, ca a le meme effet que d'annuler la selection
	// prevenir aussi la fenetre des addons
	g_PropertieManager.SendMessage(M_DISPLAY_PROPERTIES_MSG);
	g_AddOnManager.SendMessage(M_SELECT_OBJECT_MSG);

	// vider la liste de selection graphique
	if(g_GUIManagerView->LockLooper())
	{
		g_GUIManagerView->RemoveAll();	
		g_GUIManagerView->UnlockLooper();
	}
	else
		g_MetosApp->SendDebbug("Error : LockLooper() in ObjectManagerWin::Delete()",VDEBUG_ERROR);

	index = 0;
	while(index < _objectList->CountItems())
	{
		// l'item courant est-il selectionné
		// si on l'efface on passe au suivant
		current = (ObjectManagerItem *)(_objectList->ItemAt(index));
		if(current!=NULL && current->IsSelected())
			result &= DeleteObject(current->Object());
		else
			index++;
	}
	// afficher le nombre d'objet
	RefreshDisplayNumberItems();

	return result;	
}

/**** effacer un objet graphique****/
status_t ObjectManagerWin::DeleteObject(VObject *object,bool needUndo)
{
	// l'objet ne doit pas etre null
	if(object==NULL)
		return B_ERROR;

	// cette fonction detruit l'objet graphique du VObject
	// selon son type mais pas le VObject, ceci est fait
	// quand on detruit l'item dans la liste des objets
	int32		indexOfObject = -1;
	int32		indexLastDeleted = -1;
	
	// on doit trouver l'index de l'objet
	indexOfObject = _objectList->FindIndexOfObject(object);
	if(indexOfObject == -1)
		return B_ERROR;
	
	// on doit trouver l'index des objets fils a effacer
	// cela peux nous etre donné par la fonction FindPlaceForNewObject()
	indexLastDeleted = _objectList->FindPlaceForNewObject(object);
	if(indexLastDeleted < 0)
		return B_ERROR;
		
	// envoyer un message de Undo et garder l'objet pour l'effacer plus tard
	ObjectManagerItem	*currentItem = NULL;
	VObject				*current = NULL;
	BMessage			undo(UNDO_ACTION_MSG);
	BMessage			*settings = NULL;
	BString				addonSource;
	int32				index;

	// detruire les items MeTOS
	indexLastDeleted--;
	for(int32 i=indexLastDeleted;i>=indexOfObject;i--)
	{
		// il faut proteger l'effacement de la liste
		// car sa touche l'affichage
		if(Lock())
		{
			currentItem = (ObjectManagerItem *)(_objectList->RemoveItem(i));		
			Unlock();
		}
		else
			g_MetosApp->SendDebbug("Error : Lock() in ObjectManagerWin::DeleteObject()",VDEBUG_ERROR);		
		
		// recuperer l'objet courant
		current = currentItem->Object();

		// effacer l'objet graphique
		switch(current->Type())
		{
		case UNDEFINED_TYPE:
			g_MetosApp->SendDebbug("Object type not define !",VDEBUG_ERROR);
			break;
		case PROJECT_TYPE:
			// on efface pas un projet
			break;
		case APPLICATION_TYPE:
		case MENU_TYPE:
		case POPUPMENU_TYPE:
		case MENUITEM_TYPE:
		case LISTITEM_TYPE:
		case BITMAP_TYPE:
		case SHELF_TYPE:
		case OTHER_TYPE:
			break;
		case WINDOW_TYPE:
			// cache la fenetre
			if(((BWindow *)(current->Object()))->Lock())
				((BWindow *)(current->Object()))->Quit();
			else
				g_MetosApp->SendDebbug("Error : Lock() in ObjectManagerWin::DeleteObject()",VDEBUG_ERROR);
			break;
		default:
			{
				// ca doit etre le premier
				if(current!=object)
					break;
				
				BWindow		*window = NULL;
				BView		*view = NULL;
				VObject		*parent = NULL;
						
				// trouver l'objet graphique
				view = dynamic_cast<BView *>(current->Object());
				if(view==NULL)
					break;
				
				// locker le looper avant !
				window = view->Window();
				if(window->Lock())
				{
					// pour une vue ou un controle c'est la meme chose
					// il faut faire attention tout de meme si le parent est
					// une tabview, on doit alors detruire un onglet
					parent = _objectList->FindObjectByID(current->ParentId());
					if(parent!=NULL && parent->Type()==TABVIEW_TYPE)
					{
						BTab	*tab = NULL;
						int32	selected = -1;
				
						// effacer l'onglet
						selected = ((BTabView *)(parent->Object()))->Selection();
						((BTabView *)(parent->Object()))->RemoveTab(selected);
						delete tab;
					}
					else
						view->RemoveSelf();

					// delocker le looper
					window->Unlock();
				}
				else
					g_MetosApp->SendDebbug("Error : Lock() in ObjectManagerWin::DeleteObject()",VDEBUG_ERROR);
			}
		}

		// annuler le pointer
		current->ResetObject();

		// recuperer des infos
		settings = current->Settings();
		settings->FindString(VOBJECT_ADDON_SOURCE,&addonSource);

		// gerer en interne dans la liste des Addons
		index = FindItemFromName(addonSource.String());
		if(index>=0)
		{
			ObjectAddOnItem		*addonObject = NULL;
			
			// ca permet ici de restaurer l'accesibilité a un objet (un projet par exemple)
			addonObject = (ObjectAddOnItem *)(_addOnList.ItemAt(index));
			if(addonObject!=NULL)
				addonObject->DeleteObject();
		}

		// si on a besoin de generer un undo
		// c'est pas le cas du new project
		if(needUndo)
		{
			// recuperer les donnees pour le undo
			undo.AddInt32(VOBJECT_ID,current->Id());
			undo.AddString(VOBJECT_ADDON_SOURCE,addonSource);
			undo.AddMessage(VOBJECT_PROPERTIES,current->Properties());
			undo.AddInt32(UNDO_ACTION_TYPE,UNDO_DELETE_OBJECT);
			g_UndoManager.SendMessage(&undo);

			// vider le message pour le suivant
			undo.MakeEmpty();
		}
		
		// virer l'objet
		_garbageList.AddItem(current);		

		// detruire l'item
		delete currentItem;		
	}
	
	return B_OK;
}

/**** creer un nouveau projet ****/
status_t ObjectManagerWin::CreateNewProject()
{
	// on doit avoir au moins plus qu'un projet
	if(_objectList->CountItems()<1)
		return B_ERROR;

	ObjectManagerItem	*item = NULL;
	BString				addonSource;
	BMessage			*settings = NULL;
	VObject				*project = NULL;
	status_t			state = B_OK;

	// selectionner le projet puis l'effacer
	item = (ObjectManagerItem *)(_objectList->ItemAt(0));
	if(item!=NULL)
	{
		// selectionner le projet
		_objectList->Select(0);
		
		// recuperer le projet
		project = item->Object();
		if(project==NULL)
			return B_ERROR;

		// verifier que c'est bien un projet
		if(project->Type()!=PROJECT_TYPE)
			return B_ERROR;
			
		// proprietes
		settings = project->Settings();
		if(settings==NULL)
			return B_ERROR;
			
		// recuperer l'addon source de cet objet
		if(settings->FindString(VOBJECT_ADDON_SOURCE,&addonSource)!=B_OK)
			return B_ERROR; 	

		
		// puis l'effacer
		state &= Delete();
	}
	
	// verifier que tout c'est bien passé
	if(state!=B_OK)
		return state;
	
	// on se sert d'un mesage de drag&drop pour recreer le projet	
	BMessage			dragMessage(M_DROP_ADDON_MSG);

	dragMessage.AddString(VOBJECT_ADDON_SOURCE,addonSource);
	state &= InstantiateObject(project,&dragMessage);

	// on doit effacer tous les undos du projet precedent
	// on protege la fenetre des Undos
	if(g_MetosApp->_undoManagerWin->Lock())
	{
		g_MetosApp->_undoManagerWin->ClearList();
		g_MetosApp->_undoManagerWin->Unlock();
	}
	else
		g_MetosApp->SendDebbug("Error : Lock() in ObjectManagerWin::CreateNewProject()",VDEBUG_ERROR);
	
	return state;
}

/**** fixer le nom d'un nouvel objet ****/
void ObjectManagerWin::SetObjectName(VObject *object,int32 index)
{
	ObjectAddOnItem		*currentItem = NULL;
	BMessage			*properties = NULL;
	BMessage			*settings = NULL;
	int32				nbObject;
	BString				newName;
	bool				beObject;

	if(index<0)
		return ;	// pas d'item selectionné
		
	// on va creer l'objet en fonction de l'item selectionné dans la liste
	currentItem = (ObjectAddOnItem *)(_addOnList.ItemAt(index));
	nbObject = currentItem->NbCreatedObject();
	newName = object->ClassName();

	// si c'est un objet basé sur un objet Be on vire le B !!!
	settings = object->Settings();
	if(settings->FindBool(VOBJECT_BE_OBJECT,&beObject)==B_OK && beObject==true)
		newName.Remove(0,1);
	// on ajoute le chiffre
	newName << nbObject;
	
	// remplacer le nom
	properties = object->Properties();
	properties->ReplaceString(VOBJECT_NAME,newName.String());
}

// ======================
// = Selection d'objets =
// ======================

/**** selection d'un objet ****/
void ObjectManagerWin::SelectObject(BMessage *datas)
{
	BMessage	display(M_DISPLAY_PROPERTIES_MSG);
	BMessage	GUIDisplay(M_SELECT_OBJECT_MSG);
	BMessage	AddOnManager(M_SELECT_OBJECT_MSG);
	BMessage	replyMessage(M_REPLY_MSG);
	uint32	 	selectedObjectId = (uint32)NULL;
	VObject 	*selectedObject = NULL;
	BHandler 	*selectedHandler = NULL;
	bool		shift;
	int32		buttons;
	BPoint		where;

	// pas d'objet selectionné
	_selectedObject = (uint32)NULL;

	// fonction appeler apres un clique sur un objet ou quand on clique dans la liste
	// dans les 2 cas on connais l'objet et si on en selectionne plusieurs (avec shift)
	// et on doit absolument savoir ce que c'est un handler ou directement un VObject
	if(datas->FindInt32(M_OBJECT_SELECTED,(int32 *)&selectedObjectId)==B_OK)
		selectedObject = ObjectFromID(selectedObjectId);
	else
	{
		// si c'est un handler on prend le pointer et on le cherche dans la liste
		if(datas->FindPointer(M_HANDLER_SELECTED,(void **)&selectedHandler)==B_OK)
			selectedObject = _objectList->FindObjectFromHandler(selectedHandler);
	}

	// on reprend tout de meme l'etat du bouton pour les proprietes speciales et le point de clique
	// pas d'action si on a un clique droit
	datas->FindInt32(M_MOUSE_BUTTON,&buttons);
	datas->FindPoint(M_POINT_SELECTED,&where);

	// selection multiple
	if(datas->FindBool(M_SHIFT_SELECTION,&shift)!=B_OK)
		shift = false;
		
	// selectionner l'objet dans la liste
	_objectList->SelectObject(selectedObject,shift);

	if(selectedObject!=NULL && !(selectedObject->IsAttached()))
		return;
	
	// afficher les proprietes
	if(selectedObject==NULL)
	{		
		g_GUIManager.SendMessage(&GUIDisplay);
		g_PropertieManager.SendMessage(&display,&replyMessage);
		g_AddOnManager.SendMessage(&AddOnManager);

		return;
	}

	// id de l'objet courant
	_selectedObject = selectedObject->Id();
	
	// informer le GUI Manager
	GUIDisplay.AddBool(M_SHIFT_SELECTION,shift);
	GUIDisplay.AddPointer(M_OBJECT_SELECTED,selectedObject);
	g_GUIManager.SendMessage(&GUIDisplay);

	// informer le Properties Manager
	// on va attendre la reponse avant tout de meme
	display.AddPointer(M_OBJECT_SELECTED,selectedObject);
	g_PropertieManager.SendMessage(&display,&replyMessage);

	// le AddOnManager
	AddOnManager.AddInt32(M_OBJECT_SELECTED,_selectedObject);
	AddOnManager.AddInt32(VOBJECT_TYPE,selectedObject->Type());
	g_AddOnManager.SendMessage(&AddOnManager);
	
	// menu special
	if(buttons==2)
		DisplaySpecialPropertiesMenu(selectedObject,where);
	else
	{
		// definir l'action du GUI Manager
		BMessage	action(M_SET_ACTION_MSG);
		
		action.AddPoint(M_POINT_SELECTED,where);
		g_GUIManager.SendMessage(&action);		
	}
}

// ============================
// = Obtention d'informations =
// ============================

/**** convertir un pointer VObject a partir d'un Handler ****/
status_t ObjectManagerWin::ConvertHandlerToObject(BMessage *message)
{
	int32	id = 0;

	// verifier que l'on ne transmet pas deja l'ID
	if(message->FindInt32(M_OBJECT_SELECTED,&id)==B_OK)
		return B_OK;
	
	BHandler	*handler = NULL;
	VObject		*object = NULL;
	
	// trouver le handler
	if(message->FindPointer(M_HANDLER_SELECTED,(void **)&handler)!=B_OK)
		return B_ERROR;

	// trouver le VObject correspondant
	object = _objectList->FindObjectFromHandler(handler);
	if(object==NULL)
		return B_ERROR;

	// remplacer la donnée dans le message
	message->RemoveName(M_HANDLER_SELECTED);
	message->AddInt32(M_OBJECT_SELECTED,object->Id());

	// ok on a tout fait
	return B_OK;
}

/**** recuperer le premier enfant d'un objet ****/
VObject *ObjectManagerWin::ChildFrom(VObject *father)
{
	int32		objectIndex;
	
	objectIndex = _objectList->FindIndexOfObject(father);
	if(objectIndex!=-1 && (objectIndex+1)<_objectList->CountItems())
		return ((ObjectManagerItem *)(_objectList->ItemAt(objectIndex+1)))->Object();
	
	// pas trouvé
	return NULL;
}

// ==========================
// = gestion des proprietes =
// ==========================

/**** afficher le menu des proprietes speciales ****/
void ObjectManagerWin::DisplaySpecialPropertiesMenu(VObject *selected,BPoint &where)
{
	BMenu				*actionMenu = NULL;

	// l'objet doit exister
	if(selected==NULL)
		return;

	// construire le menu action
	if(_options==NULL)
		BuildOptionsMenu();

	// bloquer ou pas ?
	SetLockMenu(selected);

	// ajouter le menu outils
	BuildToolsMenu();

	// verifier qu'il existe un menu action
	// l'ajouter avec un separateur
	actionMenu = selected->BuildActionMenu();
	if(actionMenu!=NULL)
		_options->AddItem(actionMenu);

	// ok on affiche le menu et on traite l'action
	_options->SetTargetForItems(this);
	BMenuItem *selectedItem = _options->Go(where);
	if(selectedItem)
	{
		BString	itemLabel;
		int32	selectedIndex;
		
		itemLabel = selectedItem->Label();
		selectedIndex = _options->IndexOf(selectedItem);
		// si on a B_ERROR c'est que cela peux etre un sous menu
		if(selectedIndex==B_ERROR)
		{
			// doit trouver lequel alors
			// le menu action speciale
			if(actionMenu!=NULL)
			{
				selectedIndex = actionMenu->IndexOf(selectedItem);
				if(selectedIndex!=B_ERROR)
					selected->ExecuteAction(itemLabel,selectedIndex);
			}
			
			// le menu tools
			if(_toolsMenu!=NULL)
			{
				selectedIndex = _toolsMenu->IndexOf(selectedItem);
				if(selectedIndex!=B_ERROR)
					SelectToolsFromMenu(selectedIndex);
			}
		}
		else
		{
			// action du menu standard
			switch(selectedIndex)
			{
			case 0:
				LockObject(selected);
				break;
			case 2:
				CutSelectedObject();
				break;
			case 3:
				CopySelectedObject();
				break;
			case 4:
				PasteSelectedObject(where);
				break;
			default:
				; // rien
			}
		}
	}

	// enlever les sous menu
	// c'est l'addon qui libere le pointer
	while(_options->CountItems()>5)
		_options->RemoveItem(5);
}

/**** mettre a jour les proprietes de l'objet ****/
void ObjectManagerWin::UpdateProperties(BMessage message)
{
	uint32		objectId = (uint32)NULL;
	VObject		*object = NULL;
	bool		needRefresh = false;	
	bool		needUndo = false;
	bool		needInvalidate = false;
	int32		index = 0;

	// trouver l'objet
	message.FindInt32(M_OBJECT_SELECTED,(int32 *)&objectId);
	if(objectId==(uint32)NULL)
		return;

	// trouver l'objet
	object = ObjectFromID(objectId);
	if(object==NULL)
		return;

	// trouvé le reste des donnees
	message.FindBool(M_NEED_REFRESH,&needRefresh);
	message.FindInt32(M_PROPERTIES_INDEX,&index);
	
	// as t-on demander un Undo
	if(message.FindBool(M_NEED_UNDO_ACTION,&needUndo)!=B_OK)
		needUndo = false;

	// doit-on reafficher
	if(message.FindBool(M_NEED_INVALIDATE,&needInvalidate)!=B_OK)
		needInvalidate = false;
	
	// l'enlever du message certaine donnee pour eviter d'essayer de le recopier
	message.RemoveData(M_OBJECT_SELECTED);
	message.RemoveData(M_PROPERTIES_INDEX);
	message.RemoveData(M_NEED_REFRESH);
	message.RemoveName(M_NEED_INVALIDATE);
	message.RemoveData(M_NEED_UNDO_ACTION);

	// gerer le Undo si il le faut !
	if(needUndo)
	{
		// garder la trace de la modification pour le undo
		BMessage		undo(UNDO_ACTION_MSG);
		BMessage		*properties = NULL;
		char			*name;
		const void		*buffer;
		type_code		type; 
		int32			count;
		ssize_t			size; 

		// recuperer la valeure actuelle de la donnée avant de la modifier
		properties = object->Properties();
		message.GetInfo(B_ANY_TYPE,0,&name,&type,&count);
		if(properties->FindData(name,type,index,&buffer,&size)==B_OK)
		{			
			undo.AddData(name,type,buffer,size);
			undo.AddInt32(VOBJECT_ID,object->Id());
			undo.AddInt32(M_PROPERTIES_INDEX,index);
			undo.AddInt32(UNDO_ACTION_TYPE,UNDO_CHANGE_PROPERTIES_OBJECT);
			g_UndoManager.SendMessage(&undo);
		}
	}

	// mettre a jour la propriete de l'objet (en interne)
	// dans le message des proprietes
	object->UpdateProperties(&message,index);

	// mettre a jour dans l'objet graphique si c'est le cas
	// mais on prend les proprietes de l'objet
	object->SetProperties(&message,index);
		
	// rafraichir si c'est demandé
	if(needRefresh)
	{
		int32	index;

		// trouver l'index de l'item a rafrachir
		index = _objectList->FindIndexOfObject(object);
		if(index<0)
			return;
		
		_objectList->InvalidateItem(index);
	}
	
	// rafraichir l'affichage
	if(needInvalidate)
		object->Invalidate();
}

/**** bloquer un objet ****/
void ObjectManagerWin::LockObject(VObject *object)
{
	// on fonction de son etat
	if(object->IsLock())
		object->UnlockObject();
	else
		object->LockObject();

	// rafraichir
	_objectList->Invalidate();
}

/**** restorer une proprietes ou un objet en fonction du message de Undo ****/
void ObjectManagerWin::RestoreUndo(BMessage *data)
{
	int32	objectId;
	int32	type;
	
	// cette fonction va essayer d'extraire les infos
	// contenu dans le message _infos pour afficher au mieux
	// des details sur le undo
	if(data->FindInt32(UNDO_ACTION_TYPE,&type)!=B_OK)
		return;
	
	// on va recuperer des elements toujours communs a tous les Undos
	// comme l'ID de l'objet (par ailleur on doit toujours trouver cet ID)
	if(data->FindInt32(VOBJECT_ID,&objectId)!=B_OK)
		return;
	
	// en fonction du type on regarde ce qu'on peux faire
	switch(type)
	{
	case UNDO_PASTE_OBJECT:
		// pour le paste on ne gere rien etant donné que l'on peux soi-meme effacer
		// l'objet apres si on veux revenir en arriere
		break;
	case UNDO_CUT_OBJECT:
	case UNDO_DELETE_OBJECT:
		{
			BMessage		properties;
			BString			addonSource;
			uint32			parentID = (uint32)NULL;
			VObject			*object = NULL;
			object_type		type;

			// on va toujours regarder si l'objet est dans la liste garbage
			// si oui on va l'attacher a nouveau, mais pas le recreer
			// sinon on va creer completement l'objet, par contre l'objet
			// parent doit forcement exister.
			if(data->FindMessage(VOBJECT_PROPERTIES,&properties)!=B_OK)
				return;

			// trouver le nom de l'addon source pour recreer l'objet
			if(data->FindString(VOBJECT_ADDON_SOURCE,&addonSource)!=B_OK)
				return;
		
			// on va verifier que le parent existe
			// sauf dans le cas d'un projet
			properties.FindInt32(VOBJECT_TYPE,(int32 *)&type);
			properties.FindInt32(VOBJECT_FATHER_ID,(int32 *)&parentID);
			if(type!=PROJECT_TYPE && ObjectFromID(parentID)==NULL)
				return;
			
			// recreer l'objet	
			InstantiateObject(object,data);
		}
		break;
	case UNDO_CHANGE_PROPERTIES_OBJECT:
		{
			BRect		frame;
			VObject		*object = NULL;
			int32		propertiesIndex;
			BMessage	display(M_OBJECT_CHANGE_MSG);
	
			// recuperer les données du Undo
			data->FindInt32(M_PROPERTIES_INDEX,&propertiesIndex);

			// on doit trouver l'objet correspondant & l'ID
			object = _objectList->FindObjectByID(objectId);
			if(object==NULL)
				break;
	
			// attention dans le cas de la taille ou l'emplacement d'un objet
			// on doit le rdimentionner ou le deplacer
			if(data->FindRect("_frame",&frame)==B_OK)
			{
				BView		*view = NULL;
				BRect		bounds;
		
				// recuperer la vue de l'objet
				if(g_GUIManagerView->LockLooper())
				{
					view = g_GUIManagerView->ViewFromObject(object);
					g_GUIManagerView->UnlockLooper();
				}
				else
					g_MetosApp->SendDebbug("Error : LockLooper() in ObjectManagerWin::RestoreUndo()",VDEBUG_ERROR);
						
				// continuer si le pointer est valide
				if(view==NULL)
					break;
		
				// recuperer la position actuelle de l'objet
				if(view->LockLooper())
				{
					bounds = view->Frame();
					view->UnlockLooper();
		
					// verifier qu'il y a une difference
					if(frame==bounds)
						break;
		
					// verifier si on doit le redimentionner
					if(bounds.Width()!=frame.Width() || bounds.Width()!=frame.Height())
						g_GUIManagerView->ResizeObjectTo(object,frame.Width(),frame.Height());
		
					// verifier si on doit le deplacer
					if(bounds.left!=frame.left || bounds.top!=frame.top)
						g_GUIManagerView->MoveObjectTo(object,frame.left,frame.top);
				}
				else
					g_MetosApp->SendDebbug("Error : LockLooper() in ObjectManagerWin::RestoreUndo()",VDEBUG_ERROR);
			}

			// enlever les infos pour qu'il ne reste que la proprietes
			// et l'index
			data->RemoveData(VOBJECT_ID);
			data->RemoveData(UNDO_ACTION_TYPE);

			// ajouter les champs utiles a l'update de la proprietes
			// on va garder une copie des données pour l'affichage
			data->what = M_OBJECT_CHANGE_MSG;	
			data->AddInt32(M_OBJECT_SELECTED,object->Id());		
	
			// modifier
			UpdateProperties(*data);

			// prevenir la fenetre des proprietes
			g_PropertieManager.SendMessage(data);
		}
		break;
	case UNDO_TYPE_UNDIFINED:
	default:
		;// non definit
	}
}

/**** selection d'un item de fonction ****/
void ObjectManagerWin::CheckFunction(BMessage *message)
{
	int32		objectID;
	int32		selected;
	status_t	state = B_OK;
	
	// verifier que l'on trouve toutes les données
	state &= message->FindInt32(M_OBJECT_SELECTED,&objectID);
	state &= message->FindInt32(VOBJECT_FUNCTION_INDEX,&selected);
	if(state!=B_OK)
		return;
	
	// mettre a jour dans l'objet
	VObject		*object = NULL;
	BMessage	*functions = NULL;
	int32		item;
	int32		index;
	
	// pour ca on doit trouver l'objet
	object = ObjectFromID(objectID);
	if(object==NULL)
		return;
	
	// si on le trouve dans la liste c'est qu'on le deselectionne
	// sinon c'est une selection
	index = 0;
	functions = object->FunctionList();
	while(functions->FindInt32(VOBJECT_FUNCTION_INDEX,index,&item)==B_OK)
	{
		if(item == selected)
		{
			functions->RemoveData(VOBJECT_FUNCTION_INDEX,index);
			return;
		}
		
		// au suivant
		index++;
	}
	
	// ok on l'a pas trouvé, alors on l'ajoute
	functions->AddInt32(VOBJECT_FUNCTION_INDEX,selected);
}

// ====================================
// = sauvegarde des objets sur disque =
// ====================================

/**** sauver les archives des objet dans le projet ****/
status_t ObjectManagerWin::FlattenAllObject(BMessage *archive)
{
	VObject		*current = NULL;
	BMessage	objectArchive(VOBJECT_ARCHIVE_MSG);
	status_t	state = B_OK;
	int32		nbItem;
	
	// ajouter les archives des objets de la liste
	nbItem = _objectList->CountItems();
	if(nbItem==0)
		return B_ERROR;

	// proteger le parcour de la liste
	if(Lock())
	{
		for(int32 i=0;i<nbItem;i++)
		{
			// l'item courant
			current = ((ObjectManagerItem *)(_objectList->ItemAt(i)))->Object();

			// ajouter l'archive de l'objet dans la hierarchie
			state &= ArchiveObject(current,&objectArchive);
			state &= archive->AddMessage(VOBJECT_ARCHIVE,&objectArchive);

			// vider pour passer au suivant
			objectArchive.MakeEmpty();
		}
		Unlock();
	}
	else
		g_MetosApp->SendDebbug("Error : Lock() in ObjectManagerWin::FlattenAllObject()",VDEBUG_ERROR);
		
	return state;
}

/**** charger les archives des objet ****/
status_t ObjectManagerWin::UnflattenAllObject(BMessage *archive)
{
	status_t	state = B_OK;
	BMessage	archivedObject;
	int32		nbArchivedObject;
	VObject 	*newObject = NULL;
	uint32		type;
	
	// trouver le nombre d'objets dans l'archive
	archive->GetInfo(VOBJECT_ARCHIVE,&type,&nbArchivedObject);

	// recuperer et creer chaque objet !
	for(int32 i = 0;i<nbArchivedObject;i++)
	{
		// trouver l'archive puis creer l'objet
		if(archive->FindMessage(VOBJECT_ARCHIVE,i,&archivedObject)==B_OK)
			state &= InstantiateObject(newObject,&archivedObject,false);
	
		// effacer pour charger le prochain
		archivedObject.MakeEmpty();	
	}
			
	return state;
}

/**** archivage de la classe ****/
status_t ObjectManagerWin::ArchiveObject(VObject *object,BMessage *archive)
{
	BMessage	*settings = NULL;
	BString		addonSource;
	status_t	state = B_OK;

	// ajouter le nom de l'addon source
	settings = object->Settings();
	settings->FindString(VOBJECT_ADDON_SOURCE,&addonSource);
	archive->AddString(VOBJECT_ADDON_SOURCE,addonSource);

	// ajouter les proprietes
	state &= archive->AddMessage(VOBJECT_PROPERTIES,object->Properties());

	// ajouter la liste des fonctions
	state &= archive->AddMessage(VOBJECT_FUNCTIONLIST,object->FunctionList());

	return state;
}

/**** creer un objet a partir d'une archive ****/
status_t ObjectManagerWin::InstantiateObject(VObject *newObject,BMessage *archive,bool newName)
{
	BMessage	properties(VOBJECT_PROPERTIES_MSG);
	BMessage	functions(VOBJECT_FUNCTION_LIST_MSG);
	BMessage	selected(M_SELECT_OBJECT_MSG);
	BMessage	derived(M_CHECK_CLASS_DERIVED_MSG);
	BMessage	replyMessage(M_REPLY_MSG);
	BPoint		dropPoint;
	status_t	state = B_OK;
	BString		addonName;
	int32		index;

	// trouver l'addon qui correspond a notre objet
	// si on le trouve c'est que ca vient d'une archive sauvegardé sur disque
	// sinon on doit trouver l'index directement
	if(archive->FindString(VOBJECT_ADDON_SOURCE,&addonName)==B_OK)
		index = FindItemFromName(addonName.String());
	else
		return B_ERROR;		

	// l'index doit etre valide
	if(index<0)
		return B_ERROR;

	// essayer de trouver un item dans la garbage list
	// sinon creer une instance de VObject sans creer l'objet graphique !
	newObject = FindObjectInGarbage(addonName.String());
	if(newObject!=NULL)
	{
		// on a trouvé un objet dans la liste des garbage
		// il est important de remttre a zero l'ID parent
		BMessage	*objectProperties = NULL;
		
		objectProperties = newObject->Properties();
		if(objectProperties!=NULL)
			objectProperties->ReplaceInt32(VOBJECT_FATHER_ID,0);
			
		// dans ce cas on va regenerer les proprietes par defaut
		// pour remplacer les ancienne si on en a pas deja
		newObject->SetDefaultProperties(&properties);	
		if(archive->FindMessage(VOBJECT_PROPERTIES,&properties)!=B_OK)
			archive->AddMessage(VOBJECT_PROPERTIES,&properties);		
	}
	else
	{
		// on a pas d'ancien object donc on va en creer un nouveau
		newObject = CreateAddOnObject(index);
	
		// as-ton reussi a creer l'objet
		if(newObject==NULL)
			return B_ERROR;

		newObject->SetDefaultProperties(newObject->Properties());	
	}

	// a-t-on des proprietes si oui on les remplacent
	// par celle que nous avons
	if(archive->FindMessage(VOBJECT_PROPERTIES,&properties)==B_OK)
		newObject->ReplaceProperties(&properties);

	// mettre a jour les infos sur ce type d'objet
	// mettre a jour les parametres de l'item
	// nombre d'objet de ce type creer etc ...	
	ObjectAddOnItem		*currentItem = NULL;

	currentItem = (ObjectAddOnItem *)(_addOnList.ItemAt(index));
	if(currentItem!=NULL)
		currentItem->CreateObject();

	// assigner un nom a l'objet
	// si on doit le faire (pas par exemple apres une lecture de fichier)
	if(newName)
		SetObjectName(newObject,index);

	// donner le nom de l'addon qui a crée l'objet
	SetObjectAddonSource(newObject,index);

	// si c'est un message de drag&drop on peux avoir un point
	// ou placer l'objet
	if(archive->FindPoint(M_DROP_POINT,&dropPoint)==B_OK)
		PlaceNewObjectIntoDropPoint(newObject,dropPoint);
	
	// creer l'objet graphiquement
	state &= newObject->CreateObject(newObject->Properties());
	state &= newObject->AfterCreateObject();

	// si on a une liste de fonction utilisées on va mettre
	// a jour celle de l'objet avec ces derniere
	if(archive->FindMessage(VOBJECT_FUNCTIONLIST,&functions)==B_OK)
	{
		newObject->SetFunctions(&functions);
		derived.AddMessage(VOBJECT_FUNCTIONLIST,&functions);
	}
	
	// verifier la derivation
	// si on a pas la liste de fonctions on prend celle de l'objet
	if(functions.IsEmpty())	
		derived.AddMessage(VOBJECT_FUNCTIONLIST,newObject->FunctionList());		

	// ajouter le nouvel objet a son futur parent
	state &= AddNewObject(newObject);

	// demander si la classe est derivée
	derived.AddMessage(VOBJECT_PROPERTIES,newObject->Properties());
	g_PropertieManager.SendMessage(&derived,&replyMessage);

	// on selectionne l'objet nouvellement creer
	// si c'est pas un type Control
	selected.AddBool(M_SHIFT_SELECTION,false);
	switch(newObject->Type())
	{
	case UNDEFINED_TYPE:
	case OTHER_TYPE:
	case PROJECT_TYPE:
	case APPLICATION_TYPE:
	case WINDOW_TYPE:
	case VIEW_TYPE:
		// selectionner l'objet
		selected.AddInt32(M_OBJECT_SELECTED,newObject->Id());		
		break;
	default:
		// selectionner le parent
		selected.AddInt32(M_OBJECT_SELECTED,newObject->Father()->Id());		
	}
	SelectObject(&selected);
	
	// retourner l'etat
	return state;
}

// =================================
// = gestion du chemin d'un projet =
// =================================

/**** retourner le chemin de sauvegarde ****/
status_t ObjectManagerWin::SavePath(BString &savePath)
{
	// cette fonction va chercher un objet de type PROJECT_TYPE
	// et extraire les infos de ce dernier pour determiner
	// le chemin de sauvegarde
	
	// on doit au moins avoir des objets dans la liste
	if(_objectList->CountItems()==0)
		return B_ERROR;
		
	// dans notre liste d'objet le projet est le premier
	// sinon il y a un probleme !!!
	ObjectManagerItem	*projectItem = NULL;
	VObject				*object = NULL;
	BMessage			*properties = NULL;
	BString				name;
	BString				path;
	status_t			state = B_OK;
	
	// verifier la validiter le l'objet
	projectItem = (ObjectManagerItem *)(_objectList->ItemAt(0));
	object = projectItem->Object();
	if(object==NULL)
		return B_ERROR;

	// ok on a un objet, ca doit etre un projet
	if(object->Type()!=PROJECT_TYPE)
		return B_ERROR;
		
	// bon on a tout, reste a trouver les proprietes
	properties = object->Properties();
	state &= properties->FindString("_path",&path);
	state &= properties->FindString(VOBJECT_NAME,&name);

	// vider la chaine avant (on sait jamais)
	savePath.SetTo("");

	// verifier la presence d'un / en fin de chemin
	if(path[path.Length()-1]!='/')
		path << "/";
	
	savePath << path;
	savePath << name;

	// ajouter l'extension
	name << ".metos";

	return state;
}

/**** retourner le chemin de sauvegarde ****/
void ObjectManagerWin::RefreshSavePath(const char *path,const char *name)
{
	// on doit au moins avoir des objets dans la liste
	if(_objectList->CountItems()==0)
		return ;

	BMessage			update(M_OBJECT_CHANGE_MSG);
	ObjectManagerItem	*projectItem = NULL;
	VObject				*object = NULL;
	BMessage			*properties = NULL;

	// verifier la validité de l'objet projet
	projectItem = (ObjectManagerItem *)(_objectList->ItemAt(0));
	object = projectItem->Object();
	if(object==NULL)
		return ;

	// rafraichir en interne les proprietes de l'objet
	properties = object->Properties();
	properties->ReplaceString("_path",path);
	properties->ReplaceString(VOBJECT_NAME,name);

	// pointeur sur l'objet
	// rafraichir l'affichage, ce qui sera fait si c'est l'item selectionné
	update.AddInt32(M_OBJECT_SELECTED,object->Id());
	update.AddString("_path",path);
	g_PropertieManager.SendMessage(&update);

	update.RemoveName("_path");
	update.AddString(VOBJECT_NAME,name);
	g_PropertieManager.SendMessage(&update);
}

// ================================
// = Menu et Couper/Copier/Coller =
// ================================

/**** menu de blockage de l'objet ****/
void ObjectManagerWin::SetLockMenu(VObject *object)
{
	// en fonction de l'objet
	if(_options->ItemAt(0)!=NULL)
		_options->ItemAt(0)->SetMarked(object->IsLock());
}

/**** creer le menu des actions ****/
void ObjectManagerWin::BuildOptionsMenu()
{
	// construire le menu popup
	_options = new BPopUpMenu("Options");

	// ajouter les actions standards
	_options->AddItem(new BMenuItem(g_Languages->GetTranslationFor(L_MENU_OBJECT_LOCK),NULL));
	_options->AddSeparatorItem();
	_options->AddItem(new BMenuItem(g_Languages->GetTranslationFor(L_MENU_OBJECT_CUT),NULL));
	_options->AddItem(new BMenuItem(g_Languages->GetTranslationFor(L_MENU_OBJECT_COPY),NULL));
	_options->AddItem(new BMenuItem(g_Languages->GetTranslationFor(L_MENU_OBJECT_PASTE),NULL));
}	

/**** creer le menu Tools ****/
void ObjectManagerWin::BuildToolsMenu()
{
	int32				useToolMenu;
	
	// charger
	useToolMenu = g_MeTOSPrefs->GetInt32(B_CONTROL_OFF,"use-popup-tools");

	// ajouter les outils dans un sous menu
	// verifier la validiter de la fenetre
	// le menu ne doit pas deja exister
	if(useToolMenu==B_CONTROL_ON)
	{
		if(_toolsMenu==NULL)
		{
			BMenuItem	*toolsMenuItem = NULL;
			BString		toolName;
			int32		index;
		
			// construire le menu des outils
			_toolsMenu = new BMenu(g_Languages->GetTranslationFor(L_MENU_OBJECT_TOOLS));
			index = 0;
			while(_toolsNameList.FindString(M_TOOLS_NAME,index,&toolName)==B_OK)
			{				
				toolsMenuItem = new BMenuItem(toolName.String(),NULL);
				_toolsMenu->AddItem(toolsMenuItem);
				
				// au suivant
				index++;
			}			
		}
		
		// ajouter les outils
		_options->AddSeparatorItem();
		_options->AddItem(_toolsMenu);		
	}
}

/**** menu tools ****/
void ObjectManagerWin::SelectToolsFromMenu(int32 index)
{
	BMessage	toolsSelected(M_TOOLS_CALL_MSG);
	
	// lancer l'action en fonction du menu !
	toolsSelected.AddInt32(M_TOOLS_INDEX,index);
	g_ToolManager.SendMessage(&toolsSelected);
}

// =====================================
// = Fonctions de copier/couper/coller =
// =====================================
// pour ces fonctions on ne va tenir
// compte que de la selection principale

/**** couper les objets selectionnés ****/
void ObjectManagerWin::CutSelectedObject()
{
	// le cut est un copy avec effacelent de l'objet en plus
	// pour le Cut on appel la fonction Delete, car elle va mettre
	// l'objet dans la liste garbage, et on va copier ces proprietes
	// dans le ClipBoard.
	// trouver l'objet courant
	CopySelectedObject();	
	
	// puis on coupe
	if(_selectedObject==(uint32)NULL)
		return;
	
	VObject		*object = NULL;

	// pour la copie on va seulement copier les proprietes
	object = ObjectFromID(_selectedObject);
	if(object==NULL)
		return;

	// selection que le principal
	BMessage	clearSelection(M_SELECT_OBJECT_MSG);
	
	clearSelection.AddInt32(M_OBJECT_SELECTED,_selectedObject);
	SelectObject(&clearSelection);

	// effacer les proprietes, pour ca on envoi un message de selection
	// vide, ca a le meme effet que d'annuler la selection
	g_PropertieManager.SendMessage(M_DISPLAY_PROPERTIES_MSG);

	// vider la liste de selection graphique
	if(g_GUIManagerView->LockLooper())
	{
		g_GUIManagerView->RemoveAll();	
		g_GUIManagerView->UnlockLooper();

		// effacer l'objet courant
		DeleteObject(object,true);
	
		// afficher le nombre d'objet
		RefreshDisplayNumberItems();		
	}
	else
		g_MetosApp->SendDebbug("Error : LockLooper() in ObjectManagerWin::CutSelectedObject()",VDEBUG_ERROR);
}								

/**** copier les objets selectionnés ****/
void ObjectManagerWin::CopySelectedObject()
{
	// pour la copie on ne fait pas de Undo, cela ne sert a rien
	// car ca n'affecte pas le projet en cours
	
	// trouver l'objet courant
	if(_selectedObject==(uint32)NULL)
		return;
	
	BMessage	clipboard(UNDO_ACTION_MSG);
	BMessage	*settings = NULL;
	VObject		*object = NULL;
	BString		addonSource;
	status_t	state = B_OK;
	
	// pour la copie on va seulement copier les proprietes
	object = ObjectFromID(_selectedObject);
	if(object==NULL)
		return;

	// recuperer des infos
	settings = object->Settings();
	settings->FindString(VOBJECT_ADDON_SOURCE,&addonSource);
		
	// recuperer les donnees pour le undo
	clipboard.AddInt32(VOBJECT_ID,object->Id());
	clipboard.AddString(VOBJECT_ADDON_SOURCE,addonSource);
	clipboard.AddMessage(VOBJECT_PROPERTIES,object->Properties());
		
	// copier ces données dans le clipboard
	// on doit le bloquer avant d'ecrire dedans
	if(be_clipboard->Lock())
	{
		BMessage	*clip = NULL;
		
		// vider
		be_clipboard->Clear();
		clip = be_clipboard->Data();
		if(clip!=NULL)
		{
			// creer le buffer
			ssize_t			size = clipboard.FlattenedSize();
			char			buffer[size];
			
			state &= clipboard.Flatten(buffer,size);
			if(state==B_OK)
			{
				state &= clip->AddData("application/octet-stream",B_MIME_TYPE,&buffer,size);
				be_clipboard->Commit();
			}
		}
		// on le debloque apres nos operation d'insertion finies
		be_clipboard->Unlock();
	}
	else
		g_MetosApp->SendDebbug("Error : Lock() in ObjectManagerWin::CopySelectedObject()",VDEBUG_ERROR);
}

/**** coller les objets selectionnés ****/
void ObjectManagerWin::PasteSelectedObject(BPoint &where)
{
	BMessage	archive(UNDO_ACTION_MSG);
	status_t	state = B_OK;
	
	// on va creer un objet avec le proprietes dans le ClipBoard
	if(be_clipboard->Lock())
	{
		BMessage	*clip = NULL;
		
		clip = be_clipboard->Data();
		if(clip!=NULL)
		{
			// creer le buffer
			const char		*buffer;
			ssize_t			size;
			
			state &= clip->FindData("application/octet-stream",B_MIME_TYPE,(const void **)&buffer,&size);
			state &= archive.Unflatten(buffer);
		}
		be_clipboard->Unlock();
	}
	else
		g_MetosApp->SendDebbug("Error : Lock() in ObjectManagerWin::PasteSelectedObject()",VDEBUG_ERROR);

	// a-ton bien trouvé le message
	if(state==B_OK)
	{
		BMessage	properties;
		VObject		*object = NULL;
				
		// remplacer les proprietes
		// attention on ne doit pas remplacer le nouvel ID et le nouveau nom
		archive.FindMessage(VOBJECT_PROPERTIES,&properties);
		properties.RemoveData(VOBJECT_ID);
		properties.RemoveData(VOBJECT_NAME);
		archive.ReplaceMessage(VOBJECT_PROPERTIES,&properties);

		// on va ajouter a l'archive le point ou le menu c'est declanché
		// mais on doit convertir ce point en fonction de la selection courante
		// on doit avoir un objet destination valide pour la convertion
		if(_selectedObject!=(uint32)NULL)
		{
			VObject		*currentSelection = NULL;
			
			// trouver le VObject correspondant a l'ID de l'objet courant
			currentSelection = ObjectFromID(_selectedObject);
			if(currentSelection!=NULL)
			{
				BView	*view = NULL;
				
				// se baser sut le type pour faire ca
				switch(currentSelection->Type())
				{
				case UNDEFINED_TYPE:
				case PROJECT_TYPE:
				case APPLICATION_TYPE:
				case MENUITEM_TYPE:
				case LISTITEM_TYPE:
				case BITMAP_TYPE:
				case SHELF_TYPE:
				case OTHER_TYPE:
					// pas pour ceux-la
					break;
				case WINDOW_TYPE:
					{
						BWindow		*window = NULL;
							
						window = dynamic_cast<BWindow *>(currentSelection->Object());
						if(window==NULL)
							break;
		
						// invalider la vue support
						if(window->Lock())
						{
							view = window->ChildAt(0);						
							window->Unlock();
						}
						else
							g_MetosApp->SendDebbug("Error : Lock() in ObjectManagerWin::PasteSelectedObject()",VDEBUG_ERROR);
					}
					break;
				default:	
					// cas normal (une vue)
					view = dynamic_cast<BView *>(currentSelection->Object());
				}
				
				// on va ajouter a l'archive le point ou le menu c'est declanché
				// si on a bien un objet qui necessite ce point
				if(view!=NULL && view->LockLooper())
				{
					BRect	bounds;
						
					bounds = view->Bounds();
					view->ConvertFromScreen(&where);
					view->UnlockLooper();
					
					// le poit doit aussi etre contenu dans le rectangle de l'objet
					if(bounds.Contains(where))
						archive.AddPoint(M_DROP_POINT,where);
				}
				else
					g_MetosApp->SendDebbug("Error : LockLooper() in ObjectManagerWin::PasteSelectedObject()",VDEBUG_ERROR);
			}
		}

		// creation du nouvel objet
		if(Lock())
		{
			InstantiateObject(object,&archive);
			Unlock();
		}
		else
			g_MetosApp->SendDebbug("Error : Lock() in ObjectManagerWin::PasteSelectedObject()",VDEBUG_ERROR);
	}
}

// ======================
// = Gestion des Addons =
// ======================

/**** Creer un VObject sans l'objet graphique ****/
VObject	*ObjectManagerWin::CreateAddOnObject(int32 item)
{
	// recuperer l'Addon_Id
	ObjectAddOnItem		*currentItem = NULL;
	VObject				*(*instantiate_VObject)();
	VObject				*newObject = NULL;
	image_id			addonId;
	
	currentItem = (ObjectAddOnItem *)(_addOnList.ItemAt(item));
	addonId = currentItem->AddonId();

	// on regarde si on arrive a trouver la metode de creation de l'objet
	// si oui on creer le VObject et on retourne l'instance
	if(get_image_symbol(addonId,"instantiate_VObject", B_SYMBOL_TYPE_TEXT,(void **)&instantiate_VObject) != B_OK)
		return NULL;

	// creation de l'objet
	newObject = (*instantiate_VObject)();	

	return newObject;
}

/**** charger les Addons ****/
void ObjectManagerWin::LoadAddOns()
{
   	ObjectAddOnItem		*addOnItem = NULL; 
	VObject				*object = NULL;
	VObject				*(*instantiate_VObject)();
	image_id			addonId;
   	BMessage			addonItem(M_CREATE_ADDON_MSG);
   	status_t 			err = B_NO_ERROR;
	app_info 			info; 
    BPath 				path;
	BString				message;
	int32				nbFiles;
	BMessage			splashInfo(M_PROGRESSSTART_MSG);
	BMessage			splashProgress(M_PROGRESSINCREASE_MSG);
	entry_ref			entryRef;
	BEntry				entry;
	BString				buffer;
	BString				className;
	
	// initialiser le compteur du splash-screen
	nbFiles = g_MetosApp->CountAddOnFile(ADDON_OBJECTS_FOLDER);
	splashInfo.AddInt32(M_TOTALPROGRESS,nbFiles);
	splashInfo.AddString(M_TEXTDISPLAY,"Load Object AddOns...");
	BMessenger(g_MetosApp->_splashScreen).SendMessage(&splashInfo);
		
   	// trouver le chemin des addons
    path = g_MetosApp->GetAddonsPath();
	path.Append(ADDON_OBJECTS_FOLDER);
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
				if(get_image_symbol(addonId,"instantiate_VObject", B_SYMBOL_TYPE_TEXT,(void **)&instantiate_VObject) == B_OK)
				{
					// on creer un VObject sans creer l'objet graphique qu'il contient
					// ca nous permet d'acceder a ces proprietes et un tas d'autre chose
					// lors de la creation d'un objet graphique on va creer les deux
					object = (*instantiate_VObject)();

					// creer l'addon
					// puis stoquer l'item de la liste (liste des AddOn)
					if(object!=NULL)
					{
						// creer l'addon
						addOnItem = new ObjectAddOnItem(addonId,object,&entry);

						// ajouter a la liste interne
						_addOnList.AddItem(addOnItem);
					
						// initialiser les proprietes
						object->SetDefaultProperties(object->Properties());
					
						// recuperer lenom de la classe
						(object->Properties())->FindString(VOBJECT_CLASS,&className);
						entry.GetRef(&entryRef);
						
						// faire creer un addon pour la liste a afficher
						addonItem.AddRef(M_ADDON_ENTRY,&entryRef);
						addonItem.AddMessage(VOBJECT_PROPERTIES,object->Properties());
						addonItem.AddMessage(VOBJECT_SETTINGS,object->Settings());
						g_AddOnManager.SendMessage(&addonItem);
						addonItem.MakeEmpty();
							
						// informer le splash-screen
						BMessenger(g_MetosApp->_splashScreen).SendMessage(&splashProgress);

						// informer que tout c'est bien passé
						message = "AddOns : ";	
						message << path.Path() << " Loaded";
						g_MetosApp->SendDebbug(message.String(),VDEBUG_DEBUG);
					}
				}
				else
					unload_add_on(addonId);
			}
		}
	}
	// informer que tout c'est bien passé
	g_MetosApp->SendDebbug("Object Addons Loading finish",VDEBUG_INFO);

	// lancer la creation des items de fonction
	LoadFunctionsFromAddons();
	
	// charger les codes externes
	LoadExternalCodesForAddons();
	
	// charger les dependances
	LoadDependenciesFromAddons();
}

/**** decharger les AddOns ****/
void ObjectManagerWin::UnloadAddOns()
{
	// liberer la memoire prise par les items
	ObjectAddOnItem		*addOn = NULL;
	int32				nbItems;
	image_id			image;
	
	nbItems = _addOnList.CountItems() - 1;
	for(int32 index=nbItems;index>=0;index--)
	{
		addOn = (ObjectAddOnItem *)(_addOnList.RemoveItem(index));
		image = addOn->AddonId();				
		delete addOn;
		if(unload_add_on(image)!=B_OK)
			printf("Error unloading Object Addon\n");
	}
}

/**** trouver l'index d'un item par nom de classe ****/
int32 ObjectManagerWin::FindItemFromName(const char *name)
{
	int32				nbItems = 0;
	ObjectAddOnItem		*currentItem = NULL;

	// rechercher dans la liste interne
	nbItems = _addOnList.CountItems();
	for(int32 i=0;i<nbItems;i++)
	{
		currentItem = (ObjectAddOnItem *)(_addOnList.ItemAt(i));
		if(currentItem!=NULL && currentItem->AddonName()==name)
			return i;
	}
	
	BString		message;
	message = "No Addon for this class : ";
	message << name;
	g_MetosApp->SendDebbug(message.String(),VDEBUG_WARNING);

	// pas trouvé
	return -1;
}

/**** renseigner dans un VObject le nom de l'addon utilisé pour le creer ****/
void ObjectManagerWin::SetObjectAddonSource(VObject *object,int32 index)
{
	ObjectAddOnItem		*currentItem = NULL;
	BMessage			*settings = NULL;
	BString				addonSource;

	if(index<0)
		return;	// pas d'item selectionné
	
	// ajouter le nom de l'addon source
	settings = object->Settings();
	currentItem = (ObjectAddOnItem *)(_addOnList.ItemAt(index));
	addonSource = currentItem->AddonName();
	settings->ReplaceString(VOBJECT_ADDON_SOURCE,addonSource);
}

/**** lancer la creation des items de fonctions de chaques objets ****/
void ObjectManagerWin::LoadFunctionsFromAddons()
{
	BMessage			functionItem(M_CREATE_FUNCTION_MSG);
	ObjectAddOnItem		*currentItem = NULL;
	int32				nbItems = 0;
	VObject				*object = NULL;
	BPath				*path = NULL;		
	entry_ref			ref;

	// ajouter pour pouvoir remplacer
	functionItem.AddRef(M_ADDON_ENTRY,&ref);
	functionItem.AddPointer(M_OBJECT_POINTER,object);				

	// parcourir la liste pour creer pour chaque addon
	// les fonctions et le code externe
	nbItems = _addOnList.CountItems();
	for(int32 i=0;i<nbItems;i++)
	{
		currentItem = (ObjectAddOnItem *)(_addOnList.ItemAt(i));
		if(currentItem!=NULL)
		{
			object = currentItem->Object();
			path = currentItem->Path();
			
			// tester la validité de l'objet
			if(object!=NULL)
			{
				// recuperer l'entry_ref
				get_ref_for_path(path->Path(),&ref);
			
				// faire creer les items de fonctions
				// envoyer le message a la fenetre proprietes puis le vider
				functionItem.ReplaceRef(M_ADDON_ENTRY,&ref);
				functionItem.ReplacePointer(M_OBJECT_POINTER,object);				
				g_PropertieManager.SendMessage(&functionItem);			
			}
		}
	}
}

/**** lancer la creation des items de code externe de chaques objets ****/
void ObjectManagerWin::LoadExternalCodesForAddons()
{
	BMessage			externalCode(M_LOAD_EXTERNAL_CODE_MSG);
	ObjectAddOnItem		*currentItem = NULL;
	int32				nbItems = 0;
	VObject				*object = NULL;

	// ajouter pour pouvoir remplacer
	externalCode.AddString(VOBJECT_CLASS,"no-name");

	// parcourir la liste pour creer pour chaque addon
	// les fonctions et le code externe
	nbItems = _addOnList.CountItems();
	for(int32 i=0;i<nbItems;i++)
	{
		currentItem = (ObjectAddOnItem *)(_addOnList.ItemAt(i));
		if(currentItem!=NULL)
		{
			// tester la validité de l'objet
			object = currentItem->Object();
			if(object!=NULL)
			{
				// lancer le chargement des fichiers externe
				externalCode.ReplaceString(VOBJECT_CLASS,object->ClassName());
				g_PropertieManager.SendMessage(&externalCode);				
			}
		}
	}
}

/**** lancer la creation des items de dependances de chaques objets ****/
void ObjectManagerWin::LoadDependenciesFromAddons()
{
	BMessage			dependencyItem(M_CREATE_DEPENDENCY_MSG);
	ObjectAddOnItem		*currentItem = NULL;
	int32				nbItems = 0;
	VObject				*object = NULL;

	// ajouter pour pouvoir remplacer
	dependencyItem.AddString(VOBJECT_CLASS,"no-name");

	// parcourir la liste pour creer pour chaque addon
	// les fonctions et le code externe
	nbItems = _addOnList.CountItems();
	for(int32 i=0;i<nbItems;i++)
	{
		currentItem = (ObjectAddOnItem *)(_addOnList.ItemAt(i));
		if(currentItem!=NULL)
		{
			// recuperer le pointeur sur l'objet puis l'envoyer
			// a la fenetre des proprietes
			object = currentItem->Object();
			if(object!=NULL)
			{
				dependencyItem.ReplaceString(VOBJECT_CLASS,object->ClassName());
				g_PropertieManager.SendMessage(&dependencyItem);
			}
		}
	}
}

// ====================================
// = Gestion de la generation de code =
// ====================================

/**** archivage dans un message toutes les infos pour pouvoir generer le code ****/
status_t ObjectManagerWin::PreGenerate(BMessage *archive,BString &generatorID)
{
	VObject		*current = NULL;
	BMessage	objectArchive(VOBJECT_ARCHIVE_MSG);
	status_t	state = B_OK;
	int32		nbItem;
	
	// ajouter les archives des objets de la liste
	nbItem = _objectList->CountItems();
	if(nbItem==0)
		return B_ERROR;

	if(Lock())
	{
		for(int32 i=0;i<nbItem;i++)
		{
			// l'item courant
			current = ((ObjectManagerItem *)(_objectList->ItemAt(i)))->Object();
	
			// ajouter l'archive de l'objet dans la hierarchie
			state &= PreGenerateObject(current,&objectArchive,generatorID);
			state &= archive->AddMessage(VOBJECT_ARCHIVE,&objectArchive);
	
			// vider pour passer au suivant
			objectArchive.MakeEmpty();
		}
	
		// ajouter la liste des dependances
		// pour le generateur donné
		state &= g_MetosApp->_propertieManagerWin->PreGenerateDependencies(&objectArchive,generatorID.String());
		state &= archive->AddMessage(DEPENDENCIES_DATA,&objectArchive);
		Unlock();
	}
	else
		g_MetosApp->SendDebbug("Error : Lock() in ObjectManagerWin::PreGenerate()",VDEBUG_ERROR);

	return state;
}

/**** archivage dans un message toutes les infos d'un objet pour pouvoir generer le code ****/
status_t ObjectManagerWin::PreGenerateObject(VObject *object,BMessage *archive,BString &generatorID)
{
	BMessage	valueAsString(VOBJECT_VALUE_AS_STRING_MSG);
	BMessage	functionList(VOBJECT_FUNCTION_LIST_MSG);
	status_t	state = B_OK;

	// ajouter les settings, les proprietes, les infos, les proprietes bloquées, la correspondance nom propreites
	state &= archive->AddMessage(VOBJECT_SETTINGS,object->Settings());
	state &= archive->AddMessage(VOBJECT_PROPERTIES,object->Properties());
	state &= archive->AddMessage(VOBJECT_INFOS,object->Infos());
	state &= archive->AddMessage(VOBJECT_LOCKED_PROPERTIES,object->LockedProperties());
	state &= archive->AddMessage(VOBJECT_NAME_AS_PROPERTIES,object->NameAsProperties());

	// transformer les valeurs sous formes de chaine de caracteres
	if(g_MetosApp->_propertieManagerWin->Lock())
	{
		g_MetosApp->_propertieManagerWin->ConvertValuesToString(object,&valueAsString);
		g_MetosApp->_propertieManagerWin->Unlock();

		// puis ajouter a notre liste
		state &= archive->AddMessage(VOBJECT_VALUE_AS_STRING,&valueAsString);

		// ajout du code externe supplementaire
		state &= AddGenerationCode(object,archive,generatorID);
	
		// recuperer les fonctions qui sont selectionnées avec les parametres etc...
		if(g_MetosApp->_propertieManagerWin->Lock())
		{
			g_MetosApp->_propertieManagerWin->PreGenerateFunctions(&functionList,object);
			g_MetosApp->_propertieManagerWin->Unlock();
		}
		
		// puis l'ajouter a notre archive
		state &= archive->AddMessage(VOBJECT_FUNCTIONLIST,&functionList);
	}
	else
		g_MetosApp->SendDebbug("Error : Lock() in ObjectManagerWin::PreGenerateObject()",VDEBUG_ERROR);

	return state;
}

/**** ajouter le code correspondant au generateur ****/
status_t ObjectManagerWin::AddGenerationCode(VObject *object,BMessage *archive,BString &generatorID)
{
	BMessage	externalCode;
	BMessage	*properties = NULL;
	BString		sourceCode;

	// on va recuperer les infos dans les proprietes
	// de l'objet
	properties = object->Properties();
	if(properties==NULL)
		return B_ERROR;
	
	// recuperer le message porteur du code
	if(properties->FindMessage(VOBJECT_EXTERNAL_CODE,&externalCode)!=B_OK)
		return B_ERROR;
	
	// extraire le code correspondant au Generateur d'id generatorID
	if(externalCode.FindString(generatorID.String(),&sourceCode)==B_OK)
		archive->AddString(VOBJECT_EXTERNAL_CODE,sourceCode);

	return B_OK;
}

/**** mettre a jour en interne le code pour un objet ****/
void ObjectManagerWin::UpdateExternalCode(BMessage *data)
{
}

// ===========================
// = Gestion des preferences =
// ===========================

/**** rafraichir les preferences ****/
void ObjectManagerWin::RefreshPreferences()
{
	ObjectManagerItem	*item = NULL;
	int32				nbItems;
	
	// rafraichir tous les items de la liste
	nbItems = _objectList->CountItems();
	for(int32 index=0;index<nbItems;index++)
	{
		item = (ObjectManagerItem *)(_objectList->ItemAt(index));
		if(item!=NULL)
			item->RefreshPreferences();
	}	
}

/**** sauver les preferences ****/
void ObjectManagerWin::SavePreferences()
{
	// on va sauver la taille des colonnes de la liste
	BRect				*bounds = NULL;
	int32				nbItem;
	int32				index;

	// charger les preferences
	g_MeTOSPrefs->Load();
	nbItem = _columnView->CountItems();
	for(index = 0;index<nbItem;index++)
	{
		bounds = _columnView->RectFrom(index);
		g_MeTOSPrefs->SetInt32(bounds->IntegerWidth(),"object-manager-column-list",index);
	}
	g_MeTOSPrefs->SetRect(Frame(),"objectManager-rect");
	g_MeTOSPrefs->Save();
}

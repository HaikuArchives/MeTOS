/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
/************************************************/
/* Fenetre qui contient le projet et les objets */
/************************************************/
#include "AddOnManagerWin.h"
#include "PropertieManagerWin.h"
#include "AddOnManagerListView.h"
#include "AddOnManagerItem.h"
#include "MetosMessageDefs.h"
#include "MetosGlobalDefs.h"
#include "CStatusView.h"
#include "CStatusItem.h"
#include "MetosApp.h"
#include "VObject.h"
#include "VPreferenceFile.h"
#include <interface/ListView.h>
#include <support/String.h>
#include <app/Messenger.h>
#include <interface/ScrollView.h>
#include <interface/Bitmap.h>

/*** constructeur de la fenetre MeTOS Application ****/
AddOnManagerWin::AddOnManagerWin(BRect frame)
: BWindow(frame,"AddOns", B_DOCUMENT_WINDOW_LOOK, B_FLOATING_SUBSET_WINDOW_FEEL, B_WILL_ACCEPT_FIRST_CLICK | B_NOT_ZOOMABLE)
{
	BScrollView		*supportScrollView;
	BRect			listRect = Bounds();

	// initialiser
	_objectID = (uint32)NULL;
	_nbSuperItem = 0;
	_nbAddons = 0;
	
	// fixer la couleur de la vue
	// construire la vue principale
	listRect.bottom -= B_V_SCROLL_BAR_WIDTH;
	_addOnManagerView = new BView(listRect,"AddOnManager-view",B_FOLLOW_ALL_SIDES,B_WILL_DRAW);
	_addOnManagerView->SetViewColor(VIEW_COLOR);

	// status
	listRect.right -= B_H_SCROLL_BAR_HEIGHT;
	_statusView = new CStatusView(BRect(listRect.left,listRect.bottom+1,listRect.right,listRect.bottom+B_V_SCROLL_BAR_WIDTH),"Status");
	_statusView->AddItem(new CStatusItem("nb-type","Types : ",85));
	_statusView->AddItem(new CStatusItem("nb-addons","AddOns : ",85));
	AddChild(_statusView);

	// liste des addons
	listRect.InsetBy(2,2);
	_addOnList = new AddOnManagerListView(listRect,"object-addon-list",this);
	supportScrollView = new BScrollView("object-addon-scroll",_addOnList,B_FOLLOW_ALL_SIDES,B_WILL_DRAW | B_FRAME_EVENTS,false,true);
	
	_addOnManagerView->AddChild(supportScrollView);
	AddChild(_addOnManagerView);	
}

/*** destructeur de la fenetre MeTOS Application ****/
AddOnManagerWin::~AddOnManagerWin()
{
	int32		nbItem;

	// sauver les preferences
	SavePreferences();

	// les items de la satus bar
	nbItem = _statusView->CountItems() - 1;
	for(int32 index = nbItem;index>=0;index--)
		delete _statusView->RemoveItem(index);

	// les addons items
	nbItem = _addOnList->FullListCountItems() - 1;
	for(int32 index = nbItem;index>=0;index--)
		delete (AddOnManagerItem *)_addOnList->RemoveItem(index);
}

/**** traiter les messages ****/
void AddOnManagerWin::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
	// renvoyer le mouvement de la roulette de la souris a la srcoll-view
	case B_MOUSE_WHEEL_CHANGED:
		BMessenger(_addOnList).SendMessage(message);
		break;
	// ajouter un item addon
	case M_CREATE_ADDON_MSG:
		AddAddonObject(message);
		break;
	// creation d'un objet reussi
	// effacement d'un objet
	case M_CREATE_OBJECT_MSG:
	case M_DELETE_OBJECT_MSG:
		LockAddon(message);
		break;
	// selection d'un objet
	case M_SELECT_OBJECT_MSG:
		ObjectSelected(message);
		break;
	// preferences
	case M_PREFERENCE_MSG:
		RefreshPreferences();
		_addOnList->Invalidate();
		break;
	default:
		BWindow::MessageReceived(message);	
	}
}

/*** Si on quitte on ferme les fenetre du projet ***/
bool AddOnManagerWin::QuitRequested()
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

/**** selection d'un objet ****/
void AddOnManagerWin::ObjectSelected(BMessage *message)
{
	// le type
	if(message->FindInt32(VOBJECT_TYPE,(int32 *)&_type)!=B_OK)
		_type = UNDEFINED_TYPE;

	// verifier si on trouve l'ID
	if(message->FindInt32(M_OBJECT_SELECTED,(int32 *)&_objectID)!=B_OK)
		_objectID = (uint32)NULL;

	// on va verifier si on bloque certain objet
	AddOnManagerItem	*addon = NULL;
	int32				nbItems;

	// parcourir la liste
	nbItems = _addOnList->CountItems();
	for(int32 i=0;i<nbItems;i++)
	{
		addon = (AddOnManagerItem *)(_addOnList->ItemAt(i));
		if(addon!=NULL)
			addon->SetAvailable(addon->CheckDepencies(_type));
	}
	
	// reafficher
	_addOnList->Invalidate();
}

/**** creation d'un objet reussi ****/
void AddOnManagerWin::LockAddon(BMessage *message)
{
	AddOnManagerItem	*addon = NULL;
	BString				addonName;
	int32				index = -1;
	bool				noMore = false;

	// ajouter la valeur de nomore
	if(message->FindBool(M_ADDON_LOCK,&noMore)!=B_OK)
		noMore = false;
		
	// trouver en interne dans la liste l'addon a mettre a jour
	if(message->FindString(VOBJECT_ADDON_SOURCE,&addonName)!=B_OK)
		return;

	// on doit trouver l'item
	addon = FindItemFromName(addonName.String());
	if(addon==NULL)
		return;
		
	// ok on met a jour
	addon->Lock(noMore);
	
	// reafficher
	index = _addOnList->FullListIndexOf(addon);
	if(index>=0)
		_addOnList->InvalidateItem(index);
}

/**** ajouter et placer dans la liste un addon ****/
void AddOnManagerWin::AddAddonObject(BMessage *data)
{
	AddOnManagerItem	*newAddon = NULL;
	AddOnManagerItem	*item = NULL;
	BMessage			properties;
	BMessage			settings;
	entry_ref			entryRef;
	int32				nbAddon;
	int32 				index;
	object_type			type;
	status_t			state = B_OK;

	// extraire les donnees du message
	state &= data->FindRef(M_ADDON_ENTRY,&entryRef);
	state &= data->FindMessage(VOBJECT_PROPERTIES,&properties);
	state &= data->FindMessage(VOBJECT_SETTINGS,&settings);
	state &= properties.FindInt32(VOBJECT_TYPE,(int32 *)&type);
	if(state!=B_OK)
		return;
	
	// si on a pas de super item on en ajoute un
	item = SuperItemForType(type);
	if(item==NULL)
	{
		// creer le super item
		newAddon = new AddOnManagerItem(type);
		_nbSuperItem++;

		// trouver ou le placer
		nbAddon = _addOnList->FullListCountItems();
		for(index=0;index<nbAddon;index++)
		{
			item = (AddOnManagerItem *)(_addOnList->FullListItemAt(index));
			if(item!=NULL && item->Type()>newAddon->Type())
				break;
		}
		
		// l'ajouter a la liste
		_addOnList->AddItem(newAddon,index);
		item = newAddon;
	}
	
	// ajouter tout de meme l'addon
	// on doit avoir trouvé un item
	if(item!=NULL)
	{
		newAddon = new AddOnManagerItem(&entryRef,&properties,&settings);
		_nbAddons++;
		
		// ajouter a la liste
		_addOnList->AddUnder(newAddon,item);
	}
	
	// rafraichir la status-view
	RefreshDisplayNumberItems();
}

/**** trouver si on a un super item pour ce type ****/
AddOnManagerItem *AddOnManagerWin::SuperItemForType(object_type type)
{
	AddOnManagerItem	*item = NULL;
	int32				nbAddon;
	
	nbAddon = _addOnList->FullListCountItems();
	for(int32 index=0;index<nbAddon;index++)
	{
		item = (AddOnManagerItem *)(_addOnList->FullListItemAt(index));
		if(item!=NULL && item->IsASuperItem() && item->Type()==type)
			return item;
	}
	
	// on a pas trouvé
	return NULL;
}

/**** trouver l'index d'un item par nom de classe ****/
AddOnManagerItem *AddOnManagerWin::FindItemFromName(const char *name)
{
	int32				nbItems = 0;
	AddOnManagerItem	*currentItem = NULL;

	// rechercher dans la liste interne
	nbItems = _addOnList->CountItems();
	for(int32 i=0;i<nbItems;i++)
	{
		currentItem = (AddOnManagerItem *)(_addOnList->ItemAt(i));
		if(currentItem!=NULL && currentItem->AddonName()==name)
			return currentItem;
	}
	
	// pas trouvé
	return NULL;
}

/**** mettre a jour le nombre d'items affiché ****/
void AddOnManagerWin::RefreshDisplayNumberItems()
{
	BString		buffer;

	// afficher le nombre de type differents
	buffer = "Types : ";
	buffer << _nbSuperItem;
	(_statusView->ItemAt(0))->SetLabel(buffer.String());

	// afficher le nombre d'items
	buffer = "AddOns : ";
	buffer << _nbAddons;
	(_statusView->ItemAt(1))->SetLabel(buffer.String());

	// rafraichir
	_statusView->Invalidate();
}

// ===============
// = Preferences =
// ===============

/**** rafraichir les preferences ****/
void AddOnManagerWin::RefreshPreferences()
{
	AddOnManagerItem	*currentItem = NULL;
	int32				nbItems = 0;
	
	// rafraichir la couleur des items
	nbItems = _addOnList->FullListCountItems();
	for(int32 i=0;i<nbItems;i++)
	{
		currentItem = (AddOnManagerItem *)(_addOnList->FullListItemAt(i));
		if(currentItem!=NULL)
			currentItem->RefreshPreferences();
	}
}

/**** sauver les preferences ****/
void AddOnManagerWin::SavePreferences()
{
	// charger les preferences
	g_MeTOSPrefs->Load();
	g_MeTOSPrefs->SetRect(Frame(),"addOnManager-rect");
	g_MeTOSPrefs->Save();
}

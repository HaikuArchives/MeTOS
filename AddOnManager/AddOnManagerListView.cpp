/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
/**************************************************************/
/* Liste qui regroupe la liste des Controles(Addon) du projet */
/**************************************************************/
#include "AddOnManagerWin.h"
#include "AddOnManagerListView.h"
#include "AddOnManagerItem.h"
#include "MetosMessageDefs.h"
#include "VObject.h"
#include "MetosApp.h"
#include "MetosGlobalDefs.h"
#include "MetosLanguagesDefs.h"
#include "VLanguage.h"

#include <interface/ScrollView.h>
#include <interface/InterfaceDefs.h>
#include <interface/PopUpMenu.h>
#include <interface/MenuItem.h>
#include <support/String.h>
#include <interface/Bitmap.h>

/*** Constructeur ***/
AddOnManagerListView::AddOnManagerListView(BRect frame,const char *name,AddOnManagerWin *support)
: BOutlineListView(frame,name,B_SINGLE_SELECTION_LIST,B_FOLLOW_ALL_SIDES,B_WILL_DRAW | B_FRAME_EVENTS)
{
	// initialiser variables et pointer
	_supportWindow = support;
	_option = NULL;
}

/*** Destructeurs ***/
AddOnManagerListView::~AddOnManagerListView()
{
}

/**** traiter les messages ****/
void AddOnManagerListView::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
	default:
		BListView::MessageReceived(message);	
	}
}

// le petit triangle dans notre cas on met rien !!
void AddOnManagerListView::DrawLatch(BRect itemRect, int32 level, bool collapsed,bool highlighted, bool misTracked)
{}

/**** clique souris ****/
void AddOnManagerListView::MouseDown(BPoint where)
{
	int32	selectedItem;

	// selectionne l'item
	selectedItem = FullListIndexOf(where);
	if(selectedItem>=0)
	{
		AddOnManagerItem	*currentAddon = NULL;
		BMessage			*currentMessage = NULL;
		int32				buttons;

		// recuperer l'objet et voir si on peux afficher le pop-up
		currentAddon = (AddOnManagerItem *)(FullListItemAt(selectedItem));

		// ca ne doit pas etre un super item
		if(currentAddon->IsASuperItem())
		{
			// etendre ou diminuer
			if(currentAddon->IsExpanded())
			{
				// deselectionner tout
				DeselectAll();
			
				// puis diminuer
				Collapse(currentAddon);
			}
			else
				Expand(currentAddon);

			// on quitte
			return;
		}

		// verifier si l'addon est dispo
		if(!currentAddon->IsAvailable() || currentAddon->IsLock())
			return; 
		
		// selectionner l'item
		Select(IndexOf(where));

		// recuperer le message courant et verifier si on a un double clique
		if(LockLooper())
		{
			currentMessage = Looper()->CurrentMessage();
			UnlockLooper();
		}
		else
			g_MetosApp->SendDebbug("Error : LockLooper() in AddOnManagerListView::MouseDown()",VDEBUG_ERROR);
		
		// le message doit etre valide
		if(currentMessage==NULL)
			return;

		// on doit trouver quel bouton a ete utilise
		if(currentMessage->FindInt32("buttons",&buttons)!=B_OK)
			return;

		// affiche le menu popup		
		if(buttons & B_SECONDARY_MOUSE_BUTTON)
		{			
			// menu de selection
			DisplayPopUp(where);
		}
		else if(buttons & B_PRIMARY_MOUSE_BUTTON)
		{
			// drag and drop
			BuildDragDrop();
		}
	}
	else
		DeselectAll();
}

/**** selection d'un item de la liste ****/
void AddOnManagerListView::DisplayPopUp(BPoint &where)
{
	int32	index = 0;

	// on va creer l'objet en fonction de l'item selectionné dans la liste
	index = FullListCurrentSelection();
	if(index<0)
		return;	// pas d'item selectionné

	AddOnManagerItem	*currentAddon = NULL;
	bool				lock;

	// recuperer l'objet et voir si on peux afficher le pop-up
	currentAddon = (AddOnManagerItem *)(FullListItemAt(index));

	// on va voir si on peux creer un objet en plus
	lock = currentAddon->IsLock();
		
	// creation du popup en fonction de l'objet
	// si l'option n'est pas desactivé
	if(lock)
		return;

	// voir en fonction de l'objet selectionné
	// si on doit avoir un autre selectionné
	if(currentAddon->Type()==UNDEFINED_TYPE)
		return;
		
	// on doit avoir selectionné le bon type d'objet avant d'ajouter
	// un nouveau
	if(!FindDependencies(currentAddon))
		return;
		
	BuildMenuPoPup();
	BMenuItem *selected = _option->Go(ConvertToScreen(where));
	if(selected)
	{
		switch(_option->IndexOf(selected))
		{
		// creation d'un objet
		case 0:
			{
				// on va simuler le drag&drop
				BMessage	drop(M_DROP_ADDON_MSG);
			
				drop.AddString(VOBJECT_ADDON_SOURCE,currentAddon->AddonName());
				g_ObjectManager.SendMessage(&drop);
			}
			break;
		default:
			; // rien
		}
	}
}

/**** Drag & Drop ****/
void AddOnManagerListView::BuildDragDrop()
{
	// message de drag&drop
	AddOnManagerItem	*currentAddon = NULL;
	BString				className;
	int32				selectedAddOn;
	BBitmap				*icon = NULL;
	BBitmap				*bitmap = NULL;
	BMessage			dragMessage(M_DROP_ADDON_MSG);

	// voir en fonction de l'objet selectionné
	// si on doit avoir un autre selectionné
	selectedAddOn = FullListCurrentSelection();
	currentAddon = (AddOnManagerItem *)(FullListItemAt(selectedAddOn));
		
	// creation du drag&drop en fonction de l'objet
	// si l'option n'est pas desactivé
	if(currentAddon->IsLock())
		return;

	if(currentAddon->Type()==UNDEFINED_TYPE)
		return;
		
	// on doit avoir selectionné le bon type d'objet avant d'ajouter
	// un nouveau
	if(!FindDependencies(currentAddon))
		return;

	// fixer l'evenement pointer
	SetMouseEventMask(B_POINTER_EVENTS,0);			

	// recuperer l'image
	icon = currentAddon->Icon();
	bitmap = new BBitmap(BRect(0,0,31,31),B_CMAP8);
	memcpy(bitmap->Bits(),icon->Bits(),icon->BitsLength());	
	
	// ajouter les elements necessaire a la creation d'un objet
	dragMessage.AddString(VOBJECT_ADDON_SOURCE,currentAddon->AddonName());
	
	// initialise le drag&drop
	DragMessage(&dragMessage,bitmap,B_OP_ALPHA,BPoint(16,16));
}

/**** creer le popup en fonction de l'objet ****/
void AddOnManagerListView::BuildMenuPoPup()
{
	AddOnManagerItem	*currentItem = NULL;
	BString				itemName;

	// recuperer le nom de l'item
	currentItem = (AddOnManagerItem *)(FullListItemAt(CurrentSelection()));
	itemName = currentItem->ClassName();
	
	// verifier que l'on ne clique pas sur le meme objet
	if(_option!=NULL && (_option->Name() != itemName))
	{
		delete _option;
		_option = NULL;
	}
	
	// le menu existe t-il deja
	if(_option==NULL)
	{
		
		itemName.Prepend(" ");
		itemName.Prepend(g_Languages->GetTranslationFor(L_ADDON_NEW));
		_option = new BPopUpMenu(itemName.String());
		_option->AddItem(new BMenuItem(itemName.String(),NULL));
	}
}

/**** trouver les dependence d'un objet si elle sont correctes ****/
bool AddOnManagerListView::FindDependencies(AddOnManagerItem *addon)
{
	// un projet peux toujours etre ajouté
	if(addon->Type() == PROJECT_TYPE && !addon->IsLock())
		return true;

	// recuperer l'element courant
	if(_supportWindow->Object()==(uint32)NULL)
		return false;

	// chercher si on a le bon type par rapport aux dependances
	return addon->CheckDepencies((object_type)(_supportWindow->Type()));
}

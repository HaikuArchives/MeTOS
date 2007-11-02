/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/

/******************************/
/* MeTOS undo list view class */
/******************************/
#include "UndoManagerListView.h"
#include "UndoManagerItem.h"
#include "MetosMessageDefs.h"
#include "MetosUndoDefs.h"
#include "MetosApp.h"
#include "MetosGlobalDefs.h"
#include "MetosLanguagesDefs.h"
#include "VLanguage.h"

#include <interface/Window.h>
#include <interface/PopUpMenu.h>
#include <interface/MenuItem.h>
#include <stdio.h>

/**** constructeur ****/
UndoManagerListView::UndoManagerListView(BRect frame,const char *name)
: BListView(frame,name,B_SINGLE_SELECTION_LIST,B_FOLLOW_ALL,B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE)
{
	// construire le menu popup
	BuildMenu();
}

/**** destructeur ****/
UndoManagerListView::~UndoManagerListView()
{
	// liberer la memoire prise par le popup
	delete _actionMenu;
}

/**** gestion des messages ****/
void UndoManagerListView::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
		default:
			BListView::MessageReceived(message);
	}
}

/**** clique souris ****/
void UndoManagerListView::MouseDown(BPoint where)
{
	BPoint		point;
	uint32		button;
	int32		index = -1;
	int32		current = -1;

	GetMouse(&point,&button);

	// recuperer l'index de l'item
	index = IndexOf(where);
	current = CurrentSelection();
	if(index<0)
	{
		// as-ton au moins un item de selectionné !
		if(current<0)
			return;
			
		// on deselectionne alors
		DeselectItem(current);
	}
	else
	{
		// ok on peux le selectionner
		SelectItem(index,button,where);
	}	
}

/**** selection d'un item ****/
void UndoManagerListView::SelectItem(int32 index,uint32 button,BPoint &where)
{
	int32	current;
	
	// si un autre item est selectionné on le deselectionne
	current = CurrentSelection();
	if(current>=0 && current!=index)
		DeselectItem(current);
	
	// selectionner le nouvel item
	// pas si il l'est deja
	if(index!=current && ((button & B_PRIMARY_MOUSE_BUTTON) || index==0 ))
	{
		Select(index);
		ExpandItem();

		// rafraichir l'affichage
		Invalidate();
	}
	
	// on peux restorer que le premier item !
	if(index>0)
		return;

	// et si on a un clique droit
	// afficher le popup
	if(!(button & B_SECONDARY_MOUSE_BUTTON))
		return;
	
	_actionMenu->SetTargetForItems(this);
	BMenuItem *selectedItem = _actionMenu->Go(ConvertToScreen(where));
	if(selectedItem)
	{
		int32	selectedIndex;

		selectedIndex = _actionMenu->IndexOf(selectedItem);		
		switch(selectedIndex)
		{
		case 0:	// restore
			RestoreItem();
		case 1:	// effacer
			DeleteItem();
			break;
		default:
			;//rien
		}
	}
}

/**** deselection d'un item ****/
void UndoManagerListView::DeselectItem(int32 index)
{
	BListItem	*item = NULL;

	item = ItemAt(index);	
	item->SetHeight(ITEM_HEIGHT);

	// deselectionner l'item
	Deselect(index);
	
	// rafraichir l'affichage
	Invalidate();
}

/**** construire le popup ****/
void UndoManagerListView::BuildMenu()
{
	// construire le menu popup
	_actionMenu = new BPopUpMenu("Actions");
	_actionMenu->AddItem(new BMenuItem(g_Languages->GetTranslationFor(L_UNDO_RESTORE),NULL));
	_actionMenu->AddItem(new BMenuItem(g_Languages->GetTranslationFor(L_UNDO_DELETE),NULL));
}

/**** etendre l'item (la taille) ****/
void UndoManagerListView::ExpandItem()
{
	int32	current;

	// verifier que un item est selectionné
	current = CurrentSelection();
	if(current<0)
		return;

	BListItem	*item = NULL;
	int32		itemSize;

	// agrandir l'item
	item = ItemAt(current);
	itemSize = ((UndoManagerItem *)item)->ItemHeight();	
	item->SetHeight(itemSize);

	// invalider l'item
	InvalidateItem(current);
}

/**** Restorer (faire le undo) ****/
void UndoManagerListView::RestoreItem(int32 index)
{
	UndoManagerItem		*item = NULL;
	
	// trouver l'item courant
	// si on ne le donne pas a la fonction
	if(index<0)
		index = CurrentSelection();
		if(index<0)
			return;
		
	// trouver l'itme et restaurer
	item = (UndoManagerItem *)(ItemAt(index));
	if(item!=NULL)
		item->Restore();
}

/**** Effacer le undo de la liste ****/
void UndoManagerListView::DeleteItem(int32 index)
{
	// trouver l'item courant
	if(index<0)
		index = CurrentSelection();
		if(index<0)
			return;
		
	// proteger l'action
	if(LockLooper())
	{
		// effacer l'item de la liste
		delete (UndoManagerItem *)(RemoveItem(index));

		// deprotegé
		UnlockLooper();

		// rafraichir le nombre d'items
		g_UndoManager.SendMessage(M_REFRESH_NUMBER_ITEMS_MSG);
	}
	else
		g_MetosApp->SendDebbug("Error : LockLooper() in UndoManagerListView::DeleteItem()",VDEBUG_ERROR);
}

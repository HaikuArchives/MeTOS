/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
/***************************************/
/* list des addons (preferences) class */
/***************************************/
#include "PreferenceManagerListView.h"
#include "PreferenceItem.h"
#include "VPreference.h"
#include "MetosApp.h"
#include "MetosGlobalDefs.h"
#include "MetosLanguagesDefs.h"
#include "VLanguage.h"

#include <interface/Window.h>
#include <stdio.h>

/**** constructeur ****/
PreferenceManagerListView::PreferenceManagerListView(BRect frame,const char *name)
: BOutlineListView(frame,name,B_SINGLE_SELECTION_LIST,B_FOLLOW_ALL_SIDES,B_WILL_DRAW | B_FRAME_EVENTS)
{
	// ajouter les super items
	BuildList();
}

/**** destructeur ****/
PreferenceManagerListView::~PreferenceManagerListView()
{
}

/**** vider la liste et liberer la memoire ****/
void PreferenceManagerListView::ClearList()
{
	// liberer la memoire prise par les items
	BListItem			*item = NULL;
	int32				index;
	int32				nbItem;

	// deselectionner l'item si il y en a un de selectionne
	UnSelectItem();
	
	// liberer les sous categories
	index = 0;
	while(index<CountItems())
	{
		// selon le niveau on sait ce que c'est comme type d'item
		item = (BListItem *)(ItemAt(index));
		if(item->OutlineLevel()>0)
		{
			RemoveItem(index);
			delete (VPreference *)(item);
		}
		else
			index++;
	}

	// puis liberer les categories
	// il ne reste que ces dernieres
	nbItem = CountItems();
	for(index = 0;index<nbItem;index++)
		delete (PreferenceItem *)(RemoveItem((int32)0));
}

/**** gestion des messages ****/
void PreferenceManagerListView::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
	default:
		BListView::MessageReceived(message);
	}
}

/**** clique souris (selection) ****/
void PreferenceManagerListView::MouseDown(BPoint where)
{
	int32		selectedItem;
	int32		selected = CurrentSelection();

	// selectionne l'item si c'est pas le meme
	selectedItem = IndexOf(where);
	if(selectedItem == selected)
		return;

	// selection de l'item
	if(selectedItem>0)
		SelectItem(selectedItem);
}

/**** selectionner un item ****/
void PreferenceManagerListView::SelectItem(int32 index)
{
	BListItem	*item = NULL;

	// item selectionne
	item = ItemAt(index);
	if(item!=NULL && item->OutlineLevel()>0)
	{
		// deselectionner l'item
		UnSelectItem();

		// selectionner l'item dans la liste
		Select(index);
		
		// puis ajouter la vue
		if(LockLooper())
		{
			Window()->ChildAt(0)->AddChild((VPreference *)item);
			UnlockLooper();
		}
		else
			g_MetosApp->SendDebbug("Error : LockLooper() in PreferenceManagerListView::SelectItem()",VDEBUG_ERROR);
	}
}

/**** deselectionner un item ****/
void PreferenceManagerListView::UnSelectItem()
{
	int32	selected;
	
	// il doit y avoir un item courant
	selected = CurrentSelection();
	if(selected<0)
		return;
	
	BListItem	*item = NULL;

	// deselectionner l'item courant
	item = ItemAt(selected);
	Deselect(selected);
	
	// puis ajouter la vue
	if(LockLooper())
	{
		Window()->ChildAt(0)->RemoveChild((VPreference *)item);
		UnlockLooper();
	}
	else
		g_MetosApp->SendDebbug("Error : LockLooper() in PreferenceManagerListView::UnSelectItem()");
}

/**** selectionner le premier item valide ****/
void PreferenceManagerListView::SelectFirstItem()
{
	BListItem	*item = NULL;
	int32		nbItems;

	// rechercher un item valide
	nbItems = CountItems();
	for(int32 i=0;i<nbItems;i++)
	{
		item = ItemAt(i);
		if(item!=NULL && item->OutlineLevel()==1)
		{
			// selectionner l'item puis sortir
			SelectItem(i);
			return ;
		}
	}
}

/**** ajouter un item ****/
bool PreferenceManagerListView::AddPreferences(BList *prefsList)
{
	bool	state = false;
	
	// verifier la validite de le liste
	if(prefsList==NULL)
		return state;

	if(prefsList->CountItems()==0)
		return state;

	//ajouter les items
	VPreference	*item = NULL;
	int32		nbItems;
	int32		index;
	
	nbItems = prefsList->CountItems();
	for(int32 i=0;i<nbItems;i++)
	{
		// recuperer le VPreference
		item = (VPreference *)(prefsList->ItemAt(i));
		index = FindItemPlace(item);
		// positionner l'item
		if(index>0)
			AddItem(item,index);
	}	

	// retourner l'etat
	return state;
}

/**** trouver ou ajouter l'item ****/
int32 PreferenceManagerListView::FindItemPlace(VPreference *newItem)
{
	// on doit avoir un item
	if(newItem==NULL)
		return -1;
		
	// chercher le nouvel emplecement
	BListItem	*item= NULL;
	int32		nbItems;
	bool		exit = false;
	
	nbItems = CountItems();
	for(int32 i=0;i<nbItems;i++)
	{
		// chercher parmi tous les items
		item = ItemAt(i);
		if(item!=NULL && item->OutlineLevel()==0)
		{
			// si on doit sortir on sort !
			if(exit)
				return i;
			
			// ok on a trouvé le bon sous-groupe
			if(((PreferenceItem *)item)->Group() == newItem->Group())
				exit = true;
		}
	}
	
	// on a rien trouvé
	// on ajoute a la fin
	return nbItems;
}

/**** ajouter les super-items ****/
void PreferenceManagerListView::BuildList()
{
	AddItem(new PreferenceItem("MeTOS",NULL,METOS_GROUP));
	AddItem(new PreferenceItem(g_Languages->GetTranslationFor(L_PREFERENCES_CODE_GENERATOR),NULL,CODE_GENERATION_GROUP));
	AddItem(new PreferenceItem(g_Languages->GetTranslationFor(L_PREFERENCES_TRANSLATOR),NULL,TRANSLATOR_GROUP));
	AddItem(new PreferenceItem(g_Languages->GetTranslationFor(L_PREFERENCES_TOOLS),NULL,TOOLS_GROUP));
	AddItem(new PreferenceItem(g_Languages->GetTranslationFor(L_PREFERENCES_OTHER),NULL,OTHER_GROUP));
}

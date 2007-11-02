/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
/***************************************************/
/* Liste qui regroupe la liste des objets du projet */
/***************************************************/
#include "ObjectManagerWin.h"
#include "ObjectManagerListView.h"
#include "ObjectManagerItem.h"
#include "MetosApp.h"
#include "VObject.h"
#include "MetosMessageDefs.h"
#include "MetosGlobalDefs.h"
#include "MetosLanguagesDefs.h"
#include "PropertieManagerWin.h"
#include "VLanguage.h"

#include <interface/ListView.h>
#include <interface/InterfaceDefs.h>
#include <stdio.h>

/*** Constructeur ***/
ObjectManagerListView::ObjectManagerListView(BRect frame,const char *name)
: BListView(frame,name,B_MULTIPLE_SELECTION_LIST,B_FOLLOW_ALL_SIDES,B_WILL_DRAW | B_FRAME_EVENTS)
{
	// message affiché si il n'y a pas d'objet
	_display = g_Languages->GetTranslationFor(L_OBJECT_MESSAGE);
}

/*** Destructeurs ***/
ObjectManagerListView::~ObjectManagerListView()
{
}

/**** traiter les messages ****/
void ObjectManagerListView::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
	default:
		BListView::MessageReceived(message);	
	}
}

/**** clique souris ****/
void ObjectManagerListView::MouseDown(BPoint where)
{
	int32		selectedItem;
	BMessage	selected(M_SELECT_OBJECT_MSG);

	// selectionne l'item
	selectedItem = IndexOf(where);
	if(selectedItem<0)
		DeselectAll();
	else
	{
		BMessage	*currentMessage = NULL;
		VObject		*selectObject = NULL;
		int32		buttons;
		bool		shift;
		
		// selection de l'objet
		shift = modifiers() & B_SHIFT_KEY;
		selectObject = ((ObjectManagerItem *)(ItemAt(selectedItem)))->Object();
		if(selectObject==NULL)
			return;
				
		// recuperer le message courant et verifier si on a un double clique
		if(LockLooper())
		{
			currentMessage = Looper()->CurrentMessage();
			UnlockLooper();
		}		
		else
			g_MetosApp->SendDebbug("Error : LockLooper() in ObjectManagerListView::MouseDown()",VDEBUG_ERROR);

		// le message doit etre valide
		if(currentMessage==NULL)
			return;

		// on doit trouver quel bouton a ete utilise
		if(currentMessage->FindInt32("buttons",&buttons)!=B_OK)
			return;
		
		// convertir le point
		ConvertToScreen(&where);

		// on dit que l'on selectionne un objet !
		selected.AddBool(M_SHIFT_SELECTION,shift);
		selected.AddInt32(M_OBJECT_SELECTED,selectObject->Id());		
		selected.AddInt32(M_MOUSE_BUTTON,buttons);
		selected.AddPoint(M_POINT_SELECTED,where);
	}
	
	// on fait une selection (peux etre vide)
	g_ObjectManager.SendMessage(&selected);
}

/**** affichage les objets ou rien ****/
void ObjectManagerListView::Draw(BRect updateRect)
{
	// si on a des fonctions on dessine normalement
	if(CountItems()>0)
		BListView::Draw(updateRect);
	else
	{
		// sinon on affiche un message
		BRect	bounds = Bounds();
		float	messageSize = StringWidth(_display.String());
		
		DrawString(_display.String(),BPoint((bounds.Width()-messageSize)/2,(bounds.Height()-10)/2));	
	}
}

/**** on redimentionne la vue ****/
void ObjectManagerListView::FrameResized(float width,float height)
{
	// fonction normale
	BListView::FrameResized(width,height);

	// si on a un objet on dessine normalement
	if(CountItems()==0)
		Invalidate();	
}

/**** selectionner l'objet dans la liste ****/
void ObjectManagerListView::SelectObject(VObject *select,bool shift)
{
	int32		index = -1;
	
	// trouver l'objet a selectionner
	index = FindIndexOfObject(select);
	
	// si il y en a un on regarde lequel
	if(index<0)
		return;

	// selection de l'objet dans la liste
	// donner l'info sur l'objet selectionné
	if(select->IsAttached())
		Select(index,shift);
}

/**** trouver un objet a partir de son handler ****/
VObject *ObjectManagerListView::FindObjectFromHandler(BHandler *handler)
{
	// liberer la memoire prise par les items
	int32	nbItem;
	VObject	*current = NULL;
	
	nbItem = CountItems();
	for(int32 index = 0;index<nbItem;index++)
	{
		current = ((ObjectManagerItem *)(ItemAt(index)))->Object();
		if(current->Object() == (BArchivable *)handler)
			return current;
	}
	
	return NULL;
}

/**** ajouter un objet a la liste ****/
void ObjectManagerListView::AddVObject(VObject *newObject,VObject *dest,int32 level)
{
	ObjectManagerItem	*newItem = NULL;
	BMessage			*properties = NULL;
	int32				place = 0;
	
	// creer un item de liste et fixer le level
	newItem = new ObjectManagerItem(newObject);

	// modifier le level
	properties = newObject->Properties();
	properties->ReplaceInt32(VOBJECT_LEVEL,level);
	
	// trouver la position ou l'ajouter
	// en rapport avec la destination
	place = FindPlaceForNewObject(dest);
	
	// ajouter a la liste
	// et rafraichir la liste
	if(LockLooper())
	{
		AddItem(newItem,place);
		Invalidate();
		UnlockLooper();	
	}
	else
		g_MetosApp->SendDebbug("Error : LockLooper() in ObjectManagerListView::AddVObject()",VDEBUG_ERROR);
}

/**** trouver l'index de l'objet dans la liste ****/
int32 ObjectManagerListView::FindIndexOfObject(VObject *object)
{
	// liberer la memoire prise par les items
	int32		nbItem;
	VObject		*current;
	
	nbItem = CountItems();
	for(int32 index = 0;index<nbItem;index++)
	{
		current = (((ObjectManagerItem *)(ItemAt(index)))->Object());
		if(current == object)
			return index;
	}
	
	return -1;
}

/**** trouver l'objet par rapport a l'id ****/
VObject	*ObjectManagerListView::FindObjectByID(uint32 id)
{
	VObject				*current = NULL;
	int32				nbItems = 0;
	
	// parcourir la liste des objets
	nbItems = CountItems();
	for(int32 i=0;i<nbItems;i++)
	{
		current = ((ObjectManagerItem *)(ItemAt(i)))->Object();
		if(current->Id()==id)
			return current;
	}

	// pas trouvé
	return NULL;
}

/**** trouver si un objet est selectionné ****/
bool ObjectManagerListView::ObjectIsSelected(VObject *selected)
{
	ObjectManagerItem	*item = NULL;
	int32				selectedItem;
	int32				index = 0;
	
	// parcourir la liste des objets selectionnés
	while((selectedItem = CurrentSelection(index))>=0)
	{
		item = (ObjectManagerItem *)(ItemAt(selectedItem));
		if(item->Object() == selected)
			return true;
		index++;
	}

	// il n'est pas dans la liste
	return false;
}


/**** trouver ou ajouter un objet ****/
int32 ObjectManagerListView::FindPlaceForNewObject(VObject *destination)
{
	VObject	*current = NULL;
	int32	selectedIndex;
	int32	selectedLevel;
	int32	nbItems;
	
	// l'objet en parametre doit exister
	if(destination==NULL)
		return 0;
	
	// on cherche quel level
	nbItems = CountItems();
	selectedIndex = FindIndexOfObject(destination)+1;
	selectedLevel = destination->Level();
	for(int32 i=selectedIndex;i<nbItems;i++)
	{
		current = ((ObjectManagerItem *)(ItemAt(i)))->Object();
		if(current->Level()<=selectedLevel)
			return i;
	}
	
	// on est arrive a la fin
	return nbItems;
}

/**** changer le level des objets a partir de l'indice "index" ****/
status_t ObjectManagerListView::ChangeLevel(int32 decal,VObject *object)
{
	VObject		*current = NULL;
	BMessage	*properties = NULL;
	int32		objectIndex;
	int32		objectLevel;
	int32		nbItems;
	
	// l'objet en parametre doit exister
	if(object==NULL)
		return B_ERROR;
	
	nbItems = CountItems();
	objectIndex = FindIndexOfObject(object) +1;
	objectLevel = object->Level();
	for(int32 index = objectIndex;index<nbItems;index++)
	{
		// verifier qu'on doit toujours decaler les objets
		current = ((ObjectManagerItem *)(ItemAt(index)))->Object();
		if(current->Level()<=objectLevel)
			break;
		
		// decaler
		properties = current->Properties();
		if(properties!=NULL)
			properties->ReplaceInt32(VOBJECT_LEVEL,(current->Level()+decal));
	}

	// rafraichir l'affichage
	Invalidate();

	return B_OK;
}

/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
/**************************************************************/
/* Liste qui regroupe la liste des Controles(Addon) du projet */
/**************************************************************/
#include "PropertieManagerWin.h"
#include "PropertieManagerListView.h"
#include "VPropertie.h"
#include "CColumnView.h"
#include "MetosMessageDefs.h"
#include "MetosGlobalDefs.h"
#include "VObject.h"
#include "MetosApp.h"
#include "MetosLanguagesDefs.h"
#include "VLanguage.h"

#include <interface/InterfaceDefs.h>


/*** Constructeur ***/
PropertieManagerListView::PropertieManagerListView(BRect frame,const char *name,PropertieManagerWin *support)
: BListView(frame,name,B_SINGLE_SELECTION_LIST,B_FOLLOW_ALL,B_WILL_DRAW | B_FRAME_EVENTS)
{
	// initialiser
	_supportWindow = support;
	_columnView = NULL;
	_display = g_Languages->GetTranslationFor(L_PROPERTIES_MESSAGE);
}

/*** Destructeurs ***/
PropertieManagerListView::~PropertieManagerListView()
{
}

/**** traiter les messages ****/
void PropertieManagerListView::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
	// on recoit une demande de mise a jour d'un plug-in properties
	case M_PROPERTIES_CHANGE_MSG:
		break;
	default:
		BListView::MessageReceived(message);	
	}
}

/**** attachement a la fenetre ****/
void PropertieManagerListView::AttachedToWindow()
{
	// fonction parente
	BListView::AttachedToWindow();

	// trouver la vue des colonnes
	_columnView = dynamic_cast<CColumnView *>(Parent()->FindView("column-properties-view"));
}

/**** dettachement de la fenetre ****/
void PropertieManagerListView::DetachedFromWindow()
{
	// deselectionner l'item courant
	UnSelectItem();
	
	// fonction parente
	BListView::DetachedFromWindow();
}

/**** activation de la fenetre ****/
void PropertieManagerListView::WindowActivated(bool state)
{
	// annuler la selection precedante
	// si on quitte la fenetre
	if(!state)
		UnSelectItem();

	// fonction d'origine
	BView::WindowActivated(state);
}

/**** clique souris ****/
void PropertieManagerListView::MouseDown(BPoint where)
{
	BMessage	*currentMessage = NULL;
	int32		buttons;
	int32		selected = CurrentSelection();
	int32		selectedItem;


	// recuperer le message courant et verifier si on a un double clique
	if(LockLooper())
	{
		currentMessage = Looper()->CurrentMessage();
		UnlockLooper();
	}
	else
		g_MetosApp->SendDebbug("Error : LockLooper() in PropertieManagerListView::MouseDown",VDEBUG_ERROR);
		
	// le message doit etre valide
	if(currentMessage==NULL)
		return;

	// on doit trouver quel bouton a ete utilise
	if(currentMessage->FindInt32("buttons",&buttons)!=B_OK)
		return;

	// enlever la selection sur le precedent
	UnSelectItem();

	// selectionne l'item
	selectedItem = IndexOf(where);
	if(selectedItem == selected || buttons & B_SECONDARY_MOUSE_BUTTON)
		return;
	
	// on clique dans la liste
	if(selectedItem>=0)
	{
		// activer la fenetre si c'est pas le cas
		if(!(Window()->IsActive()))
			Window()->Activate(true);
		
		// selectionner l'item puis appeler la fonction de l'item
		Select(selectedItem);
		SelectItem();
	}
}

/**** affichage les proprietes ou rien ****/
void PropertieManagerListView::Draw(BRect updateRect)
{
	// si on a un objet on dessine normalement
	if(CountItems()>0)
	{
		// si on a trouvé la vue des colonnes
		if(_columnView!=NULL && _columnView->CountItems()>0)
		{
			float	size;
			
			// mettre a jour dans les VPropertie
			size = ((BRect *)(_columnView->RectFrom(0)))->Width() + SIZE_START_COLOMN + 1;	
			for(int32 index=0;index<CountItems();index++)
				((VPropertie *)(ItemAt(index)))->SetSizeProperties(size);
		}
		
		// appel du draw standard
		BListView::Draw(updateRect);
	}
	else
	{
		// sinon on affiche un message
		BRect	bounds = Bounds();
		float	messageSize = StringWidth(_display.String());
		
		DrawString(_display.String(),BPoint((bounds.Width()-messageSize)/2,(bounds.Height()-10)/2));	
	}
}

/**** on redimentionne la vue ****/
void PropertieManagerListView::FrameResized(float width,float height)
{
	// fonction normale
	BListView::FrameResized(width,height);

	// si on a un pas d'objet on invalide pour redessiner le texte
	if(_supportWindow->Object()==(uint32)NULL)
		Invalidate();	
}

/**** selection d'un item de la liste ****/
void PropertieManagerListView::SelectItem()
{
	int32	index = 0;

	// verifier que l'item est selectionné
	index = CurrentSelection();
	if(index<0)
		return;	// pas d'item selectionné
		
	// on appel la fonction de l'item
	VPropertie	*currentProperties = ((VPropertie *)ItemAt(index));
	
	// verifier si il est editable
	if(!currentProperties->IsEnable())
		return;

	// repositionner le rectangle de l'item
	// si on a scrollé ( on rescal a zero en fait)
	BRect		itemFrame = ItemFrame(index);
	BRect		bounds = Bounds();

	itemFrame.OffsetBy(0,-bounds.top);

	// selectionner l'objet
	// est rafraichir les proprietes
	currentProperties->SetProperties(((PropertieManagerWin *)Window())->Properties());
	itemFrame.left += currentProperties->SizeProperties();
	currentProperties->SelectItem(this,itemFrame);

	// proteger
	if(LockLooper())
	{
		// rafraichir l'affichage
		Invalidate();

		// debloquer
		UnlockLooper();
	}
	else
		g_MetosApp->SendDebbug("Error : LockLooper() in PropertieManagerListView::SelectItem()",VDEBUG_ERROR);
}

/**** deselection de l'item en edition ****/
void PropertieManagerListView::UnSelectItem()
{
	int32	index = 0;

	index = CurrentSelection();
	if(index<0)
		return;	// pas d'item selectionné
	
	// on appel la fonction de l'item
	VPropertie	*currentProperties = ((VPropertie *)ItemAt(index));
	
	// deselectionner graphiquement l'item
	if(LockLooper())
	{
		Deselect(index);
		UnlockLooper();
	}
	else
		g_MetosApp->SendDebbug("Error : LockLooper() in PropertieManagerListView::UnSelectItem()",VDEBUG_ERROR);

	// verifier si il est editable
	if(!currentProperties->IsEnable())
		return;

	BMessage	newValue(M_PROPERTIES_CHANGE_MSG);
	BRect		bounds = ItemFrame(index);

	// mise a jour de la taille
	bounds.left += 151;

	// deselectionner l'objet
	currentProperties->UnSelectItem(this,bounds);

	// proteger
	if(LockLooper())
	{
		Invalidate();

		// deproteger
		UnlockLooper();
	}
	else
		g_MetosApp->SendDebbug("Error : LockLooper() in PropertieManagerListView::UnSelectItem()",VDEBUG_ERROR);
		
	// la propriete doit avoir changé
	if(!(currentProperties->IsChange()))
		return;
		
	// recuperer la valeur pour mettre a jour l'objet
	// verifier si on doit rafraichir l'objet
	// ajouter l'objet pour savoir quel objet mettre a jour
	// et ajouter la valeur de son index
	currentProperties->Properties(&newValue);
	newValue.AddInt32(M_OBJECT_SELECTED,_supportWindow->Object());
	newValue.AddInt32(M_PROPERTIES_INDEX,currentProperties->Index());
	newValue.AddBool(M_NEED_REFRESH,currentProperties->IsNeedRefresh());
	newValue.AddBool(M_NEED_INVALIDATE,true);
	newValue.AddBool(M_NEED_UNDO_ACTION,true);

	// ok on sait que la propriete a changé
	currentProperties->SetChange(false);

	// changer la valeur
	g_PropertieManager.SendMessage(&newValue);
}

/**** gerer le clavier avant reception par un objet graphique d'un item ****/
void PropertieManagerListView::ManageKeyUp(BMessage *message)
{
	int32		raw_char = 0;
		
	// on doit trouver le caractere
	if(message->FindInt32("raw_char",&raw_char)!=B_OK)
		return;
		
	switch(raw_char)
	{
	// selection de l'item suivant
	// si aucun n'est selectionné c'est le premier qui le sera
	// de même si on est sur le dernier on va selectionné le premier
	case B_TAB:
		{
			int32	item = 0;
			
			// selection de l'item suivant
			if(CurrentSelection()>=0 && CurrentSelection()<(CountItems()-1))
				item = CurrentSelection() + 1;
			
			// enlever la selection precedante
			UnSelectItem();

			// selectionner l'item puis appeler la fonction de l'item
			Select(item);
			SelectItem();
		}
		break;
	// deselection
	case B_ESCAPE:
		UnSelectItem();
		break;
	default:
		; // on fait rien
	}
}
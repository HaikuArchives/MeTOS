/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
/*****************************/
/* MeTOS code listview class */
/*****************************/
#include "CPPCodeListView.h"
#include "CPPCodeWin.h"
#include "MetosMessageDefs.h"
#include "CColumnView.h"
#include "CPPCodeItem.h"
#include "VLanguage.h"

#include <interface/PopUpMenu.h>
#include <interface/MenuItem.h>

/**** constructeur ****/
CPPCodeListView::CPPCodeListView(BRect frame,const char *name)
: BListView(frame,name,B_SINGLE_SELECTION_LIST,B_FOLLOW_ALL_SIDES,B_WILL_DRAW | B_FRAME_EVENTS)
{
	// message
	_display = (g_CPPCodeWin->Language())->GetTranslationFor("CPP_LIST_MESSAGE");

	// construire le menu
	BuildPopUpMenu();
}

/**** destructeur ****/
CPPCodeListView::~CPPCodeListView()
{
	delete _popUpMenu;
}

/**** gestion desmessages ****/
void CPPCodeListView::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
	default:
		BListView::MessageReceived(message);
	}
}

/**** attachement a la fenetre ****/
void CPPCodeListView::AttachedToWindow()
{
	// fonction parente
	BListView::AttachedToWindow();

	// trouver la vue des colonnes
	_columnView = dynamic_cast<CColumnView *>(Parent()->FindView("column-files-view"));
}

/**** clique souris ****/
void CPPCodeListView::MouseDown(BPoint where)
{
	BMessage	*currentMessage = NULL;
	int32		buttons = 0;
	int32		current = 0;
	
	// recuperer le message courant et verifier si on a un double clique
	if(LockLooper())
	{
		currentMessage = Looper()->CurrentMessage();
		UnlockLooper();
	}
		
	// le message doit etre valide
	if(currentMessage==NULL)
		return;
		
	// on doit trouver quel bouton a ete utilise
	if(currentMessage->FindInt32("buttons",&buttons)!=B_OK)
		return;

	// trouver la selection
	current = IndexOf(where);
	if(current<0)
		return;
		
	// selection de l'item
	Select(current);
	
	if(buttons & B_SECONDARY_MOUSE_BUTTON)
	{
		// afficher le popup
		BMenuItem *selected = _popUpMenu->Go(ConvertToScreen(where));
		if(selected)
			Window()->PostMessage(selected->Message());
	}
}

/**** affichage les proprietes ou rien ****/
void CPPCodeListView::Draw(BRect updateRect)
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
				((CPPCodeItem *)(ItemAt(index)))->SetColumnRect(size);
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
void CPPCodeListView::FrameResized(float width,float height)
{
	// fonction normale
	BListView::FrameResized(width,height);

	// si on a un objet on dessine normalement
	if(CountItems()==0)
		Invalidate();	
}

/***** construction du menu ****/
void CPPCodeListView::BuildPopUpMenu()
{
	_popUpMenu = new BPopUpMenu("code-selection");
	_popUpMenu->AddItem(new BMenuItem((g_CPPCodeWin->Language())->GetTranslationFor("CPP_MENU_HEADER"),new BMessage(M_HEADER_DISPLAY_MSG)));
	_popUpMenu->AddItem(new BMenuItem((g_CPPCodeWin->Language())->GetTranslationFor("CPP_MENU_SOURCE"),new BMessage(M_SOURCE_DISPLAY_MSG)));
}
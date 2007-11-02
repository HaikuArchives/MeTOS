/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
/******************************/
/* fenetre de gestion du undo */
/******************************/
#include "MetosApp.h"
#include "UndoManagerWin.h"
#include "UndoManagerListView.h"
#include "UndoManagerItem.h"
#include "MetosUndoDefs.h"
#include "MetosMessageDefs.h"
#include "MetosGlobalDefs.h"
#include "ObjectManagerWin.h"
#include "VPreferenceFile.h"
#include "CStatusView.h"
#include "CStatusItem.h"
#include "MetosLanguagesDefs.h"
#include "VLanguage.h"

#include <app/Messenger.h>
#include <ScrollView.h>
#include <interface/View.h>


/**** cnostructeur ****/
UndoManagerWin::UndoManagerWin(BRect frame)
: BWindow(frame,g_Languages->GetTranslationFor(L_UNDO_TITLE), B_FLOATING_WINDOW, B_NOT_ZOOMABLE | B_WILL_ACCEPT_FIRST_CLICK)
{
	BScrollView		*scrollView = NULL;
	BRect			listRect = Bounds();
	BView			*_undoManagerView;

	// vue support
	listRect.bottom -= B_V_SCROLL_BAR_WIDTH;	
	_undoManagerView = new BView(listRect,"undoManagerView",B_FOLLOW_ALL_SIDES, B_WILL_DRAW);
	_undoManagerView->SetViewColor(VIEW_COLOR);

	// status
	_statusView = new CStatusView(BRect(listRect.left,listRect.bottom+1,listRect.right,listRect.bottom+B_V_SCROLL_BAR_WIDTH),"Status");
	_statusView->AddItem(new CStatusItem("nb-items",g_Languages->GetTranslationFor(L_UNDO_UNDOS),85));
	AddChild(_statusView);

	// ajouter la liste
	listRect.right -= B_H_SCROLL_BAR_HEIGHT;
	listRect.InsetBy(2,2);
	_undoListView = new UndoManagerListView(listRect,"navigation-undo-list");
	scrollView = new BScrollView("Undo",_undoListView,B_FOLLOW_ALL_SIDES,B_WILL_DRAW | B_FRAME_EVENTS,false,true);

	// ajouter les vues
	_undoManagerView->AddChild(scrollView);
	AddChild(_undoManagerView);
	
	// charger les preferences
	RefreshPreferences();
}

/**** destructeur ****/
UndoManagerWin::~UndoManagerWin()
{
	int32	nbItem;
	
	// sauver les prefs
	SavePreferences();
	
	nbItem = _statusView->CountItems();
	for(int32 index = 0;index<nbItem;index++)
		delete _statusView->RemoveItem((int32)0);

	// vider la liste 
	ClearList();
}

/**** gestion des messages ****/
void UndoManagerWin::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
	case UNDO_ACTION_MSG:
		{
			// detacher le message, on le garde dans l'item
			message = DetachCurrentMessage();

			// creer un item
			AddNewUndoAction(message);
		}
		break;
	// appel depuis l'exterieur pour restorer
	// l'item en haut de la liste
	case UNDO_RESTORE_MSG:
		_undoListView->RestoreItem(0);
		_undoListView->DeleteItem(0);
		break;
	// rafraichir l'affichage
	case M_REFRESH_NUMBER_ITEMS_MSG:
		RefreshDisplayNumberItems();
		break;
	// preferences
	case M_PREFERENCE_MSG:
		RefreshPreferences();
		_undoListView->Invalidate();
		break;
	default:
		BWindow::MessageReceived(message);
	}
}

/**** fermeture de la fenetre ****/
bool UndoManagerWin::QuitRequested()
{
	// quitter
	if(g_DoQuit)
		return (true);
	
	return	(false);
}

/**** archiver la liste des actions ****/
status_t UndoManagerWin::ArchiveUndo(BMessage *archive)
{
	// verifier que l'archive est valide
	if(archive==NULL)
		return B_ERROR;

	//sauver la liste des actions dans le projet
	UndoManagerItem		*item = NULL;
	BMessage			*datas = NULL;
	status_t			result = B_OK;
	int32				nbItems;

	nbItems = _undoListView->CountItems();
	for(int32 i=0;i<nbItems;i++)
	{
		// item courant
		item = (UndoManagerItem *)(_undoListView->ItemAt(i));
		
		// données
		datas = item->Infos();
		result &= archive->AddMessage(UNDO_LIST_ITEM,datas);
	}	
	
	// resultat
	return result;
}

/**** recuperer les undo contenu dans une archive ****/
status_t UndoManagerWin::InstantiateUndo(BMessage *archive)
{
	// verifier que l'archive est valide
	if(archive==NULL)
		return B_ERROR;

	//sauver la liste des actions dans le projet
	BMessage			*datas = NULL;
	BMessage			buffer;
	status_t			result = B_OK;
	int32				index;

	// a ce niveau on va toujours effacer ce que la liste peux contenir
	// car cette fonction est appelée que lors d'un chargement de projet
	if(Lock())
	{
		ClearList();
		Unlock();
	}
	else
		g_MetosApp->SendDebbug("Error : Lock() in UndoManagerWin::InstantiateUndo()",VDEBUG_ERROR);

	index = 0;
	while((result = archive->FindMessage(UNDO_LIST_ITEM,index,&buffer))==B_OK)
	{
		// copier le message
		datas = new BMessage();
		*datas = buffer;
		
		// creer l'item
		AddNewUndoAction(datas);

		// passer au suivant
		index++;
	}
	
	// resultat
	return result;
}

/**** vider la liste ****/
void UndoManagerWin::ClearList()
{
	// liberer la memoire prise par les items
	int32	nbItem;
	
	// on efface l'item
	nbItem = _undoListView->CountItems();
	for(int32 index = 0;index<nbItem;index++)
		delete (UndoManagerItem *)(_undoListView->RemoveItem((int32)0));
}

// ==========================
// = Gestion de l'affichage =
// ==========================

/**** mettre a jour le nombre d'items affiché ****/
void UndoManagerWin::RefreshDisplayNumberItems()
{
	BString		buffer;

	// afficher le nombre d'items
	buffer = "Undos : ";
	buffer << _undoListView->CountItems();
	(_statusView->ItemAt(0))->SetLabel(buffer.String());
	
	if(Lock())
	{
		// rafraichir l'affichage
		_statusView->Invalidate();
		Unlock();
	}
	else
		g_MetosApp->SendDebbug("Error : Lock() in UndoManagerWin::RefreshDisplayNumberItems()",VDEBUG_ERROR);
}

// =================================
// = Gestion des Undos de la liste =
// =================================

/**** ajouter un item ****/
void UndoManagerWin::AddNewUndoAction(BMessage *message)
{
	// on doit tout de meme avoir un message
	// a ajouter sinon il y a une erreur
	if(message==NULL)
		return;
	
	// verifier si on depasse pas le nombre de undo
	int32		nbUndo = 0;
	
	// effacer l'ancien message
	if(Lock())
	{
		nbUndo = _undoListView->CountItems()-1;
		if(nbUndo>=_undoMax)
			for(int32 index=nbUndo;index>=_undoMax;index--)
				delete (UndoManagerItem *)(_undoListView->RemoveItem(index));

		// ajouter le nouvel item
		_undoListView->AddItem(new UndoManagerItem(message),0);	
		Unlock();

		// rafraichir l'affichage
		RefreshDisplayNumberItems();
	}
	else
		g_MetosApp->SendDebbug("Error : Lock() in UndoManagerWin::AddNewUndoAction()",VDEBUG_ERROR);
}

// ===========================
// = Gestion des preferences =
// ===========================

/**** rafraichir les preferences ****/
void UndoManagerWin::RefreshPreferences()
{
	UndoManagerItem		*item = NULL;
	int32				nbItems;
	
	// nombre de undo max
	// on enleve 1 au nombre car on calcul avec 0 compté
	_undoMax = g_MeTOSPrefs->GetInt32(40,"max-undo");
	_undoMax--;

	// rafraichir tous les items de la liste
	nbItems = _undoListView->CountItems();
	for(int32 index=0;index<nbItems;index++)
	{
		item = (UndoManagerItem *)(_undoListView->ItemAt(index));
		if(item!=NULL)
			item->RefreshPreferences();
	}	
}

/**** sauver les preferences ****/
void UndoManagerWin::SavePreferences()
{
	// charger les preferences
	g_MeTOSPrefs->Load();
	g_MeTOSPrefs->SetRect(Frame(),"undoManager-rect");
	g_MeTOSPrefs->Save();
}

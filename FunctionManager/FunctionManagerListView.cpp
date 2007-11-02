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
#include "FunctionManagerListView.h"
#include "FunctionManagerItem.h"
#include "ObjectManagerWin.h"
#include "MetosMessageDefs.h"
#include "MetosGlobalDefs.h"
#include "VPreferenceFile.h"
#include "VObject.h"
#include "FunctionItem.h"
#include "MetosApp.h"
#include "MetosLanguagesDefs.h"
#include "VLanguage.h"

#include <storage/NodeInfo.h>
#include <interface/InterfaceDefs.h>
#include <interface/PopUpMenu.h>
#include <interface/MenuItem.h>

// debug
#include <iostream.h>

/*** Constructeur ***/
FunctionManagerListView::FunctionManagerListView(BRect frame,const char *name,PropertieManagerWin *support)
: BOutlineListView(frame,name,B_SINGLE_SELECTION_LIST,B_FOLLOW_ALL,B_WILL_DRAW | B_FRAME_EVENTS)
{
	// initialisation
	_supportWindow = support;
	_nbCheckedFunctions = 0;
	_display = g_Languages->GetTranslationFor(L_FUNCTIONS_MESSAGE);
	
	// creation du menu
	BuildActionMenu();
}

/*** Destructeurs ***/
FunctionManagerListView::~FunctionManagerListView()
{	
}

/**** gestion des messages ****/
void FunctionManagerListView::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
	// sauver la selection de fonctions comme defaut
	case M_SAVEASDEFAULT_FUNTION_MSG:
		SaveListAsDefault();
		break;
	// trouver le meilleur constructeur
	case M_FINDBEST_CONSTRUCTOR_MSG:
		SelectBestConstructor();
		break;
	// gerer le drag&drop d'un fichier headers
	case B_SIMPLE_DATA:
		DragAndDropHeader(message);
		break;
	default:
		BOutlineListView::MessageReceived(message);	
	}
}

/**** clique souris ****/
void FunctionManagerListView::MouseDown(BPoint where)
{
	BMessage	*currentMessage = NULL;
	int32		buttons;

	// recuperer le message courant et verifier si on a un double clique
	if(LockLooper())
	{
		currentMessage = Looper()->CurrentMessage();
		UnlockLooper();
	}
	else
		g_MetosApp->SendDebbug("Error : LockLooper() in FunctionManagerListView::MouseDown()",VDEBUG_ERROR);
		
	// le message doit etre valide
	if(currentMessage==NULL)
		return;

	// on doit trouver quel bouton a ete utilise
	if(currentMessage->FindInt32("buttons",&buttons)!=B_OK)
		return;

	// activer la fenetre si c'est pas le cas
	if(!(Window()->IsActive()))
		Window()->Activate(true);

	// si c'est le bouton droit on defini la liste comme celle
	// utilisée par defaut pour cette objet
	if(buttons & B_SECONDARY_MOUSE_BUTTON)
	{
		// ok on affiche le menu et on traite l'action
		BMenuItem *selectedItem = _actionMenu->Go(ConvertToScreen(where));
		if(selectedItem!=NULL)
			BMessenger(this).SendMessage(selectedItem->Message());
	}
	else
	{
		FunctionItem	*item = NULL;
		int32		selected;

		// selectionne l'item
		selected = FullListIndexOf(where);
		item = (FunctionItem *)(FullListItemAt(selected));
		if(item==NULL)
			return;

		// ca ne doit pas etre un super item
		if(item->IsASuperItem())
		{
			// etendre ou diminuer
			if(item->IsExpanded())
				Collapse(item);
			else
				Expand(item);

			// on quitte
			return;
		}

		// selectionner l'item
		if(LockLooper())
		{
			Select(IndexOf(where));
			UnlockLooper();
		}
		else
			g_MetosApp->SendDebbug("Error : LockLooper() in FunctionManagerListView::MouseDown()",VDEBUG_ERROR);

		// on clique dans la liste
		if(selected>=0)
		{
			bool		superItem;

			// proteger
			if(LockLooper())
			{
				// recuperer les infos
				superItem = item->IsASuperItem();

				// si c'est pas un super item on check
				if(!superItem)
					SelectFunction(item);
				
				// rafraichir le super item
				CheckSuperItem(item);
			
				// rafraichir graphiquement
				selected = IndexOf(where);
				InvalidateItem(selected);

				// proteger
				UnlockLooper();
			}
			else
				g_MetosApp->SendDebbug("Error : LockLooper() in FunctionManagerListView::MouseDown()",VDEBUG_ERROR);
		}
	}
}

/**** affichage les proprietes ou rien ****/
void FunctionManagerListView::Draw(BRect updateRect)
{
	// si on a des fonctions on dessine normalement
	if(FullListCountItems()>0)
		BListView::Draw(updateRect);
	else
	{
		// sinon on affiche un message
		BRect	bounds = Bounds();
		float	messageSize = StringWidth(_display.String());
		
		DrawString(_display.String(),BPoint((bounds.Width()-messageSize)/2,(bounds.Height()-10)/2));	
	}
}

// le petit triangle dans notre cas on met rien !!
void FunctionManagerListView::DrawLatch(BRect itemRect, int32 level, bool collapsed,bool highlighted, bool misTracked)
{}

/**** on redimentionne la vue ****/
void FunctionManagerListView::FrameResized(float width,float height)
{
	// fonction normale
	BListView::FrameResized(width,height);

	// si on a rien dans la liste on invalide pour
	// redessiner le texte
	if(FullListCountItems()<=0)
		Invalidate();	
}

/**** selectionner une fonction ****/
void FunctionManagerListView::SelectFunction(FunctionItem *function)
{
	FunctionManagerItem		*item = NULL;
	BMessage				selectFunction(M_SELECT_FUNCTION_MSG);
	BMessage				*properties = NULL;
	BString					className;
	int32					selected;
	bool					checked;
	uint32					objectID;
	
	// recuperer le nom de la classe
	properties = _supportWindow->Properties();
	objectID = _supportWindow->Object();

	// recuperer les données
	if(properties->FindString(VOBJECT_CLASS,&className)!=B_OK)
		return;

	// trouver la liste interne des fonctions
	item = _supportWindow->FunctionItemFromClass(className.String());
	if(item==NULL)
		return;

	selected = item->FunctionList()->IndexOf(function);
	if(selected<0)
		return;

	// verifier si on doit deriver
	checked = function->IsChecked();
	if(checked)
		_nbCheckedFunctions--;
	else
		_nbCheckedFunctions++;	

	// selectionner
	function->SetChecked(!checked);
				
	// prevenir que l'on a selectionné ou annulé la selection
	selectFunction.AddInt32(M_OBJECT_SELECTED,objectID);
	selectFunction.AddInt32(VOBJECT_FUNCTION_INDEX,selected);
	g_ObjectManager.SendMessage(&selectFunction);
				
	// verifier si on doit fixer la derivation
	CheckClassIsDerived();
}

/**** verifier et fixer si la classe doit etre derivé ****/
void FunctionManagerListView::CheckClassIsDerived()
{
	BMessage		newValue(M_PROPERTIES_CHANGE_MSG);
	FunctionItem	*current = NULL;
	int32			nbFunction;
	BString			name;
	BMessage		*properties = NULL;
	BString			className;
	int32			level = 0;
	status_t		state = B_OK;
	bool			derived = false;
	uint32			objectID;
	
	// recuperer le nom de la classe
	properties = _supportWindow->Properties();
	objectID = _supportWindow->Object();

	// recuperer les données
	state &= properties->FindString(VOBJECT_CLASS,&className);
	state &= properties->FindInt32(VOBJECT_LEVEL,&level);
	if(state!=B_OK)
		return;
			
	if(level==1)
		derived = true;
	else
	{
		bool	hasConstructor;
		int32	nbChecked;
		
		// verifier la liste
		nbChecked = 0;
		hasConstructor = false;
		nbFunction = FullListCountItems();
		for(int32 index=0;index<nbFunction;index++)
		{
			// chercher a en trouver au moins 1 qui n'est pas le constructeur
			current = (FunctionItem *)(FullListItemAt(index));
			if(!current->IsASuperItem() && current->IsChecked())
			{
				nbChecked++;
				name = current->MethodeName();
				if(name.FindFirst(className,0)!=B_ERROR)
					hasConstructor = true;

				derived = hasConstructor && (nbChecked>1);
				if(derived)
					break;
			}
		}
	}
	
	// modifier en interne
	properties->ReplaceBool(VOBJECT_DERIVED_CLASS,derived);
	
	// modifier l'objet	
	newValue.AddInt32(M_OBJECT_SELECTED,objectID);
	newValue.AddBool(VOBJECT_DERIVED_CLASS,derived);
	newValue.AddBool(M_NEED_REFRESH,true);
	newValue.AddInt32(M_PROPERTIES_INDEX,0);

	// changer la valeur
	g_PropertieManager.SendMessage(&newValue);

	// prevenir
	g_PropertieManager.SendMessage(M_REFRESH_FUNCTION_LIST_MSG);					
}

/**** trouver si un item est selectionné dans un groupe ****/
void FunctionManagerListView::CheckSuperItem(FunctionItem *item)
{
	FunctionItem	*current = NULL;
	FunctionItem	*superItem = NULL;
	int32		nbItemsUnderCurrent;
	int32		index;
	bool		checked = false;
	
	// trouver le super item
	superItem = (FunctionItem *)(Superitem(item));
	if(superItem==NULL)
		return;

	// trouver le nombre de sous item
	nbItemsUnderCurrent = CountItemsUnder(superItem,false);
	if(nbItemsUnderCurrent==0)
		return;

	// verifier si au moins un est selectionné
	index = FullListIndexOf(superItem) + 1;
	for(int32 i=index;i<(nbItemsUnderCurrent+index);i++)
	{
		current = (FunctionItem *)FullListItemAt(i);
		if(current!=NULL && (checked = current->IsChecked())==true)
			break;
	}

	// on va modifier l'etat et rafraichir si il le faut
	if(superItem->IsChecked()!=checked)
	{
		// selection de l'item
		SelectFunction(superItem);
	
		// rafraichir le super item
		index = IndexOf(superItem);
		InvalidateItem(index);
	}
}

/**** Ajouter une metode a la liste ****/
void FunctionManagerListView::DisplayMethodes(FunctionManagerItem *item,BMessage *selected)
{
	FunctionItem	*current = NULL;
	FunctionItem	*superItem = NULL;
	BList			*functionList = NULL;
	BString			currentName;
	BString			superItemName;
	int32			nbFunction;
	int32			indexMessage;
	int32			functionIndex;
	bool			findIndex = false;
	
	// l'objet doit etre valide
	if(item==NULL)
		return;

	// reinitialiser
	functionList = item->FunctionList();
	_nbCheckedFunctions = 0;

	// parcourir la liste et ajouter les metodes
	nbFunction = functionList->CountItems();
	for(int32 index = 0;index<nbFunction;index++)
	{
		// recuperer la fonction
		current = (FunctionItem *)(functionList->ItemAt(index));
		currentName = current->MethodeName();

		indexMessage = 0;
		findIndex = false;
		if(selected!=NULL && !selected->IsEmpty())
		{
			while(!findIndex && selected->FindInt32(VOBJECT_FUNCTION_INDEX,indexMessage,&functionIndex)==B_OK)
			{
				// si on trouve l'index on va le retirer du message
				// ca evite de reparcourir pour rien
				findIndex = (functionIndex==index);
				if(findIndex)
				{
					selected->RemoveData(VOBJECT_FUNCTION_INDEX,indexMessage);
					break;
				}
			
				// passer au suivant
				indexMessage++;
			}
		}

		// selectionner
		current->SetChecked(findIndex);

		// si l'item est selectionné on augmente notre compteur
		if(findIndex)
			_nbCheckedFunctions++;
		
		// verifier si c'est un super-Item
		if(current->IsASuperItem())
		{
			AddItem(current);
			Collapse(current);
			superItem = current;
			superItemName = currentName;
		}
		else
		{	
			if(superItemName == currentName)
				AddUnder(current,superItem);
			else
				AddItem(current);
		}
	}
	
	// verifier si on doit fixer la derivation
	CheckClassIsDerived();
}

/**** creer le menu d'action de la vue ****/
void FunctionManagerListView::BuildActionMenu()
{
	_actionMenu = new BPopUpMenu("action-menu");
	_actionMenu->AddItem(new BMenuItem(g_Languages->GetTranslationFor(L_FUNCTIONS_SAVE_AS_DEFAULT),new BMessage(M_SAVEASDEFAULT_FUNTION_MSG)));
	_actionMenu->AddItem(new BMenuItem(g_Languages->GetTranslationFor(L_FUNCTIONS_BEST_CONSTRUCTOR),new BMessage(M_FINDBEST_CONSTRUCTOR_MSG)));
}

/**** sauver la liste par defaut ****/
void FunctionManagerListView::SaveListAsDefault()
{
	FunctionManagerItem		*item = NULL;
	BMessage				*properties = NULL;
	BString					className;
	
	// trouver les proprietes
	properties = _supportWindow->Properties();
	
	// verifier si on trouve le nom de la classe
	if(properties->FindString(VOBJECT_CLASS,&className)!=B_OK)
		return;

	// trouver la liste interne des fonctions
	item = _supportWindow->FunctionItemFromClass(className.String());
	if(item==NULL && item->FunctionList()!=NULL)
		return;

	// sauvergarder
	item->SaveListAsDefault();
}

/**** trouver le meilleur constructeur ****/
void FunctionManagerListView::SelectBestConstructor()
{
	FunctionItem			*bestConstructor = NULL;
	FunctionManagerItem		*functionsItem = NULL;
	BMessage				*properties = NULL;
	BString					className;

	// trouver les proprietes
	properties = _supportWindow->Properties();
	if(properties->FindString(VOBJECT_CLASS,&className)==B_OK)
		functionsItem = _supportWindow->FunctionItemFromClass(className.String());

	// on doit avoir trouver l'item de liste des fonctions
	if(functionsItem==NULL)
		return;
	
	// trouver le constructeur
	bestConstructor = functionsItem->FindBestConstructor();

	// checker le meilleur constructeur
	if(bestConstructor!=NULL && !bestConstructor->IsChecked())
	{
		SelectFunction(bestConstructor);
			
		// checker le super item
		CheckSuperItem(bestConstructor);
			
		// sauver la liste
		functionsItem->SaveListAsDefault();
		
		// rafraichir l'affichage
		InvalidateItem(FullListIndexOf(bestConstructor));
	}
}

// =================
// = Drag and drop =
// =================

/**** gerer le drag&drop d'un fichier header ****/
void FunctionManagerListView::DragAndDropHeader(BMessage *message)
{
	// cette fonction va servir a redefinir la liste des fonctions
	// pour l'objet selectionné et ceci pour la classe de cet objet
	BPath					filePath;
	BNode					node;
	BNodeInfo				nodeInfo;
	entry_ref				ref;
	char					type[255];	
	
	// trouver le fichier
	if(message->FindRef("refs",&ref)!=B_OK)
		return;
	
	// initailiser le chemin
	filePath.SetTo(&ref);
	if(filePath.InitCheck()!=B_OK)
		return;
	
	// definir le nodeInfo pour verifier le type MIME
	node.SetTo(&ref);
	if(node.InitCheck()!=B_OK || nodeInfo.SetTo(&node)!=B_OK)
		return;
	
	// cerifier le type MIME
	if(nodeInfo.GetType(type)!=B_OK || strcmp(type,METOS_HEADER_MIME_TYPE)!=0)
		return;
	
	FunctionManagerItem		*functionsItem = NULL;
	BMessage				*properties = NULL;
	BString					className;

	// trouver les proprietes
	properties = _supportWindow->Properties();
	if(properties->FindString(VOBJECT_CLASS,&className)==B_OK)
		functionsItem = _supportWindow->FunctionItemFromClass(className.String());

	// on doit avoir trouver l'item de liste des fonctions
	if(functionsItem==NULL)
		return;

	// appel de la fonction interne de l'item
	if(functionsItem->FindFromHeaderFile(filePath.Path())==B_OK)
	{	
		// vider la liste puis reafficher les nouveaux items
		MakeEmpty();
		DisplayMethodes(functionsItem,NULL);
		
		// selectionner le meilleur constructeur
		SelectBestConstructor();
	}
}

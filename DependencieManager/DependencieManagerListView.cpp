/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
/********************************************/
/* MeTOS DependencieManager list view class */
/********************************************/
#include "DependencieManagerListView.h"
#include "DependencieManagerItem.h"
#include "DependencieItem.h"
#include "PropertieManagerWin.h"
#include "CColumnView.h"
#include "MetosMessageDefs.h"
#include "MetosGlobalDefs.h"
#include "MetosLanguagesDefs.h"
#include "MetosApp.h"
#include "VLanguage.h"

#include <interface/PopUpMenu.h>
#include <interface/MenuItem.h>
#include <interface/TextControl.h>
#include <storage/Entry.h>
#include <storage/Path.h>
#include <stdio.h>

/**** constructeur ****/
DependencieManagerListView::DependencieManagerListView(BRect frame,const char *name,PropertieManagerWin *support)
: BListView(frame,name,B_SINGLE_SELECTION_LIST,B_FOLLOW_ALL_SIDES,B_WILL_DRAW | B_FRAME_EVENTS)
{
	// recuperer le pointer sur la fenetre
	_supportWindow = support;
	_currentManagerItem = NULL;
	_options = NULL;
	_editor = new BTextControl(BRect(0,0,10,10),"dependency-path-editor",NULL,"",NULL);
	_selected = DEPENDENCIES_PATH_SELECTED;
	_display = g_Languages->GetTranslationFor(L_DEPENDENCIES_MESSAGE);
	
	// taille des polices
	SetFontSize(10);

	// creer le menu popup
	BuildOptionMenu();
}

/**** destructeur ****/
DependencieManagerListView::~DependencieManagerListView()
{
	// liberer la memoire
	delete _options;
	delete _editor;
}

/**** gestion des messages ****/
void DependencieManagerListView::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
	// drag & drop
	case B_SIMPLE_DATA:
		DropFile(message);
		break;
	// effacer un item
	case M_REMOVE_DEPENDENCY_MSG:
		RemoveDependency();
		break;
	// sauver par defaut la configuration actuelle
	case M_SAVE_DEPENDENCIES_MSG:
		SaveAsDefault();
		break;
	// sinon c'est pour la liste
	default:
		BListView::MessageReceived(message);	
	}
}

/**** attachement a la fenetre ****/
void DependencieManagerListView::AttachedToWindow()
{
	// fonction parent
	BListView::AttachedToWindow();

	// fixer la vue colonne
	_columnView = _supportWindow->ColumnView(1);

	// enlever le menu des generateurs
	if(_options!=NULL)
		_options->AddItem(_supportWindow->GeneratorMenu());
}

/**** detachement de la fenetre ****/
void DependencieManagerListView::DetachedFromWindow()
{
	// enlever le menu des generateurs
	if(_options!=NULL)
		_options->RemoveItem(_supportWindow->GeneratorMenu());
	
	// si on est en edit on va detacher le controle
	UnselectItem();
	
	// fonction parent
	BListView::DetachedFromWindow();
}

/**** affichage les dependances ou rien ****/
void DependencieManagerListView::Draw(BRect updateRect)
{
	// si on a des fonctions on dessine normalement
	if(CountItems()>0)
		BListView::Draw(updateRect);
	else
	{
		// sinon on affiche un message
		BRect	bounds = Bounds();
		float	messageSize = StringWidth(_display.String());
		
		SetHighColor(BLACK_COLOR);
		DrawString(_display.String(),BPoint((bounds.Width()-messageSize)/2,(bounds.Height()-10)/2));	
	}
}

/**** on redimentionne la vue ****/
void DependencieManagerListView::FrameResized(float width,float height)
{
	// fonction normale
	BListView::FrameResized(width,height);

	// si on a rien dans la liste on invalide pour
	// redessiner le texte
	if(CountItems()<=0)
		Invalidate();	
}

/**** clique souris ****/
void DependencieManagerListView::MouseDown(BPoint where)
{
	BMessage	*currentMessage = NULL;
	int32		index = -1;
	int32		buttons;

	// recuperer le message courant et verifier si on a un double clique
	if(LockLooper())
	{
		currentMessage = Looper()->CurrentMessage();
		UnlockLooper();
	}
	else
		g_MetosApp->SendDebbug("Error : LockLooper() in DependencieManagerListView::MouseDown()",VDEBUG_ERROR);
		
	// le message doit etre valide
	if(currentMessage==NULL)
		return;

	// on doit trouver quel bouton a ete utilise
	if(currentMessage->FindInt32("buttons",&buttons)!=B_OK)
		return;

	// activer la fenetre si c'est pas le cas
	if(!(Window()->IsActive()))
		Window()->Activate(true);	

	// positionner le type d'edition
	// sinon on selectionne l'item
	if(LockLooper())
	{
		index = IndexOf(where);
		SelectItem(index,buttons,where.x);
		UnlockLooper();
	}
	else
		g_MetosApp->SendDebbug("Error : LockLooper() in DependencieManagerListView::MouseDown()",VDEBUG_ERROR);

	// si c'est le bouton droit on defini la liste comme celle
	// utilisée par defaut pour cette objet
	if(buttons & B_SECONDARY_MOUSE_BUTTON)
	{
		// on va faire apparaitre le popup d'option
		BMenuItem *selectedItem = _options->Go(ConvertToScreen(where));
		if(selectedItem!=NULL)
		{
			BMessage	*message = selectedItem->Message();
				
			// verifier si c'est le menu des generateurs
			if(selectedItem->Menu()==_supportWindow->GeneratorMenu())
				selectedItem->SetMarked(true);
				
			// sinon on renvoi le message
			if(message!=NULL)
				BMessenger(this).SendMessage(selectedItem->Message());
		}
	}
}

/**** afficher les dependance de cet item ****/
void DependencieManagerListView::DisplayDependencies(DependencieManagerItem *dependencies)
{
	// retenir l'element courant
	_currentManagerItem = dependencies;

	// deselectionner l'item courant si il y en a un
	UnselectItem();

	// si c'est un pointeur NULL on vide la liste uniquement
	// sinon on va la remplir avec les item de la liste
	MakeEmpty();
	if(_currentManagerItem!=NULL)
	{
		DependencieItem		*current = NULL;
		BList				*dependenciesList = NULL;
		int32				nbItems = 0;
		
		// il faut parcourir la liste pour trouver
		// les elements du generateur selectionné
		dependenciesList = _currentManagerItem->Dependencies();
		nbItems = dependenciesList->CountItems();
		for(int32 index=0;index<nbItems;index++)
		{
			current = (DependencieItem *)(dependenciesList->ItemAt(index));
			if(current!=NULL && strcmp(current->GeneratorID(),(_supportWindow->GeneratorID())->String())==0)
				AddItem(current);
		}
	}
}

/**** creer le menu popup des options ****/
void DependencieManagerListView::BuildOptionMenu()
{
	// menus
	_options = new BPopUpMenu(g_Languages->GetTranslationFor(L_DEPENDENCIES_OPTIONS));

	// creer les items principaux
	_options->AddItem(new BMenuItem(g_Languages->GetTranslationFor(L_DEPENDENCIES_SAVE_AS_DEFAULT),new BMessage(M_SAVE_DEPENDENCIES_MSG)));
	_options->AddItem(new BMenuItem(g_Languages->GetTranslationFor(L_DEPENDENCIES_REMOVE),new BMessage(M_REMOVE_DEPENDENCY_MSG)));
	_options->AddSeparatorItem();

	// definir la vue comme destinataire des messages
	_options->SetTargetForItems(this);
}

// ===================================
// = Gestion des items de dependance =
// ===================================

/**** selection d'un item ****/
void DependencieManagerListView::SelectItem(int32 index,uint32 buttons,float &x)
{
	// si on est en edition on doit enlever le controle edit
	if(_editor->Parent()!=NULL)
	{
		UnselectItem();
		return;
	}
	
	// si l'index est negatif on deselctionne tout
	if(index<0)
	{
		DeselectAll();
		return;
	}
	
	// sinon on a plusieurs choix
	// selection simple avec bouton droit ou gauche
	// si l'item est deja selectionné, seul un second clique
	// du bouton droit passe en mode edition.
	if(!IsItemSelected(index))
		Select(index);
	else
	{
		// on passe en edition que avec le bouton droit
		if(!(buttons & B_PRIMARY_MOUSE_BUTTON))
			return;
			
		// avant tout verifier que un item est selectionné
		if(_currentManagerItem==NULL)
			return;
		
		DependencieItem		*item = NULL;
		BList				*dependencies = NULL;
		BRect				bounds = ItemFrame(index);

		dependencies = _currentManagerItem->Dependencies();
		if(dependencies==NULL)
			return;
		
		// positioner le type d'edition
		_selected = DEPENDENCIES_PATH_SELECTED;
		if(x>_columnView->RectFrom(1)->left)
			_selected = DEPENDENCIES_MIME_SELECTED;

		// recuperer l'item et afficher l'editeur
		item = (DependencieItem *)(dependencies->ItemAt(index));
		
		// en fonction de la selection on sait quoi editer
		if(_selected==DEPENDENCIES_PATH_SELECTED)
			_editor->SetText(item->Path());
		else
			_editor->SetText(item->MIME());			
		
		// deplacer le rectangle pour l'icon
		bounds.left += 16;
		
		// ajouter l'item
		AddChild(_editor);
		_editor->ResizeTo(bounds.Width()-6,bounds.Height()-5);
		_editor->MoveTo(bounds.left+3,bounds.top+2);
		
		// pour fixer un bug d'affichage
		_editor->ChildAt(0)->MoveTo(3,3);
		_editor->ChildAt(0)->ResizeTo(bounds.Width()-12,bounds.Height()-10);
		_editor->MakeFocus(true);
	}
}

/**** on deselectionne un item ****/
void DependencieManagerListView::UnselectItem()
{
	// verifier si un item est en selection
	if(_editor->Parent()==NULL)
		return;

	// avant tout verifier que un item est selectionné
	if(_currentManagerItem!=NULL)
	{
		const char		*text = NULL;
		int32			index = -1;
		BList			*dependencies = NULL;
	
		// mettre a jour le chemin
		text = _editor->Text();
		index = CurrentSelection();
		dependencies = _currentManagerItem->Dependencies();
		if(text!=NULL && index>=0 && dependencies!=NULL)
		{
			DependencieItem		*item = NULL;
			
			// modifier l'item courant
			item = (DependencieItem *)(dependencies->ItemAt(index));

			// en fonction de la selection on sait quoi editer
			if(_selected==DEPENDENCIES_PATH_SELECTED)
				item->SetPath(text);
			else
				item->SetMIME(text);	
	
			// reaffichage
			InvalidateItem(index);
		}
	}
		
	// detacher
	RemoveChild(_editor);
}

/**** drag&drop ****/
void DependencieManagerListView::DropFile(BMessage *message)
{
	// avant tout verifier que un item est selectionné
	if(_currentManagerItem==NULL)
		return;

	DependencieItem		*newItem = NULL;
	entry_ref			refs;

	// on doit trouver un entry_ref si c'est un fichier
	if(message->FindRef("refs",&refs)!=B_OK)
		return;		

	// ok on va ajouter ce fichier
	// mais avec un type inconnu
	newItem = _currentManagerItem->AddDependency(&refs,(_supportWindow->GeneratorID())->String());
	if(newItem==NULL)
		return;
	
	// ajouter l'item	
	AddItem(newItem);
	
	// si c'est le premier on va rafraichir l'affichage
	if(CountItems()==1)
		Invalidate();
}

/**** effacer un element ****/
void DependencieManagerListView::RemoveDependency()
{
	// si on a un item courant on peux effacer
	if(_currentManagerItem==NULL)
		return;
	
	// trouver l'item selectionné dans la liste
	DependencieItem		*item = NULL;
	int32				currentIndex = CurrentSelection();
	BList				*dependencies = _currentManagerItem->Dependencies();

	// pas d'item selectionné
	// ou pas de liste
	if(currentIndex<0 || dependencies==NULL)
		return;

	item = (DependencieItem *)(ItemAt(currentIndex));
	if(item!=NULL && RemoveItem(item) && dependencies->RemoveItem(item))
		delete item;
}

/**** sauver la configuration actuelle ****/
void DependencieManagerListView::SaveAsDefault()
{
	// si on a un item courant on peux sauver
	if(_currentManagerItem==NULL)
		return;

	// sauver
	_currentManagerItem->SaveDependency(_supportWindow->GeneratorList());
}

// ===============
// = Generateurs =
// ===============

/**** forcer la selection d'un generateur ****/
void DependencieManagerListView::SelectGenerator(const char *ID)
{
	// reafficher avec l'item courant
	DisplayDependencies(_currentManagerItem);
}

/**** enlever le menu des generateurs pour eviter de l'effacer 2 fois ****/
void DependencieManagerListView::RemoveGeneratorMenu()
{
	// virer le menu des generateurs
	if(_options!=NULL)
		_options->RemoveItem(_supportWindow->GeneratorMenu());
}

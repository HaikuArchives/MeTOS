/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
/********************************/
/* MeTOS code editor view class */
/********************************/
#include "CodeManagerTextView.h"
#include "CodeManagerItem.h"
#include "MetosApp.h"
#include "PropertieManagerWin.h"
#include "MetosGlobalDefs.h"
#include "MetosMessageDefs.h"
#include "VPreferenceFile.h"
#include "VObject.h"
#include "MetosLanguagesDefs.h"
#include "VLanguage.h"

#include <app/Looper.h>
#include <interface/PopUpMenu.h>
#include <interface/MenuItem.h>
#include <stdio.h>

/**** constructeur ****/
CodeManagerTextView::CodeManagerTextView(BRect frame,const char *name,BRect textRect,PropertieManagerWin *support)
: BTextView(frame,name,textRect,new BFont(be_fixed_font),NULL,B_FOLLOW_ALL_SIDES,B_WILL_DRAW)
{
	BFont	fixed(be_fixed_font);

	// initialiser la police ici aussi
	// pour que le draw utilise cette derniere
	SetFont(&fixed);

	// recuperer le pointeur sur la fenetre
	// et initialiser d'autre valeur
	_supportWindow = support;
	_options = NULL;
	_isEmpty = true;
	_display = g_Languages->GetTranslationFor(L_CODE_MESSAGE);

	// creer le menu popup
	BuildOptionMenu();
}

/**** destructeur ****/
CodeManagerTextView::~CodeManagerTextView()
{
	// liberer la memoire
	delete _options;
}

/**** gestion des messages ****/
void CodeManagerTextView::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
	// mise a jour du code externe
	case M_UPDATE_EXTERNAL_CODE_MSG:
		UpdateExternalCode();
		break;
	// ajout d'un item de code
	case M_ADD_EXTERNAL_CODE_MSG:
		g_PropertieManager.SendMessage(M_ADD_EXTERNAL_CODE_MSG);
		break;
	default:
		BTextView::MessageReceived(message);
	}
}

/**** attachement a la fenetre ****/
void CodeManagerTextView::AttachedToWindow()
{
	// fonction parent
	BTextView::AttachedToWindow();

	// enlever le menu des generateurs
	if(_options!=NULL)
		_options->AddItem(_supportWindow->GeneratorMenu());

}

/**** detachement de la fenetre ****/
void CodeManagerTextView::DetachedFromWindow()
{
	// enlever le menu des generateurs
	RemoveGeneratorMenu();	
	
	// fonction parent
	BTextView::DetachedFromWindow();
}

/**** affichage les proprietes ou rien ****/
void CodeManagerTextView::Draw(BRect updateRect)
{
	// si on a des fonctions on dessine normalement
	if(TextLength()>0)
		BTextView::Draw(updateRect);
	else
	{
		// sinon on affiche un message
		BRect	bounds = Bounds();
		float	messageSize = StringWidth(_display.String());
		
		DrawString(_display.String(),BPoint((bounds.Width()-messageSize)/2,(bounds.Height()-10)/2));	
	}
}

/**** clique souris ****/
void CodeManagerTextView::MouseDown(BPoint where)
{
	BMessage	*currentMessage = NULL;
	int32		buttons;

	// recuperer le message courant et verifier si on a un double clique
	if(Looper()->Lock())
	{
		currentMessage = Looper()->CurrentMessage();
		Looper()->Unlock();
	}
	else
		g_MetosApp->SendDebbug("Error : LockLooper() in CodeManagerTextView::MouseDown()",VDEBUG_ERROR);
	
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
		// on va faire apparaitre le popup d'option
		BMenuItem *selectedItem = _options->Go(ConvertToScreen(where));
		if(selectedItem!=NULL)
		{
			// recuperer le label et l'inserer dans la vue de texte
			// si c'est ce menu de selectionné ou celui des proprietes speciales
			if(selectedItem->Menu()==_propertiesMenu || selectedItem->Menu()==_specialProperties)
				Insert(selectedItem->Label());
			else
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
	else
		// sinon c'est la fonctionnalité standard de la vue texte
		BTextView::MouseDown(where);
}

/**** affichage du code ****/
void CodeManagerTextView::DisplayCode(BMessage *objectCode)
{
	BString		*generatorID = NULL;
	BString		buffer;
	BString		sourceCode;

	// par defaut on a rien dans le texte
	sourceCode.SetTo("");

	// si on passe un message en parametre
	// c'est qu'on transmet une liste de code
	// on va alors la recopier dans notre message en interne
	if(objectCode!=NULL)
	{
		// recuperer le code qui nous interesse
		generatorID = _supportWindow->GeneratorID();
		if(objectCode->FindString(generatorID->String(),&sourceCode)!=B_OK)
			sourceCode.SetTo("");
	}
		
	// placer le code dans la vue
	SetText(sourceCode.String());
	
	// invalider si la vue est attaché a la fenetre
	if(Window()!=NULL)
	{
		// proteger l'appel du rafraichissement
		if(LockLooper())
		{
			Invalidate();
			UnlockLooper();	
		}
		else
			g_MetosApp->SendDebbug("Error : LockLooper() in CodeManagerTextView::DisplayCode()",VDEBUG_ERROR);		
	}
}

/**** gestion du clavier ****/
void CodeManagerTextView::KeyUp(const char *bytes, int32 numBytes)
{
	// fonction parent
	BTextView::KeyUp(bytes,numBytes);

	// si on efface verifier si il ya encore du texte
	if((!_isEmpty && TextLength()==0 && (bytes[0]==B_DELETE || bytes[0]==B_BACKSPACE)) || (_isEmpty && TextLength()>0))
	{
		// rafraichir alors pour afficher qu'il n'y a plus de texte
		// proteger l'appel du rafraichissement
		if(LockLooper())
		{
			Invalidate();
			UnlockLooper();	
		}
		else
			g_MetosApp->SendDebbug("Error : LockLooper() in CodeManagerTextView::KeyUp()",VDEBUG_ERROR);		

		// ok la vue est vide
		_isEmpty = (TextLength()==0);
	}
}

// ===================
// = Gestion du code =
// ===================

/**** mise a jour du code ****/
void CodeManagerTextView::UpdateExternalCode()
{
/*	BString		*generatorID = NULL;
	
	// l'id du generateur ne doit pas etre vide
	generatorID = _supportWindow->GeneratorID();
	if(generatorID->Length()==0)
		return;
	
	// on modifie en interne le code, puis on l'envoi
	// au gestionnaire d'objet, l'entree peux ne pas exister
	if(_externalCode.ReplaceString(generatorID->String(),Text())!=B_OK)
		_externalCode.AddString(generatorID->String(),Text());
		
	// mettre a jour
	BMessage	newValue(M_PROPERTIES_CHANGE_MSG);

	newValue.AddMessage(VOBJECT_EXTERNAL_CODE,&_externalCode);
	newValue.AddInt32(M_OBJECT_SELECTED,_supportWindow->Object());
	newValue.AddInt32(M_PROPERTIES_INDEX,0);
	newValue.AddBool(M_NEED_REFRESH,false);
	newValue.AddBool(M_NEED_INVALIDATE,true);
	newValue.AddBool(M_NEED_UNDO_ACTION,true);

	// changer la valeur
	g_PropertieManager.SendMessage(&newValue);*/
}

// =====================
// = Gestion des menus =
// =====================

/**** creer le menu popup des options ****/
void CodeManagerTextView::BuildOptionMenu()
{
	// menus
	_options = new BPopUpMenu("options");
	_propertiesMenu = new BMenu(g_Languages->GetTranslationFor(L_MENU_CODE_ADD_PROPERTIES));
	
	// creer le menu des proprietes speciales
	_specialProperties = new BMenu(g_Languages->GetTranslationFor(L_MENU_CODE_ADD_SPECIALS));
	_specialProperties->AddItem(new BMenuItem("#member-name#",NULL));
	_specialProperties->AddItem(new BMenuItem("#father-object#",NULL));
	_specialProperties->AddItem(new BMenuItem("#child-object#",NULL));
		
	// ajouts des items
	_options->AddItem(new BMenuItem(g_Languages->GetTranslationFor(L_MENU_CODE_UPDATE),new BMessage(M_UPDATE_EXTERNAL_CODE_MSG)));
	_options->AddItem(new BMenuItem(g_Languages->GetTranslationFor(L_MENU_CODE_ADD_NEW_ITEM),new BMessage(M_ADD_EXTERNAL_CODE_MSG)));
	_options->AddSeparatorItem();
	_options->AddItem(_propertiesMenu);
	_options->AddItem(_specialProperties);
	_options->AddSeparatorItem();

	// definir la vue comme destinataire des messages
	_options->SetTargetForItems(this);
}

/**** creer le menu des proprietes ****/
void CodeManagerTextView::BuildPropertiesMenu(BMessage *properties,BMessage *locked)
{
	// reinitialiser le nom de la classe
	_className.SetTo("");
	
	// on doit avoir les infos sur les proprietes bloquées
	if(locked==NULL || properties==NULL)
		return;
	
	// recuperer le nom de la classe
	properties->FindString(VOBJECT_CLASS,&_className);

	// prendre toutes les proprietes qui sont visible
	BMenuItem		*menuItem = NULL;
	BString			indexLockedName;
	BString			menuLabel;
	int32			stateProperties = -1;
	type_code		dataType;
	int32			count;
	char			*buffer;
	int32			nbIndex = 0;
	int16			indexLocked = 0;
	bool			findIndex = false;
	int32			menuItemIndex = 0;
	
	// parcourir les proprietes
	for(int32 i = 0;properties->GetInfo(B_ANY_TYPE, i, &buffer, &dataType, &count) == B_OK;i++)
	{
		// initialiser pour la recherche de proprietes bloquees
		indexLockedName = buffer;
		indexLockedName << VOBJECT_LOCKED_INDEX;

		// chercher dans la liste interne
		for(int32 indexValue = 0;indexValue<count;indexValue++)
		{
			// verifier que l'on peux afficher cette proprietes
			// on ne doit pas trouver alors son nom dans la liste des bloquées
			if(locked->FindInt32(buffer,&stateProperties)!=B_OK)
				stateProperties = VOBJECT_PROPERTIES_ENABLED;
			else
			{
				// on a trouvé une info sur le blockage de propriete
				// on va verifier si on specifie un index ou plusieur index
				// si on en trouve pas on laisse l'etat comme il est
				locked->GetInfo(indexLockedName.String(),&dataType,&nbIndex);
				if(nbIndex>0)
				{
					// chercher les indexs
					findIndex = false;
					for(int32 indexSearch=0;(!findIndex && locked->FindInt16(indexLockedName.String(),indexSearch,&indexLocked)==B_OK);indexSearch++)
						findIndex = (indexLocked==indexValue);
					
					// si on a le nom dans les bloquée mais pas avec l'index courant
					// on laisse la valeur d'origine
					if(!findIndex)
						stateProperties = VOBJECT_PROPERTIES_ENABLED;
				}
			}			

			// doit-on finalement afficher la propriete
			if(stateProperties == VOBJECT_PROPERTIES_ENABLED || stateProperties == VOBJECT_PROPERTIES_DISABLED)
			{
				// definir le label
				menuLabel.SetTo("#");
				if(indexValue>0)
				{
					// on ajoute l'index si il est superieur a zero
					menuLabel << indexValue;
					menuLabel << ":";
				}
				menuLabel << buffer;
				menuLabel << "#";
				
				// affecter l'item de menu
				menuItem = _propertiesMenu->ItemAt(menuItemIndex);
				if(menuItem==NULL)
					_propertiesMenu->AddItem(new BMenuItem(menuLabel.String(),NULL));	
				else
					menuItem->SetLabel(menuLabel.String());
				
				// passer a l'index de menu en plus
				menuItemIndex++;
			}
		}	
	}
	
	// vider les elements en trop du menu
	for(int32 index=menuItemIndex;index<_propertiesMenu->CountItems();index++)
		delete _propertiesMenu->RemoveItem(menuItemIndex);
}

// ==============================
// = Selection d'un Generateurs =
// ==============================

/**** forcer la selection d'un generateur ****/
void CodeManagerTextView::SelectGenerator(const char *ID)
{
	// rafraichir si il y a du code en interne
	// pour le generateur selectionné
//	DisplayCode();
}

/**** enlever le menu des generateurs pour eviter de l'effacer 2 fois ****/
void CodeManagerTextView::RemoveGeneratorMenu()
{
	// virer le menu des generateurs
	if(_options!=NULL)
		_options->RemoveItem(_supportWindow->GeneratorMenu());
}
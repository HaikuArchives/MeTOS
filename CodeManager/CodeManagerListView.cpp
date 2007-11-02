/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
/*************************/
/* MeTOS list view class */
/*************************/
#include "MetosApp.h"
#include "CodeManagerListView.h"
#include "MetosGlobalDefs.h"
#include "MetosMessageDefs.h"
#include "PropertieManagerWin.h"
#include "CodeManagerItem.h"
#include "VPreferenceFile.h"
#include "VObject.h"
#include "MetosLanguagesDefs.h"
#include "VLanguage.h"

#include <interface/PopUpMenu.h>
#include <interface/MenuItem.h>
#include <stdlib.h>

/**** constructeur ****/
CodeManagerListView::CodeManagerListView(BRect frame,const char *name,PropertieManagerWin *support)
: BListView(frame,name,B_SINGLE_SELECTION_LIST,B_FOLLOW_ALL_SIDES,B_WILL_DRAW | B_FRAME_EVENTS)
{
	// initialiser
	_supportWindow = support;
	_className = (char *)(malloc(1));
	_className[0] = '\0';
	_display = g_Languages->GetTranslationFor(L_CODE_MESSAGE);
	
	// creer le menu popup
	BuildOptionMenu();
}

/**** destructeur ****/
CodeManagerListView::~CodeManagerListView()
{
	int32	nbIndex = 0;
	
	// vider la vue dans le cache
	EmptyCodeList();
	
	// sauver les listes de codes externes
	SaveExternalCodeFiles();
	
	// vider la liste interne des codes
	nbIndex = _codeListCache.CountItems() - 1;
	for(int32 index=nbIndex;index>=0;index--)
		delete (VPreferenceFile *)(_codeListCache.RemoveItem(index));

	// vider la liste interne des items
	nbIndex = _codeItemCache.CountItems() - 1;
	for(int32 index=nbIndex;index>=0;index--)
		delete (CodeManagerItem *)(_codeItemCache.RemoveItem(index));

	// liberer la memoire
	free(_className);
}	

/**** gestion des messages ****/
void CodeManagerListView::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
	// definir par defaut le code selectionné
	case M_SET_DEFAULT_EXTERNAL_CODE_MSG:
		SetDefaultCode();
		break;
	// sauver les items de code
	case M_SAVE_EXTERNAL_CODE_MSG:
		SaveExternalCodeFiles();
		break;
	// charger les codes
	case M_LOAD_EXTERNAL_CODE_MSG:
		break;
	default:
		BListView::MessageReceived(message);
	}
}

/**** affichage les proprietes ou rien ****/
void CodeManagerListView::Draw(BRect updateRect)
{
	// si on a un objet on dessine normalement
	if(CountItems()>0)
	{
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

/**** gestion du clique souris ****/
void CodeManagerListView::MouseDown(BPoint where)
{
	// on va selectionner un item et afficher son code
	BMessage	*currentMessage = NULL;
	int32		buttons;
	int32		index;

	// recuperer le message courant et verifier si on a un double clique
	if(LockLooper())
	{
		currentMessage = Looper()->CurrentMessage();
		UnlockLooper();
	}
	else
		g_MetosApp->SendDebbug("Error : LockLooper() in CodeManagerListView::MouseDown()",VDEBUG_ERROR);
		
	// le message doit etre valide
	if(currentMessage==NULL)
		return;

	// on doit trouver quel bouton a ete utilise
	if(currentMessage->FindInt32("buttons",&buttons)!=B_OK)
		return;

	// activer la fenetre si c'est pas le cas
	if(!(Window()->IsActive()))
		Window()->Activate(true);

	// savoir quel item on doit selectionner
	index = IndexOf(where);
		
	// sinon c'est la selection de l'objet
	if(index<0)
	{
		// pas d'objet a selectionner
		UnSelectCodeFromList();
	
		// on quitte
		return;
	}
	
	// sinon selectionner l'index
	SelectCodeFromList(index);

	// le bouton droit va afficher le menu pop-up
	if(buttons & B_SECONDARY_MOUSE_BUTTON)
	{
			// on va faire apparaitre le popup d'option
		BMenuItem *selectedItem = _options->Go(ConvertToScreen(where));

		// on doit avoir selectionné un item
		if(selectedItem!=NULL)
		{
			BMessage	*message = selectedItem->Message();
				
			// ok on affiche le menu et on traite l'action
			if(message!=NULL)
				BMessenger(this).SendMessage(message);
		}
	}
}

/**** on redimentionne la vue ****/
void CodeManagerListView::FrameResized(float width,float height)
{
	// fonction normale
	BListView::FrameResized(width,height);

	// si on a un pas d'objet on invalide pour redessiner le texte
	if(CountItems()<=0)
		Invalidate();	
}

/**** trouver dans notre liste interne le message contenant les codes externes ****/
VPreferenceFile *CodeManagerListView::FindCodeFromClass(const char *className)
{
	// on va chercher dans notre liste interne et si on trouve pas on va charger
	// le fichier si il existe
	VPreferenceFile	*externalCode = NULL;
	const char		*name = NULL;
	int32			nbItems = 0;
	
	nbItems = _codeListCache.CountItems();
	for(int32 index=0;index<nbItems;index++)
	{
		externalCode = (VPreferenceFile *)(_codeListCache.ItemAt(index));
		if(externalCode!=NULL && externalCode->FindString(VOBJECT_CLASS,&name)==B_OK)
			if(strcmp(name,className)==0)
				return externalCode;
	}
			
	// si on est la on a rien trouvé
	return NULL;
}	

/**** afficher les codes externes ****/
void CodeManagerListView::DisplayCode(const char *className)
{
	// on va afficher dans la liste les items chargés du fichier
	// correspondant au generateur selectionné pour le nom de la classe
	// donné en parametre
	
	// on doit avoir un nom correct
	if(className==NULL)
		return;

	// on va charger ici le code que si le nom est different
	if(strcmp(className,_className)!=0)
	{
		VPreferenceFile	*codeList = NULL;
		const char		*generatorID = NULL;
		char			*buffer = NULL;
		size_t			nameSize = 0;
			
		// vider la liste
		EmptyCodeList();	
				
		// on va recopier le nom
		// pour ca on va reallouer la place necessaire
		nameSize = strlen(className) + 1;
		buffer = (char *)(realloc(_className,nameSize));
		if(buffer==NULL)
			return;
	
		// recopier le nom
		_className = buffer;
		memcpy(_className,className,nameSize);

		// charger les codes externes
		codeList = FindCodeFromClass(_className);
		if(codeList==NULL)
			return;
	
		// trouver le generateur actuel
		generatorID = (_supportWindow->GeneratorID())->String();
		if(generatorID==NULL)
			return;
	
		// ajouter les codes correspondant au generateur dans la liste
		CodeManagerItem		*item = NULL;
		const char			*code = NULL;
		BString				defaultName;
		int32				index = 0;
		int32				defaultIndex = 0;

		// construire le nom de l'index par defaut
		defaultName = generatorID;
		defaultName << "-";
		defaultName << M_DEFAULT_INDEX;
			
		// puis ajouter ou modifier la valeur par defaut
		if(codeList->FindInt32(defaultName.String(),&defaultIndex)!=B_OK)
			defaultIndex = 0;
		
		// on va rechercher dans notre fichier code en cache
		// qui correspond a la classe les codes pour le generateur selectionné
		while(codeList->FindString(generatorID,index,&code)==B_OK)
		{
			// creer un nouvel item de code
			item = CreateCodeItem();
			if(item!=NULL)
			{
				// ok on le modifie et on l'ajoute a la liste
				item->SetCode(code);
				item->SetGenerator(generatorID);
				item->SetDefault(defaultIndex==index);
		
				// ajouter l'item
				AddItem(item);
			}
			
			// passer au suivant
			index++;
		}
		
		// si on a des items on doit reafficher
		if(CountItems()>0)
			Invalidate();
	}
}

// ===================================
// = Gestion fichier de code externe =
// ===================================

/**** charger le fichier de code externe ****/
void CodeManagerListView::LoadExternalCodeFile(BMessage *message)
{
	const char	*className = NULL;

	// on doit trouver le nom de la classe
	if(message->FindString(VOBJECT_CLASS,&className)!=B_OK)
		return;

	// si on arrive la on doit charger le code du fichier
	VPreferenceFile		*codeFile = NULL;

	// charger puis remplir la liste interne d'items
	// si on arrive pas a le charger on vide le message
	codeFile = new VPreferenceFile(className,ADDON_EXTERNAL_CODES_FOLDER);
	if(codeFile->Load()!=B_OK)
		codeFile->MakeEmpty();
	
	// on va ajouter le nom de la classe
	// et le flag d'etat
	codeFile->AddString(VOBJECT_CLASS,className);
	codeFile->AddBool(M_IS_MODIFIED,false);
	
	// on va ajouter cette liste de code dans notre cache
	_codeListCache.AddItem(codeFile);
}

/**** sauver les fichiers de code externe ****/
void CodeManagerListView::SaveExternalCodeFiles()
{
	VPreferenceFile		*item = NULL;
	int32				nbIndex = 0;
	bool				modified = false;

	// on va enregistrer tous les fichiers de code
	// si il ne sont pas vide
	nbIndex = _codeListCache.CountItems();
	for(int32 index=0;index<nbIndex;index++)
	{
		item = (VPreferenceFile *)(_codeListCache.ItemAt(index));
		if(item!=NULL)
		{
			// trouver le flag de modification
			if(item->FindBool(M_IS_MODIFIED,&modified)!=B_OK)
				modified = false;
			
			// on vire le nom de la classe
			// et l'etat
			item->RemoveName(VOBJECT_CLASS);
			item->RemoveName(M_IS_MODIFIED);		
			if(modified && !item->IsEmpty())
				item->Save();
		}
	}
}

/**** definir l'item comme defaut ****/
void CodeManagerListView::SetDefaultCode()
{
	CodeManagerItem		*item = NULL;
	VPreferenceFile		*codeFile = NULL;
	const char			*generatorID = NULL;
	BString				defaultName;
	int32				defaultIndex;

	// on doit avoir un generateur courant
	generatorID = (_supportWindow->GeneratorID())->String();
	if(generatorID==NULL)
		return;

	// on doit trouver le fichier de code
	codeFile = FindCodeFromClass(_className);
	if(codeFile==NULL)
		return;

	// on va deja enlever le defaut du precedent
	// construire le nom de l'index par defaut
	defaultName = generatorID;
	defaultName << "-";
	defaultName << M_DEFAULT_INDEX;
			
	// puis ajouter ou modifier la valeur par defaut
	if(codeFile->FindInt32(defaultName.String(),&defaultIndex)!=B_OK)
	{
		item = (CodeManagerItem *)(ItemAt(defaultIndex));
		if(item!=NULL)
			item->SetDefault(false);
	}

	// reaffichage
	InvalidateItem(defaultIndex);
	
	// recuperer la selection courante
	defaultIndex = CurrentSelection();
	if(defaultIndex<0)
		return;
	
	// puis le fixer a l'actuel selectionné
	if(codeFile->ReplaceInt32(defaultName.String(),defaultIndex)!=B_OK)
		codeFile->AddInt32(defaultName.String(),defaultIndex);

	// modifier l'item
	item = (CodeManagerItem *)(ItemAt(defaultIndex));
	if(item!=NULL)
		item->SetDefault(false);	

	// reaffichage
	InvalidateItem(defaultIndex);
}

/**** selectionner un code de la liste ****/
void CodeManagerListView::SelectCodeFromList(int32 index)
{
	// on va ici afficher le code de l'item selectionné dans
	// la vue d'edition du code

	// selectionner visuellement l'item
	if(LockLooper())
	{
		Select(index);
		InvalidateItem(index);
		UnlockLooper();
	}
	else
		g_MetosApp->SendDebbug("Error : LockLooper() in CodeManagerListView::SelectCodeFromList()",VDEBUG_ERROR);
	
	// envoyer le message de selection de l'item
	g_PropertieManager.SendMessage(M_SELECT_EXTERNAL_CODE_MSG);
}

/**** deselectionner un code de la liste ****/
void CodeManagerListView::UnSelectCodeFromList()
{
	// deselectionner les item
	Deselect(CurrentSelection());

	// envoyer le message de selection de l'item
	// ici il est necessaire pour lancer l'effacement du texte dans la vue
	g_PropertieManager.SendMessage(M_SELECT_EXTERNAL_CODE_MSG);
}

// =====================
// = Gestion des items =
// =====================

/**** ajouter un nouvel item de code ****/
void CodeManagerListView::AddExternalCodeItem(const char *code)
{
	// verifier la validité du code
	if(code==NULL)
		return;

	// on va ajouter un item avec le generateur courant
	CodeManagerItem		*newItem = NULL;
	VPreferenceFile		*codeFile = NULL;
	const char			*generatorID = NULL;
	
	// on doit avoir un generateur courant
	generatorID = (_supportWindow->GeneratorID())->String();
	if(generatorID==NULL)
		return;

	// on doit trouver le fichier de code
	codeFile = FindCodeFromClass(_className);
	if(codeFile==NULL)
		return;
		
	// creer un item puis l'ajouter
	newItem = CreateCodeItem();
	if(newItem!=NULL)
	{
		int32		defaultIndex;
		
		// ajouter dans le fichier le nouveau code
		codeFile->AddString(generatorID,code);

		// definir le generateur et le code
		newItem->SetCode(code);
		newItem->SetGenerator(generatorID);
		newItem->SetDefault(false);
		
		// si c'est le premier item
		// on va le considerer comme defaut
		defaultIndex = CountItems();
		if(defaultIndex==0)
		{
			BString		defaultName;
			
			// mettre la valeur par defaut a vrai
			newItem->SetDefault(true);

			// construire le nom de l'index par defaut
			defaultName = generatorID;
			defaultName << "-";
			defaultName << M_DEFAULT_INDEX;
			
			// puis ajouter ou modifier la valeur par defaut
			if(codeFile->ReplaceInt32(defaultName.String(),defaultIndex)!=B_OK)
				codeFile->AddInt32(defaultName.String(),defaultIndex);
		}		
		
		// ajouter l'item
		AddItem(newItem);
		
		// on va rafraichir l'affichage complet si c'est le premier
		if(CountItems()==1)
			Invalidate();
	}
}

/**** effacer un item de code ****/
void CodeManagerListView::RemoveExternalCodeItem(BMessage *message)
{
}

/**** recuperer un code item du cache ****/
CodeManagerItem *CodeManagerListView::CreateCodeItem()
{
	// on va donner l'item en position 0 si il y en a un
	if(_codeItemCache.CountItems()<=0)
		return new CodeManagerItem();
	
	// retourner l'element en position 0
	return (CodeManagerItem *)(_codeItemCache.RemoveItem((int32)0));
}

/**** vider la liste des codes et transferer dans le cache ****/
void CodeManagerListView::EmptyCodeList()
{
	int32		nbItems = 0;

	// vider la liste view et placer les items dans le cache
	nbItems = CountItems() - 1;
	for(int32 index=nbItems;index>=0;index--)
		_codeItemCache.AddItem(RemoveItem(index));
}

// ======================
// Gestion du menu pop-up
// ======================

/**** construire le pop-up ****/
void CodeManagerListView::BuildOptionMenu()
{
	// menus
	_options = new BPopUpMenu("options");
	_options->AddItem(new BMenuItem(g_Languages->GetTranslationFor(L_MENU_CODE_SAVE_AS_DEFAULT),new BMessage(M_SET_DEFAULT_EXTERNAL_CODE_MSG)));
	_options->AddSeparatorItem();
	_options->AddItem(new BMenuItem(g_Languages->GetTranslationFor(L_MENU_CODE_SAVE),new BMessage(M_SAVE_EXTERNAL_CODE_MSG)));
	_options->AddItem(new BMenuItem(g_Languages->GetTranslationFor(L_MENU_CODE_LOAD),new BMessage(M_LOAD_EXTERNAL_CODE_MSG)));
}
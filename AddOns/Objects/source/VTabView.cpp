/*****************************/
/* Addon du BTabView de BeOS */
/*****************************/

#include "VObject.h"
#include "MetosMessageDefs.h"
#include "VTabView.h"

#include <Application.h>
#include <TabView.h>
#include <Message.h>
#include <String.h>
#include <PopUpMenu.h>
#include <MenuItem.h>
#include <stdio.h>

extern "C" __declspec(dllexport) VObject *instantiate_VObject();

/**** function de creation de l'objet pour l'architecture Addon ****/
VObject* instantiate_VObject()
{
	// creer le VObject apres MeTOS va appeler la methode CreateObject() pour reelement
	// creer l'objet graphique (fenetre,vue etc...)
	return (new VTabView());
}

/**** Constructeur ****/
VTabView::VTabView()
: VObject()
{
	// definir les proprietes de bases pour l'objet
	// -si c'est un objet Be (BView,BButton etc ...)
	// -Donner le nom reel de la classe (c'est utile si vous creer un objet Be)
	//  se parametre est a false par defaut donc normalement vous n'avez pas a y toucher
	// -la metode d'ajout

	// attention ces proprietes existe deja utiliser Replace
	_properties->ReplaceInt32(VOBJECT_TYPE,TABVIEW_TYPE);
	_properties->ReplaceString(VOBJECT_CLASS,"BTabView");

	// parametres du VObject
	_settings->ReplaceBool(VOBJECT_BE_OBJECT,true);
	_settings->AddInt32(VOBJECT_DEPEND_ON,WINDOW_TYPE);
	_settings->AddInt32(VOBJECT_DEPEND_ON,VIEW_TYPE);
	_settings->AddInt32(VOBJECT_DEPEND_ON,BOX_TYPE);

	// ajouter les infos
	_infos->AddString(VOBJECT_VERSION,"1.0");
	_infos->AddString(VOBJECT_AUTHOR,"CKJ - Vincent Cedric");
	_infos->AddString(VOBJECT_ABOUT,"VTabView is Be BTabView Object\nCreate to use TabView from Be API.");
	_infos->AddString(VOBJECT_EMAIL,"ckj.beos@wanadoo.fr");
	_infos->AddString(VOBJECT_WWW,"http://perso.wanadoo.fr/cvincent/Home_page.html");

	// correspondance nom/variable
	_nameAsProperties->AddString("frame","_frame");
	_nameAsProperties->AddString("name","_name");
	_nameAsProperties->AddString("width","_but_width");
	_nameAsProperties->AddString("resizingMode","_resize_mode");
	_nameAsProperties->AddString("flags","_flags");
	
	// creation du menu
	_actionMenu = new BMenu("Select...");
}

/**** destructeur ****/
VTabView::~VTabView()
{
	// on doit liberer la memoire prise par le menu popup
	delete _actionMenu;
}

/**** creation de l'objet (graphique BButton) ****/
status_t VTabView::CreateObject(BMessage *archive)
{
	// on part du principe que le message est correcte
	// pas de probleme on creer l'objet
	// mais pour la TabView c'est speciale on utilise pas instantiate
	// on creer un objet selon les parametres
	// on a un probleme a cause de la vue conteneur
	status_t	state;
	BRect		frame;
	BString		name;
	int8		butWidth;
	int32		resizeMode;
	int32		flags;
	
	state = archive->FindRect("_frame",&frame);
	state &= archive->FindString("_name",&name);
	state &= archive->FindInt8("_but_width",&butWidth);
	state &= archive->FindInt32("_resize_mode",&resizeMode);
	state &= archive->FindInt32("_flags",&flags);
	
	if(state!=B_OK)
		return B_ERROR;
	
	// instancier l'objet
	// ajouter cette vue qui est la vue container
	_object = new BTabView(frame,name.String(),(button_width)butWidth,(uint32)resizeMode,(uint32)flags);
	((BTabView *)_object)->ChildAt(0)->SetName("_view_container");

	if(_object==NULL)
		return B_ERROR;
	
	return B_OK;
}

/**** renseigner les valeurs par defaut ****/
status_t VTabView::SetDefaultProperties(BMessage *properties)
{
	status_t	state;
	
	// ajouter les valeurs par defaut
	state = properties->AddRect("_frame",BRect(20,20,200,200));
	state &= properties->AddInt8("_but_width",B_WIDTH_AS_USUAL);
	state &= properties->AddInt32("_resize_mode",B_FOLLOW_NONE);
	state &= properties->AddInt32("_flags",B_FULL_UPDATE_ON_RESIZE | B_WILL_DRAW | B_NAVIGABLE_JUMP | B_FRAME_EVENTS | B_NAVIGABLE);
	state &= properties->AddInt32("_color",0xff);
	state &= properties->AddInt32("_color",0xd8d8d8ff);
	state &= properties->AddInt32("_color",0xd8d8d8ff);
	
	return state;
}

/**** changer un parametre de l'objet ****/
status_t VTabView::SetProperties(BMessage *value,int32 index)
{
	if(value==NULL)
		return B_ERROR;
		
	if(LooperLock())
	{
		BString		name;

		if(value->FindString("_name",&name)==B_OK)
			((BTabView *)_object)->SetName(name.String());
	
		LooperUnlock();

		return B_OK;	
	}
	else
		printf("Erreur : LooperLock() dans VTabView::SetProperties()\n");

	return B_ERROR;
}

/**** creer le menu d'action supplementaire ****/
BMenu *VTabView::BuildActionMenu()
{
	BString	tabName;
	int32	tabNumber;

	// recuperer le nombre de tabs
	if(((BTabView *)_object)->LockLooper())
	{
		tabNumber = ((BTabView *)_object)->CountTabs();
		((BTabView *)_object)->UnlockLooper();
	
		// construire le menu en fonction
		if(_actionMenu->CountItems()!=tabNumber)
		{
		
			// on doit alors le reconstruire
			delete _actionMenu;
			_actionMenu = new BMenu("Select...");

			// puis ajouter les items
			for(int32 i=0;i<tabNumber;i++)
			{
				tabName = (((BTabView *)_object)->TabAt(i))->Label();
				_actionMenu->AddItem(new BMenuItem(tabName.String(),NULL));
			}
		}
	
		return _actionMenu;
	}
	else
		printf("Erreur : LockLooper() dans VTabView::BuildActionMenu()\n");

	return NULL;
}

/**** executer l'action du menu de numero index ou de nom name ****/
void VTabView::ExecuteAction(BString &name,int32 index)
{
	// recuperer le nombre de tabs
	// si c'est pas en dehors on selectionne
	if(LooperLock())
	{
		int32	tabNumber;
		tabNumber = ((BTabView *)_object)->CountTabs();
		if(index<tabNumber)
			// selectionner l'onglet
			((BTabView *)_object)->Select(index);	
	
		LooperUnlock();

		// rafraichir les objet et la liste des objets
		be_app->PostMessage(M_REFRESH_OBJECTS_MSG);
	}
	else
		printf("Erreur : LooperLock() dans VTabView::ExecuteAction()\n");
}

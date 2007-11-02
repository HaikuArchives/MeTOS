#include "VObject.h"
#include "VOutlineListView.h"

#include <String.h>
#include <OutlineListView.h>
#include <Message.h>
#include <stdio.h>

extern "C" __declspec(dllexport) VObject *instantiate_VObject();

/**** function de creation de l'objet pour l'architecture Addon ****/
VObject* instantiate_VObject()
{
	// creer le VObject apres MeTOS va appeler la methode CreateObject() pour reelement
	// creer l'objet graphique (fenetre,vue etc...)
	return (new VOutlineListView());
}

/**** Constructeur ****/
VOutlineListView::VOutlineListView()
: VObject()
{
	// definir les proprietes de bases pour l'objet
	// -si c'est un objet Be (BOutlineListView,BButton etc ...)
	// -Donner le nom reel de la classe (c'est utile si vous creer un objet Be)
	//  se parametre est a false par defaut donc normalement vous n'avez pas a y toucher
	// -la metode d'ajout

	// attention ces proprietes existe deja utiliser Replace
	_properties->ReplaceInt32(VOBJECT_TYPE,LISTVIEW_TYPE);
	_properties->ReplaceString(VOBJECT_CLASS,"BOutlineListView");

	// parametres VObject
	_settings->ReplaceBool(VOBJECT_BE_OBJECT,true);
	_settings->AddInt32(VOBJECT_DEPEND_ON,WINDOW_TYPE);
	_settings->AddInt32(VOBJECT_DEPEND_ON,VIEW_TYPE);
	_settings->AddInt32(VOBJECT_DEPEND_ON,BOX_TYPE);
	_settings->AddInt32(VOBJECT_DEPEND_ON,TABVIEW_TYPE);
	_settings->AddInt32(VOBJECT_DEPEND_ON,SCROLLVIEW_TYPE);
	_settings->AddInt32(VOBJECT_DEPEND_ON,CONTAINER_TYPE);

	// ajouter les infos
	_infos->AddString(VOBJECT_VERSION,"1.0");
	_infos->AddString(VOBJECT_AUTHOR,"CKJ - Vincent Cedric");
	_infos->AddString(VOBJECT_ABOUT,"BOutlineListView is the Be OutlineListView object\nIt's the simple outlined list View from BeOS API.");
	_infos->AddString(VOBJECT_EMAIL,"ckj.beos@wanadoo.fr");
	_infos->AddString(VOBJECT_WWW,"http://perso.wanadoo.fr/cvincent/Home_page.html");

	// bloquer certaine proprietes
	// on doit ajouter la chaine #locked-index# qui est VOBJECT_LOCKED_INDEX
	// pour specifier des indexes
	_lockedProperties->AddInt32("_color",VOBJECT_PROPERTIES_HIDDEN);

	// correspondance nom/variable
	_nameAsProperties->AddString("frame","_frame");
	_nameAsProperties->AddString("name","_name");
	_nameAsProperties->AddString("resizeMask","_resize_mode");
	_nameAsProperties->AddString("flags","_flags");
	_nameAsProperties->AddString("type","_lv_type");
}

/**** creation de l'objet (graphique BButton) ****/
status_t VOutlineListView::CreateObject(BMessage *archive)
{
	// on part du principe que le message est correcte
	// pas de probleme on creer l'objet
	_object = BOutlineListView::Instantiate(archive);
	if(_object==NULL)
		return B_ERROR;
	
	return B_OK;
}

/**** renseigner les valeurs par defaut ****/
status_t VOutlineListView::SetDefaultProperties(BMessage *properties)
{
	status_t	state;
	
	// ajouter les valeurs par defaut
	// ATTENTION ne pas ajouter la propriete "_name" le programme
	// l'ajoute tout seul a chaque objet
	state = properties->AddRect("_frame",BRect(30,30,140,140));
	state &= properties->AddInt32("_resize_mode",B_FOLLOW_LEFT | B_FOLLOW_TOP);
	state &= properties->AddInt32("_flags",B_WILL_DRAW | B_NAVIGABLE | B_FRAME_EVENTS);
	state &= properties->AddInt32("_color",0xff);
	state &= properties->AddInt32("_color",0xffffffff);
	state &= properties->AddInt32("_color",0xffffffff);
	state &= properties->AddInt32("_lv_type",B_SINGLE_SELECTION_LIST);

	return state;
}

/**** changer un parametre de l'objet ****/
status_t VOutlineListView::SetProperties(BMessage *value,int32 index)
{
	if(value==NULL)
		return B_ERROR;

	// avant tout locker le looper (la fenetre)
	if(LooperLock())
	{
		// changer les proprietes	
		BString		name;
		
		if(value->FindString("_name",&name)==B_OK)
			((BOutlineListView *)_object)->SetName(name.String());
		
		// pas oublier de delocker le looper
		LooperUnlock();
	
		return B_OK;	
	}
	else
		printf("Erreur : LooperLock() dans VOutlineListView::SetProperties()\n");
	
	return B_ERROR;
}
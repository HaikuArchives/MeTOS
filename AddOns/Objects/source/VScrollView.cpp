#include "VObject.h"
#include "VScrollView.h"

#include <String.h>
#include <ScrollView.h>
#include <Message.h>
#include <stdio.h>

extern "C" __declspec(dllexport) VObject *instantiate_VObject();

/**** function de creation de l'objet pour l'architecture Addon ****/
VObject* instantiate_VObject()
{
	// creer le VObject apres MeTOS va appeler la methode CreateObject() pour reelement
	// creer l'objet graphique (fenetre,vue etc...)
	return (new VScrollView());
}

/**** Constructeur ****/
VScrollView::VScrollView()
: VObject()
{
	// definir les proprietes de bases pour l'objet
	// -si c'est un objet Be (BView,BButton etc ...)
	// -Donner le nom reel de la classe (c'est utile si vous creer un objet Be)
	//  se parametre est a false par defaut donc normalement vous n'avez pas a y toucher
	// -la metode d'ajout

	// attention ces proprietes existe deja utiliser Replace
	_properties->ReplaceInt32(VOBJECT_TYPE,SCROLLVIEW_TYPE);
	_properties->ReplaceString(VOBJECT_CLASS,"BScrollView");

	// parametres VObject
	_settings->ReplaceBool(VOBJECT_BE_OBJECT,true);
	_settings->AddInt32(VOBJECT_DEPEND_ON,WINDOW_TYPE);
	_settings->AddInt32(VOBJECT_DEPEND_ON,VIEW_TYPE);
	_settings->AddInt32(VOBJECT_DEPEND_ON,BOX_TYPE);
	_settings->AddInt32(VOBJECT_DEPEND_ON,TABVIEW_TYPE);

	// ajouter les infos
	_infos->AddString(VOBJECT_VERSION,"1.0");
	_infos->AddString(VOBJECT_AUTHOR,"CKJ - Vincent Cedric");
	_infos->AddString(VOBJECT_ABOUT,"VScrollView is the Be BScrollView Object\nUse to add a scroll view on another view.");
	_infos->AddString(VOBJECT_EMAIL,"ckj.beos@wanadoo.fr");
	_infos->AddString(VOBJECT_WWW,"http://perso.wanadoo.fr/cvincent/Home_page.html");

	// correspondance nom/variable
	_nameAsProperties->AddString("name","_name");
	_nameAsProperties->AddString("target",VOBJECT_CHILD_OBJECT);
	_nameAsProperties->AddString("resizeMask","_resize_mode");
	_nameAsProperties->AddString("flags","_flags");
	_nameAsProperties->AddString("horizontal","_scroll_bar");
	_nameAsProperties->AddString("vertical","_scroll_bar");
	_nameAsProperties->AddString("border","_style");
}

/**** creation de l'objet (graphique BButton) ****/
status_t VScrollView::CreateObject(BMessage *archive)
{
	// on part du principe que le message est correcte
	// pas de probleme on creer l'objet
	// mais pour la ScrollView c'est speciale on utilise pas instantiate
	// on creer un objet selon les parametres
	// on a un probleme a cause de la vue conteneur et des scrollBar
	status_t	state;
	BRect		frame;
	BString		name;
	int32		resizeMode;
	int32		flags;
	bool		horizontal;
	bool		vertical;
	int32		borderStyle;
	
	state = archive->FindRect("_frame",&frame);
	state &= archive->FindString("_name",&name);
	state &= archive->FindInt32("_resize_mode",&resizeMode);
	state &= archive->FindInt32("_flags",&flags);
	state &= archive->FindBool("_scroll_bar",0,&horizontal);
	state &= archive->FindBool("_scroll_bar",1,&vertical);
	state &= archive->FindInt32("_style",&borderStyle);
	
	if(state!=B_OK)
		return B_ERROR;
	
	// instancier l'objet
	BView	*container = NULL;
	
	// tenir compte de la taille des scroll-bar
	frame.right -= B_V_SCROLL_BAR_WIDTH;
	frame.bottom -= B_H_SCROLL_BAR_HEIGHT;
	container = new BView(frame,"_view_container",B_FOLLOW_ALL_SIDES, B_WILL_DRAW);
	_object = new BScrollView(name.String(),container,(uint32)resizeMode,(uint32)flags,horizontal,vertical,(border_style)borderStyle);

	if(_object==NULL)
		return B_ERROR;
	
	return B_OK;
}

/**** renseigner les valeurs par defaut ****/
status_t VScrollView::SetDefaultProperties(BMessage *properties)
{
	status_t	state;
	
	// ajouter les valeurs par defaut
	// ATTENTION ne pas ajouter la propriete "_name" le programme
	// l'ajoute tout seul a chaque objet
	state = properties->AddRect("_frame",BRect(30,30,140,140));
	state &= properties->AddInt32("_resize_mode",B_FOLLOW_NONE);
	state &= properties->AddInt32("_flags",B_WILL_DRAW);
	state &= properties->AddBool("_scroll_bar",true);
	state &= properties->AddBool("_scroll_bar",true);
	state &= properties->AddInt32("_style",B_FANCY_BORDER);

	return state;
}

/**** changer un parametre de l'objet ****/
status_t VScrollView::SetProperties(BMessage *value,int32 index)
{
	if(value==NULL)
		return B_ERROR;
		
	if(LooperLock())
	{
		BString		name;

		if(value->FindString("_name",&name)==B_OK)
			((BScrollView *)_object)->SetName(name.String());

		LooperUnlock();

		return B_OK;	
	}
	else
		printf("Erreur : LooperLock() dans VScrollView::SetProperties()\n");

	return B_ERROR;
}
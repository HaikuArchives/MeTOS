#include "VObject.h"
#include "VScrollBar.h"

#include <String.h>
#include <ScrollBar.h>
#include <Message.h>

extern "C" __declspec(dllexport) VObject *instantiate_VObject();

/**** function de creation de l'objet pour l'architecture Addon ****/
VObject* instantiate_VObject()
{
	// creer le VObject apres MeTOS va appeler la methode CreateObject() pour reelement
	// creer l'objet graphique (fenetre,vue etc...)
	return (new VScrollBar());
}

/**** Constructeur ****/
VScrollBar::VScrollBar()
: VObject()
{
	// definir les proprietes de bases pour l'objet
	// -si c'est un objet Be (BView,BButton etc ...)
	// -Donner le nom reel de la classe (c'est utile si vous creer un objet Be)
	//  se parametre est a false par defaut donc normalement vous n'avez pas a y toucher
	// -la metode d'ajout

	// attention ces proprietes existe deja utiliser Replace
	_properties->ReplaceInt32(VOBJECT_TYPE,SCROLLBAR_TYPE);
	_properties->ReplaceString(VOBJECT_CLASS,"BScrollBar");

	// parametres VObject
	_settings->ReplaceBool(VOBJECT_BE_OBJECT,true);
	_settings->AddInt32(VOBJECT_DEPEND_ON,WINDOW_TYPE);
	_settings->AddInt32(VOBJECT_DEPEND_ON,VIEW_TYPE);

	// ajouter les infos
	_infos->AddString(VOBJECT_VERSION,"1.0");
	_infos->AddString(VOBJECT_AUTHOR,"CKJ - Vincent Cedric");
	_infos->AddString(VOBJECT_ABOUT,"VScrollBar is the Be BScrollBar Object\nUse to add a scroll bar on another view.");
	_infos->AddString(VOBJECT_EMAIL,"ckj.beos@wanadoo.fr");
	_infos->AddString(VOBJECT_WWW,"http://perso.wanadoo.fr/cvincent/Home_page.html");

	// bloquer certaine proprietes
	_lockedProperties->AddInt32("_val",VOBJECT_PROPERTIES_HIDDEN);

	// correspondance nom/variable
	_nameAsProperties->AddString("frame","_frame");
	_nameAsProperties->AddString("name","_name");
	_nameAsProperties->AddString("target",VOBJECT_CHILD_OBJECT);
	_nameAsProperties->AddString("min","_range");
	_nameAsProperties->AddString("max","_range");
	_nameAsProperties->AddString("orientation","_orient");
}

/**** creation de l'objet (graphique BButton) ****/
status_t VScrollBar::CreateObject(BMessage *archive)
{
	// on part du principe que le message est correcte
	// pas de probleme on creer l'objet
	_object = BScrollBar::Instantiate(archive);
	if(_object==NULL)
		return B_ERROR;

	return B_OK;
}

/**** renseigner les valeurs par defaut ****/
status_t VScrollBar::SetDefaultProperties(BMessage *properties)
{
	status_t	state = B_OK;
	
	// ajouter les valeurs par defaut
	// ATTENTION ne pas ajouter la propriete "_name" le programme
	// l'ajoute tout seul a chaque objet
	state &= properties->AddRect("_frame",BRect(10,10,100,24));
	state &= properties->AddInt32("_resize_mode",0x3234);//B_FOLLOW_NONE);
	state &= properties->AddInt32("_flags",0x20000000);//B_WILL_DRAW);
	state &= properties->AddInt32("_color",0xff);
	state &= properties->AddInt32("_color",0xffffffff);
	state &= properties->AddInt32("_color",0xffffffff);
	state &= properties->AddFloat("_range",0.0000);
	state &= properties->AddFloat("_range",100.0000);
	state &= properties->AddFloat("_steps",1.0000);
	state &= properties->AddFloat("_steps",10.0000);
	state &= properties->AddFloat("_val",0.0000);
	state &= properties->AddInt32("_orient",0);
	state &= properties->AddFloat("_prop",0.0000);

	return state;
}

/**** changer un parametre de l'objet ****/
status_t VScrollBar::SetProperties(BMessage *value)
{
	if(value==NULL)
		return B_ERROR;
		
	BString		name;

	LooperLock();

	if(value->FindString("_name",&name)==B_OK)
		((BScrollBar *)_object)->SetName(name.String());

	LooperUnlock();
	
	return B_OK;	
}
#include "VObject.h"
#include "VMenuItem.h"

#include <String.h>
#include <MenuItem.h>
#include <Message.h>
#include <Window.h>
#include <stdio.h>

extern "C" __declspec(dllexport) VObject *instantiate_VObject();

/**** function de creation de l'objet pour l'architecture Addon ****/
VObject* instantiate_VObject()
{
	// creer le VObject apres MeTOS va appeler la methode CreateObject() pour reelement
	// creer l'objet graphique (fenetre,vue etc...)
	return (new VMenuItem());
}

/**** Constructeur ****/
VMenuItem::VMenuItem()
: VObject()
{
	// definir les proprietes de bases pour l'objet
	// -si c'est un objet Be (BView,BButton etc ...)
	// -Donner le nom reel de la classe (c'est utile si vous creer un objet Be)
	//  se parametre est a false par defaut donc normalement vous n'avez pas a y toucher
	// -la metode d'ajout

	// attention ces proprietes existe deja utiliser Replace
	_properties->ReplaceInt32(VOBJECT_TYPE,MENUITEM_TYPE);
	_properties->ReplaceString(VOBJECT_CLASS,"BMenuItem");

	// parametres VObject
	_settings->ReplaceBool(VOBJECT_BE_OBJECT,true);
	_settings->AddInt32(VOBJECT_DEPEND_ON,MENU_TYPE);
	_settings->AddInt32(VOBJECT_DEPEND_ON,POPUPMENU_TYPE);

	// ajouter les infos
	_infos->AddString(VOBJECT_VERSION,"1.0");
	_infos->AddString(VOBJECT_AUTHOR,"CKJ - Vincent Cedric");
	_infos->AddString(VOBJECT_ABOUT,"BMenuItem is the Be MenuItem object\nIt's the simple MenuItem from BeOS API.");
	_infos->AddString(VOBJECT_EMAIL,"ckj.beos@wanadoo.fr");
	_infos->AddString(VOBJECT_WWW,"http://perso.wanadoo.fr/cvincent/Home_page.html");

	// correspondance nom/variable
	_nameAsProperties->AddString("label","_label");
	_nameAsProperties->AddString("message","_msg");
	_nameAsProperties->AddString("shortcut","_shortcut");
	_nameAsProperties->AddString("modifiers","_mods");
}

/**** destructeur ****/
VMenuItem::~VMenuItem()
{
}

/**** creation de l'objet (graphique BButton) ****/
status_t VMenuItem::CreateObject(BMessage *archive)
{
	// on part du principe que le message est correcte
	// pas de probleme on creer l'objet
	_object = BMenuItem::Instantiate(archive);
	if(_object==NULL)
		return B_ERROR;
		
	return B_OK;
}

/**** renseigner les valeurs par defaut ****/
status_t VMenuItem::SetDefaultProperties(BMessage *properties)
{
	BMessage	emptyMessage((uint32)0x0);
	status_t	state;

	
	// ajouter les valeurs par defaut
	// ATTENTION ne pas ajouter la propriete "_name" le programme
	// l'ajoute tout seul a chaque objet
	state = properties->AddString("_label","MenuItem");
	state &= properties->AddInt32("_shortcut",0x0);
	state &= properties->AddInt32("_mods",0x2);
	state &= properties->AddMessage("_msg",&emptyMessage);

	return state;
}

/**** changer un parametre de l'objet ****/
status_t VMenuItem::SetProperties(BMessage *value,int32 index)
{
	if(value==NULL)
		return B_ERROR;

	// avant tout locker le looper (la fenetre)
	if(LooperLock())
	{
		// changer les proprietes	
		BString		label;
	
		if(value->FindString("_label",&label)==B_OK)
			((BMenuItem *)_object)->SetLabel(label.String());

		// pas oublier de delocker le looper
		LooperUnlock();
		
		return B_OK;	
	}
	else
		printf("Erreur : LooperLock() dans VMenuItem::SetProperties()\n");

	return B_ERROR;
}
#include "VObject.h"
#include "VMenu.h"

#include <String.h>
#include <Menu.h>
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
	return (new VMenu());
}

/**** Constructeur ****/
VMenu::VMenu()
: VObject()
{
	// definir les proprietes de bases pour l'objet
	// -si c'est un objet Be (BView,BButton etc ...)
	// -Donner le nom reel de la classe (c'est utile si vous creer un objet Be)
	//  se parametre est a false par defaut donc normalement vous n'avez pas a y toucher
	// -la metode d'ajout

	// attention ces proprietes existe deja utiliser Replace
	_properties->ReplaceInt32(VOBJECT_TYPE,MENU_TYPE);
	_properties->ReplaceString(VOBJECT_CLASS,"BMenu");

	// parametres VObject
	_settings->ReplaceBool(VOBJECT_BE_OBJECT,true);
	_settings->AddInt32(VOBJECT_DEPEND_ON,MENUBAR_TYPE);
	_settings->AddInt32(VOBJECT_DEPEND_ON,MENU_TYPE);

	// ajouter les infos
	_infos->AddString(VOBJECT_VERSION,"1.0");
	_infos->AddString(VOBJECT_AUTHOR,"CKJ - Vincent Cedric");
	_infos->AddString(VOBJECT_ABOUT,"BMenu is the Be Menu object\nIt's the simple Menu from BeOS API.");
	_infos->AddString(VOBJECT_EMAIL,"ckj.beos@wanadoo.fr");
	_infos->AddString(VOBJECT_WWW,"http://perso.wanadoo.fr/cvincent/Home_page.html");

	// bloquer certaine proprietes
	_lockedProperties->AddInt32("_fname",VOBJECT_PROPERTIES_DISABLED);
	_lockedProperties->AddInt32("_fflt",VOBJECT_PROPERTIES_DISABLED);
	_lockedProperties->AddInt32("_color",VOBJECT_PROPERTIES_DISABLED);

	// correspondance nom/variable
	_nameAsProperties->AddString("title","_name");
	_nameAsProperties->AddString("layout","_layout");
}

/**** destructeur ****/
VMenu::~VMenu()
{
}

/**** creation de l'objet (graphique BButton) ****/
status_t VMenu::CreateObject(BMessage *archive)
{
	// on part du principe que le message est correcte
	// pas de probleme on creer l'objet
	_object = BMenu::Instantiate(archive);
	if(_object==NULL)
		return B_ERROR;
		
	return B_OK;
}

/**** renseigner les valeurs par defaut ****/
status_t VMenu::SetDefaultProperties(BMessage *properties)
{
	status_t	state;
	
	// ajouter les valeurs par defaut
	// ATTENTION ne pas ajouter la propriete "_name" le programme
	// l'ajoute tout seul a chaque objet
	state = properties->AddRect("_frame",BRect(0,0,0,0));
	state &= properties->AddInt32("_flags",B_WILL_DRAW);
	state &= properties->AddString("_fname","Swis721 BT");
	state &= properties->AddString("_fname","Roman");
	state &= properties->AddFloat("_fflt",11.0000);
	state &= properties->AddFloat("_fflt",-1.0000);
	state &= properties->AddFloat("_fflt",-1.0000);
	state &= properties->AddInt32("_color",0xff);
	state &= properties->AddInt32("_color",0xd8d8d800);
	state &= properties->AddInt32("_color",0xd8d8d800); // c'est la couleur de fond
	state &= properties->AddInt32("_layout",B_ITEMS_IN_COLUMN);
	state &= properties->AddBool("_rsize_to_fit",true);

	return state;
}

/**** changer un parametre de l'objet ****/
status_t VMenu::SetProperties(BMessage *value,int32 index)
{
	if(value==NULL)
		return B_ERROR;

	// changer les proprietes	
	if(LooperLock())
	{
		BString		name;
		if(value->FindString("_name",&name)==B_OK)
			((BView *)_object)->SetName(name.String());
		
		LooperUnlock();

		return B_OK;	
	}
	else
		printf("Erreur : LooperLock() dans VMenu::SetProperties()\n");

	return B_ERROR;
}
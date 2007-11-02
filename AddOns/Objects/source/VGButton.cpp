#include "VObject.h"
#include "VGButton.h"
#include "GButton.h"

#include <String.h>
#include <Message.h>
#include <Button.h>
#include <stdio.h>

extern "C" __declspec(dllexport) VObject *instantiate_VObject();

/**** function de creation de l'objet pour l'architecture Addon ****/
VObject* instantiate_VObject()
{
	// creer le VObject apres MeTOS va appeler la methode CreateObject() pour reelement
	// creer l'objet graphique (fenetre,vue etc...)
	return (new VGButton());
}

/**** Constructeur ****/
VGButton::VGButton()
: VObject()
{
	// definir les proprietes de bases pour l'objet
	// -si c'est un objet Be (BView,BButton etc ...)
	// -Donner le nom reel de la classe (c'est utile si vous creer un objet Be)
	//  se parametre est a false par defaut donc normalement vous n'avez pas a y toucher
	// -la metode d'ajout

	// attention ces proprietes existe deja utiliser Replace
	_properties->ReplaceInt32(VOBJECT_TYPE,BUTTON_TYPE);
	_properties->ReplaceString(VOBJECT_CLASS,"GButton");

	// parametres du VObject
	_settings->ReplaceBool(VOBJECT_BE_OBJECT,false);
	_settings->AddInt32(VOBJECT_DEPEND_ON,WINDOW_TYPE);
	_settings->AddInt32(VOBJECT_DEPEND_ON,VIEW_TYPE);
	_settings->AddInt32(VOBJECT_DEPEND_ON,BOX_TYPE);

	// ajouter les infos
	_infos->AddString(VOBJECT_VERSION,"1.0");
	_infos->AddString(VOBJECT_AUTHOR,"CKJ - Vincent Cedric");
	_infos->AddString(VOBJECT_ABOUT,"VGButton is the Gonx GButton Object.");
	_infos->AddString(VOBJECT_EMAIL,"ckj.beos@wanadoo.fr");
	_infos->AddString(VOBJECT_WWW,"http://perso.wanadoo.fr/cvincent/Home_page.html");
	
	// correspondance nom/variable
	_nameAsProperties->AddString("frame","_frame");
	_nameAsProperties->AddString("name","_name");
	_nameAsProperties->AddString("label","_label");
	_nameAsProperties->AddString("message","_msg");
	_nameAsProperties->AddString("resizingMode","_resize_mode");
	_nameAsProperties->AddString("flags","_flags");
}

/**** creation de l'objet (graphique BButton) ****/
status_t VGButton::CreateObject(BMessage *archive)
{
	// on part du principe que le message est correcte
	// pas de probleme on creer l'objet
	_object = GButton::Instantiate(archive);
	if(_object==NULL)
		return B_ERROR;
	
	return B_OK;
}

/**** renseigner les valeurs par defaut ****/
status_t VGButton::SetDefaultProperties(BMessage *properties)
{
	BMessage	emptyMessage((uint32)0x0);
	status_t	state;
	
	// ajouter les valeurs par defaut
	state = properties->AddRect("_frame",BRect(70,70,140,140));
	state &= properties->AddMessage("_msg",&emptyMessage);
	state &= properties->AddString("_label","Button");
	state &= properties->AddInt32("_resize_mode",B_FOLLOW_LEFT | B_FOLLOW_TOP);
	state &= properties->AddInt32("_flags",B_WILL_DRAW | B_NAVIGABLE);

	return state;
}

/**** changer un parametre de l'objet ****/
status_t VGButton::SetProperties(BMessage *value,int32 index)
{
	if(value==NULL)
		return B_ERROR;

	if(LooperLock())
	{
		BString		buffer;
		// le nom de l'objet
		if(value->FindString("_name",&buffer)==B_OK)
			((BButton *)_object)->SetName(buffer.String());

		// le label
		if(value->FindString("_label",&buffer)==B_OK)
			((BButton *)_object)->SetLabel(buffer.String());

		LooperUnlock();
	
		return B_OK;	
	}
	else
		printf("Erreur : LooperLock() dans VGButton::SetProperties()\n");

	return B_ERROR;
}
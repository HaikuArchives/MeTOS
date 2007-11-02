#include "VObject.h"
#include "VTextControl.h"

#include <String.h>
#include <TextControl.h>
#include <Message.h>
#include <stdio.h>

extern "C" __declspec(dllexport) VObject *instantiate_VObject();

/**** function de creation de l'objet pour l'architecture Addon ****/
VObject* instantiate_VObject()
{
	// creer le VObject apres MeTOS va appeler la methode CreateObject() pour reelement
	// creer l'objet graphique (fenetre,vue etc...)
	return (new VTextControl());
}

/**** Constructeur ****/
VTextControl::VTextControl()
: VObject()
{
	// definir les proprietes de bases pour l'objet
	// -si c'est un objet Be (BTextControl,BButton etc ...)
	// -Donner le nom reel de la classe (c'est utile si vous creer un objet Be)
	//  se parametre est a false par defaut donc normalement vous n'avez pas a y toucher
	// -la metode d'ajout

	// attention ces proprietes existe deja utiliser Replace
	_properties->ReplaceInt32(VOBJECT_TYPE,TEXTCONTROL_TYPE);
	_properties->ReplaceString(VOBJECT_CLASS,"BTextControl");

	// parametres VObject
	_settings->ReplaceBool(VOBJECT_BE_OBJECT,true);
	_settings->AddInt32(VOBJECT_DEPEND_ON,WINDOW_TYPE);
	_settings->AddInt32(VOBJECT_DEPEND_ON,VIEW_TYPE);
	_settings->AddInt32(VOBJECT_DEPEND_ON,BOX_TYPE);

	// ajouter les infos
	_infos->AddString(VOBJECT_VERSION,"1.0");
	_infos->AddString(VOBJECT_AUTHOR,"CKJ - Vincent Cedric");
	_infos->AddString(VOBJECT_ABOUT,"BTextControl is the Be View object\nIt's the simple TextView from BeOS API.");
	_infos->AddString(VOBJECT_EMAIL,"ckj.beos@wanadoo.fr");
	_infos->AddString(VOBJECT_WWW,"http://perso.wanadoo.fr/cvincent/Home_page.html");

	// bloquer certaine proprietes
	_lockedProperties->AddInt32("_color",VOBJECT_PROPERTIES_HIDDEN);
	_lockedProperties->AddInt32("_tab",VOBJECT_PROPERTIES_HIDDEN);
	_lockedProperties->AddInt32("_sel",VOBJECT_PROPERTIES_HIDDEN);
	_lockedProperties->AddInt32("_runs",VOBJECT_PROPERTIES_HIDDEN);

	// correspondance nom/variable
	_nameAsProperties->AddString("frame","_frame");
	_nameAsProperties->AddString("name","_name");
	_nameAsProperties->AddString("label","_label");
	_nameAsProperties->AddString("initial_text","_text");
	_nameAsProperties->AddString("message","_msg");
	_nameAsProperties->AddString("rmask","_resize_mode");
	_nameAsProperties->AddString("flags","_flags");
}

/**** creation de l'objet (graphique BButton) ****/
status_t VTextControl::CreateObject(BMessage *archive)
{
	status_t	state = B_OK;
	BRect		frame;
	BString		name;
	BString		label;
	BString		text;
	BMessage	msg;
	int32		resizeMode;
	int32		flags;
	
	// on part du principe que le message est correcte
	// pas de probleme on creer l'objet
	state &= archive->FindRect("_frame",&frame);
	state &= archive->FindString("_name",&name);
	state &= archive->FindInt32("_resize_mode",&resizeMode);
	state &= archive->FindInt32("_flags",&flags);
	state &= archive->FindMessage("_msg",&msg);
	state &= archive->FindString("_label",&label);
	state &= archive->FindString("_text",&text);
	if(state!=B_OK)
		return B_ERROR;

	_object = new BTextControl(frame,name.String(),label.String(),text.String(),new BMessage(msg.what),resizeMode,flags);
	if(_object==NULL)
		return B_ERROR;
	
	return B_OK;
}

/**** renseigner les valeurs par defaut ****/
status_t VTextControl::SetDefaultProperties(BMessage *properties)
{
	BMessage	emptyMessage((uint32)0x0);
	status_t	state = B_OK;
	
	// ajouter les valeurs par defaut
	// ATTENTION ne pas ajouter la propriete "_name" le programme
	// l'ajoute tout seul a chaque objet
	state &= properties->AddRect("_frame",BRect(30,30,140,140));
	state &= properties->AddInt32("_resize_mode",B_FOLLOW_LEFT | B_FOLLOW_TOP);
	state &= properties->AddInt32("_flags",B_WILL_DRAW | B_NAVIGABLE);
	state &= properties->AddMessage("_msg",&emptyMessage);
	state &= properties->AddString("_fname","Swis721 BT");
	state &= properties->AddString("_fname","Roman");
	state &= properties->AddFloat("_fflt",10.0000);
	state &= properties->AddFloat("_fflt",-1.0000);
	state &= properties->AddFloat("_fflt",-1.0000);
	state &= properties->AddInt32("_color",0xff);
	state &= properties->AddInt32("_color",0xffffffff);
	state &= properties->AddInt32("_color",0xffffffff);
	state &= properties->AddString("_label","label");
	state &= properties->AddString("_text","");
	state &= properties->AddInt32("_a_label",0x0);
	state &= properties->AddInt32("_a _text",0x0);
	state &= properties->AddFloat("_divide",25.0000);
	
	return state;
}

/**** changer un parametre de l'objet ****/
status_t VTextControl::SetProperties(BMessage *value,int32 index)
{
	if(value==NULL)
		return B_ERROR;

	// avant tout locker le looper (la fenetre)
	if(LooperLock())
	{
		// changer les proprietes	
		BString		name;
		
		if(value->FindString("_name",&name)==B_OK)
			((BTextControl *)_object)->SetName(name.String());
		
		// pas oublier de delocker le looper
		LooperUnlock();

		return B_OK;	
	}
	else
		printf("Erreur : LooperLock() dans VTextControl::SetProperties()\n");

	return B_ERROR;
}
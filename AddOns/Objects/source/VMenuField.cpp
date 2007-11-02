#include "VObject.h"
#include "VMenuField.h"

#include <String.h>
#include <MenuField.h>
#include <Menu.h>
#include <Message.h>
#include <stdio.h>

extern "C" __declspec(dllexport) VObject *instantiate_VObject();

/**** function de creation de l'objet pour l'architecture Addon ****/
VObject* instantiate_VObject()
{
	// creer le VObject apres MeTOS va appeler la methode CreateObject() pour reelement
	// creer l'objet graphique (fenetre,vue etc...)
	return (new VMenuField());
}

/**** Constructeur ****/
VMenuField::VMenuField()
: VObject()
{
	// definir les proprietes de bases pour l'objet
	// -si c'est un objet Be (BView,BButton etc ...)
	// -Donner le nom reel de la classe (c'est utile si vous creer un objet Be)
	//  se parametre est a false par defaut donc normalement vous n'avez pas a y toucher
	// -la metode d'ajout

	// attention ces proprietes existe deja utiliser Replace
	_properties->ReplaceInt32(VOBJECT_TYPE,MENUFIELD_TYPE);
	_properties->ReplaceString(VOBJECT_CLASS,"BMenuField");

	// parametres VObject
	_settings->ReplaceBool(VOBJECT_BE_OBJECT,true);
	_settings->AddInt32(VOBJECT_DEPEND_ON,WINDOW_TYPE);
	_settings->AddInt32(VOBJECT_DEPEND_ON,VIEW_TYPE);
	_settings->AddInt32(VOBJECT_DEPEND_ON,BOX_TYPE);

	// ajouter les infos
	_infos->AddString(VOBJECT_VERSION,"1.0");
	_infos->AddString(VOBJECT_AUTHOR,"CKJ - Vincent Cedric");
	_infos->AddString(VOBJECT_ABOUT,"BMenuBar is the Be MenuField object\nIt's the simple Menu field from BeOS API.");
	_infos->AddString(VOBJECT_EMAIL,"ckj.beos@wanadoo.fr");
	_infos->AddString(VOBJECT_WWW,"http://perso.wanadoo.fr/cvincent/Home_page.html");

	// correspondance nom/variable
	_nameAsProperties->AddString("frame","_frame");
	_nameAsProperties->AddString("name","_name");
	_nameAsProperties->AddString("label","_label");
	_nameAsProperties->AddString("menu",VOBJECT_CHILD_OBJECT);
	_nameAsProperties->AddString("resize","_resize_mode");
}

/**** destructeur ****/
VMenuField::~VMenuField()
{
}

/**** creation de l'objet (graphique BButton) ****/
status_t VMenuField::CreateObject(BMessage *archive)
{
	BMenu		*menu = NULL;
	status_t	state;
	BRect		frame;
	BString		name;
	BString		label;
	int32		resizeMode;
	int32		flags;
	
	state = archive->FindRect("_frame",&frame);
	state &= archive->FindString("_name",&name);
	state &= archive->FindString("_label",&label);
	state &= archive->FindInt32("_resize_mode",&resizeMode);
	state &= archive->FindInt32("_flags",&flags);
	
	if(state!=B_OK)
		return B_ERROR;

	// on va utiliser un new car on doit avoir un menu et on va ajouter
	// un menu "factice" qui sera remplacer apres
	menu = new BMenu("Empty");
	_object = new BMenuField(frame,name.String(),label.String(),menu,(uint32)resizeMode,(uint32)flags);

	if(_object==NULL)
		return B_ERROR;
		
	return B_OK;
}

/**** renseigner les valeurs par defaut ****/
status_t VMenuField::SetDefaultProperties(BMessage *properties)
{
	status_t	state;
	
	// ajouter les valeurs par defaut
	// ATTENTION ne pas ajouter la propriete "_name" le programme
	// l'ajoute tout seul a chaque objet
	state = properties->AddRect("_frame",BRect(0,0,140,25));
	state &= properties->AddString("_label","MenuField");
	state &= properties->AddInt32("_resize_mode",B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	state &= properties->AddInt32("_flags",B_WILL_DRAW);
	state &= properties->AddString("_fname","Swis721 BT");
	state &= properties->AddString("_fname","Roman");
	state &= properties->AddFloat("_fflt",12.0000);
	state &= properties->AddFloat("_fflt",-1.0000);
	state &= properties->AddFloat("_fflt",-1.0000);
	state &= properties->AddInt32("_color",0xff);
	state &= properties->AddInt32("_color",0xdbdbdb00);
	state &= properties->AddInt32("_color",0xdbdbdb00); // c'est la couleur de fond

	return state;
}

/**** changer un parametre de l'objet ****/
status_t VMenuField::SetProperties(BMessage *value,int32 index)
{
	if(value==NULL)
		return B_ERROR;

	// avant tout locker le looper (la fenetre)
	if(LooperLock())
	{		
		// changer les proprietes	
		BString		name;
		uint32		color;
		if(value->FindString("_name",&name)==B_OK)
			((BView *)_object)->SetName(name.String());
		
		if(value->FindInt32("_color",(int32 *)&color)==B_OK && index==2)
		{
			((BView *)_object)->SetViewColor(((uint8 *)(&color))[3],((uint8 *)(&color))[2],((uint8 *)(&color))[1],((uint8 *)(&color))[0]);
			((BView *)_object)->Invalidate();
		}
	
		// pas oublier de delocker le looper
		LooperUnlock();

		return B_OK;	
	}
	else
		printf("Erreur : LooperLock() dans VMenuField::SetProperties()\n");

	return B_ERROR;
}
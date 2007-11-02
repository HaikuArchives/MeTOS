#include "VObject.h"
#include "VMenuBar.h"

#include <String.h>
#include <MenuBar.h>
#include <Message.h>
#include <stdio.h>

extern "C" __declspec(dllexport) VObject *instantiate_VObject();

/**** function de creation de l'objet pour l'architecture Addon ****/
VObject* instantiate_VObject()
{
	// creer le VObject apres MeTOS va appeler la methode CreateObject() pour reelement
	// creer l'objet graphique (fenetre,vue etc...)
	return (new VMenuBar());
}

/**** Constructeur ****/
VMenuBar::VMenuBar()
: VObject()
{
	// definir les proprietes de bases pour l'objet
	// -si c'est un objet Be (BView,BButton etc ...)
	// -Donner le nom reel de la classe (c'est utile si vous creer un objet Be)
	//  se parametre est a false par defaut donc normalement vous n'avez pas a y toucher
	// -la metode d'ajout

	// attention ces proprietes existe deja utiliser Replace
	_properties->ReplaceInt32(VOBJECT_TYPE,MENUBAR_TYPE);
	_properties->ReplaceString(VOBJECT_CLASS,"BMenuBar");

	// parametres VObject
	_settings->ReplaceBool(VOBJECT_BE_OBJECT,true);
	_settings->AddInt32(VOBJECT_DEPEND_ON,WINDOW_TYPE);
	_settings->AddInt32(VOBJECT_DEPEND_ON,VIEW_TYPE);

	// ajouter les infos
	_infos->AddString(VOBJECT_VERSION,"1.0");
	_infos->AddString(VOBJECT_AUTHOR,"CKJ - Vincent Cedric");
	_infos->AddString(VOBJECT_ABOUT,"BMenuBar is the Be MenuBar object\nIt's the simple Menu bar from BeOS API.");
	_infos->AddString(VOBJECT_EMAIL,"cedric-vincent@wanadoo.fr");
	_infos->AddString(VOBJECT_WWW,"http://perso.wanadoo.fr/cvincent/Home_page.html");

	// bloquer certaine proprietes
	_lockedProperties->AddInt32("_fname",VOBJECT_PROPERTIES_DISABLED);
	_lockedProperties->AddInt32("_fflt",VOBJECT_PROPERTIES_DISABLED);
	_lockedProperties->AddInt32("_color",VOBJECT_PROPERTIES_DISABLED);

	// correspondance nom/variable
	_nameAsProperties->AddString("frame","_frame");
	_nameAsProperties->AddString("title","_name");
	_nameAsProperties->AddString("resizeMask","_resize_mode");
	_nameAsProperties->AddString("layout","_layout");
	_nameAsProperties->AddString("resizeToFit","_rsize_to_fit");
}

/**** destructeur ****/
VMenuBar::~VMenuBar()
{
}

/**** creation de l'objet (graphique BButton) ****/
status_t VMenuBar::CreateObject(BMessage *archive)
{
	// on part du principe que le message est correcte
	// pas de probleme on creer l'objet
	_object = BMenuBar::Instantiate(archive);
	if(_object==NULL)
		return B_ERROR;
		
	return B_OK;
}

/**** renseigner les valeurs par defaut ****/
status_t VMenuBar::SetDefaultProperties(BMessage *properties)
{
	status_t	state;
	
	// ajouter les valeurs par defaut
	// ATTENTION ne pas ajouter la propriete "_name" le programme
	// l'ajoute tout seul a chaque objet
	state = properties->AddRect("_frame",BRect(0,0,140,25));
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
	state &= properties->AddBool("_rsize_to_fit",true);

	return state;
}

/**** changer un parametre de l'objet ****/
status_t VMenuBar::SetProperties(BMessage *value,int32 index)
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
		printf("Erreur : LooperLock() dans VMenuBar::SetProperties()\n");

	return B_ERROR;
}
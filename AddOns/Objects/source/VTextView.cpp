#include "VObject.h"
#include "VTextView.h"

#include <String.h>
#include <TextView.h>
#include <Message.h>

extern "C" __declspec(dllexport) VObject *instantiate_VObject();

// données flatten pour la proprietes "_run"
const unsigned char _runs[] = {
0x41,0x6c,0x69,0x21,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,
0x53,0x77,0x69,0x73,0x37,0x32,0x31,0x20,0x42,0x54,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x52,0x6f,0x6d,0x61,0x6e,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x41,0x20,0x00,0x00,0x42,0xb4,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0x00,0x00
};

/**** function de creation de l'objet pour l'architecture Addon ****/
VObject* instantiate_VObject()
{
	// creer le VObject apres MeTOS va appeler la methode CreateObject() pour reelement
	// creer l'objet graphique (fenetre,vue etc...)
	return (new VTextView());
}

/**** Constructeur ****/
VTextView::VTextView()
: VObject()
{
	// definir les proprietes de bases pour l'objet
	// -si c'est un objet Be (BTextView,BButton etc ...)
	// -Donner le nom reel de la classe (c'est utile si vous creer un objet Be)
	//  se parametre est a false par defaut donc normalement vous n'avez pas a y toucher
	// -la metode d'ajout

	// attention ces proprietes existe deja utiliser Replace
	_properties->ReplaceInt32(VOBJECT_TYPE,TEXTVIEW_TYPE);
	_properties->ReplaceString(VOBJECT_CLASS,"BTextView");

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
	_infos->AddString(VOBJECT_ABOUT,"BTextView is the Be View object\nIt's the simple TextView from BeOS API.");
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
	_nameAsProperties->AddString("textRect","_trect");
	_nameAsProperties->AddString("resizeMask","_resize_mode");
	_nameAsProperties->AddString("flags","_flags");
}

/**** creation de l'objet (graphique BButton) ****/
status_t VTextView::CreateObject(BMessage *archive)
{
	// on part du principe que le message est correcte
	// pas de probleme on creer l'objet
	_object = BTextView::Instantiate(archive);
	if(_object==NULL)
		return B_ERROR;
	
	return B_OK;
}

/**** renseigner les valeurs par defaut ****/
status_t VTextView::SetDefaultProperties(BMessage *properties)
{
	status_t	state;
	
	// ajouter les valeurs par defaut
	// ATTENTION ne pas ajouter la propriete "_name" le programme
	// l'ajoute tout seul a chaque objet
	state = properties->AddRect("_frame",BRect(30,30,140,140));
	state &= properties->AddInt32("_resize_mode",B_FOLLOW_ALL);
	state &= properties->AddInt32("_flags",B_WILL_DRAW);
	state &= properties->AddInt32("_color",0xff);
	state &= properties->AddInt32("_color",0xffffffff);
	state &= properties->AddInt32("_color",0xffffffff); // c'est la couleur de fond
	state &= properties->AddString("_text","");
	state &= properties->AddFloat("_tab",28.0000);
	state &= properties->AddInt32("_max",0x7fffffff);
	state &= properties->AddBool("_wrap",true);
	state &= properties->AddRect("_trect",BRect(0,0,110,110));
	state &= properties->AddInt32("_sel",0);
	state &= properties->AddInt32("_sel",0);
	state &= properties->AddInt32("_col_sp",4);
	state &= properties->AddData("_runs",B_RAW_TYPE,(const void *)_runs,160);
	
	return state;
}

/**** changer un parametre de l'objet ****/
status_t VTextView::SetProperties(BMessage *value,int32 index)
{
	if(value==NULL)
		return B_ERROR;

	// changer les proprietes	
	BString		name;
	uint32		color;

	// avant tout locker le looper (la fenetre)
	LooperLock();
		
	if(value->FindString("_name",&name)==B_OK)
		((BTextView *)_object)->SetName(name.String());
		
	if(value->FindInt32("_color",(int32 *)&color)==B_OK && index==2)
	{
		((BTextView *)_object)->SetViewColor(((uint8 *)(&color))[3],((uint8 *)(&color))[2],((uint8 *)(&color))[1],((uint8 *)(&color))[0]);
		((BTextView *)_object)->Invalidate();
	}
	
	// pas oublier de delocker le looper
	LooperUnlock();

	return B_OK;	
}
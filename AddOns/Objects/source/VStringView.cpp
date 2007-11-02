#include "VObject.h"
#include "VStringView.h"
#include "MetosMessageDefs.h"

#include <String.h>
#include <StringView.h>
#include <Message.h>
#include <Font.h>
#include <stdio.h>

extern "C" __declspec(dllexport) VObject *instantiate_VObject();

/**** function de creation de l'objet pour l'architecture Addon ****/
VObject* instantiate_VObject()
{
	// creer le VObject apres MeTOS va appeler la methode CreateObject() pour reelement
	// creer l'objet graphique (fenetre,vue etc...)
	return (new VStringView());
}

/**** Constructeur ****/
VStringView::VStringView()
: VObject()
{
	// definir les proprietes de bases pour l'objet
	// -si c'est un objet Be (BStringView,BButton etc ...)
	// -Donner le nom reel de la classe (c'est utile si vous creer un objet Be)
	//  se parametre est a false par defaut donc normalement vous n'avez pas a y toucher
	// -la metode d'ajout

	// attention ces proprietes existe deja utiliser Replace
	_properties->ReplaceInt32(VOBJECT_TYPE,STRINGVIEW_TYPE);
	_properties->ReplaceString(VOBJECT_CLASS,"BStringView");

	// parametres VObject
	_settings->ReplaceBool(VOBJECT_BE_OBJECT,true);
	_settings->AddInt32(VOBJECT_DEPEND_ON,WINDOW_TYPE);
	_settings->AddInt32(VOBJECT_DEPEND_ON,VIEW_TYPE);
	_settings->AddInt32(VOBJECT_DEPEND_ON,BOX_TYPE);

	// ajouter les infos
	_infos->AddString(VOBJECT_VERSION,"1.0");
	_infos->AddString(VOBJECT_AUTHOR,"CKJ - Vincent Cedric");
	_infos->AddString(VOBJECT_ABOUT,"BStringView is the Be StringView object\nIt's the simple StringView from BeOS API.");
	_infos->AddString(VOBJECT_EMAIL,"ckj.beos@wanadoo.fr");
	_infos->AddString(VOBJECT_WWW,"http://perso.wanadoo.fr/cvincent/Home_page.html");

	// bloquer certaine proprietes
	_lockedProperties->AddInt32("_color",VOBJECT_PROPERTIES_HIDDEN);

	// correspondance nom/variable
	_nameAsProperties->AddString("bounds","_frame");
	_nameAsProperties->AddString("name","_name");
	_nameAsProperties->AddString("text","_text");
	_nameAsProperties->AddString("resizeFlags","_resize_mode");
	_nameAsProperties->AddString("flags","_flags");
}

/**** creation de l'objet (graphique BButton) ****/
status_t VStringView::CreateObject(BMessage *archive)
{
	// on part du principe que le message est correcte
	// pas de probleme on creer l'objet
	_object = BStringView::Instantiate(archive);
	if(_object==NULL)
		return B_ERROR;
	
	return B_OK;
}

/**** renseigner les valeurs par defaut ****/
status_t VStringView::SetDefaultProperties(BMessage *properties)
{
	status_t	state;
	
	// ajouter les valeurs par defaut
	// ATTENTION ne pas ajouter la propriete "_name" le programme
	// l'ajoute tout seul a chaque objet
	state = properties->AddRect("_frame",BRect(30,30,140,60));
	state &= properties->AddInt32("_resize_mode",B_FOLLOW_LEFT | B_FOLLOW_TOP);
	state &= properties->AddInt32("_flags",B_WILL_DRAW);
	state &= properties->AddString("_fname","Swis721 BT");
	state &= properties->AddString("_fname","Roman");
	state &= properties->AddFloat("_fflt",10.0000);
	state &= properties->AddFloat("_fflt",90.0000);
	state &= properties->AddFloat("_fflt",0.0000);
	state &= properties->AddInt32("_color",0xff);
	state &= properties->AddInt32("_color",0xffffffff);
	state &= properties->AddInt32("_color",0xffffffff); // c'est la couleur de fond
	state &= properties->AddString("_text","Text");
	state &= properties->AddInt32("_align",0);

	return state;
}

/**** changer un parametre de l'objet ****/
status_t VStringView::SetProperties(BMessage *value,int32 index)
{
	if(value==NULL)
		return B_ERROR;

	// avant tout locker le looper (la fenetre)
	if(LooperLock())
	{
		// changer les proprietes
		BString		name;
		BString		text;
		BString		fname;
		BString		fstyle;
		alignment	align;
		float		fontSize;

		if(value->FindString("_name",&name)==B_OK)
			((BStringView *)_object)->SetName(name.String());

		if(value->FindString("_text",&text)==B_OK)
			((BStringView *)_object)->SetText(text.String());

		if(value->FindInt32("_align",(int32 *)&align)==B_OK)
			((BStringView *)_object)->SetAlignment(align);
	
		if(value->FindFloat("_fflt",&fontSize)==B_OK && index==0)
			((BStringView *)_object)->SetFontSize(fontSize);
	
		if(value->FindString("_fname",&fname)==B_OK)
		{
			font_family	family;
			font_style	style;
			BFont		font;
		
			// dans ce cas la on recupere les elements dans les
			// proprietes interne de l'objet
			_properties->FindString("_fname",0,&fname);
			strcpy(family,fname.String());
			_properties->FindString("_fname",1,&fname);
			strcpy(style,fname.String());
		
			// definir la police et son style
			((BStringView *)_object)->GetFont(&font);
			font.SetFamilyAndStyle(family,style);
			((BStringView *)_object)->SetFont(&font,B_FONT_FAMILY_AND_STYLE);
		}
	
		// pas oublier de delocker le looper
		LooperUnlock();

		return B_OK;	
	}
	else
		printf("Erreur : LooperLock() dans VStringView::SetProperties()\n");
	
	return B_ERROR;
}
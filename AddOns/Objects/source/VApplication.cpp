// on utilise un message pour gerer les projets
// car on en utilise deja pour la proprietes des objets
// donc c'est plus facile pour sauver et relire les données
 
#include "VApplication.h"
#include "VObject.h"

#include <Message.h>

extern "C" __declspec(dllexport) VObject *instantiate_VObject();

/**** function de creation de l'objet pour l'architecture Addon ****/
VObject* instantiate_VObject()
{
	// creer le VObject apres MeTOS va appeler la methode CreateObject() pour reelement
	// creer l'objet graphique (fenetre,vue etc...)
	return (new VApplication());
}
/**** constructeur ****/
VApplication::VApplication()
: VObject()
{
	// attention ces proprietes existe deja utiliser Replace
	_properties->ReplaceInt32(VOBJECT_TYPE,APPLICATION_TYPE);
	_properties->ReplaceString(VOBJECT_CLASS,"BApplication");

	// parametres du VObject
	_settings->ReplaceBool(VOBJECT_BE_OBJECT,true);
	_settings->ReplaceBool(VOBJECT_ONLY_ONE,true);
	_settings->AddInt32(VOBJECT_DEPEND_ON,PROJECT_TYPE);

	// ajouter les dependances et la version
	// et les infos sur l'auteur
	_infos->AddString(VOBJECT_VERSION,"1.0");
	_infos->AddString(VOBJECT_AUTHOR,"CKJ - Vincent Cedric");
	_infos->AddString(VOBJECT_ABOUT,"VApplication is the Be BApplication Object\nYou must create one for an application.");
	_infos->AddString(VOBJECT_EMAIL,"ckj.beos@wanadoo.fr");
	_infos->AddString(VOBJECT_WWW,"http://perso.wanadoo.fr/cvincent/Home_page.html");

	// correspondance nom/variable
	_nameAsProperties->AddString("signature","_mime_signature");
}

/**** destructeur ****/
VApplication::~VApplication()
{
}

/**** fixer le parametres par defaut ****/
status_t VApplication::SetDefaultProperties(BMessage *properties)
{
	status_t	state;
	
	// ajouter les valeurs par defaut
	state = properties->AddString("_mime_signature","application/x-vnd.MeTOS.Application");

	return state;
}

/**** modifier un parametre ****/
status_t VApplication::SetProperties(BMessage *value,int32 index)
{
	return B_OK;
}

/**** pour etre compatible VObject ****/
status_t VApplication::CreateObject(BMessage *archive)
{ return B_OK; }
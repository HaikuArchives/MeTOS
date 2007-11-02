// on utilise un message pour gerer les projets
// car on en utilise deja pour la proprietes des objets
// donc c'est plus facile pour sauver et relire les données
 
#include "VProject.h"
#include "VObject.h"

#include <Message.h>

extern "C" __declspec(dllexport) VObject *instantiate_VObject();

/**** function de creation de l'objet pour l'architecture Addon ****/
VObject* instantiate_VObject()
{
	// creer le VObject apres MeTOS va appeler la methode CreateObject() pour reelement
	// creer l'objet graphique (fenetre,vue etc...)
	return (new VProject());
}
/**** constructeur ****/
VProject::VProject()
: VObject()
{
	// attention ces proprietes existe deja utiliser Replace
	_properties->ReplaceInt32(VOBJECT_TYPE,PROJECT_TYPE);
	_properties->ReplaceString(VOBJECT_CLASS,"BProject");

	// parametres du VObject
	// le projet ne depend d'aucun type donc indefini
	_settings->ReplaceBool(VOBJECT_BE_OBJECT,false);
	_settings->ReplaceBool(VOBJECT_ONLY_ONE,true);
	_settings->AddInt32(VOBJECT_DEPEND_ON,UNDEFINED_TYPE);
	
	// ajouter les dependances et la version
	// et les infos sur l'auteur
	_infos->AddString(VOBJECT_VERSION,"1.0");
	_infos->AddString(VOBJECT_AUTHOR,"CKJ - Vincent Cedric");
	_infos->AddString(VOBJECT_ABOUT,"VProject is the first Object\nYou must create one before other.");
	_infos->AddString(VOBJECT_EMAIL,"ckj.beos@wanadoo.fr");
	_infos->AddString(VOBJECT_WWW,"http://perso.wanadoo.fr/cvincent/Home_page.html");

	// on ne peux pas deriver un Projet !
	_properties->RemoveName(VOBJECT_DERIVED_CLASS);
}

/**** destructeur ****/
VProject::~VProject()
{
}

/**** fixer le parametres par defaut ****/
status_t VProject::SetDefaultProperties(BMessage *properties)
{
	status_t	state;
	
	// ajouter les valeurs par defaut
	state = properties->AddString("_path","/boot/home");
	state = properties->AddString("_binary_name","BeApp");
	state = properties->AddInt32("_type_binary",0);
	state = properties->AddString("_resource","");
	state = properties->AddInt32("_optimize",2);
	state = properties->AddString("_defines","");
	state = properties->AddBool("_warnings",true);
	state = properties->AddBool("_symbols",false);
	state = properties->AddBool("_debugger",false);
	state = properties->AddString("_compiler_flags","");
	state = properties->AddString("_linker_flags","");
	state = properties->AddString("_app_version","");	

	return state;
}

/**** modifier un parametre ****/
status_t VProject::SetProperties(BMessage *value,int32 index)
{
	return B_OK;
}

/**** pour etre compatible VObject ****/
status_t VProject::CreateObject(BMessage *archive)
{ return B_OK; }
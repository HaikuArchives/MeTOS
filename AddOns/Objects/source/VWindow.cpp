#include "VObject.h"
#include "VWindow.h"

#include <String.h>
#include <Window.h>
#include <Message.h>
#include <stdio.h>

extern "C" __declspec(dllexport) VObject *instantiate_VObject();

/**** function de creation de l'objet pour l'architecture Addon ****/
VObject* instantiate_VObject()
{
	// creer le VObject apres MeTOS va appeler la methode CreateObject() pour reelement
	// creer l'objet graphique (fenetre,vue etc...)
	return (new VWindow());
}

/**** Constructeur ****/
VWindow::VWindow()
: VObject()
{
	// definir les proprietes de bases pour l'objet
	// -si c'est un objet Be (BView,BButton etc ...)
	// -Donner le nom reel de la classe (c'est utile si vous creer un objet Be)
	//  se parametre est a false par defaut donc normalement vous n'avez pas a y toucher
	// -la metode d'ajout

	// attention ces proprietes existe deja utiliser Replace
	_properties->ReplaceInt32(VOBJECT_TYPE,WINDOW_TYPE);
	_properties->ReplaceString(VOBJECT_CLASS,"BWindow");

	// parametres du VObject
	_settings->ReplaceBool(VOBJECT_BE_OBJECT,true);
	_settings->AddInt32(VOBJECT_DEPEND_ON,APPLICATION_TYPE);
	_settings->AddInt32(VOBJECT_DEPEND_ON,PROJECT_TYPE);

	// ajouter les dependances et la version
	// et les infos sur l'auteur
	_infos->AddString(VOBJECT_VERSION,"1.0");
	_infos->AddString(VOBJECT_AUTHOR,"CKJ - Vincent Cedric");
	_infos->AddString(VOBJECT_ABOUT,"BWindow is the Be window object\nIt's the simple window from BeOS API.");
	_infos->AddString(VOBJECT_EMAIL,"ckj.beos@wanadoo.fr");
	_infos->AddString(VOBJECT_WWW,"http://perso.wanadoo.fr/cvincent/Home_page.html");

	// bloquer certaine proprietes
	_lockedProperties->AddInt32("_port_cap",VOBJECT_PROPERTIES_HIDDEN);

	// correspondance nom/variable
	_nameAsProperties->AddString("frame","_frame");
	_nameAsProperties->AddString("title","_title");
	_nameAsProperties->AddString("feel","_wfeel");
	_nameAsProperties->AddString("look","_wlook");
	_nameAsProperties->AddString("flags","_flags");
	_nameAsProperties->AddString("workspace","_wspace");
}

/**** creation de l'objet (graphique BButton) ****/
status_t VWindow::CreateObject(BMessage *archive)
{
	// on part du principe que le message est correcte
	// pas de probleme on creer l'objet
	_object = BWindow::Instantiate(archive);
	if(_object==NULL)
		return B_ERROR;
	
	return B_OK;
}

/**** renseigner les valeurs par defaut ****/
status_t VWindow::SetDefaultProperties(BMessage *properties)
{
	status_t	state;
	
	// ajouter les valeurs par defaut
	state = properties->AddInt32("_port_cap",100);
	state &= properties->AddRect("_frame",BRect(100,100,400,400));
	state &= properties->AddString("_title","New BWindow");
	state &= properties->AddInt32("_wlook",B_DOCUMENT_WINDOW_LOOK);
	state &= properties->AddInt32("_wfeel",B_NORMAL_WINDOW_FEEL);
	state &= properties->AddInt32("_flags",B_NOT_ZOOMABLE);
	state &= properties->AddInt32("_wspace",B_CURRENT_WORKSPACE);

	return state;
}

/**** changer un parametre de l'objet ****/
status_t VWindow::SetProperties(BMessage *value,int32 index)
{
	// ATTENTION on ne va pas modifier le style
	// modal etc ... car ca risque de rendre innaccesible
	// la fenetre dans l'application

	if(value==NULL)
		return B_ERROR;
			
	if(LooperLock())
	{
		BString			title;
		window_look		look;
	
		// le titre
		if(value->FindString("_title",&title)==B_OK)
			((BWindow *)_object)->SetTitle(title.String());

		// le look
		if(value->FindInt32("_wlook",(int32 *)&look)==B_OK)
			((BWindow *)_object)->SetLook(look);

		LooperUnlock();

		return B_OK;	
	}
	else
		printf("Erreur : LooperLock() dans VWindow::SetProperties()\n");

	return B_ERROR;
}
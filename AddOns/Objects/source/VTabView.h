#ifndef _VTABVIEW_H
#define _VTABVIEW_H
/*****************************/
/* Addon du BTabView de BeOS */
/*****************************/

#include "VObject.h"

class BMenu;

class VTabView : public VObject
{
public:
	VTabView();
	~VTabView();
	
	status_t	CreateObject(BMessage *archive); 			// creer l'objet de l'interface (obligatoire)
	status_t	SetDefaultProperties(BMessage *properties);	// renseigner les valeurs par defaut
	status_t	SetProperties(BMessage *value,int32 index);	// changer un parametre

	BMenu		*BuildActionMenu();							// creer le menu d'action supplementaire
	void		ExecuteAction(BString &name,int32 index);	// executer l'action du menu de numero index ou de nom name

private:
	BMenu	*_actionMenu;	// menu des action speciales
};

#endif
#ifndef _VLISTVIEW_H
#define _VLISTVIEW_H
/******************************/
/* Addon du BListView de BeOS */
/******************************/

#include "VObject.h"

class VListView : public VObject
{
public:
	VListView();
	
	status_t	CreateObject(BMessage *archive); 			// creer l'objet de l'interface (obligatoire)
	status_t	SetDefaultProperties(BMessage *properties);	// renseigner les valeurs par defaut
	status_t	SetProperties(BMessage *value,int32 index);	// changer un parametre
};

#endif
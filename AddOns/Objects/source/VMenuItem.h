#ifndef _VMENUITEM_H
#define _VMENUITEM_H
/******************************/
/* Addon du BMenuItem de BeOS */
/******************************/

#include "VObject.h"

class BMenu;

class VMenuItem : public VObject
{
public:
	VMenuItem();
	virtual ~VMenuItem();
	
	status_t	CreateObject(BMessage *archive); 			// creer l'objet de l'interface (obligatoire)
	status_t	SetDefaultProperties(BMessage *properties);	// renseigner les valeurs par defaut
	status_t	SetProperties(BMessage *value,int32 index);	// changer un parametre
};

#endif
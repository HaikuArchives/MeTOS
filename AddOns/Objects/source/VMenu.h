#ifndef _VMENU_H
#define _VMENU_H
/**************************/
/* Addon du BMenu de BeOS */
/**************************/

#include "VObject.h"

class BMenu;

class VMenu : public VObject
{
public:
	VMenu();
	virtual ~VMenu();
	
	status_t	CreateObject(BMessage *archive); 			// creer l'objet de l'interface (obligatoire)
	status_t	SetDefaultProperties(BMessage *properties);	// renseigner les valeurs par defaut
	status_t	SetProperties(BMessage *value,int32 index);	// changer un parametre
};

#endif
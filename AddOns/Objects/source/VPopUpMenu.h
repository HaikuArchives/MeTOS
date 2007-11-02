#ifndef _VPOPUPMENU_H
#define _VPOPUPMENU_H
/*******************************/
/* Addon du BPopUpMenu de BeOS */
/*******************************/

#include "VObject.h"

class BPopUpMenu;

class VPopUpMenu : public VObject
{
public:
	VPopUpMenu();
	virtual ~VPopUpMenu();
	
	status_t	CreateObject(BMessage *archive); 			// creer l'objet de l'interface (obligatoire)
	status_t	SetDefaultProperties(BMessage *properties);	// renseigner les valeurs par defaut
	status_t	SetProperties(BMessage *value,int32 index);	// changer un parametre
};

#endif
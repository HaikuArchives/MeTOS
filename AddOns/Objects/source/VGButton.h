#ifndef _VGBUTTON_H
#define _VGBUTTON_H
/***********************************/
/* Addon du GButton de la lib Gonx */
/***********************************/

#include "VObject.h"

class VGButton : public VObject
{
public:
	VGButton();
	
	status_t	CreateObject(BMessage *archive); 			// creer l'objet de l'interface (obligatoire)
	status_t	SetDefaultProperties(BMessage *properties);	// renseigner les valeurs par defaut
	status_t	SetProperties(BMessage *value,int32 index);	// changer un parametre
};

#endif
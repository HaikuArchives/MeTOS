#ifndef _VWINDOW_H
#define _VWINDOW_H
/****************************/
/* Addon du BWindow de BeOS */
/****************************/

#include "VObject.h"

class VWindow : public VObject
{
public:
	VWindow();
	
	status_t	CreateObject(BMessage *archive); 			// creer l'objet de l'interface (obligatoire)
	status_t	SetDefaultProperties(BMessage *properties);	// renseigner les valeurs par defaut
	status_t	SetProperties(BMessage *value,int32 index);	// changer un parametre
};

#endif
#ifndef _VVIEW_H
#define _VVIEW_H
/**************************/
/* Addon du BView de BeOS */
/**************************/

#include "VObject.h"

class VView : public VObject
{
public:
	VView();
	
	status_t	CreateObject(BMessage *archive); 			// creer l'objet de l'interface (obligatoire)
	status_t	SetDefaultProperties(BMessage *properties);	// renseigner les valeurs par defaut
	status_t	SetProperties(BMessage *value,int32 index);	// changer un parametre
};

#endif
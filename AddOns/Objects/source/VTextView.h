#ifndef _VTEXTVIEW_H
#define _VTEXTVIEW_H
/******************************/
/* Addon du BTextView de BeOS */
/******************************/

#include "VObject.h"

class VTextView : public VObject
{
public:
	VTextView();
	
	status_t	CreateObject(BMessage *archive); 			// creer l'objet de l'interface (obligatoire)
	status_t	SetDefaultProperties(BMessage *properties);	// renseigner les valeurs par defaut
	status_t	SetProperties(BMessage *value,int32 index);	// changer un parametre
};

#endif
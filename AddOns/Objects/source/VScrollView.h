#ifndef _VSCROLLVIEW_H
#define _VSCROLLVIEW_H
/********************************/
/* Addon du BScrollView de BeOS */
/********************************/

#include "VObject.h"

class VScrollView : public VObject
{
public:
	VScrollView();
	
	status_t	CreateObject(BMessage *archive); 			// creer l'objet de l'interface (obligatoire)
	status_t	SetDefaultProperties(BMessage *properties);	// renseigner les valeurs par defaut
	status_t	SetProperties(BMessage *value,int32 index);	// changer un parametre
};

#endif
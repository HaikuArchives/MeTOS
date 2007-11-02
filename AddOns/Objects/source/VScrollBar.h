#ifndef _VSCROLLBAR_H
#define _VSCROLLBAR_H
/********************************/
/* Addon du BScrollBar de BeOS */
/********************************/
#include "VObject.h"
#include <ScrollBar.h>

// VObject
class VScrollBar : public VObject
{
public:
	VScrollBar();
	
	status_t	CreateObject(BMessage *archive); 			// creer l'objet de l'interface (obligatoire)
	status_t	SetDefaultProperties(BMessage *properties);	// renseigner les valeurs par defaut
	status_t	SetProperties(BMessage *value);				// changer un parametre
};

#endif
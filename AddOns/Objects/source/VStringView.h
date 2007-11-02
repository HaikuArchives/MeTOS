#ifndef _VSTRINGVIEW_H
#define _VSTRINGVIEW_H
/********************************/
/* Addon du BStringView de BeOS */
/********************************/

#include "VObject.h"
#include <Font.h>

class VStringView : public VObject
{
public:
	VStringView();
	
	status_t	CreateObject(BMessage *archive); 			// creer l'objet de l'interface (obligatoire)
	status_t	SetDefaultProperties(BMessage *properties);	// renseigner les valeurs par defaut
	status_t	SetProperties(BMessage *value,int32 index);	// changer un parametre
};

#endif
#ifndef _VTEXTCONTROL_H
#define _VTEXTCONTROL_H
/*********************************/
/* Addon du BTextControl de BeOS */
/*********************************/

#include "VObject.h"

class VTextControl : public VObject
{
public:
	VTextControl();
	
	status_t	CreateObject(BMessage *archive); 			// creer l'objet de l'interface (obligatoire)
	status_t	SetDefaultProperties(BMessage *properties);	// renseigner les valeurs par defaut
	status_t	SetProperties(BMessage *value,int32 index);	// changer un parametre
};

#endif
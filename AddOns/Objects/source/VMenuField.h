#ifndef _VMENUFIELD_H
#define _VMENUFIELD_H
/*******************************/
/* Addon du BMenuField de BeOS */
/*******************************/

#include "VObject.h"

class VMenuField : public VObject
{
public:
	VMenuField();
	virtual ~VMenuField();
	
	status_t	CreateObject(BMessage *archive); 			// creer l'objet de l'interface (obligatoire)
	status_t	SetDefaultProperties(BMessage *properties);	// renseigner les valeurs par defaut
	status_t	SetProperties(BMessage *value,int32 index);	// changer un parametre
};

#endif
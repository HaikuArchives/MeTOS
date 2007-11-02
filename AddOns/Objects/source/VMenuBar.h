#ifndef _VMENUBAR_H
#define _VMENUBAR_H
/*****************************/
/* Addon du BMenuBar de BeOS */
/*****************************/

#include "VObject.h"

class VMenuBar : public VObject
{
public:
	VMenuBar();
	virtual ~VMenuBar();
	
	status_t	CreateObject(BMessage *archive); 			// creer l'objet de l'interface (obligatoire)
	status_t	SetDefaultProperties(BMessage *properties);	// renseigner les valeurs par defaut
	status_t	SetProperties(BMessage *value,int32 index);	// changer un parametre
};

#endif
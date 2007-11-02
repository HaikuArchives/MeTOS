#ifndef _VOUTLINELISTVIEW_H
#define _VOUTLINELISTVIEW_H
/*************************************/
/* Addon du BOutlineListView de BeOS */
/*************************************/

#include "VObject.h"

class VOutlineListView : public VObject
{
public:
	VOutlineListView();
	
	status_t	CreateObject(BMessage *archive); 			// creer l'objet de l'interface (obligatoire)
	status_t	SetDefaultProperties(BMessage *properties);	// renseigner les valeurs par defaut
	status_t	SetProperties(BMessage *value,int32 index);	// changer un parametre
};

#endif
#ifndef _VBUTTON_H
#define _VBUTTON_H
/**************************/
/* Addon of BeOS BButton  */
/**************************/

#include "VObject.h"

class VButton : public VObject
{
public:
	VButton();
	
	status_t	CreateObject(BMessage *archive); 			// create the GUI object (needed)
	status_t	SetDefaultProperties(BMessage *properties);	// set default values
	status_t	SetProperties(BMessage *value,int32 index);	// modify a parameter
};

#endif
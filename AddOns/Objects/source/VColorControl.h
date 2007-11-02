#ifndef _VCOLORCONTROL_H
#define _VCOLORCONTROL_H
/********************************/
/* Addon of BeOS BColorControl  */
/********************************/

#include "VObject.h"

class VColorControl : public VObject
{
public:
	VColorControl();
	
	status_t	CreateObject(BMessage *archive); 			// create the GUI object (needed)
	status_t	SetDefaultProperties(BMessage *properties);	// set default values
	status_t	SetProperties(BMessage *value,int32 index);	// modify a parameter
};

#endif
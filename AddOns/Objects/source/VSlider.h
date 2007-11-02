#ifndef _VSLIDER_H
#define _VSLIDER_H
/**************************/
/* Addon of BeOS BSlider  */
/**************************/

#include "VObject.h"

class VSlider : public VObject
{
public:
	VSlider();
	
	status_t	CreateObject(BMessage *archive); 			// create the GUI object (needed)
	status_t	SetDefaultProperties(BMessage *properties);	// set default values
	status_t	SetProperties(BMessage *value,int32 index);	// modify a parameter
};

#endif
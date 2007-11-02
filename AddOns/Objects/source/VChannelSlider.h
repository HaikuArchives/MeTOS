#ifndef _VCHANNELSLIDER_H
#define _VCHANNELSLIDER_H
/********************************/
/* Addon of BeOS BChannelSlider */
/********************************/

#include "VObject.h"

class VChannelSlider : public VObject
{
public:
	VChannelSlider();
	
	status_t	CreateObject(BMessage *archive); 			// create the GUI object (needed)
	status_t	SetDefaultProperties(BMessage *properties);	// set default values
	status_t	SetProperties(BMessage *value,int32 index);	// modify a parameter
};

#endif
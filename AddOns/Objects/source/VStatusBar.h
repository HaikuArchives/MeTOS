#ifndef _VSTATUSBAR_H
#define _VSTATUSBAR_H
/*****************************/
/* Addon of BeOS BStatusBar  */
/*****************************/

#include "VObject.h"

class VStatusBar : public VObject
{
public:
	VStatusBar();
	
	status_t	CreateObject(BMessage *archive); 			// create the GUI object (needed)
	status_t	SetDefaultProperties(BMessage *properties);	// set default values
	status_t	SetProperties(BMessage *value,int32 index);	// modify a parameter
};

#endif
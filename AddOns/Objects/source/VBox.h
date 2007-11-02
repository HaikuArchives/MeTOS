#ifndef _VBOX_H
#define _VBOX_H
/***********************/
/* Addon of BeOS BBox  */
/***********************/

#include "VObject.h"

class VBox : public VObject
{
public:
	VBox();
	
	status_t	CreateObject(BMessage *archive); 			// create the GUI object (needed)
	status_t	SetDefaultProperties(BMessage *properties);	// set default values
	status_t	SetProperties(BMessage *value,int32 index);	// modify a parameter
};

#endif
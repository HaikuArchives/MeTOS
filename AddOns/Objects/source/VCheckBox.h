#ifndef _VCHECKBOX_H
#define _VCHECKBOX_H

#include "VObject.h"

/*! \b BCheckBox AddOn
*
* A MeTOS addon for the BCheckBox control.
* Written by \b m7m_GuyHaviv, with reference and help from \b VCedric
*/
class VCheckBox : public VObject
{
public:
	VCheckBox();
	
	status_t CreateObject(BMessage *archive); 				///< creates the GUI object (needed)
	status_t SetDefaultProperties(BMessage *properties);	///< set default Property Values;
	status_t SetProperties(BMessage *value,int32 index);	///<	modifies Properties
};

#endif

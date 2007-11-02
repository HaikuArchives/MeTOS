#ifndef _VRADIOBUTTON_H
#define _VRADIOBUTTON_H

#include "VObject.h"

/*! \b BRadioButton AddOn
*
* A MeTOS addon for the BRadioButton control.
* Written by \b m7m_GuyHaviv, with reference and help from \b VCedric
*/
class VRadioButton : public VObject
{
public:
	VRadioButton();
	
	status_t CreateObject(BMessage *archive);				///< creates the GUI object (needed)
	status_t SetDefaultProperties(BMessage *properties);	///< set default Property Values;
	status_t SetProperties(BMessage *value,int32 index);	///<	 modifies Properties
};

#endif

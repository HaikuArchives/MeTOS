#include "VObject.h"
#include "VRadioButton.h"

#include <String.h>
#include <RadioButton.h>
#include <Message.h>
#include <stdio.h>

extern "C" __declspec(dllexport) VObject *instantiate_VObject();


/*! \b Instances the VObject.
*
* note that metos actually calls \b \c CreateObject() to create the GUI object.
*/
VObject* instantiate_VObject()
{
	return (new VRadioButton());
}


/*! \b Constructor. Defines the basic properties of the Addon.
*
*	\b Note some fields use <b><tt> ReplaceType </b></tt> instead of <b><tt> AddType </b></tt>
* because they already exists in \c VObject
*/
VRadioButton::VRadioButton()
: VObject()
{
	
	//g: i do not understand those fields.. i did my best :)
	// todo: change RODIOBUTTON_TYPE to RADIOBUTTON_TYPE in VObject.h
	_properties->ReplaceInt32(VOBJECT_TYPE,RODIOBUTTON_TYPE);
	_properties->ReplaceString(VOBJECT_CLASS, "BRadioButton");
	
	// VObject parameters
	_settings->ReplaceBool(VOBJECT_BE_OBJECT,true);
	_settings->AddInt32(VOBJECT_DEPEND_ON,WINDOW_TYPE);
	_settings->AddInt32(VOBJECT_DEPEND_ON,VIEW_TYPE);
	_settings->AddInt32(VOBJECT_DEPEND_ON,BOX_TYPE);
	
	//! AddOn Info.
	_infos->AddString(VOBJECT_VERSION,"0.1");
	_infos->AddString(VOBJECT_AUTHOR,"m7m - Guy Haviv");
	_infos->AddString(VOBJECT_ABOUT, "wrapper for BRadioButton control.");
	_infos->AddString(VOBJECT_EMAIL,"mul_m7m@bezeqint.net");
		/**/ _infos->AddString(VOBJECT_WWW,"www.meretz.org.il");
	
	/*! <b> The <i> wrapped class'</i> constructor parameters. </b>
	*	these are actually strings that are placed as the object's \b Properties in metos
	*/
	_nameAsProperties->AddString("frame","_frame");
	_nameAsProperties->AddString("name","_name");
	_nameAsProperties->AddString("label","_label");
	_nameAsProperties->AddString("message","_msg");
	// todo: work on these:
	_nameAsProperties->AddString("resizeMask","_resize_mode");
	_nameAsProperties->AddString("flags","_flags");
}


/*! Creates the object (BRadioButton GUI)
* g: Assumes the message given contains all we need,
* creates the GUI object.
*/
status_t VRadioButton::CreateObject(BMessage *archive)
{
	_object = BRadioButton::Instantiate(archive);
	if(_object==NULL)
		return B_ERROR;
		
	return B_OK;
}


/*! Sets the default properties..
* these values are used in \c CreateObject() to \cInstantiate the object.
* in order to fill these up, one needs to take a look at the \b BeBook.
*/
status_t VRadioButton::SetDefaultProperties(BMessage *properties)
{
	BMessage emptyMessage((uint32)0x0);
	status_t state;
	
	// todo: possibly review more params from the BeBook.
	state = properties->AddRect("_frame",BRect(70,70,140,90));
	state &= properties->AddMessage("_msg",&emptyMessage);
	state &= properties->AddString("_label","RadioButton");
	state &= properties->AddInt32("_resize_mode", B_FOLLOW_LEFT | B_FOLLOW_TOP);
	state &= properties->AddInt32("_flags",B_WILL_DRAW | B_NAVIGABLE);
	
	return state;
}


/*! Sets a parameter
*
*/
status_t VRadioButton::SetProperties(BMessage *value,int32 index)
{
	if (value==NULL)
		return B_ERROR;
		
	if(LooperLock())
	{
		BString buffer;

		//name
		if (value->FindString("_name",&buffer)==B_OK)
			((BRadioButton*)_object)->SetName(buffer.String());
	
		//label
		if (value->FindString("_label",&buffer)==B_OK)
			((BRadioButton*)_object)->SetLabel(buffer.String());

		LooperUnlock();
		// todo: add more field sets (before the UnlockLooper()...) ?

		return B_OK;
	}
	else
		printf("Erreur : LooperLock() dans VRadioButton::SetProperties()\n");

	return B_ERROR;
}


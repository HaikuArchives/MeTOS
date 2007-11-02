#include "VObject.h"
#include "VButton.h"

#include <String.h>
#include <Button.h>
#include <Message.h>
#include <stdio.h>

extern "C" __declspec(dllexport) VObject *instantiate_VObject();

/**** Function to create the MeTOS Addon ****/
VObject* instantiate_VObject()
{
	// create the VObject and after that MeTOS call CreateObject() for really
	// create the GUI object (window,view etc...)
	return (new VButton());
}

/**** Constructor ****/
VButton::VButton()
: VObject()
{
	// define here the basic properties of the Addon
	// - if it's a Be Object (BView,BButton etc ...)
	//	 this value is false by default change only if you make an object from Be API
	// - name of Class (it's very usefull)
	// - infos
	// - 

	// Attention !! this properties exist use Replace instead Add
	_properties->ReplaceInt32(VOBJECT_TYPE,BUTTON_TYPE);
	_properties->ReplaceString(VOBJECT_CLASS,"BButton");

	// parameters of VObject
	_settings->ReplaceBool(VOBJECT_BE_OBJECT,true);
	_settings->AddInt32(VOBJECT_DEPEND_ON,WINDOW_TYPE);
	_settings->AddInt32(VOBJECT_DEPEND_ON,VIEW_TYPE);
	_settings->AddInt32(VOBJECT_DEPEND_ON,BOX_TYPE);

	// Add information about Addon
	_infos->AddString(VOBJECT_VERSION,"1.0");
	_infos->AddString(VOBJECT_AUTHOR,"CKJ - Vincent Cedric");
	_infos->AddString(VOBJECT_ABOUT,"VButton is the Be BButton Object\nUse for create a button.");
	_infos->AddString(VOBJECT_EMAIL,"ckj.beos@wanadoo.fr");
	_infos->AddString(VOBJECT_WWW,"http://perso.wanadoo.fr/cvincent/Home_page.html");
	
	// Add what is the name of constructor parameters for properties name
	_nameAsProperties->AddString("frame","_frame");
	_nameAsProperties->AddString("name","_name");
	_nameAsProperties->AddString("label","_label");
	_nameAsProperties->AddString("message","_msg");
	_nameAsProperties->AddString("resizeMask","_resize_mode");
	_nameAsProperties->AddString("flags","_flags");
}

/**** Create the Object (BButton GUI) ****/
status_t VButton::CreateObject(BMessage *archive)
{
	// we think the message given contain all we need
	// no problem we create GUI object
	_object = BButton::Instantiate(archive);
	if(_object==NULL)
		return B_ERROR;
	
	return B_OK;
}

/**** default value ****/
status_t VButton::SetDefaultProperties(BMessage *properties)
{
	BMessage	emptyMessage((uint32)0x0);
	status_t	state;
	
	// add default value
	// there values are needed by the Instantiate method of BButton
	// for know what you need refer to BeBook
	state = properties->AddRect("_frame",BRect(70,70,140,140));
	state &= properties->AddMessage("_msg",&emptyMessage);
	state &= properties->AddString("_label","Button");
	state &= properties->AddInt32("_resize_mode",B_FOLLOW_LEFT | B_FOLLOW_TOP);
	state &= properties->AddInt32("_flags",B_WILL_DRAW | B_NAVIGABLE);

	return state;
}

/**** change a parameter ****/
status_t VButton::SetProperties(BMessage *value,int32 index)
{
	// here add the code to take effect from a change into properties list of MeTOS
	if(value==NULL)
		return B_ERROR;
	
	if(LooperLock())
	{
		BString		buffer;

		// name of BButton
		if(value->FindString("_name",&buffer)==B_OK)
			((BButton *)_object)->SetName(buffer.String());

		// label of BButton
		if(value->FindString("_label",&buffer)==B_OK)
			((BButton *)_object)->SetLabel(buffer.String());

		LooperUnlock();
		
		return B_OK;
	}
	else
		printf("Erreur : LooperLock() dans VButton::SetProperties()\n");
	
	return B_ERROR;	
}
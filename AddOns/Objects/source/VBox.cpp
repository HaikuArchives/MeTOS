#include "VObject.h"
#include "VBox.h"

#include <String.h>
#include <Box.h>
#include <Message.h>
#include <stdio.h>

extern "C" __declspec(dllexport) VObject *instantiate_VObject();

/**** Function to create the MeTOS Addon ****/
VObject* instantiate_VObject()
{
	// create the VObject and after that MeTOS call CreateObject() for really
	// create the GUI object (window,view etc...)
	return (new VBox());
}

/**** Constructor ****/
VBox::VBox()
: VObject()
{
	// define here the basic properties of the Addon
	// - if it's a Be Object (BView,BButton etc ...)
	//	 this value is false by default change only if you make an object from Be API
	// - name of Class (it's very usefull)
	// - infos
	// - 

	// Attention !! this properties exist use Replace instead Add
	_properties->ReplaceInt32(VOBJECT_TYPE,BOX_TYPE);
	_properties->ReplaceString(VOBJECT_CLASS,"BBox");

	// parameters of VObject
	_settings->ReplaceBool(VOBJECT_BE_OBJECT,true);
	_settings->AddInt32(VOBJECT_DEPEND_ON,WINDOW_TYPE);
	_settings->AddInt32(VOBJECT_DEPEND_ON,VIEW_TYPE);

	// Add information about Addon
	_infos->AddString(VOBJECT_VERSION,"1.0");
	_infos->AddString(VOBJECT_AUTHOR,"CKJ - Vincent Cedric");
	_infos->AddString(VOBJECT_ABOUT,"VBox is the Be BBox Object\nUse for create a box.");
	_infos->AddString(VOBJECT_EMAIL,"ckj.beos@wanadoo.fr");
	_infos->AddString(VOBJECT_WWW,"http://perso.wanadoo.fr/cvincent/Home_page.html");
	
	// bloquer certaine proprietes
	_lockedProperties->AddInt32("_color",VOBJECT_PROPERTIES_HIDDEN);

	// Add what is the name of constructor parameters for properties name
	_nameAsProperties->AddString("bounds","_frame");
	_nameAsProperties->AddString("name","_name");
	_nameAsProperties->AddString("resizeFlags","_resize_mode");
	_nameAsProperties->AddString("flags","_flags");
	_nameAsProperties->AddString("border","_style");
}

/**** Create the Object (BBox GUI) ****/
status_t VBox::CreateObject(BMessage *archive)
{
	// we think the message given contain all we need
	// no problem we create GUI object
	_object = BBox::Instantiate(archive);
	if(_object==NULL)
		return B_ERROR;
	
	return B_OK;
}

/**** default value ****/
status_t VBox::SetDefaultProperties(BMessage *properties)
{
	status_t	state;
	
	// add default value
	// there values are needed by the Instantiate method of BButton
	// for know what you need refer to BeBook
	state = properties->AddRect("_frame",BRect(70,70,140,140));
	state &= properties->AddString("_label","Box");
	state &= properties->AddInt32("_style",B_FANCY_BORDER);
	state &= properties->AddInt32("_resize_mode",B_FOLLOW_LEFT | B_FOLLOW_TOP);
	state &= properties->AddInt32("_flags",B_WILL_DRAW | B_NAVIGABLE);
	state &= properties->AddString("_fname","Swis721 BT");
	state &= properties->AddString("_fname","Bold");
	state &= properties->AddFloat("_fflt",12.0000);
	state &= properties->AddFloat("_fflt",-1.0000);
	state &= properties->AddFloat("_fflt",-1.0000);
	state &= properties->AddInt32("_color",0xff);
	state &= properties->AddInt32("_color",0xd8d8d8ff);
	state &= properties->AddInt32("_color",0xd8d8d8ff); // c'est la couleur de fond

	return state;
}

/**** change a parameter ****/
status_t VBox::SetProperties(BMessage *value,int32 index)
{
	// here add the code to take effect from a change into properties list of MeTOS
	if(value==NULL)
		return B_ERROR;
	
	if(LooperLock())
	{
		BString		buffer;

		// name of BButton
		if(value->FindString("_name",&buffer)==B_OK)
			((BBox *)_object)->SetName(buffer.String());

		// label of BButton
		if(value->FindString("_label",&buffer)==B_OK)
			((BBox *)_object)->SetLabel(buffer.String());

		LooperUnlock();
	
		return B_OK;
	}
	else
		printf("Erreur : LooperLock() dans VBox::SetProperties()\n");
	
	return B_ERROR;	
}
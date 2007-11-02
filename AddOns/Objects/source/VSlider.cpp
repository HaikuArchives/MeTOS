#include "VObject.h"
#include "VSlider.h"

#include <String.h>
#include <Slider.h>
#include <Message.h>
#include <stdio.h>

extern "C" __declspec(dllexport) VObject *instantiate_VObject();

/**** Function to create the MeTOS Addon ****/
VObject* instantiate_VObject()
{
	// create the VObject and after that MeTOS call CreateObject() for really
	// create the GUI object (window,view etc...)
	return (new VSlider());
}

/**** Constructor ****/
VSlider::VSlider()
: VObject()
{
	// define here the basic properties of the Addon
	// - if it's a Be Object (BView,BButton etc ...)
	//	 this value is false by default change only if you make an object from Be API
	// - name of Class (it's very usefull)
	// - infos
	// - 

	// Attention !! this properties exist use Replace instead Add
	_properties->ReplaceInt32(VOBJECT_TYPE,SLIDER_TYPE);
	_properties->ReplaceString(VOBJECT_CLASS,"BSlider");

	// parameters of VObject
	_settings->ReplaceBool(VOBJECT_BE_OBJECT,true);
	_settings->AddInt32(VOBJECT_DEPEND_ON,WINDOW_TYPE);
	_settings->AddInt32(VOBJECT_DEPEND_ON,VIEW_TYPE);
	_settings->AddInt32(VOBJECT_DEPEND_ON,BOX_TYPE);

	// Add information about Addon
	_infos->AddString(VOBJECT_VERSION,"1.0");
	_infos->AddString(VOBJECT_AUTHOR,"CKJ - Vincent Cedric");
	_infos->AddString(VOBJECT_ABOUT,"VSlider is the Be BSlider Object\nUse for create a slider.");
	_infos->AddString(VOBJECT_EMAIL,"ckj.beos@wanadoo.fr");
	_infos->AddString(VOBJECT_WWW,"http://perso.wanadoo.fr/cvincent/Home_page.html");

	// bloquer certaine proprietes
	_lockedProperties->AddInt32("_color",VOBJECT_PROPERTIES_HIDDEN);
	_lockedProperties->AddInt32("_hashcount",VOBJECT_PROPERTIES_HIDDEN);
	_lockedProperties->AddInt32("_hashloc",VOBJECT_PROPERTIES_HIDDEN);
	_lockedProperties->AddInt32("_sstyle",VOBJECT_PROPERTIES_HIDDEN);
	_lockedProperties->AddInt32("_orient",VOBJECT_PROPERTIES_HIDDEN);
	_lockedProperties->AddInt32("_bthickness",VOBJECT_PROPERTIES_HIDDEN);
	
	// Add what is the name of constructor parameters for properties name
	_nameAsProperties->AddString("frame","_frame");
	_nameAsProperties->AddString("name","_name");
	_nameAsProperties->AddString("label","_label");
	_nameAsProperties->AddString("message","_msg");
	_nameAsProperties->AddString("minValue","_min");
	_nameAsProperties->AddString("maxValue","_max");
	_nameAsProperties->AddString("thumbType","_sstyle");
	_nameAsProperties->AddString("resizingMode","_resize_mode");
	_nameAsProperties->AddString("flags","_flags");
}

/**** Create the Object (BSlider GUI) ****/
status_t VSlider::CreateObject(BMessage *archive)
{
	// we think the message given contain all we need
	// no problem we create GUI object
	_object = BSlider::Instantiate(archive);
	if(_object==NULL)
		return B_ERROR;
	
	return B_OK;
}

/**** default value ****/
status_t VSlider::SetDefaultProperties(BMessage *properties)
{
	BMessage	emptyMessage((uint32)0x0);
	status_t	state;
	
	// add default value
	// there values are needed by the Instantiate method of BButton
	// for know what you need refer to BeBook
	state = properties->AddRect("_frame",BRect(70,70,140,140));
	state &= properties->AddMessage("_msg",&emptyMessage);
	state &= properties->AddInt32("_resize_mode",B_FOLLOW_LEFT | B_FOLLOW_TOP);
	state &= properties->AddInt32("_flags",B_FRAME_EVENTS | B_WILL_DRAW | B_NAVIGABLE);
	state &= properties->AddString("_fname","Swis721 BT");
	state &= properties->AddString("_fname","Roman");
	state &= properties->AddFloat("_fflt",12.0000);
	state &= properties->AddFloat("_fflt",-1.0000);
	state &= properties->AddFloat("_fflt",-1.0000);
	state &= properties->AddInt32("_color",0xff);
	state &= properties->AddInt32("_color",0xffffffff);
	state &= properties->AddInt32("_color",0xffffffff); // c'est la couleur de fond
	state &= properties->AddString("_label","Slider");
	state &= properties->AddInt32("_sdelay",20000);
	state &= properties->AddInt32("_bcolor",0x505050ff);
	state &= properties->AddInt32("_min",0);
	state &= properties->AddInt32("_max",100);
	state &= properties->AddInt32("_incrementvalue",1);
	state &= properties->AddInt32("_hashcount",0);
	state &= properties->AddInt16("_hashloc",0);
	state &= properties->AddInt16("_sstyle",0);
	state &= properties->AddInt32("_orient",0);
	state &= properties->AddFloat("_bthickness",-1.0000);

	return state;
}

/**** change a parameter ****/
status_t VSlider::SetProperties(BMessage *value,int32 index)
{
	// here add the code to take effect from a change into properties list of MeTOS
	if(value==NULL)
		return B_ERROR;
	
	if(LooperLock())
	{
		BString		buffer;
		orientation	orient;

		// name of BSlider
		if(value->FindString("_name",&buffer)==B_OK)
			((BSlider *)_object)->SetName(buffer.String());

		// label of BSlider
		if(value->FindString("_label",&buffer)==B_OK)
			((BSlider *)_object)->SetLabel(buffer.String());

		// orientation of BSlider
		if(value->FindInt32("_orient",(int32 *)&orient)==B_OK)
			((BSlider *)_object)->SetOrientation(orient);

		LooperUnlock();
	
		return B_OK;	
	}
	else
		printf("Erreur : LooperLock() dans VSlider::SetProperties()\n");

	return B_ERROR;
}
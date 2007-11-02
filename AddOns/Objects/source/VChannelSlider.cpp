#include "VObject.h"
#include "VChannelSlider.h"

#include <String.h>
#include <ChannelSlider.h>
#include <Message.h>
#include <stdio.h>

extern "C" __declspec(dllexport) VObject *instantiate_VObject();

/**** Function to create the MeTOS Addon ****/
VObject* instantiate_VObject()
{
	// create the VObject and after that MeTOS call CreateObject() for really
	// create the GUI object (window,view etc...)
	return (new VChannelSlider());
}

/**** Constructor ****/
VChannelSlider::VChannelSlider()
: VObject()
{
	// define here the basic properties of the Addon
	// - if it's a Be Object (BView,BChannelSlider etc ...)
	//	 this value is false by default change only if you make an object from Be API
	// - name of Class (it's very usefull)
	// - infos
	// - 

	// Attention !! this properties exist use Replace instead Add
	_properties->ReplaceInt32(VOBJECT_TYPE,SLIDER_TYPE);
	_properties->ReplaceString(VOBJECT_CLASS,"BChannelSlider");

	// parameters of VObject
	_settings->ReplaceBool(VOBJECT_BE_OBJECT,true);
	_settings->AddInt32(VOBJECT_DEPEND_ON,WINDOW_TYPE);
	_settings->AddInt32(VOBJECT_DEPEND_ON,VIEW_TYPE);
	_settings->AddInt32(VOBJECT_DEPEND_ON,BOX_TYPE);

	// Add information about Addon
	_infos->AddString(VOBJECT_VERSION,"1.0");
	_infos->AddString(VOBJECT_AUTHOR,"CKJ - Vincent Cedric");
	_infos->AddString(VOBJECT_ABOUT,"VChannelSlider is the Be BChannelSlider Object\nUse for create a channel slider.");
	_infos->AddString(VOBJECT_EMAIL,"ckj.beos@wanadoo.fr");
	_infos->AddString(VOBJECT_WWW,"http://perso.wanadoo.fr/cvincent/Home_page.html");

	// bloquer certaine proprietes
	_lockedProperties->AddInt32("_color",VOBJECT_PROPERTIES_HIDDEN);
	
	// Add what is the name of constructor parameters for properties name
	_nameAsProperties->AddString("area","_frame");
	_nameAsProperties->AddString("name","_name");
	_nameAsProperties->AddString("label","_label");
	_nameAsProperties->AddString("model","_msg");
	_nameAsProperties->AddString("o","_orient");
	_nameAsProperties->AddString("channels","be:_m_channel_count");
	_nameAsProperties->AddString("resize","_resize_mode");
	_nameAsProperties->AddString("flags","_flags");
}

/**** Create the Object (BChannelSlider GUI) ****/
status_t VChannelSlider::CreateObject(BMessage *archive)
{
	// we think properties is correct
	// but we can't use Instantiate, because the BColorControl
	// contain other views into the message, to skip this problem,
	// we create a new BChannelSlider with standard constructor.
	status_t		state = B_OK;
	BRect			frame;
	BString			name;
	BString			label;
	BMessage		message((uint32)0x0);
	orientation		orient;
	int32			channels;
	int32			resize;
	int32			flags;	

	// find properties we need
	state &= archive->FindRect("_frame",&frame);
	state &= archive->FindString("_name",&name);
	state &= archive->FindString("_label",&label);
	state &= archive->FindInt32("_resize_mode",&resize);
	state &= archive->FindInt32("_flags",&flags);
	state &= archive->FindMessage("_msg",&message);
    state &= archive->FindInt32("be:_m_channel_count",&channels);
    state &= archive->FindInt32("_orient",(int32 *)&orient);

	// create the object
	_object = new BChannelSlider(frame,name.String(),label.String(),new BMessage(message.what),orient,channels,resize,flags);
	if(_object==NULL)
		return B_ERROR;
	
	return B_OK;
}

/**** default value ****/
status_t VChannelSlider::SetDefaultProperties(BMessage *properties)
{
	BMessage	emptyMessage((uint32)0x0);
	status_t	state = B_OK;
	
	// add default value
	// there values are needed by the Instantiate method of BChannelSlider
	// for know what you need refer to BeBook
	state &= properties->AddRect("_frame",BRect(10,10,60,60));
	state &= properties->AddInt32("_resize_mode",B_FOLLOW_LEFT | B_FOLLOW_TOP);
	state &= properties->AddInt32("_flags",B_WILL_DRAW | B_NAVIGABLE);
	state &= properties->AddString("_fname","Swis721 BT");
	state &= properties->AddString("_fname","Roman");
	state &= properties->AddInt32("_color",0xff);
	state &= properties->AddInt32("_color",0xd8d8d8ff);
	state &= properties->AddInt32("_color",0xd8d8d8ff); // c'est la couleur de fond
	state &= properties->AddMessage("_msg",&emptyMessage);
	state &= properties->AddString("_label","label");
    state &= properties->AddInt32("be:_m_channel_count",1);
    state &= properties->AddInt32("be:_m_value_channel",0);
    state &= properties->AddInt32("be:_m_channel_min",0);
    state &= properties->AddInt32("be:_m_channel_max",100);
    state &= properties->AddInt32("be:_m_channel_val",0);
    state &= properties->AddInt32("_orient",B_VERTICAL);

	return state;
}

/**** change a parameter ****/
status_t VChannelSlider::SetProperties(BMessage *value,int32 index)
{
	// here add the code to take effect from a change into properties list of MeTOS
	if(value==NULL)
		return B_ERROR;
	
	if(LooperLock())
	{
		BString			buffer;
		orientation		orient;

		// name of BChannelSlider
		if(value->FindString("_name",&buffer)==B_OK)
			((BChannelSlider *)_object)->SetName(buffer.String());

		// l'orientation
		if(value->FindInt32("_orient",(int32 *)&orient)==B_OK)
			((BChannelSlider *)_object)->SetOrientation(orient);

		LooperUnlock();
	
		return B_OK;	
	}
	else
		printf("Erreur : LooperLock() dans VChannelSlider::SetProperties()\n");
	
	return B_ERROR;
}
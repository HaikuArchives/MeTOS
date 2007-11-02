#include "VObject.h"
#include "VColorControl.h"

#include <String.h>
#include <ColorControl.h>
#include <Message.h>
#include <stdio.h>

extern "C" __declspec(dllexport) VObject *instantiate_VObject();

/**** Function to create the MeTOS Addon ****/
VObject* instantiate_VObject()
{
	// create the VObject and after that MeTOS call CreateObject() for really
	// create the GUI object (window,view etc...)
	return (new VColorControl());
}

/**** Constructor ****/
VColorControl::VColorControl()
: VObject()
{
	// define here the basic properties of the Addon
	// - if it's a Be Object (BView,BColorControl etc ...)
	//	 this value is false by default change only if you make an object from Be API
	// - name of Class (it's very usefull)
	// - infos
	// - 

	// Attention !! this properties exist use Replace instead Add
	_properties->ReplaceInt32(VOBJECT_TYPE,CONTROL_TYPE);
	_properties->ReplaceString(VOBJECT_CLASS,"BColorControl");

	// parameters of VObject
	_settings->ReplaceBool(VOBJECT_BE_OBJECT,true);
	_settings->AddInt32(VOBJECT_DEPEND_ON,WINDOW_TYPE);
	_settings->AddInt32(VOBJECT_DEPEND_ON,VIEW_TYPE);
	_settings->AddInt32(VOBJECT_DEPEND_ON,BOX_TYPE);

	// Add information about Addon
	_infos->AddString(VOBJECT_VERSION,"1.0");
	_infos->AddString(VOBJECT_AUTHOR,"CKJ - Vincent Cedric");
	_infos->AddString(VOBJECT_ABOUT,"VColorControl is the Be BColorControl Object\nUse for create a color control.");
	_infos->AddString(VOBJECT_EMAIL,"ckj.beos@wanadoo.fr");
	_infos->AddString(VOBJECT_WWW,"http://perso.wanadoo.fr/cvincent/Home_page.html");

	// bloquer certaine proprietes
	_lockedProperties->AddInt32("_start",VOBJECT_PROPERTIES_HIDDEN);
	_lockedProperties->AddInt32("_label",VOBJECT_PROPERTIES_HIDDEN);
	_lockedProperties->AddInt32("_color",VOBJECT_PROPERTIES_HIDDEN);
	_lockedProperties->AddInt32("_use_off",VOBJECT_PROPERTIES_HIDDEN);
	
	// Add what is the name of constructor parameters for properties name
	_nameAsProperties->AddString("start","_start");
	_nameAsProperties->AddString("layout","_layout");
	_nameAsProperties->AddString("cell_size","_csize");
	_nameAsProperties->AddString("name","_name");
	_nameAsProperties->AddString("message","_msg");
	_nameAsProperties->AddString("use_offscreen","_use_off");
}

/**** Create the Object (BColorControl GUI) ****/
status_t VColorControl::CreateObject(BMessage *archive)
{
	// we think properties is correct
	// but we can't use Instantiate, because the BColorControl
	// contain other views into the message, to skip this problem,
	// we create a new BColorControl with standard constructor.
	status_t				state = B_OK;
	BRect					frame;
	BPoint					start;
	color_control_layout	layout;
	float					cell_size;
	BString					name;
	BMessage				message((uint32)0x0);
	bool					use_offscreen;

	// find properties we need
	state &= archive->FindRect("_frame",&frame);
	state &= archive->FindMessage("_msg",&message);
	state &= archive->FindString("_name",&name);
	state &= archive->FindInt32("_layout",(int32 *)&layout);
	state &= archive->FindFloat("_csize",&cell_size);
	state &= archive->FindBool("_use_off",&use_offscreen);
	if(state!=B_OK)
		return B_ERROR;

	// change the start point
	start = frame.LeftTop();
	_properties->ReplacePoint("_start",start);

	// create the object
	_object = new BColorControl(start,layout,cell_size,name.String(),new BMessage(message.what),use_offscreen);
	if(_object==NULL)
		return B_ERROR;
	
	return B_OK;
}

/**** default value ****/
status_t VColorControl::SetDefaultProperties(BMessage *properties)
{
	BMessage	emptyMessage((uint32)0x0);
	status_t	state = B_OK;
	
	// add default value
	// there values are needed by the Instantiate method of BColorControl
	// for know what you need refer to BeBook
	state &= properties->AddRect("_frame",BRect(10,10,340,78));
	state &= properties->AddPoint("_start",BPoint(10,10));
	state &= properties->AddInt32("_flags",B_WILL_DRAW | B_NAVIGABLE);
	state &= properties->AddString("_fname","Swis721 BT");
	state &= properties->AddString("_fname","Roman");
	state &= properties->AddInt32("_color",0xff);
	state &= properties->AddInt32("_color",0xffffffff);
	state &= properties->AddInt32("_color",0xffffffff); // c'est la couleur de fond
	state &= properties->AddMessage("_msg",&emptyMessage);
	state &= properties->AddString("_label","");
	state &= properties->AddInt32("_layout",B_CELLS_32x8);
	state &= properties->AddFloat("_csize",8.0000);
	state &= properties->AddBool("_use_off",false);

	return state;
}

/**** change a parameter ****/
status_t VColorControl::SetProperties(BMessage *value,int32 index)
{
	// here add the code to take effect from a change into properties list of MeTOS
	if(value==NULL)
		return B_ERROR;
	
	
	if(LooperLock())
	{
		BRect		frame;	
		BString		buffer;
		float		cellSize;

		// the BColorControl have a exeption !!!
		// it use a point to be create not a rect, but metos use rect !
		// tht's why you must change yourself the point
		if(value->FindRect("_frame",&frame)==B_OK)
			_properties->ReplacePoint("_start",frame.LeftTop());
	
		// name of BColorControl
		if(value->FindString("_name",&buffer)==B_OK)
			((BColorControl *)_object)->SetName(buffer.String());

		// Cell Size
		if(value->FindFloat("_csize",&cellSize)==B_OK)
			((BColorControl *)_object)->SetCellSize(cellSize);

		LooperUnlock();
	
		return B_OK;
	}
	else
		printf("Erreur : LooperLock() dans VColorControl::SetProperties()\n");
	
	return B_ERROR;	
}
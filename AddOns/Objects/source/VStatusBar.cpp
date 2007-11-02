#include "VObject.h"
#include "VStatusBar.h"

#include <String.h>
#include <StatusBar.h>
#include <Message.h>
#include <stdio.h>

extern "C" __declspec(dllexport) VObject *instantiate_VObject();

/**** Function to create the MeTOS Addon ****/
VObject* instantiate_VObject()
{
	// create the VObject and after that MeTOS call CreateObject() for really
	// create the GUI object (window,view etc...)
	return (new VStatusBar());
}

/**** Constructor ****/
VStatusBar::VStatusBar()
: VObject()
{
	// define here the basic properties of the Addon
	// - if it's a Be Object (BView,BButton etc ...)
	//	 this value is false by default change only if you make an object from Be API
	// - name of Class (it's very usefull)
	// - infos
	// - 

	// Attention !! this properties exist use Replace instead Add
	_properties->ReplaceInt32(VOBJECT_TYPE,STATUSBAR_TYPE);
	_properties->ReplaceString(VOBJECT_CLASS,"BStatusBar");

	// parameters of VObject
	_settings->ReplaceBool(VOBJECT_BE_OBJECT,true);
	_settings->AddInt32(VOBJECT_DEPEND_ON,WINDOW_TYPE);
	_settings->AddInt32(VOBJECT_DEPEND_ON,VIEW_TYPE);
	_settings->AddInt32(VOBJECT_DEPEND_ON,BOX_TYPE);

	// Add information about Addon
	_infos->AddString(VOBJECT_VERSION,"1.0");
	_infos->AddString(VOBJECT_AUTHOR,"CKJ - Vincent Cedric");
	_infos->AddString(VOBJECT_ABOUT,"VStatusBar is the Be BStatusBar Object\nUse for create a slider.");
	_infos->AddString(VOBJECT_EMAIL,"ckj.beos@wanadoo.fr");
	_infos->AddString(VOBJECT_WWW,"http://perso.wanadoo.fr/cvincent/Home_page.html");
	
	// bloquer certaine proprietes
	_lockedProperties->AddInt32("_color",VOBJECT_PROPERTIES_HIDDEN);
	_lockedProperties->AddInt32("_val",VOBJECT_PROPERTIES_HIDDEN);

	// Add what is the name of constructor parameters for properties name
	_nameAsProperties->AddString("frame","_frame");
	_nameAsProperties->AddString("name","_name");
	_nameAsProperties->AddString("label","_label");
	_nameAsProperties->AddString("trailing_label","_tlabel");
}

/**** Create the Object (BStatusBar GUI) ****/
status_t VStatusBar::CreateObject(BMessage *archive)
{
	// we think the message given contain all we need
	// no problem we create GUI object
	_object = BStatusBar::Instantiate(archive);
	if(_object==NULL)
		return B_ERROR;
	
	return B_OK;
}

/**** default value ****/
status_t VStatusBar::SetDefaultProperties(BMessage *properties)
{
	status_t	state;
	
	// add default value
	// there values are needed by the Instantiate method of BButton
	// for know what you need refer to BeBook
	state = properties->AddRect("_frame",BRect(70,70,140,140));
	state &= properties->AddInt32("_resize_mode",B_FOLLOW_LEFT | B_FOLLOW_TOP);
	state &= properties->AddInt32("_flags",B_WILL_DRAW | B_NAVIGABLE);
	state &= properties->AddString("_fname","Swis721 BT");
	state &= properties->AddString("_fname","Roman");
	state &= properties->AddFloat("_fflt",12.0000);
	state &= properties->AddFloat("_fflt",-1.0000);
	state &= properties->AddFloat("_fflt",-1.0000);
	state &= properties->AddInt32("_color",0xff);
	state &= properties->AddInt32("_color",0xffffffff);
	state &= properties->AddInt32("_color",0xffffffff); // c'est la couleur de fond
	state &= properties->AddFloat("_high",17.9873);
	state &= properties->AddInt32("_bcolor",0x3296ffec);
	state &= properties->AddFloat("_val",50.0000);
	state &= properties->AddFloat("_max",100.0000);
	state &= properties->AddString("_label","Label");
	state &= properties->AddString("_tlabel","Trailing label");
	state &= properties->AddString("_text","Text");
	state &= properties->AddString("_ttext","Trailing text");

	return state;
}
#include <stdio.h>
/**** change a parameter ****/
status_t VStatusBar::SetProperties(BMessage *value,int32 index)
{
	// here add the code to take effect from a change into properties list of MeTOS
	if(value==NULL)
		return B_ERROR;
	
	if(LooperLock())
	{
		BString		buffer;
		uint32		color;

		// name of BStatusBar
		if(value->FindString("_name",&buffer)==B_OK)
			((BStatusBar *)_object)->SetName(buffer.String());

		// text of BStatusBar
		if(value->FindString("_text",&buffer)==B_OK)
			((BStatusBar *)_object)->Update(0,buffer.String(),NULL);

		// trailing text of BStatusBar
		if(value->FindString("_ttext",&buffer)==B_OK)
			((BStatusBar *)_object)->Update(0,NULL,buffer.String());

		// Color of progress Bar
		if(value->FindInt32("_bcolor",(int32 *)&color)==B_OK)
		{
			rgb_color	barColor;
		
			// get rgb_color from uint32
	 		barColor.red = ((uint8 *)(&color))[3];
	 		barColor.green = ((uint8 *)(&color))[2];
	 		barColor.blue = ((uint8 *)(&color))[1];
	 		barColor.alpha = ((uint8 *)(&color))[0];

			// update
			((BStatusBar *)_object)->SetBarColor(barColor);
		}
	
		LooperUnlock();
	
		return B_OK;	
	}
	else
		printf("Erreur : LooperLock() dans VStatusBar::SetProperties()\n");

	return B_ERROR;
}
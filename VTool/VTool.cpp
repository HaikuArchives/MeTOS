/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/

/********************/
/* MeTOS tool class */
/********************/

#include "VTool.h"
#include "VPreferenceFile.h"
#include "MetosGlobalDefs.h"
#include "MetosMessageDefs.h"

#include <storage/Entry.h>
#include <interface/Bitmap.h>
#include <storage/AppFileInfo.h>

/**** constructeur ****/
VTool::VTool(const char *name,image_id addonId,BEntry *entry)
: BButton(BRect(0,0,35,35),name,"",NULL)
{
	BAppFileInfo	fileinfo;
	BFile			file(entry, B_READ_ONLY);

	// recuperer l'image de l'addon pour faire appel
	// ulterieurement au fonction externe de chaque AddOns
	_addonId = addonId;
	_entered = false;
	
	// pour les preferences
	RefreshPreferences();
	
	// recuperer le fichier
	fileinfo.SetTo(&file);	

	// charger l'icon
	_icon = new BBitmap(BRect(0,0,31,31 ), B_CMAP8);
	if(fileinfo.GetIcon(_icon, B_LARGE_ICON) != B_OK)
		fileinfo.GetTrackerIcon(_icon, B_LARGE_ICON);
}

/**** destructeur ****/
VTool::~VTool()
{
	// se detacher
	RemoveSelf();
	
	// puis liberer la memoire
	delete _icon;
}

/**** recuperer le nom de l'outil ****/
const char *VTool::Name()
{
	const char *name = NULL;

	// bloquer le looper
	// ne pas oublier de demander la fonction parente explicitement
	// sinon ca donne du recursif !
	if(LockLooper())
	{
		name = BButton::Name();
		UnlockLooper();
	}
	else
		printf("Erreur : LockLooper() dans VTool::Name()\n");
	
	return name;
}


/**** dessiner le bouton ****/
void VTool::Draw(BRect updateRect)
{
	BRect bounds = Bounds();
	
	// recuperer la couleur de l'interface
	rgb_color	menuColor = ui_color(B_MENU_BACKGROUND_COLOR);
	
	// bouton selectionne
	if(!Value())
	{
		SetHighColor(255,255,255);	
		StrokeLine(BPoint(bounds.left,bounds.top),BPoint(bounds.right,bounds.top));
		StrokeLine(BPoint(bounds.left,bounds.top),BPoint(bounds.left,bounds.bottom));
		SetHighColor(172,172,172);	
		StrokeLine(BPoint(bounds.left,bounds.bottom),BPoint(bounds.right,bounds.bottom));
		StrokeLine(BPoint(bounds.right,bounds.top),BPoint(bounds.right,bounds.bottom));
	}
	else
	{
		SetHighColor(172,172,172);	
		StrokeLine(BPoint(bounds.left,bounds.top),BPoint(bounds.right,bounds.top));
		StrokeLine(BPoint(bounds.left,bounds.top),BPoint(bounds.left,bounds.bottom));
		SetHighColor(255,255,255);	
		StrokeLine(BPoint(bounds.left,bounds.bottom),BPoint(bounds.right,bounds.bottom));
		StrokeLine(BPoint(bounds.right,bounds.top),BPoint(bounds.right,bounds.bottom));
	}

	// le fond gris
	bounds.InsetBy(1,1);
	SetHighColor(menuColor);
	if(_entered)
		SetHighColor(_buttonColor);	
	FillRect(bounds);

	if(Value())
		bounds.OffsetBy(1,1);	

	// appel de la fonction custom
	DrawTool(bounds);
}

/**** dessin standard du bouton ****/
void VTool::DrawTool(BRect bounds)
{
	SetDrawingMode(B_OP_ALPHA);
	DrawBitmap(_icon,BPoint(bounds.left+1,bounds.top+1));
	SetDrawingMode(B_OP_COPY);
}

/**** gestion des messages ****/
void VTool::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
	// rafraichir les preferences
	case M_PREFERENCE_MSG:
		RefreshPreferences();
		break;
	default:
		BButton::MessageReceived(message);
	}
}

/**** clique sur le bouton ****/
void VTool::MouseDown(BPoint point)
{
	BPoint		where;
	uint32		button;

	GetMouse(&where,&button);

	// convertir le point
	if(LockLooper())
	{
		ConvertToScreen(&point);
		UnlockLooper();
	}
	else
		printf("Erreur : LockLooper() dans VTool::MouseDown()\n");
	
	// valeur a un
	switch(button)
	{
	case B_PRIMARY_MOUSE_BUTTON:
		SetValue(1);
		DoAction(&point);
		break;
	// selection de l'outils
	case B_SECONDARY_MOUSE_BUTTON:
		SelectTools(&point);
		break;
	default:
		;//rien
	}
}

/**** clique droit selection (peux servir pour autre chose) ****/
void VTool::SelectTools(BPoint *where)
{ }

/**** on relache le clique sur le bouton ****/
void VTool::MouseUp(BPoint point)
{
	// action du bouton
	SetValue(0);
}

/**** deplacement de la souris ****/
void VTool::MouseMoved(BPoint point, uint32 transit,const BMessage *message)
{
	switch(transit)
	{
	case B_ENTERED_VIEW:
		_entered = true;
		Invalidate();
		break;
	case B_EXITED_VIEW:
		_entered = false;
		SetValue(0);
		Invalidate();
		break;
	case B_INSIDE_VIEW:
		if(!_entered)
			Invalidate();
	}
}

/**** rafraichir les preferences ****/
void VTool::RefreshPreferences()
{
	rgb_color tempColor = {200,200,255,255};
	_buttonColor = g_ColorsPrefs->GetRGBColor(tempColor,"list-color");
}

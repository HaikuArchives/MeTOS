/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
/*****************************************/
/* Item de la liste des Controles(Addon) */
/*****************************************/
#include "ObjectManagerItem.h"
#include "MetosGlobalDefs.h"
#include "VObject.h"
#include "AddOnManagerWin.h"
#include "ObjectManagerWin.h"
#include "CColumnView.h"
#include "VPreferenceFile.h"
#include "MetosApp.h"
#include <interface/View.h>
#include <interface/Bitmap.h>
#include <app/Message.h>
#include <storage/File.h>
#include <storage/AppFileInfo.h>
#include <storage/Path.h>

/**** Constructeur ****/
ObjectManagerItem::ObjectManagerItem(VObject *object)
: BListItem()
{
	BAppFileInfo	fileinfo;
	BFile			file;
	BString			className;
	BString			addonSource;

	// pointer sur le VObject
	_object = object;
	
	// recuperer le nom de la classe
	(object->Properties())->FindString(VOBJECT_CLASS,&className);
	(object->Settings())->FindString(VOBJECT_ADDON_SOURCE,&addonSource);

	// recuperer le chemin du fichier addon
	addonSource.Prepend("/");
	addonSource.Prepend(ADDON_OBJECTS_FOLDER);
	addonSource.Prepend("/");
	if(g_MetosApp->Lock())
	{
		addonSource.Prepend((g_MetosApp->GetAddonsPath()).Path());
		g_MetosApp->Unlock();
	}
	else
		g_MetosApp->SendDebbug("Error : Lock() in ObjectManagerItem::ObjectManagerItem()",VDEBUG_ERROR);

	// recuperer le fichier
	file.SetTo(addonSource.String(),B_READ_ONLY);
	fileinfo.SetTo(&file);	

	// charger l'icon
	_miniIcon = new BBitmap(BRect(0,0,15,15),B_CMAP8);
	if(fileinfo.GetIcon(_miniIcon, B_MINI_ICON)!=B_OK)
		fileinfo.GetTrackerIcon(_miniIcon,B_MINI_ICON);
	
	if(g_MetosApp->Lock())
	{
		_columnView = g_MetosApp->_objectManagerWin->ColumnView();
		_lockIcon = g_MetosApp->GetBitmap("Lock");
		_checkIcon = g_MetosApp->GetBitmap("CheckFunction");
		g_MetosApp->Unlock();
	}
	else
		g_MetosApp->SendDebbug("Error : Lock() in ObjectManagerItem::ObjectManagerItem()",VDEBUG_ERROR);

	// preferences
	RefreshPreferences();
}

/**** Destructeur ****/
ObjectManagerItem::~ObjectManagerItem()
{
	// l'objet n'est pas effacé ici car il peux se trouverdans la
	// garbage liste
	// l'icon
	delete _lockIcon;	
	delete _miniIcon;
	delete _checkIcon;
}

/**** Dessin des Items ****/
void ObjectManagerItem::DrawItem(BView *owner,BRect bounds,bool complete)
{
	BMessage	*properties = NULL;
	BRect		*position = NULL;
	float		levelOffset;
	bool		derived;

	// dessiner le cadre
	owner->SetHighColor(VIEW_COLOR);
	owner->StrokeLine(BPoint(0,bounds.bottom),BPoint(bounds.right,bounds.bottom));

	// couleur de fond
	if(_object->IsAttached())
	{
		if(IsSelected())
			owner->SetHighColor(_itemColor);
		else
			owner->SetHighColor(WHITE_COLOR);
	}
	else
		owner->SetHighColor(LIGHT_1_COLOR);
	
	bounds.bottom--;	
	owner->SetLowColor(owner->HighColor());
	owner->FillRect(bounds);

	// decaler par rapport au level
	position = _columnView->RectFrom(0);
	levelOffset = position->left + _object->Level() *10;

	// afficher l'icone
	if(position->left+levelOffset+16<position->right)
	{
		owner->SetDrawingMode(B_OP_ALPHA);
		owner->DrawBitmap(_miniIcon,BPoint(position->left+levelOffset,bounds.top+2));
		owner->SetDrawingMode(B_OP_COPY);
	}
	
	// afficher le nom de l'objet
	levelOffset += 25;
	owner->SetHighColor(BLACK_COLOR);
	properties = _object->Properties();
	properties->FindString(VOBJECT_NAME,&_truncBuffer);
	owner->TruncateString(&_truncBuffer,B_TRUNCATE_END,position->Width()-levelOffset);
	owner->DrawString(_truncBuffer.String(),BPoint(position->left+levelOffset,bounds.top+14));

	// l'etat bloqué ou pas
	position = _columnView->RectFrom(1);
	owner->SetHighColor(VIEW_COLOR);
	owner->StrokeLine(BPoint(position->left,bounds.bottom),BPoint(position->left,bounds.top));
	if(_object->IsLock())
	{
		owner->SetDrawingMode(B_OP_ALPHA);
		owner->DrawBitmap(_lockIcon,BPoint(position->left+(position->Width()-12)/2,bounds.top+2));
		owner->SetDrawingMode(B_OP_COPY);
	}
	
	// objet derivé ou pas
	properties->FindBool(VOBJECT_DERIVED_CLASS,&derived);
	position = _columnView->RectFrom(2);	
	owner->StrokeLine(BPoint(position->left,bounds.bottom),BPoint(position->left,bounds.top));	
	owner->SetHighColor(BLACK_COLOR);
	if(derived)
	{
		owner->SetDrawingMode(B_OP_ALPHA);
		owner->DrawBitmap(_checkIcon,BPoint(position->left+(position->Width()-12)/2,bounds.top+2));
		owner->SetDrawingMode(B_OP_COPY);
	}
	
	// afficher l'ID de l'objet
	position = _columnView->RectFrom(3);
	_truncBuffer = " Id : ";
	_truncBuffer << (uint32)(_object->Id());
	owner->SetHighColor(VIEW_COLOR);
	owner->StrokeLine(BPoint(position->left,bounds.bottom),BPoint(position->left,bounds.top));
	owner->StrokeLine(BPoint(position->right+1,bounds.bottom),BPoint(position->right+1,bounds.top));
	owner->SetHighColor(BLACK_COLOR);
	owner->TruncateString(&_truncBuffer,B_TRUNCATE_END,position->Width()-10);
	owner->DrawString(_truncBuffer.String(),BPoint(position->left+10,bounds.top+14));
}

/**** Changer la taille ****/
void ObjectManagerItem::Update(BView *owner, const BFont *font)
{
	SetHeight(20);
}

/**** rafraichir les preferences ****/
void ObjectManagerItem::RefreshPreferences()
{
	rgb_color tempColor = {200,200,255,255};
	_itemColor = g_ColorsPrefs->GetRGBColor(tempColor,"list-color");
}

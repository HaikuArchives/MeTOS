/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
/*****************************/
/* MeTOS PreferencItem class */
/*****************************/
#include "PreferenceItem.h"
#include <interface/Bitmap.h>

/**** constructeur ****/
PreferenceItem::PreferenceItem(const char *name,BBitmap *icon,preference_group group)
: BListItem()
{
	// initialiser
	_name = name;
	_group = group;
	_icon = icon;
}

/**** destructeur ****/
PreferenceItem::~PreferenceItem()
{
}

/**** dessin de l'item ****/
void PreferenceItem::DrawItem(BView *owner,BRect bounds,bool complete)
{
	owner->SetHighColor(255,255,255);
	owner->SetLowColor(owner->HighColor());
	owner->FillRect(bounds);

	// dessiner le titre
	owner->SetHighColor(0,0,0);
	owner->DrawString(_name.String(),BPoint(bounds.left+5,bounds.top+10));
}

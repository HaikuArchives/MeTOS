/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _PREFERENCEITEM_H
#define _PREFERENCEITEM_H
/*****************************/
/* MeTOS PreferencItem class */
/*****************************/
#include "VPreference.h"
#include <interface/ListItem.h>
#include <support/String.h>

class BBitmap;

class PreferenceItem : public BListItem
{
public:
	PreferenceItem(const char *name,BBitmap *icon,preference_group group);
	virtual ~PreferenceItem();

	inline	preference_group	Group()		{ return _group; }		// recuperer le groupe de l'item
	
protected:
	BString				_name;		// nom
	preference_group	_group;		// groupe
	BBitmap				*_icon;		// icon

			void	DrawItem(BView *owner,BRect bounds,bool complete);	// dessin
};

#endif

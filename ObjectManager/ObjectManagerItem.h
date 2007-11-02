/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _OBJECTMANAGERSITEM_H
#define _OBJECTMANAGERSITEM_H
/********************/
/* MeTOS view class */
/********************/
#include <interface/ListItem.h>
#include <support/String.h>
#include <interface/InterfaceDefs.h>

class VObject;
class BBitmap;
class CColumnView;
class BView;

class ObjectManagerItem : public BListItem
{
public:
	ObjectManagerItem(VObject *object);
	virtual ~ObjectManagerItem();

	virtual void	DrawItem(BView *owner,BRect bounds,bool complete = false);
	virtual void	Update(BView *owner, const BFont *font);
			void	RefreshPreferences();

	inline	VObject	*Object()				{ return _object; }			// pointer sur le VObject

protected:
	CColumnView	*_columnView;
	VObject		*_object;
	BBitmap		*_miniIcon;
	BBitmap		*_lockIcon;
	BBitmap		*_checkIcon;
	BString		_truncBuffer;
	rgb_color	_itemColor;
};

#endif

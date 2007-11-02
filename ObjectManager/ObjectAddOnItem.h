/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _OBJECTADDONITEM_H
#define _OBJECTADDONITEM_H
/**********************************/
/* MeTOS Addon Manager item class */
/**********************************/
#include "VObject.h"
#include <interface/ListItem.h>
#include <support/String.h>
#include <interface/InterfaceDefs.h>
#include <storage/Path.h>

class BEntry;
class BBitmap;
class BMessage;

class ObjectAddOnItem
{
public:
	ObjectAddOnItem(image_id addonId,VObject *object,BEntry *entry);
	virtual ~ObjectAddOnItem();

			void		CreateObject();
			void		DeleteObject();
			status_t	Archive(BMessage *data);						// archivage dans un message des proprietes
			
	inline	image_id	AddonId()			{ return _addonId; }		// retourne l'ID de l'addon
	inline	int32		Type()				{ return _type; }			// trouver le type de l'item
	inline	BString		ClassName()			{ return _classeName; }
	inline	BString		AddonName()			{ return _name; }
	inline	bool		NoMore()			{ return _noMore; }
	inline	int32		NbCreatedObject()	{ return _nbObject; }
	inline	BPath		*Path()				{ return &_path; }
	inline	VObject		*Object()			{ return _object; }

protected:
	// version addon
	BString			_classeName;
	VObject			*_object;
	bool			_onlyOne;
	BBitmap			*_miniIcon;
	image_id		_addonId;
	BString			_name;
	int32			_nbObject;
	bool			_noMore;
	BPath			_path;
	object_type		_type;			// type de l'item (ou de l'objet)
};

#endif

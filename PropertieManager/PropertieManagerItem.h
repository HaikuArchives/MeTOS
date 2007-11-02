/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _PROPERTIEMANAGERITEM_H
#define _PROPERTIEMANAGERITEM_H
/*************************************/
/* MeTOS PropertieManagerItem class */
/*************************************/
#include <kernel/image.h>
#include <app/Message.h>

class VPropertie;
class BEntry;

class PropertieManagerItem
{
public:
	PropertieManagerItem(image_id addonId,BEntry *entry);
	virtual ~PropertieManagerItem();

	inline	image_id	AddonId()			{ return _addonId; }		// retourne l'ID de l'addon
	inline	BEntry		*Entry()			{ return _entry; }			// le fichier source (le plug-in)

protected:
	image_id		_addonId;		// id de l'addon (image_id)
	BEntry			*_entry;		// fichier
};

#endif

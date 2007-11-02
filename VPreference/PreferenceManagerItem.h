/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _PREFERENCEMANAGERSITEM_H
#define _PREFERENCEMANAGERSITEM_H
/*************************************/
/* MeTOS PreferenceManagerItem class */
/*************************************/
#include <kernel/image.h>

class BEntry;

class PreferenceManagerItem
{
public:
	PreferenceManagerItem(image_id addonId,BEntry *entry);
	virtual ~PreferenceManagerItem();

	inline	image_id	AddonId()			{ return _addonId; }	// retourne l'ID de l'addon
	inline	BEntry		*Entry()			{ return _entry; }		// le fichier source (le plug-in)

protected:
	image_id		_addonId;		// id de l'addon (image_id)
	BEntry			*_entry;		// fichier
};

#endif


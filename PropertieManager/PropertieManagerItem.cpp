/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
/*************************************/
/* MeTOS PropertieManagerItem class */
/*************************************/
// Cette classe va permettre de decharger
// correctement les addons en fin de programme
#include "PropertieManagerItem.h"
#include <storage/Entry.h>

/**** constructeur ****/
PropertieManagerItem::PropertieManagerItem(image_id addonId,BEntry *entry)
{
	_addonId = addonId;				// id de l'addon (image_id)
	_entry = new BEntry(*entry);	// fichier
}

/**** destructeur ****/
PropertieManagerItem::~PropertieManagerItem()
{
	// liberer la memoire
	delete _entry;
}

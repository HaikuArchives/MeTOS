/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
/*******************************/
/* class PreferenceManagerItem */
/*******************************/
#include "PreferenceManagerItem.h"

/**** constructor ****/
PreferenceManagerItem::PreferenceManagerItem(image_id addonId,BEntry *entry)
{
	// recuperer les données
	_addonId = addonId;
	_entry = entry;
}

/**** destructor ****/
PreferenceManagerItem::~PreferenceManagerItem()
{
}

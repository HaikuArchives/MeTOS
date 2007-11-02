/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
/******************/
/* AddOns interne */
/******************/
#include "ObjectAddOnItem.h"
#include "MetosMessageDefs.h"
#include "MetosGlobalDefs.h"
#include "MetosApp.h"
#include "VObject.h"
#include <interface/Bitmap.h>
#include <interface/View.h>
#include <app/Message.h>
#include <storage/Path.h>

/**** Constructeur version addon ****/
ObjectAddOnItem::ObjectAddOnItem(image_id addonId,VObject *object,BEntry *entry)
{
	// le chemin
	_object = object;
	_path.SetTo(entry);
	
	// recuperer l'image de l'addon pour faire appel
	// ulterieurement au fonction externe de chaque AddOns
	(_object->Properties())->FindString(VOBJECT_CLASS,&_classeName);
	(_object->Settings())->FindBool(VOBJECT_ONLY_ONE,&_onlyOne);
	_name = _path.Leaf();
	_addonId = addonId;
	_type = object->Type();
	_nbObject = 0;
	_noMore = false;

	// tres important !!
	// on modifie l'addon source directement dans les settings de l'objet
	(_object->Settings())->ReplaceString(VOBJECT_ADDON_SOURCE,_name);
}

/**** Destructeur ****/
ObjectAddOnItem::~ObjectAddOnItem()
{
	// liberer la memoire
	delete _object;
}

/**** ajouter un objet ****/
void ObjectAddOnItem::CreateObject()
{
	BMessage	lockAddon(M_CREATE_OBJECT_MSG);

	// calculer pour voir si on peux encore en creer d'autre
	_nbObject++;
	_noMore = (_onlyOne && _nbObject>0);
	
	// ajouter la valeur de nomore
	// et le nom pour savoir de quel addon ca vient
	lockAddon.AddBool(M_ADDON_LOCK,_noMore);
	lockAddon.AddString(VOBJECT_ADDON_SOURCE,_name);
	g_AddOnManager.SendMessage(&lockAddon);
}

/**** effacer un objet ****/
void ObjectAddOnItem::DeleteObject()
{
	BMessage	lockAddon(M_DELETE_OBJECT_MSG);

	// calculer pour voir si on peux encore en creer d'autre
	_nbObject--;
	_noMore = (_onlyOne && !(_nbObject==0));

	// ajouter la valeur de nomore
	// et le nom pour savoir de quel addon ca vient
	lockAddon.AddBool(M_ADDON_LOCK,_noMore);
	lockAddon.AddString(VOBJECT_ADDON_SOURCE,_name);
	g_AddOnManager.SendMessage(&lockAddon);
}

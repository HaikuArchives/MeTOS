/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _ADDONMANAGERSITEM_H
#define _ADDONMANAGERSITEM_H
/**********************************/
/* MeTOS Addon Manager item class */
/**********************************/
#include "VObject.h"
#include <interface/ListItem.h>
#include <support/String.h>
#include <interface/InterfaceDefs.h>
#include <app/Message.h>

class BBitmap;

class AddOnManagerItem : public BListItem
{
public:
	AddOnManagerItem(entry_ref *entry,BMessage *properties,BMessage *settings);	// version addon
	AddOnManagerItem(object_type type);											// version superitem
	virtual ~AddOnManagerItem();

	virtual void	DrawItem(BView *owner,BRect bounds,bool complete = false);
	virtual void	Update(BView *owner, const BFont *font);

	inline	BMessage	*Settings()					{ return &_settings; }		// settings de l'objet (c'est pour les dependances)
	inline	bool		IsASuperItem()				{ return _isASuperItem; }	// trouver si c'est un superitem
	inline	bool		IsLock()					{ return _lock; }			// l'item est-il bloqué (pas de creation possible)
	inline	bool		IsAvailable()				{ return _available; }		// l'addon est ou pas disponible
	inline	int32		Type()						{ return _type; }			// type gerer par l'addonitem
	inline	BBitmap		*Icon()						{ return _icon; }			// image de l'addon (icon)
	inline	BString		AddonName()					{ return _name; }			// nom de l'addon (du fichier)
	inline	BString		ClassName()					{ return _className; }		// nom de la classe gerer
	inline	void		SetAvailable(bool value)	{ _available = value; }		// definir si l'addon est ou pas disponible
			void		Lock(bool value);										// bloquer ou pas la creation de nouvel objet
			bool		CheckDepencies(object_type type);						// veifier si on peux creer cet object
			void		RefreshPreferences();									// rafraichir les preferences

protected:
	// version addon
	BMessage		_properties;
	BMessage		_settings;
	BString			_className;
	BString			_name;
	BString			_displayClassName;	// pour l'affichage
	BString			_displayType;		// pour l'affichage
	bool			_lock;				// bloque contre d'autre creation
	bool			_available;			// rend disponible (par exemple si c'est le bon type)
	BBitmap			*_icon;				// icon de l'objet
	BBitmap			*_lockIcon;			// icon du lock
	object_type		_type;				// type de l'item (ou de l'objet)
	bool			_isASuperItem;		// c'est un super item
	//autre
	rgb_color		_itemColor;					// couleur de selection
	rgb_color		_unAvailableColor;			// couleur de l'item si il est indisponible
	rgb_color		_unAvailableTextColor;		// couleur du text si l'item est indisponible
	
			BString		TypeName(int32 type);
};

#endif

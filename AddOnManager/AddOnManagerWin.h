/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _ADDONMANAGERWINDOW_H
#define _ADDONMANAGERWINDOW_H
/************************************/
/* fenetre de de gestion des AddOns */
/************************************/
#include "VObject.h"
#include <interface/Window.h>
#include <support/List.h>
#include <storage/Mime.h>

// message
#define		M_SELECTADDON_MSG	'Mase'

class AddOnManagerListView;
class AddOnManagerItem;
class CStatusView;
class BView;
class BEntry;
class BMessenger;
class BMessage;

class AddOnManagerWin : public BWindow
{
// attributs
protected:

// fonctions
public:
	AddOnManagerWin(BRect);
	virtual ~AddOnManagerWin();

	virtual void		MessageReceived(BMessage *message);
	virtual bool		QuitRequested();

	inline	uint32				Object()	{ return _objectID; }		// ID de l'objet courant
	inline	int32				Type()		{ return _type; }			// type de l'objet courant
			void				AddAddonObject(BMessage *data);			// ajouter et placer dans la liste un addon
			AddOnManagerItem	*SuperItemForType(object_type type);	// trouver le super item pour un type
			
private:
	AddOnManagerListView	*_addOnList;
	BView					*_addOnManagerView;
	CStatusView				*_statusView;
	uint32					_objectID;
	object_type				_type;
	int32					_nbSuperItem;
	int32					_nbAddons;
	
			AddOnManagerItem	*FindItemFromName(const char *name);	// trouver un addon item a partir de son nom
			void				LockAddon(BMessage *message);			// bloquer ou pas la creation de nouvel addon
			void				ObjectSelected(BMessage *message);		// selection d'un objet
			void				RefreshDisplayNumberItems();			// affichage de la status-view
			void				RefreshPreferences();					// rafraichir les preferences
			void				SavePreferences();						// sauver les preferences
};

#endif

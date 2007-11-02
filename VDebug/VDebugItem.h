/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _VDEBUGITEM_H
#define _VDEBUGITEM_H
/*********************/
/* Pour le debbugage */
/*********************/
#include <interface/ListItem.h>
#include <support/String.h>
#include <interface/InterfaceDefs.h>

class BBitmap;

class VDebugItem : public BListItem
{
public:
	VDebugItem(const char *message,int32 type);
	virtual	~VDebugItem();

			void		RefreshPreferences();	// gestion des preferences										// rafraichir les preferences

protected:
	char			*_message;			// ben le message
	int32			_type;				// type du message
	BBitmap			*_infoBitmap;		// image pour le type de message
	rgb_color		_itemColor;			// couleur de selection
	
			void	DrawItem(BView *owner,BRect bounds,bool complete);	// dessin
};

#endif

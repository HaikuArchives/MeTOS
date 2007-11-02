#ifndef _ALIGNOBJECTS_H
#define _ALIGNOBJECTS_H
/*********************/
/* MeTOS tools class */
/*********************/
#include "VTool.h"
#include <app/Messenger.h>
#include <storage/Resources.h>

// messages
#define			ALIGNOBECTS_ORIENTATION		'Aoor'

class BPopUpMenu;
class BEntry;
class BBitmap;

class AlignObjects : public VTool
{
public:
	AlignObjects(image_id addonId,BEntry *entry);
	~AlignObjects();
	
	virtual	void	MessageReceived(BMessage *message);			// gestion des messages
			void	DoAction(BPoint *where);
			void	SelectTools(BPoint *where);
			void	RefreshPreferences();
	
private:
	BMessenger	_application;
	BResources	_ressource;
	BPopUpMenu	*_orientationMenu;
	int32		_stepGrid;
	int32		_orientation;
	BBitmap		*_displayedBitmap;
	BBitmap		*_icon[3];
	
			void	DrawTool(BRect bounds);					// dessin personnalisé
			void	ChangeOrientation(BMessage *message);	// changer l'orientation
			void	LoadBitmap();							// charger les icons
			BBitmap *GetBitmap(const char *name);			// trouver une image dans les ressources
};

#endif

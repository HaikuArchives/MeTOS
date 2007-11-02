#ifndef _DELETEOBJECT_H
#define _DELETEOBJECT_H
/*********************/
/* MeTOS tools class */
/*********************/
#include "VTool.h"
#include <app/Messenger.h>

// messages
#define			PAINTTOCOLOR_SELECT_COLOR		'Ptsc'

class BPopUpMenu;
class BEntry;

class PaintToColor : public VTool
{
public:
	PaintToColor(image_id addonId,BEntry *entry);
	~PaintToColor();

	virtual	void	MessageReceived(BMessage *message);			// gestion des messages
			void	DrawTool(BRect bounds);						// dessiner le bouton soit-meme
			void	DoAction(BPoint *where);
			void	SelectTools(BPoint *where);
			void	RefreshPreferences();

private:
	BMessenger	_application;
	rgb_color	_toolColor;
	BPopUpMenu	*_colorMenu;

			void	ChangeColor(BMessage *message);					// changer la couleur courante
			void	UintToRGB(uint32 &color,rgb_color &rgbColor);	// transformer un uint32 en rgb_color
			void	RGBToUint(rgb_color &rgbColor,uint32 &color);	// transformer un rgb_color en uint32
};

#endif

#ifndef _PAINTTOCOLORPREFS_H
#define _PAINTTOCOLORPREFS_H
/*****************************/
/* MeTOS preference class    */
/* pour l'outil PointToColor */
/*****************************/
#include "VPreference.h"
#include "ListItem.h"
#include <String.h>

// messages
#define		SELECT_PAINT_COLOR		'Spcl'
#define		DEFINE_PAINT_COLOR		'Dpcl'
#define		DELCOLOR_PAINT_COLOR	'Dcpc'
#define		ADDCOLOR_PAINT_COLOR	'Acpc'

class VPreferenceFile;
class BListView;
class BColorControl;
class BButton;
class ColorListItem;

// gestion des preferences generales
class PaintToColorPrefs : public VPreference
{
public:
	PaintToColorPrefs(BRect frame);

			void		AttachedToWindow();
			void		MessageReceived(BMessage *message);
			void		SavePrefs();		// sauver les preferences
	
protected:
	// color-manager
	VPreferenceFile		*_prefFile;
	BBox				*_paintToColor;
	BListView			*_colorList;
	BColorControl		*_colorChoice;
	BButton				*_colorDel;
	BButton				*_colorAdd;
	ColorListItem		*_edited;

			void	UintToRGB(uint32 &color,rgb_color &rgbColor);	// transformer un uint32 en rgb_color
			void	RGBToUint(rgb_color &rgbColor,uint32 &color);	// transformer un rgb_color en uint32
};

// item qui represente la couleur
class ColorListItem : public BListItem
{
public:
	ColorListItem(rgb_color color);

			void		DrawItem(BView *owner,BRect bounds,bool complete);	// dessin
			void		SetColor(rgb_color value);							// definir la couleur

	inline	rgb_color	Color()						{ return _color; }		// recuperer la couleur
	
protected:
	rgb_color	_color;
	BString		_labelRGB;
	BString		_labelUint;
};

#endif
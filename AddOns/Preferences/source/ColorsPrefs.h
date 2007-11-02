#ifndef _METOSCOLORSPREFS_H
#define _METOSCOLORSPREFS_H
/**************************/
/* MeTOS preference class */
/* pour l'application     */
/**************************/
#include "VPreference.h"
#include <support/String.h>

// messages
#define		SELECT_METOS_COLOR		'Smcl'
#define		DEFINE_METOS_COLOR		'Dmcl'

class BColorControl;
class ColorListItem;
class BListView;
class BBox;

// gestion des couleurs
class ColorsPrefs : public VPreference
{
public:
	ColorsPrefs(BRect frame);
	virtual	~ColorsPrefs();

			void		AttachedToWindow();
			void		MessageReceived(BMessage *message);
			void		SavePrefs();						// sauver les preferences
	
protected:
	// color-manager
	BBox			*_paintToColor;
	BListView		*_colorList;
	BColorControl	*_colorChoice;
	ColorListItem	*_edited;

			void	UintToRGB(uint32 &color,rgb_color &rgbColor);	// transformer un uint32 en rgb_color
			void	RGBToUint(rgb_color &rgbColor,uint32 &color);	// transformer un rgb_color en uint32
};

// item qui represente la couleur
class ColorListItem : public BListItem
{
public:
	ColorListItem(rgb_color color,const char *name);

			void		DrawItem(BView *owner,BRect bounds,bool complete);	// dessin
			void		SetColor(rgb_color value);							// definir la couleur

	inline	rgb_color	Color()						{ return _color; }		// recuperer la couleur
	inline	BString		Name()						{ return _name; }		// recuperer le nom

protected:
	rgb_color	_color;
	BString		_labelRGB;
	BString		_name;
};

#endif

#ifndef __COLOR_H
#define __COLOR_H
/*****************************************/
/* Addon de la proprietes _color de BeOS */
/*****************************************/
#include "VPropertie.h"
#include <support/String.h>

class BEntry;
class BTextControl;
class BColorControl;

class _Color : public VPropertie
{
public:
	_Color(entry_ref *entry);
	~_Color();

			VPropertie* InstantiateVPropertie(entry_ref *entry);						// clonage
			void		Draw(BView *owner,BRect bounds,bool complete = false);			// permet a l'utilisateur de dessiner ce qu'il veux
			void		SetProperties(BMessage *properties);							// acces aux proprietes
			void		Properties(BMessage *objectProperties);							// modifier les proprietes
			void		SelectItem(BView *owner,BRect bounds);							// selectionne l'item
			void		UnSelectItem(BView *owner,BRect bounds);						// deselectionne l'item
			void		ValueAsString(BMessage *properties,BString &value,int32 index);	// recuperer la valeur sour forme de chaine

private:
	BColorControl	*_colorControl;
	BRect			_controlSize;
	rgb_color		_rgbColor;
	
			void	SetRGBColor(uint32 &color);		// transormer un uint32 en rgb_color
			void	GetUINTColor(uint32 &color);	// recuperer un rgb_color en uint32
			bool	ColorChanged(rgb_color &color_1,rgb_color &color_2);	// tester la difference des couleurs
};

#endif

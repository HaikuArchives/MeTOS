#ifndef __LAYOUT_H
#define __LAYOUT_H
/*******************************************/
/* Addon de la proprietes _layout de MeTOS */
/*******************************************/
#include "VPropertie.h"
#include <support/String.h>
#include <interface/ColorControl.h>

class BMenuField;

class _Layout : public VPropertie
{
public:
	_Layout(entry_ref *entry);
	~_Layout();

			VPropertie* InstantiateVPropertie(entry_ref *entry);						// clonage
			void		Draw(BView *owner,BRect bounds,bool complete = false);			// permet a l'utilisateur de dessiner ce qu'il veux
			void		SetProperties(BMessage *properties);							// acces aux proprietes
			void		Properties(BMessage *objectProperties);							// modifier les proprietes
			void		SelectItem(BView *owner,BRect bounds);							// selectionne l'item
			void		UnSelectItem(BView *owner,BRect bounds);						// deselectionne l'item
			void		ValueAsString(BMessage *properties,BString &value,int32 index);	// recuperer la valeur sour forme de chaine

private:
	BMenuField				*_layoutMenuField;
	color_control_layout	_layout;
};

#endif

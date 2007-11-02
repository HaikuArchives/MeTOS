#ifndef __RECT_H
#define __RECT_H
/******************************/
/* Addon du type RECT de BeOS */
/******************************/
#include "VPropertie.h"
#include <interface/Rect.h>

class RECT : public VPropertie
{
public:
	RECT(entry_ref *entry);

			VPropertie* InstantiateVPropertie(entry_ref *entry);						// clonage
			void		Draw(BView *owner,BRect bounds,bool complete = false);			// permet a l'utilisateur de dessiner ce qu'il veux
			void		SetProperties(BMessage *properties);							// acces aux proprietes
			void		Properties(BMessage *objectProperties);							// modifier les proprietes
			void		ValueAsString(BMessage *properties,BString &value,int32 index);	// recuperer la valeur sour forme de chaine

private:
	BRect	_rect;
};

#endif

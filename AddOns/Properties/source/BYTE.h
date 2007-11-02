#ifndef __BYTE_H
#define __BYTE_H
/******************************/
/* Addon du type BYTE de BeOS */
/******************************/
#include "VPropertie.h"
#include <support/String.h>

class BTextControl;

class BYTE : public VPropertie
{
public:
	BYTE(entry_ref *entry);
	~BYTE();

			VPropertie* InstantiateVPropertie(entry_ref *entry);						// clonage
			void		Draw(BView *owner,BRect bounds,bool complete = false);			// permet a l'utilisateur de dessiner ce qu'il veux
			void		SetProperties(BMessage *properties);							// acces aux proprietes
			void		Properties(BMessage *objectProperties);							// modifier les proprietes
			void		SelectItem(BView *owner,BRect bounds);							// selectionne l'item
			void		UnSelectItem(BView *owner,BRect bounds);						// deselectionne l'item
			void		ValueAsString(BMessage *properties,BString &value,int32 index);	// recuperer la valeur sour forme de chaine

private:
	int8			_byte;
	BTextControl	*_editor;
};

#endif

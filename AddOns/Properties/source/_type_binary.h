#ifndef __TYPE_BINARY_H
#define __TYPE_BINARY_H
/***************************************/
/* Addon de la proprietes _type_binary */
/***************************************/
#include "VPropertie.h"
#include <support/String.h>
#include <interface/Window.h>

class BMenuField;

class _TypeBinary : public VPropertie
{
public:
	_TypeBinary(entry_ref *entry);
	~_TypeBinary();

			VPropertie* InstantiateVPropertie(entry_ref *entry);						// clonage
			void		Draw(BView *owner,BRect bounds,bool complete = false);			// permet a l'utilisateur de dessiner ce qu'il veux
			void		SetProperties(BMessage *properties);							// acces aux proprietes
			void		Properties(BMessage *objectProperties);							// modifier les proprietes
			void		SelectItem(BView *owner,BRect bounds);							// selectionne l'item
			void		UnSelectItem(BView *owner,BRect bounds);						// deselectionne l'item
			void		ValueAsString(BMessage *properties,BString &value,int32 index);	// recuperer la valeur sour forme de chaine

private:
	BMenuField		*_typeBinaryMenuField;
	int32			_typeBinary;
};

#endif

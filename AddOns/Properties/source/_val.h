#ifndef __VAL_H
#define __VAL_H
/****************************************/
/* Addon de la proprietes _Val de MeTOS */
/****************************************/
#include "VPropertie.h"
#include <support/String.h>

class BCheckBox;

class _Val : public VPropertie
{
public:
	_Val(entry_ref *entry);
	~_Val();

			VPropertie* InstantiateVPropertie(entry_ref *entry);						// clonage
			void		Draw(BView *owner,BRect bounds,bool complete = false);			// permet a l'utilisateur de dessiner ce qu'il veux
			void		SetProperties(BMessage *properties);							// acces aux proprietes
			void		Properties(BMessage *objectProperties);							// modifier les proprietes
			void		SelectItem(BView *owner,BRect bounds);							// selectionne l'item
			void		UnSelectItem(BView *owner,BRect bounds);						// deselectionne l'item
			void		ValueAsString(BMessage *properties,BString &value,int32 index);	// recuperer la valeur sour forme de chaine

private:
	int32		_value;
	BCheckBox	*_checkBox;
};

#endif

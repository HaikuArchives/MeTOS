#ifndef __BOOL_H
#define __BOOL_H
/******************************/
/* Addon du type BOOL de BeOS */
/******************************/
#include "VPropertie.h"

class BCheckBox;

class BOOL : public VPropertie
{
public:
	BOOL(entry_ref *entry);
	~BOOL();

			VPropertie* InstantiateVPropertie(entry_ref *entry);						// clonage
			void		Draw(BView *owner,BRect bounds,bool complete = false);			// permet a l'utilisateur de dessiner ce qu'il veux
			void		SetProperties(BMessage *properties);							// acces aux proprietes
			void		Properties(BMessage *objectProperties);							// modifier les proprietes
			void		SelectItem(BView *owner,BRect bounds);							// selectionne l'item
			void		UnSelectItem(BView *owner,BRect bounds);						// deselectionne l'item
			void		ValueAsString(BMessage *properties,BString &value,int32 index);	// recuperer la valeur sour forme de chaine

private:
	bool		_boolean;
	BCheckBox	*_checkBox;
};

#endif

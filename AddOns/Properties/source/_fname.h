#ifndef __FNAME_H
#define __FNAME_H
/******************************************/
/* Addon de la proprietes _Fname de MeTOS */
/******************************************/
#include "VPropertie.h"

class BMenuField;

class _Fname : public VPropertie
{
public:
	_Fname(entry_ref *entry);
	~_Fname();

			VPropertie* InstantiateVPropertie(entry_ref *entry);						// clonage
			void		Draw(BView *owner,BRect bounds,bool complete = false);			// permet a l'utilisateur de dessiner ce qu'il veux
			void		SetProperties(BMessage *properties);							// acces aux proprietes
			void		Properties(BMessage *objectProperties);							// modifier les proprietes
			void		SelectItem(BView *owner,BRect bounds);							// selectionne l'item
			void		UnSelectItem(BView *owner,BRect bounds);						// deselectionne l'item
			void		ValueAsString(BMessage *properties,BString &value,int32 index);	// recuperer la valeur sour forme de chaine

private:
	BString		_fname;
	BString		_fName;
	BString		_fStyle;
	uint32		_objectID;
	BMenuField	*_fontMenu;
	BMenuField	*_fontStyleMenu;
	BMenuField	*_selector;

			void		RebuildFontStyleMenu();		// regenerer le menu des styles
			void		UpdateFontStyle();			// mettre a jour le style
			void		CheckMarcked();				// selectionner l'item de menu
};

#endif

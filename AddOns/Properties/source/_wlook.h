#ifndef __WLOOK_H
#define __WLOOK_H
/*****************************************/
/* Addon de la proprietes _look de MeTOS */
/*****************************************/
#include "VPropertie.h"
#include <support/String.h>
#include <interface/Window.h>

class BMenuField;

class _Wlook : public VPropertie
{
public:
	_Wlook(entry_ref *entry);
	~_Wlook();

			VPropertie* InstantiateVPropertie(entry_ref *entry);						// clonage
			void		Draw(BView *owner,BRect bounds,bool complete = false);			// permet a l'utilisateur de dessiner ce qu'il veux
			void		SetProperties(BMessage *properties);							// acces aux proprietes
			void		Properties(BMessage *objectProperties);							// modifier les proprietes
			void		SelectItem(BView *owner,BRect bounds);							// selectionne l'item
			void		UnSelectItem(BView *owner,BRect bounds);						// deselectionne l'item
			void		ValueAsString(BMessage *properties,BString &value,int32 index);	// recuperer la valeur sour forme de chaine

private:
	BMenuField		*_wlookMenuField;
	window_look		_wlook;
};

#endif

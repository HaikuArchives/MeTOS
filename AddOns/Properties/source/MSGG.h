#ifndef __MSGG_H
#define __MSGG_H
/******************************/
/* Addon du type MSGG de BeOS */
/******************************/
#include "VPropertie.h"
#include <app/Message.h>

class BTextControl;

class MSGG : public VPropertie
{
public:
	MSGG(entry_ref *entry);
	~MSGG();

			VPropertie* InstantiateVPropertie(entry_ref *entry);						// clonage
			void		Draw(BView *owner,BRect bounds,bool complete = false);			// permet a l'utilisateur de dessiner ce qu'il veux
			void		SetProperties(BMessage *properties);							// acces aux proprietes
			void		Properties(BMessage *objectProperties);							// modifier les proprietes
			void		SelectItem(BView *owner,BRect bounds);							// selectionne l'item
			void		UnSelectItem(BView *owner,BRect bounds);						// deselectionne l'item
			void		ValueAsString(BMessage *properties,BString &value,int32 index);	// recuperer la valeur sour forme de chaine

private:
	BMessage		_message;
	BTextControl	*_editor;

			void		ConvertWhatToString(uint32 what,BString &string);		// convertir un what de BMessage en chaine
};

#endif

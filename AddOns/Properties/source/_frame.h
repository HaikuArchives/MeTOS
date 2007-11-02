#ifndef __FRAME_H
#define __FRAME_H
/*****************************************/
/* Addon de la proprietes _frame de BeOS */
/*****************************************/
#include "VPropertie.h"
#include <interface/Rect.h>

class _Frame : public VPropertie
{
public:
	_Frame(entry_ref *entry);

			VPropertie* InstantiateVPropertie(entry_ref *entry);						// clonage
			void		Draw(BView *owner,BRect bounds,bool complete = false);			// permet a l'utilisateur de dessiner ce qu'il veux
			void		SetProperties(BMessage *properties);							// acces aux proprietes
			void		Properties(BMessage *objectProperties);							// modifier les proprietes
			void		ValueAsString(BMessage *properties,BString &value,int32 index);	// recuperer la valeur sour forme de chaine

private:
	BRect	_frame;
};

#endif

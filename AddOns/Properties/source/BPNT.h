#ifndef __BPNT_H
#define __BPNT_H
/******************************/
/* Addon du type BPNT de BeOS */
/******************************/
#include "VPropertie.h"
#include <interface/Point.h>

class BPNT : public VPropertie
{
public:
	BPNT(entry_ref *entry);

			VPropertie* InstantiateVPropertie(entry_ref *entry);						// clonage
			void		Draw(BView *owner,BRect bounds,bool complete = false);			// permet a l'utilisateur de dessiner ce qu'il veux
			void		SetProperties(BMessage *properties);							// acces aux proprietes
			void		Properties(BMessage *objectProperties);							// modifier les proprietes
			void		ValueAsString(BMessage *properties,BString &value,int32 index);	// recuperer la valeur sour forme de chaine

private:
	BPoint	_point;
};

#endif

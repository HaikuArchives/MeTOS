#ifndef __RESIZEMODE_H
#define __RESIZEMODE_H
/****************************************/
/* Addon de la proprietes name de MeTOS */
/****************************************/
#include "VPropertie.h"
#include <support/String.h>

class BBox;
class BRadioButton;

class _ResizeMode : public VPropertie
{
public:
	_ResizeMode(entry_ref *entry);
	~_ResizeMode();

			VPropertie* InstantiateVPropertie(entry_ref *entry);						// clonage
			void		Draw(BView *owner,BRect bounds,bool complete = false);			// permet a l'utilisateur de dessiner ce qu'il veux
			void		SetProperties(BMessage *properties);							// acces aux proprietes
			void		Properties(BMessage *objectProperties);							// modifier les proprietes
			void		SelectItem(BView *owner,BRect bounds);							// selectionne l'item
			void		UnSelectItem(BView *owner,BRect bounds);						// deselectionne l'item
			void		ValueAsString(BMessage *properties,BString &value,int32 index);	// recuperer la valeur sour forme de chaine

private:
	uint32				_resize_mode;
	// premier groupe
	BBox				*_groupH;
	BRadioButton		*_bFollowLeft;
	BRadioButton		*_bFollowRight;
	BRadioButton		*_bFollowLeftRight;
	BRadioButton		*_bFollowHCenter;
	// second
	BBox				*_groupV;
	BRadioButton		*_bFollowTop;
	BRadioButton		*_bFollowBottom;
	BRadioButton		*_bFollowTopBottom;
	BRadioButton		*_bFollowVCenter;

			void		AddBox(BView *owner,BBox *box,float x,float y);				// ajouter le BBox a l'ecran
			void		ConvertModeToString(uint32 &mode,BString &string);			// recuperer sous forme de chaine la valeur du resize_mode
};

#endif

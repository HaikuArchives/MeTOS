#ifndef __FLAGS_H
#define __FLAGS_H
/*****************************************/
/* Addon de la proprietes _flags de BeOS */
/*****************************************/
#include "VPropertie.h"
#include "VObject.h"

class BCheckBox;

class _Flags : public VPropertie
{
public:
	_Flags(entry_ref *entry);
	~_Flags();

			VPropertie* InstantiateVPropertie(entry_ref *entry);						// clonage
			void		Draw(BView *owner,BRect bounds,bool complete = false);			// permet a l'utilisateur de dessiner ce qu'il veux
			void		SetProperties(BMessage *properties);							// acces aux proprietes
			void		Properties(BMessage *objectProperties);							// modifier les proprietes
			void		SelectItem(BView *owner,BRect bounds);							// selectionne l'item
			void		UnSelectItem(BView *owner,BRect bounds);						// deselectionne l'item
			void		ValueAsString(BMessage *properties,BString &value,int32 index);	// recuperer la valeur sour forme de chaine

private:
	int32		_flags;
	int32		_type;
	float		_newSize;
	// les checkBox des flags BViews
	BCheckBox	*_checkBWillDraw;			// B_WILL_DRAW
	BCheckBox	*_checkBPulseNeeded;		// B_PULSE_NEEDED
	BCheckBox	*_checkBFrameEvents;		// B_FRAME_EVENTS
	BCheckBox	*_checkBFullUpdate;			// B_FULL_UPDATE_ON_RESIZE
	BCheckBox	*_checkBNavigable;			// B_NAVIGABLE
	BCheckBox	*_checkBNavigableJump;		// B_NAVIGABLE_JUMP
	BCheckBox	*_checkBSubpixelPrecise;	// B_SUBPIXEL_PRECISE
	// creer les checkBox des flags des BWindow
	BCheckBox	*_checkBNotMovable;			// B_NOT_MOVABLE
	BCheckBox	*_checkBNotClosable;		// B_NOT_CLOSABLE
	BCheckBox	*_checkBNotZoomable;		// B_NOT_ZOOMABLE
	BCheckBox	*_checkBNotMinimizable;		// B_NOT_MINIMIZABLE
	BCheckBox	*_checkBNotHResizable;		// B_NOT_H_RESIZABLE
	BCheckBox	*_checkBNotVResizable;		// B_NOT_V_RESIZABLE
	BCheckBox	*_checkBNotResizable;		// B_NOT_RESIZABLE
	BCheckBox	*_checkBOutlineResize;		// B_OUTLINE_RESIZE
	BCheckBox	*_checkBWillAcceptFC;		// B_WILL_ACCEPT_FIRST_CLICK
	BCheckBox	*_checkBAvoidFront;			// B_AVOID_FRONT
	BCheckBox	*_checkBAvoidFocus;			// B_AVOID_FOCUS
	BCheckBox	*_checkBNoWorkspaceA;		// B_NO_WORKSPACE_ACTIVATION
	BCheckBox	*_checkBNotAnchoredOA;		// B_NOT_ANCHORED_ON_ACTIVATE
	BCheckBox	*_checkBAsynchronousC;		// B_ASYNCHRONOUS_CONTROLS

			void		AddCheckBox(BView *owner,BCheckBox *checkBox,float x,float y);	// ajouter la checkBox a l'ecran
};

#endif

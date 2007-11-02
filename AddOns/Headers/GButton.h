/****************************************************
*													* 		
*	GONX GUILib										*
*	Version:	0.1									*
*	File: GButton.h		 							*
*	Ideas from: http://cotito.free.fr				*	
*													*
*	Authors: 	Johan Nilsson						*
*				Mikael Konradson					*	
*													*	
****************************************************/

#ifndef GBUTTON_H
#define GBUTTON_H
#include <string.h>
#include <Control.h>

class GButton : public BControl
{
	public:
		GButton(BRect frame,const char *name,const char *label, BMessage *message,uint32 resizingMode = B_FOLLOW_LEFT | B_FOLLOW_TOP, uint32 flags = B_WILL_DRAW | B_NAVIGABLE);
		GButton(BMessage *archive);
		~GButton();
		
	static  BArchivable     *Instantiate(BMessage *data); 
	virtual status_t        Archive(BMessage *data, bool deep = true) const;

	virtual void SetLabel(const char *string);	
	virtual void MouseDown(BPoint p);
	virtual void MouseUp(BPoint p);
	virtual void MouseMoved(BPoint p,uint32 code,const BMessage *a_message);

	virtual void MakeDefault(bool state);
			bool IsDefault() const;
	virtual void AttachedToWindow();
	virtual	void DetachedFromWindow();
	virtual void Draw(BRect updateRect);	
	virtual void MakeFocus(bool focusState = true);
	virtual void KeyDown(const char *bytes,int32 numBytes);
	virtual void KeyUp(const char *bytes,int32 numBytes);

private:
	//Added our varibles to private
			BRect DrawDefault(BRect bounds, bool enabled);
			bool fDrawAsDefault; 
};
#endif

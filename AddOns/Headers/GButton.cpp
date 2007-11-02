/****************************************************
*													* 		
*	GONX GUILib										*
*	Version:	0.1									*
*	File: GButtonpp		 							*
*	Ideas from: http://cotito.free.fr				*	
*													*
*	Authors: 	Johan Nilsson						*
*				Mikael Konradson					*	
*													*	
****************************************************/

#include "GButton.h"

GButton::GButton(BRect frame,const char *name, const char *label, BMessage *message,uint32 resizingMode, uint32 flags) 
: BControl(frame,name,label,message, resizingMode, flags)
,fDrawAsDefault(false)
{
	if (Bounds().Height() < 24.0f)
		ResizeTo(Bounds().Width(), 24.0f);
}
GButton::~GButton()
{
	//Destructor
}
//------------------------------------------------------------------------------
GButton::GButton(BMessage *archive)
: BControl(archive)
{
	if ( archive->FindBool ( "_default", &fDrawAsDefault ) != B_OK )
		fDrawAsDefault = false;
}
//------------------------------------------------------------------------------
BArchivable *GButton::Instantiate ( BMessage *archive )
{
	if ( validate_instantiation(archive, "GButton"))
		return new GButton(archive);
	else
		return NULL;
}
//------------------------------------------------------------------------------
status_t GButton::Archive(BMessage* archive, bool deep) const
{
	status_t err = BControl::Archive(archive, deep);

	if (err != B_OK)
		return err;
	
	if (fDrawAsDefault)
		err = archive->AddBool("_default", fDrawAsDefault);

	return err;
}

//------------------------------------------------------------------------------
bool GButton::IsDefault() const
{
	return fDrawAsDefault;
}
//****************** T I T L E **************************************//
void GButton::SetLabel(const char *string)
{
	BControl::SetLabel(string);
}
//****************** D R A W I N G **********************************//
void GButton::Draw(BRect updateRect)
{
	font_height fh;
	BRect r;
	SetHighColor(ViewColor());
	FillRect(Bounds());
	if(IsEnabled())
	{
		if(Value())	
		{	
			r = Bounds();
			SetPenSize(1.0);
			BRect r2 = r;
			r2.left += 4;
			r2.right -= 4;
			r2.bottom -= 4;
			r2.top += 4;
			//Background
			//SetHighColor(Parent()->ViewColor());
			SetHighColor(247,247,247); 
			FillRect(r2);

			//shadow inside button
			SetHighColor(223,223,218); 
			StrokeLine(BPoint(r2.left,r2.top),BPoint(r2.right+1,r2.top)); //top shadow
			SetHighColor(252,252,251); 
			StrokeLine(BPoint(r2.right+1,r2.top),BPoint(r2.right+1,r2.bottom-1));//right and left shadow
			StrokeLine(BPoint(r2.left-1,r2.top+1),BPoint(r2.left-1,r2.bottom));
			SetHighColor(252,252,251); 
			StrokeLine(BPoint(r2.left-1,r2.bottom),BPoint(r2.right,r2.bottom)); //bottom shadow

			
			//Top and bottom Outline
			SetHighColor(140,140,121);
			StrokeLine(BPoint(r2.left,r2.top-1),BPoint(r2.right+1,r2.top-1));
			StrokeLine(BPoint(r2.left-1,r2.bottom+1),BPoint(r2.right,r2.bottom+1));

			//topleft and bottom right corners outline
			StrokeLine(BPoint(r2.left,r2.top-1),BPoint(r2.left-2,r2.top+1));
			StrokeLine(BPoint(r2.right,r2.bottom+1),BPoint(r2.right+2,r2.bottom-1));

			//left and right outline
			StrokeLine(BPoint(r2.left-2,r2.top+2),BPoint(r2.left-2,r2.bottom));
			StrokeLine(BPoint(r2.right+2,r2.top),BPoint(r2.right+2,r2.bottom-2));
			
			SetDrawingMode(B_OP_ALPHA);

			//Shadow
			//Dark Shadow
			SetHighColor(140,140,121,130);
			StrokeLine(BPoint(r2.left,r2.bottom+2),BPoint(r2.right-1,r2.bottom+2)); //bottom shadow
			StrokeLine(BPoint(r2.right,r2.bottom+2),BPoint(r2.right+3,r2.bottom-1)); //bottomright shadow
			StrokeLine(BPoint(r2.right+3,r2.top+1),BPoint(r2.right+3,r2.bottom-2)); //rightside shadow

			SetDrawingMode(B_OP_COPY);
			SetHighColor(140,140,121);

		}
		else
		{
			r = Bounds();
			SetPenSize(1.0);
			BRect r2 = r;
			r2.left += 4;
			r2.right -= 4;
			r2.bottom -= 4;
			r2.top += 4;
			//Background
			//SetHighColor(Parent()->ViewColor());
			SetHighColor(244,244,240); 
			FillRect(r2);

			//shadow inside button
			SetHighColor(252,252,251); 
			StrokeLine(BPoint(r2.left,r2.top),BPoint(r2.right+1,r2.top)); //top shadow
			SetHighColor(223,223,218); 
			StrokeLine(BPoint(r2.right+1,r2.top),BPoint(r2.right+1,r2.bottom-1));//right and left shadow
			StrokeLine(BPoint(r2.left-1,r2.top+1),BPoint(r2.left-1,r2.bottom));
			SetHighColor(225,225,214); 
			StrokeLine(BPoint(r2.left-1,r2.bottom),BPoint(r2.right,r2.bottom)); //bottom shadow

			
			//Top and bottom Outline
			SetHighColor(140,140,121);
			StrokeLine(BPoint(r2.left,r2.top-1),BPoint(r2.right+1,r2.top-1));
			StrokeLine(BPoint(r2.left-1,r2.bottom+1),BPoint(r2.right,r2.bottom+1));

			//topleft and bottom right corners outline
			StrokeLine(BPoint(r2.left,r2.top-1),BPoint(r2.left-2,r2.top+1));
			StrokeLine(BPoint(r2.right,r2.bottom+1),BPoint(r2.right+2,r2.bottom-1));

			//left and right outline
			StrokeLine(BPoint(r2.left-2,r2.top+2),BPoint(r2.left-2,r2.bottom));
			StrokeLine(BPoint(r2.right+2,r2.top),BPoint(r2.right+2,r2.bottom-2));
			
			SetDrawingMode(B_OP_ALPHA);

			//Shadow
			//Dark Shadow
			SetHighColor(140,140,121,180);
			StrokeLine(BPoint(r2.left,r2.bottom+2),BPoint(r2.right-1,r2.bottom+2)); //bottom shadow
			StrokeLine(BPoint(r2.right,r2.bottom+2),BPoint(r2.right+3,r2.bottom-1)); //bottomright shadow
			StrokeLine(BPoint(r2.right+3,r2.top+1),BPoint(r2.right+3,r2.bottom-2)); //rightside shadow

			//Light Shadow
			SetHighColor(140,140,121,90);
			StrokeLine(BPoint(r2.left+1,r2.bottom+3),BPoint(r2.right-1,r2.bottom+3)); //bottom shadow
			StrokeLine(BPoint(r2.right,r2.bottom+3),BPoint(r2.right+3,r2.bottom)); //bottomright shadow
			StrokeLine(BPoint(r2.right+4,r2.top+2),BPoint(r2.right+4,r2.bottom-1)); //rightside shadow

			SetDrawingMode(B_OP_COPY);
			SetHighColor(0,0,0);
		}
	}
	else
	{
		r = Bounds();
		SetPenSize(1.0);
		BRect r2 = r;
		r2.left += 4;
		r2.right -= 4;
		r2.bottom -= 4;
		r2.top += 4;
		//Background
		SetHighColor(244,244,240); 
		FillRect(r2);

		//shadow inside button
		SetHighColor(252,252,251); 
		StrokeLine(BPoint(r2.left,r2.top),BPoint(r2.right+1,r2.top)); //top shadow
		SetHighColor(223,223,218); 
		StrokeLine(BPoint(r2.right+1,r2.top),BPoint(r2.right+1,r2.bottom-1));//right and left shadow
		StrokeLine(BPoint(r2.left-1,r2.top+1),BPoint(r2.left-1,r2.bottom));
		SetHighColor(225,225,214); 
		StrokeLine(BPoint(r2.left-1,r2.bottom),BPoint(r2.right,r2.bottom)); //bottom shadow

		
		//Top and bottom Outline
		SetHighColor(140,140,121);
		StrokeLine(BPoint(r2.left,r2.top-1),BPoint(r2.right+1,r2.top-1));
		StrokeLine(BPoint(r2.left-1,r2.bottom+1),BPoint(r2.right,r2.bottom+1));

		//topleft and bottom right corners outline
		StrokeLine(BPoint(r2.left,r2.top-1),BPoint(r2.left-2,r2.top+1));
		StrokeLine(BPoint(r2.right,r2.bottom+1),BPoint(r2.right+2,r2.bottom-1));

		//left and right outline
		StrokeLine(BPoint(r2.left-2,r2.top+2),BPoint(r2.left-2,r2.bottom));
		StrokeLine(BPoint(r2.right+2,r2.top),BPoint(r2.right+2,r2.bottom-2));
		
		SetDrawingMode(B_OP_ALPHA);

		//Shadow
		//Dark Shadow
		SetHighColor(140,140,121,180);
		StrokeLine(BPoint(r2.left,r2.bottom+2),BPoint(r2.right-1,r2.bottom+2)); //bottom shadow
		StrokeLine(BPoint(r2.right,r2.bottom+2),BPoint(r2.right+3,r2.bottom-1)); //bottomright shadow
		StrokeLine(BPoint(r2.right+3,r2.top+1),BPoint(r2.right+3,r2.bottom-2)); //rightside shadow

		//Light Shadow
		SetHighColor(140,140,121,90);
		StrokeLine(BPoint(r2.left+1,r2.bottom+3),BPoint(r2.right-1,r2.bottom+3)); //bottom shadow
		StrokeLine(BPoint(r2.right,r2.bottom+3),BPoint(r2.right+3,r2.bottom)); //bottomright shadow
		StrokeLine(BPoint(r2.right+4,r2.top+2),BPoint(r2.right+4,r2.bottom-1)); //rightside shadow

		SetDrawingMode(B_OP_COPY);
		SetHighColor(140,140,121);

	}

	be_plain_font->GetHeight(&fh);
	MovePenTo((Bounds().right/2)-((be_plain_font->StringWidth(Label())/2)),(Bounds().bottom/2)+(fh.ascent/2) - (!Value()? 1:0));
	DrawString(Label());
	if(BControl::IsFocus() && IsEnabled()) {
		SetHighColor(80,114,154);
		BPoint p1,p2;
		p1.x = (Bounds().right/2)-((be_plain_font->StringWidth(Label())/2))-1;
		p1.y = (Bounds().bottom/2)+(fh.ascent/2)+1;
		p2.x =  p1.x + be_plain_font->StringWidth(Label());
		p2.y = p1.y;
		StrokeLine(p1,p2);
	}
		if (fDrawAsDefault)
		{
			DrawDefault(Bounds(),true);
		}
	BControl::Draw(updateRect);
}
//****************** D R A W  DEFAULT **********************************//
BRect GButton::DrawDefault(BRect bounds, bool enabled)
{	
	MakeFocus();
	return BRect();
}
//****************** M O U S E **********************************//
void GButton::MouseDown(BPoint p)
{
	if(BControl::IsEnabled())
	{
		BControl::SetValue(B_CONTROL_ON);
		Invalidate();
	}
}
void GButton::MouseUp(BPoint p)
{
	if(BControl::IsEnabled())
	{
		BControl::SetValue(B_CONTROL_OFF);
		Invalidate();
		Invoke();
	}
}
void GButton::MouseMoved(BPoint p, uint32 code, const BMessage *a_message)
{
	BPoint ptemp;
	uint32 buttons;
	
	if(code == B_EXITED_VIEW)
	if(BControl::IsEnabled() && BControl::Value())
	{
		BControl::SetValue(B_CONTROL_OFF);
		Invalidate();
	}
	if(code == B_ENTERED_VIEW)
	{
		
		GetMouse(&ptemp,&buttons);
		if(buttons)
		{
			MouseDown(p);
		}
	}
}

void GButton::MakeFocus(bool focusState = true)
{	
	if(IsEnabled())
	{
		BControl::MakeFocus(focusState);
	}
}
void GButton::KeyDown(const char *bytes,int32 numBytes) {
	if(IsEnabled() && !BControl::Value())
	{
		if(BControl::IsFocus() && ((bytes[0] == B_SPACE) || (bytes[0] == B_ENTER)))
		{
			BControl::SetValue(B_CONTROL_ON);
			Invoke();
			Invalidate();
		}
		BControl::KeyDown(bytes,numBytes);
	}
}
void GButton::KeyUp(const char *bytes,int32 numBytes) {
	if(IsEnabled())
	{
		if(BControl::IsFocus() && ((bytes[0] == B_SPACE) || (bytes[0] == B_ENTER)))
		{
			BControl::SetValue(B_CONTROL_OFF);
			Invoke();
			Invalidate();
		}
		BControl::KeyUp(bytes,numBytes);
	}
}
//***********************************************************************//
void GButton::MakeDefault(bool state)
{
	if (state == IsDefault())
		return;

	fDrawAsDefault = state;
}
//**********************************************************************//
void GButton::AttachedToWindow()
{
	if (Parent())
	{
		SetViewColor(Parent()->ViewColor());
		SetLowColor(Parent()->ViewColor());
	}
}
//******************* D E T A C H ********************************//
void GButton::DetachedFromWindow()
{
	BView::DetachedFromWindow();
}


/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/

/*************************/
/* vue custom des outils */
/*************************/
#include "ToolManagerView.h"
#include "MetosGlobalDefs.h"

/**** constructeur ****/
ToolManagerView::ToolManagerView(BRect frame,const char *name)
: BView(frame,name,B_FOLLOW_ALL_SIDES, B_WILL_DRAW | B_FRAME_EVENTS)
{
}

/**** destructeur ****/
ToolManagerView::~ToolManagerView()
{
}

/**** dessiner la vue un peu autrement ****/
void ToolManagerView::Draw(BRect updateRect)
{
	BRect	bounds = Bounds();
	
	SetHighColor(WHITE_COLOR);
	StrokeLine(BPoint(bounds.left,bounds.top),BPoint(bounds.right,bounds.top));
	StrokeLine(BPoint(bounds.left,bounds.top),BPoint(bounds.left,bounds.bottom));	

	SetHighColor(DARK_2_COLOR);
	StrokeLine(BPoint(bounds.left,bounds.bottom),BPoint(bounds.right,bounds.bottom));
	StrokeLine(BPoint(bounds.right,bounds.top),BPoint(bounds.right,bounds.bottom));	

	bounds.InsetBy(FRAME_SIZE-1,FRAME_SIZE-1);
	SetHighColor(DARK_2_COLOR);
	StrokeLine(BPoint(bounds.left,bounds.top),BPoint(bounds.right,bounds.top));
	StrokeLine(BPoint(bounds.left,bounds.top),BPoint(bounds.left,bounds.bottom));	

	SetHighColor(WHITE_COLOR);
	StrokeLine(BPoint(bounds.left,bounds.bottom),BPoint(bounds.right,bounds.bottom));
	StrokeLine(BPoint(bounds.right,bounds.top),BPoint(bounds.right,bounds.bottom));	

	bounds.InsetBy(1,1);
	SetHighColor(DARK_3_COLOR);
	FillRect(bounds);
}

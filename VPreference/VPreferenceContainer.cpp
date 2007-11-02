/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
/*************************/
/* MeTOS Container class */
/* pour les preferences  */
/*************************/
#include "VPreferenceContainer.h"

/**** constructeur ****/
VPreferenceContainer::VPreferenceContainer(BRect frame,const char *name)
: BView(frame,name,B_FOLLOW_NONE, B_WILL_DRAW)
{
	// fixer la couleur de la vue
	// ca sert surtout si on ajoute des controles qui utilise cette couleur
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
}

/**** destructeur ****/
VPreferenceContainer::~VPreferenceContainer()
{
}

/**** fonction de dessin de la vue ****/
void VPreferenceContainer::Draw(BRect updateRect)
{
	BRect	bounds = Bounds();

	// le rectangle generale
	SetHighColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR),B_LIGHTEN_MAX_TINT));
	StrokeLine(BPoint(bounds.left,bounds.top),BPoint(bounds.right,bounds.top));	
	StrokeLine(BPoint(bounds.left,bounds.top),BPoint(bounds.left,bounds.bottom));

	// le rectangle generale
	SetHighColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR),B_DARKEN_4_TINT));
	StrokeLine(BPoint(bounds.left,bounds.bottom),BPoint(bounds.right,bounds.bottom));	
	StrokeLine(BPoint(bounds.right,bounds.top),BPoint(bounds.right,bounds.bottom));

	// remplir le rectangle
	bounds.InsetBy(1,1);
	SetHighColor(ui_color(B_PANEL_BACKGROUND_COLOR));		
	FillRect(bounds);
	
	// le rectangle de point
	SetLowColor(HighColor());
	SetHighColor(255,255,255);
	StrokeLine(BPoint(bounds.left+1,bounds.top),BPoint(bounds.left+1,bounds.top+15),B_MIXED_COLORS);
	SetHighColor(100,100,100);
	StrokeLine(BPoint(bounds.left+2,bounds.top+1),BPoint(bounds.left+2,bounds.top+15),B_MIXED_COLORS);
	SetHighColor(255,255,255);
	StrokeLine(BPoint(bounds.left+3,bounds.top),BPoint(bounds.left+3,bounds.top+15),B_MIXED_COLORS);
	SetHighColor(100,100,100);
	StrokeLine(BPoint(bounds.left+4,bounds.top+1),BPoint(bounds.left+4,bounds.top+15),B_MIXED_COLORS);
	SetHighColor(255,255,255);
	StrokeLine(BPoint(bounds.left+5,bounds.top),BPoint(bounds.left+5,bounds.top+15),B_MIXED_COLORS);
	SetHighColor(100,100,100);
	StrokeLine(BPoint(bounds.left+6,bounds.top+1),BPoint(bounds.left+6,bounds.top+15),B_MIXED_COLORS);

	// delimiter le titre
	SetHighColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR),B_DARKEN_2_TINT));
	StrokeLine(BPoint(bounds.left,bounds.top+17),BPoint(bounds.right,bounds.top+17));
	SetHighColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR),B_LIGHTEN_MAX_TINT));
	StrokeLine(BPoint(bounds.left,bounds.top+18),BPoint(bounds.right,bounds.top+18));
	
	// le titre
	SetHighColor(65,65,65);
	DrawString(Name(),BPoint(bounds.left+15,bounds.top+12));
}

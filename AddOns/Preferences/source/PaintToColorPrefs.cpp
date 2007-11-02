// =======================================
// = Preferences de l'outil PointToColor =
// =======================================
#include "PaintToColorPrefs.h"
#include "MetosMessageDefs.h"
#include "MetosGlobalDefs.h"
#include "VPreferenceFile.h"

#include <Box.h>
#include <ListView.h>
#include <ScrollView.h>
#include <ColorControl.h>
#include <Button.h>

/**** constructeur ****/
PaintToColorPrefs::PaintToColorPrefs(BRect frame)
: VPreference(frame,"Paint To Color",TOOLS_GROUP)
{
	BScrollView			*supportScroll = NULL;
	BRect				bounds = Bounds();
	rgb_color			RGBColor;
	uint32				color;
	int32				index;

	// initialiser
	_edited = NULL;

	// charger les preferences
	_prefFile = new VPreferenceFile("PaintToColor",TOOLS_GROUP_PATH);
	_prefFile->Load();
	
	// decallage a cause du titre
	bounds.top += VPREFERENCE_TITLE_HIGH;
	bounds.InsetBy(7,7);

	// groupe pincipale
	_paintToColor = new BBox(bounds,"manage-color");
	_paintToColor->SetLabel("Manage color...");
	
	bounds.OffsetTo(0,0);
	bounds.InsetBy(12,20);
	bounds.bottom -= 50;
	bounds.right -= B_V_SCROLL_BAR_WIDTH;
	// liste des couleurs
	_colorList = new BListView(bounds,"color-list");
	_colorList->SetSelectionMessage(new BMessage(SELECT_PAINT_COLOR));
	supportScroll = new BScrollView("support-scroll",_colorList,B_FOLLOW_LEFT | B_FOLLOW_TOP,0,false,true);

	// coix de la couleur
	_colorChoice = new BColorControl(BPoint(bounds.left,bounds.bottom+7),B_CELLS_32x8,4.0,"color-choice",new BMessage(DEFINE_PAINT_COLOR));

	// ajouter et effacer une couleur
	_colorDel = new BButton(BRect(bounds.right-18,bounds.bottom+7,bounds.right+15,bounds.bottom+27),"color-del","Del",new BMessage(DELCOLOR_PAINT_COLOR));
	_colorAdd = new BButton(BRect(bounds.right-18,bounds.bottom+37,bounds.right+15,bounds.bottom+57),"color-add","Add",new BMessage(ADDCOLOR_PAINT_COLOR));

	_paintToColor->AddChild(supportScroll);
	_paintToColor->AddChild(_colorChoice);
	_paintToColor->AddChild(_colorDel);
	_paintToColor->AddChild(_colorAdd);
	AddChild(_paintToColor);
	
	// charger les couleurs
	index = 0;
	while(_prefFile->FindInt32("color",index,(int32 *)&color)==B_OK)
	{
		UintToRGB(color,RGBColor);
		_colorList->AddItem(new ColorListItem(RGBColor));	
		index++;
	}
}

/**** attachement a la fenetre ****/
void PaintToColorPrefs::AttachedToWindow()
{
	// attachement standard
	VPreference::AttachedToWindow();
	
	// definir les destinataires des messages
	_colorList->SetTarget(this);
	_colorChoice->SetTarget(this);
	_colorDel->SetTarget(this);
	_colorAdd->SetTarget(this);
}

/**** gestion des messages ****/
void PaintToColorPrefs::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
	// selection d'une couleur
	case SELECT_PAINT_COLOR:
		{
			if(_colorList->CurrentSelection()>=0)
			{
				_edited = (ColorListItem *)(_colorList->ItemAt(_colorList->CurrentSelection()));
				_colorChoice->SetValue(_edited->Color());
			}
		}
		break;
	// changer une couleur
	case DEFINE_PAINT_COLOR:
		{
			if(_edited!=NULL)
			{
				_edited->SetColor(_colorChoice->ValueAsColor());
				_colorList->InvalidateItem(_colorList->IndexOf(_edited));
			}
		}
		break;
	// effacer une couleur
	case DELCOLOR_PAINT_COLOR:
		{
			if(_colorList->CurrentSelection()>=0)
			{
				delete (ColorListItem *)(_colorList->RemoveItem(_colorList->CurrentSelection()));
				_edited = NULL;
				_colorChoice->SetValue(0);
			}
		}
		break;
	// ajouter une nouvelle couleur
	case ADDCOLOR_PAINT_COLOR:
		_colorList->AddItem(new ColorListItem(_colorChoice->ValueAsColor()));
		_colorList->Select(_colorList->CountItems()-1);
		break;
	default:
		VPreference::MessageReceived(message);
	}
}

/**** sauver les preferences ****/
void PaintToColorPrefs::SavePrefs()
{
	ColorListItem		*item = NULL;
	int32				nbColor;
	rgb_color			RGBColor;
	uint32				color;

	// vider les prefs
	_prefFile->Load();
	
	// ajouter les couleurs
	nbColor = _colorList->CountItems();
	for(int32 index=0;index<nbColor;index++)
	{
		item = (ColorListItem *)(_colorList->ItemAt(index));
		if(item!=NULL)
		{
			RGBColor = item->Color();
			RGBToUint(RGBColor,color);
			_prefFile->AddInt32("color",color);
		}
	}
	
	// sauver
	_prefFile->Save();
}

// =============================
// = Transformation de couleur =
// =============================

/**** transormer un uint32 en rgb_color ****/
void PaintToColorPrefs::UintToRGB(uint32 &color,rgb_color &rgbColor)
{
	 rgbColor.red = ((uint8 *)(&color))[3];
	 rgbColor.green = ((uint8 *)(&color))[2];
	 rgbColor.blue = ((uint8 *)(&color))[1];
	 rgbColor.alpha = ((uint8 *)(&color))[0];
}

/**** transormer un rgb_color en uint32 ****/
void PaintToColorPrefs::RGBToUint(rgb_color &rgbColor,uint32 &color)
{
	 ((uint8 *)(&color))[3] = rgbColor.red;
	 ((uint8 *)(&color))[2] = rgbColor.green;
	 ((uint8 *)(&color))[1] = rgbColor.blue;
	 ((uint8 *)(&color))[0] = rgbColor.alpha;
}


// ==================================
// = item qui represente la couleur =
// ==================================

/**** constructeur ****/
ColorListItem::ColorListItem(rgb_color color)
: BListItem()
{
	// definir la couleur
	SetColor(color);
}

/**** dessin de l'item ****/
void ColorListItem::DrawItem(BView *owner,BRect bounds,bool complete)
{
	BRect		ItemFrame;
		
	// cadre
	ItemFrame.left = bounds.left + 3;
	ItemFrame.top = bounds.top + 1;
	ItemFrame.bottom = bounds.bottom - 1;
	ItemFrame.right = ItemFrame.bottom - ItemFrame.top + 3;

	// selection ou pas
	if(IsSelected())
		owner->SetHighColor(200,200,255,255);
	else
		owner->SetHighColor(255,255,255,255);	
	owner->SetLowColor(owner->HighColor());	
	owner->FillRect(bounds);
	
	// le cadre
	owner->SetHighColor(220,220,220,255);
	owner->StrokeLine(BPoint(ItemFrame.left,ItemFrame.top),BPoint(ItemFrame.right,ItemFrame.top));
	owner->StrokeLine(BPoint(ItemFrame.left,ItemFrame.top),BPoint(ItemFrame.left,ItemFrame.bottom));
	owner->SetHighColor(110,110,110,255);
	owner->StrokeLine(BPoint(ItemFrame.right,ItemFrame.top),BPoint(ItemFrame.right,ItemFrame.bottom));
	owner->StrokeLine(BPoint(ItemFrame.left,ItemFrame.bottom),BPoint(ItemFrame.right,ItemFrame.bottom));

	// le rectangle de couleur
	ItemFrame.InsetBy(1,1);
	owner->SetHighColor(_color);
	owner->FillRect(ItemFrame);

	// label
	owner->SetHighColor(BLACK_COLOR);
	owner->DrawString(_labelRGB.String(),BPoint(bounds.left+20, bounds.bottom - 2));
	owner->DrawString(_labelUint.String(),BPoint(bounds.left+191, bounds.bottom - 2));
}

/**** changer la couleur ****/
void ColorListItem::SetColor(rgb_color value)
{
	uint32		uintColor;
	
	// initialiser
	// on force l'alpha a 255
	_color = value;
	_color.alpha = 255;
	
	// le label
	_labelRGB = "rgb_color : r:";
	_labelRGB << (int32)_color.red << " g:" << (int32)_color.green;
	_labelRGB << " b:" << (int32)_color.blue << " a:" << (int32)_color.alpha;

	// convertir
	((uint8 *)(&uintColor))[3] = _color.red;
	((uint8 *)(&uintColor))[2] = _color.green;
	((uint8 *)(&uintColor))[1] = _color.blue;
	((uint8 *)(&uintColor))[0] = _color.alpha;

	_labelUint = "uint32 : ";
	_labelUint << uintColor;
}

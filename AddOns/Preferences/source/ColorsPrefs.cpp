// ========================
// = Gestion des couleurs =
// ========================
#include "ColorsPrefs.h"
#include "MetosMessageDefs.h"
#include "MetosGlobalDefs.h"
#include "VPreferenceFile.h"
#include "VLanguage.h"

#include <interface/Box.h>
#include <interface/ScrollView.h>
#include <interface/ListView.h>
#include <interface/ColorControl.h>

/**** constructeur ****/
ColorsPrefs::ColorsPrefs(BRect frame)
: VPreference(frame,"Colors",METOS_GROUP)
{
	BScrollView			*supportScroll = NULL;
	BRect				bounds = Bounds();
	rgb_color			RGBColor;

	// initialiser
	_edited = NULL;

	// decallage a cause du titre
	bounds.top += VPREFERENCE_TITLE_HIGH;
	bounds.InsetBy(7,7);

	// groupe pincipale
	_paintToColor = new BBox(bounds,"change-color");
	_paintToColor->SetLabel(_language->GetTranslationFor("COLORS_CHANGE"));
	
	bounds.OffsetTo(0,0);
	bounds.InsetBy(12,20);
	bounds.bottom -= 50;
	bounds.right -= B_V_SCROLL_BAR_WIDTH;
	// liste des couleurs
	_colorList = new BListView(bounds,"color-list");
	_colorList->SetSelectionMessage(new BMessage(SELECT_METOS_COLOR));
	supportScroll = new BScrollView("support-scroll",_colorList,B_FOLLOW_LEFT | B_FOLLOW_TOP,0,false,true);

	// choix de la couleur
	_colorChoice = new BColorControl(BPoint(bounds.left,bounds.bottom+7),B_CELLS_32x8,4.0,"color-choice",new BMessage(DEFINE_METOS_COLOR));

	_paintToColor->AddChild(supportScroll);
	_paintToColor->AddChild(_colorChoice);
	AddChild(_paintToColor);
	
	// trouver les variables avec "color" dans le nom et de type INT32
	//BJF: added intermediate step (use temp rgb_color)
	rgb_color rgbtemp1 = {0,255,0,100};
	RGBColor = g_ColorsPrefs->GetRGBColor(rgbtemp1,"selction-color");
	_colorList->AddItem(new ColorListItem(RGBColor,"selction-color"));

	rgb_color rgbtemp2 = {0,155,0,100};
	RGBColor = g_ColorsPrefs->GetRGBColor(rgbtemp2,"selction-back-color");
	_colorList->AddItem(new ColorListItem(RGBColor,"selction-back-color"));	

	rgb_color rgbtemp3 = {200,200,255,255};
	RGBColor = g_ColorsPrefs->GetRGBColor(rgbtemp3,"list-color");
	_colorList->AddItem(new ColorListItem(RGBColor,"list-color"));

	rgb_color rgbtemp4 = {230,230,230,255};
	RGBColor = g_ColorsPrefs->GetRGBColor(rgbtemp4,"unAvailable-color");
	_colorList->AddItem(new ColorListItem(RGBColor,"unAvailable-color"));

	rgb_color rgbtemp5 = {230,0,0,255};
	RGBColor = g_ColorsPrefs->GetRGBColor(rgbtemp5,"unAvailable-text-color");
	_colorList->AddItem(new ColorListItem(RGBColor,"unAvailable-text-color"));	

	rgb_color rgbtemp6 = VIEW_COLOR;
	RGBColor = g_ColorsPrefs->GetRGBColor(rgbtemp6,"undo-item-color");
	_colorList->AddItem(new ColorListItem(RGBColor,"undo-item-color"));
}

/**** destructeur ****/
ColorsPrefs::~ColorsPrefs()
{
	int32	nbItems;
	
	// liberer le memoire prise par chaque items
	nbItems = _colorList->CountItems();
	for(int32 i=0;i<nbItems;i++)
		delete (ColorListItem *)(_colorList->RemoveItem((int32)0));
}


/**** attachement a la fenetre ****/
void ColorsPrefs::AttachedToWindow()
{
	// attachement standard
	VPreference::AttachedToWindow();
	
	// definir les destinataires des messages
	_colorList->SetTarget(this);
	_colorChoice->SetTarget(this);
}

/**** gestion des messages ****/
void ColorsPrefs::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
	// selection d'une couleur
	case SELECT_METOS_COLOR:
		{
			if(_colorList->CurrentSelection()>=0)
			{
				_edited = (ColorListItem *)(_colorList->ItemAt(_colorList->CurrentSelection()));
				_colorChoice->SetValue(_edited->Color());
			}
		}
		break;
	// changer une couleur
	case DEFINE_METOS_COLOR:
		{
			if(_edited!=NULL)
			{
				_edited->SetColor(_colorChoice->ValueAsColor());
				_colorList->InvalidateItem(_colorList->IndexOf(_edited));
			}
		}
		break;
	default:
		VPreference::MessageReceived(message);
	}
}

/**** sauver les preferences ****/
void ColorsPrefs::SavePrefs()
{
	ColorListItem		*item = NULL;
	int32				nbColor;

	// relire avant
	g_ColorsPrefs->Load();
	
	// ajouter les couleurs
	nbColor = _colorList->CountItems();
	for(int32 index=0;index<nbColor;index++)
	{
		item = (ColorListItem *)(_colorList->ItemAt(index));
		if(item!=NULL)
			g_ColorsPrefs->SetRGBColor(item->Color(),(item->Name()).String());
	}
	
	// sauver
	g_ColorsPrefs->Save();
}

// =============================
// = Transformation de couleur =
// =============================

/**** transormer un uint32 en rgb_color ****/
void ColorsPrefs::UintToRGB(uint32 &color,rgb_color &rgbColor)
{
	 rgbColor.red = ((uint8 *)(&color))[3];
	 rgbColor.green = ((uint8 *)(&color))[2];
	 rgbColor.blue = ((uint8 *)(&color))[1];
	 rgbColor.alpha = ((uint8 *)(&color))[0];
}

/**** transormer un rgb_color en uint32 ****/
void ColorsPrefs::RGBToUint(rgb_color &rgbColor,uint32 &color)
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
ColorListItem::ColorListItem(rgb_color color,const char *name)
: BListItem()
{
	// le nom
	_name = name;
	
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
	owner->DrawString(_name.String(),BPoint(bounds.left+20, bounds.bottom - 2));
	owner->DrawString(_labelRGB.String(),BPoint(bounds.left+161, bounds.bottom - 2));
}

/**** changer la couleur ****/
void ColorListItem::SetColor(rgb_color value)
{
	// initialiser
	// on force l'alpha a 255
	_color = value;
	_color.alpha = 255;
	
	// le label
	_labelRGB = "r:";
	_labelRGB << (int32)_color.red << " g:" << (int32)_color.green;
	_labelRGB << " b:" << (int32)_color.blue << " a:" << (int32)_color.alpha;
}

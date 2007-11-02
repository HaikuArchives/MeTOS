/*****************************************/
/* Addon de la proprietes _color de BeOS */
/*****************************************/
#include "_color.h"
#include <interface/InterfaceDefs.h>
#include <interface/ColorControl.h>
#include <storage/Entry.h>
#include <stdio.h>

extern "C" __declspec(dllexport) VPropertie *instantiate_VPropertie();
extern "C" __declspec(dllexport) status_t	get_Managed_Name(BMessage *archive);

/**** constructeur ****/
_Color::_Color(entry_ref *entry)
: VPropertie("_color",entry)
{
	// noms gerés
	BMessage	*managedName = ManagedNames();
	
	// ne pas oublier de renseigner ces valeurs sinon l'addon ne pourras pas etre utilisée
	//BJF: added intermediate step (use temp variable for rgb_color)
	rgb_color rgbtemp = {0,0,0,255};	
	_rgbColor = rgbtemp;
	
	// ajouter les noms gerés
	managedName->AddString(VPROPERTIE_ALTERN_NAME,"_color");
	managedName->AddString(VPROPERTIE_ALTERN_NAME,"_bcolor");

	// le controle d'edition
	_colorControl = new BColorControl(BPoint(0,0),B_CELLS_32x8,8,"color-control");
	_controlSize = _colorControl->Bounds();
}

/**** destructeur ****/
_Color::~_Color()
{
	// liberer le control color
	delete _colorControl;
}

/**** function de clonage d'une propriete ****/
VPropertie* _Color::InstantiateVPropertie(entry_ref *entry)
{
	// creer le MObject apres MeTOS va appeler la methode CreateObject() pour reelement
	// creer l'objet graphique (fenetre,vue etc...)
	return (new _Color(entry));
}

/**** gerer l'affichage ****/
void _Color::Draw(BView *owner,BRect bounds,bool complete)
{
	BString		color;
	rgb_color	highColor;
	
	// sauver la couleur de fond
	highColor = owner->HighColor();
	
	// construire la chaine
	// en verite c'est stocké ainsi : ABGR
	color << " R: ";
	color << (int32)(_rgbColor.red);
	color << " G: ";
	color << (int32)(_rgbColor.green);
	color << " B: ";
	color << (int32)(_rgbColor.blue);
	color << " A: ";
	color << (int32)(_rgbColor.alpha);

	// dessiner le rectangle de couleur et son cadre
	owner->SetHighColor(tint_color(ui_color(B_MENU_BACKGROUND_COLOR),B_DARKEN_2_TINT));
	owner->StrokeLine(BPoint(bounds.left+4,bounds.top+4),BPoint(bounds.left+17,bounds.top+4));	
	owner->StrokeLine(BPoint(bounds.left+4,bounds.top+4),BPoint(bounds.left+4,bounds.top+17));	
	owner->SetHighColor(tint_color(ui_color(B_MENU_BACKGROUND_COLOR),B_LIGHTEN_2_TINT));
	owner->StrokeLine(BPoint(bounds.left+4,bounds.top+17),BPoint(bounds.left+17,bounds.top+17));	
	owner->StrokeLine(BPoint(bounds.left+17,bounds.top+4),BPoint(bounds.left+17,bounds.top+17));	
	
	owner->SetHighColor(_rgbColor);
	owner->FillRect(BRect(bounds.left+5,bounds.top+5,bounds.left+16,bounds.top+16));

	// restaurer la couleur de fond
	owner->SetHighColor(highColor);
	
	// apres on va l'afficher
	owner->SetLowColor(owner->HighColor());
	owner->SetHighColor(0,0,0);
	owner->DrawString(color.String(),BPoint(bounds.left+21,bounds.top+15));	
}

/**** recuperer la valeur qui nous interesse ****/
void _Color::SetProperties(BMessage *properties)
{
	uint32	color;
	
	if(properties!=NULL)
	{
		// find properties
		properties->FindInt32(PropertiesName(),Index(),(int32 *)&color);
		
		// fixer la couleur
		SetRGBColor(color);
	}
}

/**** renvoyer la bonne valeur ****/
void _Color::Properties(BMessage *objectProperties)
{
	uint32	color;

	// recuperer dans un uint32 le rgb_color
	GetUINTColor(color);
	
	// redonner la propriete
	objectProperties->AddInt32(PropertiesName(),color);
}

/**** selection l'item pour editer la donnée ****/
void _Color::SelectItem(BView *owner,BRect bounds)
{
	// do editing here
	owner->AddChild(_colorControl);
	// background of color Control
	_colorControl->SetViewColor(_itemColor);
	_colorControl->SetLowColor(_itemColor);
	// background of edits of color Control
	_colorControl->ChildAt(0)->SetViewColor(_itemColor);
	_colorControl->ChildAt(0)->SetLowColor(_itemColor);
	_colorControl->ChildAt(1)->SetViewColor(_itemColor);
	_colorControl->ChildAt(1)->SetLowColor(_itemColor);
	_colorControl->ChildAt(2)->SetViewColor(_itemColor);
	_colorControl->ChildAt(2)->SetLowColor(_itemColor);
	// position
	_colorControl->SetValue(_rgbColor);
	_colorControl->MoveTo(bounds.left+3,bounds.bottom);
	SetHeight(bounds.Height() + _controlSize.Height() + 5);
}

/**** deselection l'item ****/
void _Color::UnSelectItem(BView *owner,BRect bounds)
{
	rgb_color	newColor;
	
	newColor = _colorControl->ValueAsColor();
	SetChange(ColorChanged(_rgbColor,newColor));
	_rgbColor = newColor;
	SetHeight(bounds.Height() - _controlSize.Height() - 5);
	owner->RemoveChild(_colorControl);
}

/**** recuperer la valeur sour forme de chaine ****/
void _Color::ValueAsString(BMessage *properties,BString &value,int32 index)
{
	int32		color;
	
	// entourer la chaine de "
	if(properties->FindInt32(PropertiesName(),index,&color)==B_OK)
	{
		value << "(rgb_color){";
		value << (int32)(((uint8 *)(&color))[3]) << ",";
		value << (int32)(((uint8 *)(&color))[2]) << ",";
		value << (int32)(((uint8 *)(&color))[1]) << ",";
		value << (int32)(((uint8 *)(&color))[0]) << "}";
	}
}

/**** transormer un uint32 en rgb_color ****/
void _Color::SetRGBColor(uint32 &color)
{
	 _rgbColor.red = ((uint8 *)(&color))[3];
	 _rgbColor.green = ((uint8 *)(&color))[2];
	 _rgbColor.blue = ((uint8 *)(&color))[1];
	 _rgbColor.alpha = ((uint8 *)(&color))[0];
}

/**** transormer un rgb_color en uint32 ****/
void _Color::GetUINTColor(uint32 &color)
{
	 ((uint8 *)(&color))[3] = _rgbColor.red;
	 ((uint8 *)(&color))[2] = _rgbColor.green;
	 ((uint8 *)(&color))[1] = _rgbColor.blue;
	 ((uint8 *)(&color))[0] = _rgbColor.alpha;
}

/**** tester si les couleurs sont differentes ****/
bool _Color::ColorChanged(rgb_color &color_1,rgb_color &color_2)
{
	bool result;
	
	result = (color_1.red != color_2.red);
	result |= (color_1.green != color_2.green);
	result |= (color_1.blue != color_2.blue);
	result |= (color_1.alpha != color_2.alpha);

	return result;
}

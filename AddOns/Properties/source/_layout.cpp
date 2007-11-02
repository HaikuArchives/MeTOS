/****************************************/
/* Addon de la proprietes _feel de BeOS */
/****************************************/
#include "_layout.h"
#include <interface/MenuField.h>
#include <interface/PopUpMenu.h>
#include <interface/MenuItem.h>
#include <interface/MenuBar.h>

/**** constructeur ****/
_Layout::_Layout(entry_ref *entry)
: VPropertie("_layout",entry)
{
	// noms gerés
	BMessage	*managedName = ManagedNames();
	BPopUpMenu	*menu = NULL;
	
	// ne pas oublier de renseigner ces valeurs sinon l'addon ne pourras pas etre utilisée
	_layout = B_CELLS_32x8;

	// ajouter les noms gerés
	managedName->AddString(VPROPERTIE_ALTERN_NAME,"_layout");

	// creer le menu field pour le selection
	menu = new BPopUpMenu("MenuField");
	menu->AddItem(new BMenuItem("B_CELLS_4x64",NULL));
	menu->AddItem(new BMenuItem("B_CELLS_8x32",NULL));
	menu->AddItem(new BMenuItem("B_CELLS_16x16",NULL));
	menu->AddItem(new BMenuItem("B_CELLS_32x8",NULL));
	menu->AddItem(new BMenuItem("B_CELLS_64x4",NULL));
	_layoutMenuField = new BMenuField(BRect(0,0,400,25),"menu-field-layout","",menu);
	_layoutMenuField->SetDivider(0);
}

/**** destructeur ****/
_Layout::~_Layout()
{
	// liberer la memoire
	delete _layoutMenuField;
}

/**** function de clonage d'une propriete ****/
VPropertie* _Layout::InstantiateVPropertie(entry_ref *entry)
{
	// creer le MObject apres MeTOS va appeler la methode CreateObject() pour reelement
	// creer l'objet graphique (fenetre,vue etc...)
	return (new _Layout(entry));
}

/**** gerer l'affichage ****/
void _Layout::Draw(BView *owner,BRect bounds,bool complete)
{
	BString		colorControlLayout;

	// recuperer la bonne chaine
	switch(_layout)
	{
	case B_CELLS_4x64:
		colorControlLayout = "B_CELLS_4x64";
		break;
	case B_CELLS_8x32:
		colorControlLayout = "B_CELLS_8x32";
		break;
	case B_CELLS_16x16:
		colorControlLayout = "B_CELLS_16x16";
		break;
	case B_CELLS_32x8:
		colorControlLayout = "B_CELLS_32x8";
		break;
	case B_CELLS_64x4:
		colorControlLayout = "B_CELLS_64x4";
		break;
	}

	// apres on va l'afficher
	owner->SetLowColor(owner->HighColor());
	owner->SetHighColor(0,0,0);
	owner->DrawString(colorControlLayout.String(),BPoint(bounds.left+5,bounds.top+15));	
}

/**** recuperer la valeur qui nous interesse ****/
void _Layout::SetProperties(BMessage *properties)
{
	// verifier que la proprietes existe dans l'item
	if(properties!=NULL)
		properties->FindInt32(PropertiesName(),Index(),(int32 *)&_layout);
}

/**** renvoyer la bonne valeur ****/
void _Layout::Properties(BMessage *objectProperties)
{
	objectProperties->AddInt32(PropertiesName(),(int32)_layout);
}

/**** selection l'item pour editer la donnée ****/
void _Layout::SelectItem(BView *owner,BRect bounds)
{
	int32	index = 0;
	
	// selectionner le bon item
	switch(_layout)
	{
	case B_CELLS_4x64:
		index = 0;
		break;
	case B_CELLS_8x32:
		index = 1;
		break;
	case B_CELLS_16x16:
		index = 2;
		break;
	case B_CELLS_32x8:
		index = 3;
		break;
	case B_CELLS_64x4:
		index = 4;
		break;
	}
	_layoutMenuField->Menu()->ItemAt(index)->SetMarked(true);

	// ajouter a la vue
	owner->AddChild(_layoutMenuField);
	_layoutMenuField->MoveTo(bounds.left+1,bounds.top);
	_layoutMenuField->MakeFocus(true);
}

/**** deselection l'item ****/
void _Layout::UnSelectItem(BView *owner,BRect bounds)
{
	BMenu		*menu = NULL;
	int32		index;
	int32		newColorControlLayout = B_CELLS_32x8;
	
	
	menu = _layoutMenuField->Menu();
	index = menu->IndexOf(menu->FindMarked());
	
	// recuperer la constante en fonction de l'index selectionné
	switch(index)
	{
	case 0:
		newColorControlLayout = B_CELLS_4x64;
		break;
	case 1:
		newColorControlLayout = B_CELLS_8x32;
		break;
	case 2:
		newColorControlLayout = B_CELLS_16x16;
		break;
	case 3:
		newColorControlLayout = B_CELLS_32x8;
		break;
	case 4:
		newColorControlLayout = B_CELLS_64x4;
		break;
	}
	
	// valider le changement
	SetChange(newColorControlLayout!=_layout);
	_layout = (color_control_layout)newColorControlLayout;
	owner->RemoveChild(_layoutMenuField);
}

/**** recuperer la valeur sour forme de chaine ****/
void _Layout::ValueAsString(BMessage *properties,BString &value,int32 index)
{
	int32		colorControlLayout = 0;
	
	if(properties->FindInt32(PropertiesName(),index,(int32 *)&colorControlLayout)==B_OK)
	{
		switch(colorControlLayout)
		{
		case B_CELLS_4x64:
			value = " B_CELLS_4x64";
			break;
		case B_CELLS_8x32:
			value = " B_CELLS_8x32";
			break;
		case B_CELLS_16x16:
			value = " B_CELLS_16x16";
			break;
		case B_CELLS_32x8:
			value = " B_CELLS_32x8";
			break;
		case B_CELLS_64x4:
			value = " B_CELLS_64x4";
			break;
		}
	}
}

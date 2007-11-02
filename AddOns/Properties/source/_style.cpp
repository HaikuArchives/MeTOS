/*****************************************/
/* Addon de la proprietes _style de BeOS */
/*****************************************/
#include "_style.h"
#include <interface/MenuField.h>
#include <interface/PopUpMenu.h>
#include <interface/MenuItem.h>

/**** constructeur ****/
_Style::_Style(entry_ref *entry)
: VPropertie("_style",entry)
{
	// noms gerés
	BMessage	*managedName = ManagedNames();
	BPopUpMenu	*menu = NULL;
	
	// ne pas oublier de renseigner ces valeurs sinon l'addon ne pourras pas etre utilisée
	_style = B_FANCY_BORDER;

	// ajouter les noms gerés
	managedName->AddString(VPROPERTIE_ALTERN_NAME,"_style");

	// creer le menu field pour le selection
	menu = new BPopUpMenu("MEnuField");
	menu->AddItem(new BMenuItem("B_PLAIN_BORDER",NULL));
	menu->AddItem(new BMenuItem("B_FANCY_BORDER",NULL));
	menu->AddItem(new BMenuItem("B_NO_BORDER",NULL));
	_styleMenuField = new BMenuField(BRect(0,0,400,25),"menu-field-style","",menu);
	_styleMenuField->SetDivider(0);
}

/**** destructeur ****/
_Style::~_Style()
{
	delete _styleMenuField;
}

/**** function de clonage d'une propriete ****/
VPropertie* _Style::InstantiateVPropertie(entry_ref *entry)
{
	// creer le MObject apres MeTOS va appeler la methode CreateObject() pour reelement
	// creer l'objet graphique (fenetre,vue etc...)
	return (new _Style(entry));
}

/**** gerer l'affichage ****/
void _Style::Draw(BView *owner,BRect bounds,bool complete)
{
	BString		style;

	// recuperer la bonne chaine
	switch(_style)
	{
	case B_PLAIN_BORDER:
		style = "B_PLAIN_BORDER";
		break;
	case B_FANCY_BORDER:
		style = "B_FANCY_BORDER";
		break;
	case B_NO_BORDER:
		style = "B_NO_BORDER";
		break;
	}

	// apres on va l'afficher
	owner->SetLowColor(owner->HighColor());
	owner->SetHighColor(0,0,0);
	owner->DrawString(style.String(),BPoint(bounds.left+5,bounds.top+15));	
}

/**** recuperer la valeur qui nous interesse ****/
void _Style::SetProperties(BMessage *properties)
{
	// verifier que la proprietes existe dans l'item
	if(properties!=NULL)
		properties->FindInt32(PropertiesName(),Index(),(int32 *)&_style);
}

/**** renvoyer la bonne valeur ****/
void _Style::Properties(BMessage *objectProperties)
{
	BString		propertiesName;
	
	objectProperties->AddInt32(PropertiesName(),(int32)_style);
}

/**** selection l'item pour editer la donnée ****/
void _Style::SelectItem(BView *owner,BRect bounds)
{
	int32	index = 0;
	
	// selectionner le bon item
	switch(_style)
	{
	case B_PLAIN_BORDER:
		index = 0;
		break;
	case B_FANCY_BORDER:
		index = 1;
		break;
	case B_NO_BORDER:
		index = 2;
		break;
	}
	_styleMenuField->Menu()->ItemAt(index)->SetMarked(true);

	// ajouter a la vue
	owner->AddChild(_styleMenuField);
	_styleMenuField->MoveTo(bounds.left+1,bounds.top);
	_styleMenuField->MakeFocus(true);
}

/**** deselection l'item ****/
void _Style::UnSelectItem(BView *owner,BRect bounds)
{
	BMenu		*menu = NULL;
	int32		index;
	int32		newStyle = B_FANCY_BORDER;
	
	
	menu = _styleMenuField->Menu();
	index = menu->IndexOf(menu->FindMarked());

	// trouver la constante selectionnée
	switch(index)
	{
	case 0:
		newStyle = B_PLAIN_BORDER;
		break;
	case 1:
		newStyle =  B_FANCY_BORDER;
		break;
	case 2:
		newStyle =  B_NO_BORDER;
		break;
	}

	// valider le changement
	SetChange(newStyle!=_style);
	_style = (border_style)newStyle;
	owner->RemoveChild(_styleMenuField);
}

/**** recuperer la valeur sour forme de chaine ****/
void _Style::ValueAsString(BMessage *properties,BString &value,int32 index)
{
	int32		style = 0;
	
	if(properties->FindInt32(PropertiesName(),index,(int32 *)&style)==B_OK)
	{
		switch(style)
		{
		case B_PLAIN_BORDER:
			value = " B_PLAIN_BORDER";
			break;
		case B_FANCY_BORDER:
			value = " B_FANCY_BORDER";
			break;
		case B_NO_BORDER:
			value = " B_NO_BORDER";
			break;
		}
	}
}

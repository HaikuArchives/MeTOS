/***************************************/
/* Addon de la proprietes _type_binary */
/***************************************/
#include "_type_binary.h"
#include <interface/MenuField.h>
#include <interface/PopUpMenu.h>
#include <interface/MenuItem.h>

/**** constructeur ****/
_TypeBinary::_TypeBinary(entry_ref *entry)
: VPropertie("_type_binary",entry)
{
	// noms gerés
	BMessage	*managedName = ManagedNames();
	BPopUpMenu	*menu = NULL;
	
	// ne pas oublier de renseigner ces valeurs sinon l'addon ne pourras pas etre utilisée
	_typeBinary = 0;	// application

	// ajouter les noms gerés
	managedName->AddString(VPROPERTIE_ALTERN_NAME,"_type_binary");

	// creer le menu field pour le selection
	menu = new BPopUpMenu("MenuField");
	menu->AddItem(new BMenuItem("Application",NULL));
	menu->AddItem(new BMenuItem("Shared",NULL));
	menu->AddItem(new BMenuItem("Static",NULL));
	menu->AddItem(new BMenuItem("Kernel Driver",NULL));
	_typeBinaryMenuField = new BMenuField(BRect(0,0,400,25),"menu-field-type-binary","",menu);
	_typeBinaryMenuField->SetDivider(0);
}

/**** destructeur ****/
_TypeBinary::~_TypeBinary()
{
	// liberer la memoire
	delete _typeBinaryMenuField;
}

/**** function de clonage d'une propriete ****/
VPropertie* _TypeBinary::InstantiateVPropertie(entry_ref *entry)
{
	// creer le MObject apres MeTOS va appeler la methode CreateObject() pour reelement
	// creer l'objet graphique (fenetre,vue etc...)
	return (new _TypeBinary(entry));
}

/**** gerer l'affichage ****/
void _TypeBinary::Draw(BView *owner,BRect bounds,bool complete)
{
	BString		typeBinary;

	// recuperer la bonne chaine
	switch(_typeBinary)
	{
	case 0:
		typeBinary = "Application";
		break;
	case 1:
		typeBinary = "Shared";
		break;
	case 2:
		typeBinary = "Static";
		break;
	case 3:
		typeBinary = "Kernel Driver";
	}

	// apres on va l'afficher
	owner->SetLowColor(owner->HighColor());
	owner->SetHighColor(0,0,0);
	owner->DrawString(typeBinary.String(),BPoint(bounds.left+5,bounds.top+15));	
}

/**** recuperer la valeur qui nous interesse ****/
void _TypeBinary::SetProperties(BMessage *properties)
{
	// verifier que la proprietes existe dans l'item
	if(properties!=NULL)
		properties->FindInt32(PropertiesName(),Index(),(int32 *)&_typeBinary);
}

/**** renvoyer la bonne valeur ****/
void _TypeBinary::Properties(BMessage *objectProperties)
{
	objectProperties->AddInt32(PropertiesName(),(int32)_typeBinary);
}

/**** selection l'item pour editer la donnée ****/
void _TypeBinary::SelectItem(BView *owner,BRect bounds)
{
	// selectionner le bon item
	_typeBinaryMenuField->Menu()->ItemAt(_typeBinary)->SetMarked(true);

	// ajouter a la vue
	owner->AddChild(_typeBinaryMenuField);
	_typeBinaryMenuField->MoveTo(bounds.left+1,bounds.top);
	_typeBinaryMenuField->MakeFocus(true);
}

/**** deselection l'item ****/
void _TypeBinary::UnSelectItem(BView *owner,BRect bounds)
{
	BMenu		*menu = NULL;
	int32		newtype = 0;
	
	// trouver la constante selectionnée
	menu = _typeBinaryMenuField->Menu();
	newtype = menu->IndexOf(menu->FindMarked());

	// valider le changement
	SetChange(newtype!=_typeBinary);
	_typeBinary = newtype;
	owner->RemoveChild(_typeBinaryMenuField);
}

/**** recuperer la valeur sour forme de chaine ****/
void _TypeBinary::ValueAsString(BMessage *properties,BString &value,int32 index)
{
	int32		typeBinary = 0;
	
	if(properties->FindInt32(PropertiesName(),index,(int32 *)&typeBinary)==B_OK)
	{
		switch(typeBinary)
		{
		case 0:
			value = "APP";
			break;
		case 1:
			value = "SHARED";
			break;
		case 2:
			value = "STATIC";
			break;
		case 3:
			value = "DRIVER";
			break;
		}
	}
}

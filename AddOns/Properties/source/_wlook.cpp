/****************************************/
/* Addon de la proprietes _look de BeOS */
/****************************************/
#include "_wlook.h"
#include <interface/MenuField.h>
#include <interface/PopUpMenu.h>
#include <interface/MenuItem.h>

/**** constructeur ****/
_Wlook::_Wlook(entry_ref *entry)
: VPropertie("_wlook",entry)
{
	// noms gerés
	BMessage	*managedName = ManagedNames();
	BPopUpMenu	*menu = NULL;
	
	// ne pas oublier de renseigner ces valeurs sinon l'addon ne pourras pas etre utilisée
	_wlook = B_DOCUMENT_WINDOW_LOOK;

	// ajouter les noms gerés
	managedName->AddString(VPROPERTIE_ALTERN_NAME,"_wlook");

	// creer le menu field pour le selection
	menu = new BPopUpMenu("MenuField");
	menu->AddItem(new BMenuItem("B_DOCUMENT_WINDOW_LOOK",NULL));
	menu->AddItem(new BMenuItem("B_TITLED_WINDOW_LOOK",NULL));
	menu->AddItem(new BMenuItem("B_FLOATING_WINDOW_LOOK",NULL));
	menu->AddItem(new BMenuItem("B_MODAL_WINDOW_LOOK",NULL));
	menu->AddItem(new BMenuItem("B_BORDERED_WINDOW_LOOK",NULL));
	menu->AddItem(new BMenuItem("B_NO_BORDER_WINDOW_LOOK",NULL));
	_wlookMenuField = new BMenuField(BRect(0,0,400,25),"menu-field-look","",menu);
	_wlookMenuField->SetDivider(0);
}

/**** destructeur ****/
_Wlook::~_Wlook()
{
	// liberer la memoire
	delete _wlookMenuField;
}

/**** function de clonage d'une propriete ****/
VPropertie* _Wlook::InstantiateVPropertie(entry_ref *entry)
{
	// creer le MObject apres MeTOS va appeler la methode CreateObject() pour reelement
	// creer l'objet graphique (fenetre,vue etc...)
	return (new _Wlook(entry));
}

/**** gerer l'affichage ****/
void _Wlook::Draw(BView *owner,BRect bounds,bool complete)
{
	BString		look;

	// recuperer la bonne chaine
	switch(_wlook)
	{
	case B_DOCUMENT_WINDOW_LOOK:
		look = "B_DOCUMENT_WINDOW_LOOK";
		break;
	case B_TITLED_WINDOW_LOOK:
		look = "B_TITLED_WINDOW_LOOK";
		break;
	case B_FLOATING_WINDOW_LOOK:
		look = "B_FLOATING_WINDOW_LOOK";
		break;
	case B_MODAL_WINDOW_LOOK:
		look = "B_MODAL_WINDOW_LOOK";
		break;
	case B_BORDERED_WINDOW_LOOK:
		look = "B_BORDERED_WINDOW_LOOK";
		break;
	case B_NO_BORDER_WINDOW_LOOK:
		look = "B_NO_BORDER_WINDOW_LOOK";
		break;
	}

	// apres on va l'afficher
	owner->SetLowColor(owner->HighColor());
	owner->SetHighColor(0,0,0);
	owner->DrawString(look.String(),BPoint(bounds.left+5,bounds.top+15));	
}

/**** recuperer la valeur qui nous interesse ****/
void _Wlook::SetProperties(BMessage *properties)
{
	// verifier que la proprietes existe dans l'item
	if(properties!=NULL)
		properties->FindInt32(PropertiesName(),Index(),(int32 *)&_wlook);
}

/**** renvoyer la bonne valeur ****/
void _Wlook::Properties(BMessage *objectProperties)
{
	objectProperties->AddInt32(PropertiesName(),(int32)_wlook);
}

/**** selection l'item pour editer la donnée ****/
void _Wlook::SelectItem(BView *owner,BRect bounds)
{
	int32	index = 0;
	
	// selectionner le bon item
	switch(_wlook)
	{
	case B_DOCUMENT_WINDOW_LOOK:
		index = 0;
		break;
	case B_TITLED_WINDOW_LOOK:
		index = 1;
		break;
	case B_FLOATING_WINDOW_LOOK:
		index = 2;
		break;
	case B_MODAL_WINDOW_LOOK:
		index = 3;
		break;
	case B_BORDERED_WINDOW_LOOK:
		index = 4;
		break;
	case B_NO_BORDER_WINDOW_LOOK:
		index = 5;
		break;
	}
	_wlookMenuField->Menu()->ItemAt(index)->SetMarked(true);

	// ajouter a la vue
	owner->AddChild(_wlookMenuField);
	_wlookMenuField->MoveTo(bounds.left+1,bounds.top);
	_wlookMenuField->MakeFocus(true);
}

/**** deselection l'item ****/
void _Wlook::UnSelectItem(BView *owner,BRect bounds)
{
	BMenu		*menu = NULL;
	int32		index;
	int32		newWlook = B_DOCUMENT_WINDOW_LOOK;
	
	
	menu = _wlookMenuField->Menu();
	index = menu->IndexOf(menu->FindMarked());

	// trouver la constante selectionnée
	switch(index)
	{
	case 0:
		newWlook = B_DOCUMENT_WINDOW_LOOK;
		break;
	case 1:
		newWlook =  B_TITLED_WINDOW_LOOK;
		break;
	case 2:
		newWlook =  B_FLOATING_WINDOW_LOOK;
		break;
	case 3:
		newWlook =  B_MODAL_WINDOW_LOOK;
		break;
	case 4:
		newWlook =  B_BORDERED_WINDOW_LOOK;
		break;
	case 5:
		newWlook =  B_NO_BORDER_WINDOW_LOOK;
		break;
	}

	// valider le changement
	SetChange(newWlook!=_wlook);
	_wlook = (window_look)newWlook;
	owner->RemoveChild(_wlookMenuField);
}

/**** recuperer la valeur sour forme de chaine ****/
void _Wlook::ValueAsString(BMessage *properties,BString &value,int32 index)
{
	int32		look = 0;
	
	if(properties->FindInt32(PropertiesName(),index,(int32 *)&look)==B_OK)
	{
		switch(look)
		{
		case B_DOCUMENT_WINDOW_LOOK:
			value = " B_DOCUMENT_WINDOW_LOOK";
			break;
		case B_TITLED_WINDOW_LOOK:
			value = " B_TITLED_WINDOW_LOOK";
			break;
		case B_FLOATING_WINDOW_LOOK:
			value = " B_FLOATING_WINDOW_LOOK";
			break;
		case B_MODAL_WINDOW_LOOK:
			value = " B_MODAL_WINDOW_LOOK";
			break;
		case B_BORDERED_WINDOW_LOOK:
			value = " B_BORDERED_WINDOW_LOOK";
			break;
		case B_NO_BORDER_WINDOW_LOOK:
			value = " B_NO_BORDER_WINDOW_LOOK";
			break;
		}
	}
}

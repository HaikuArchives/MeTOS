/****************************************/
/* Addon de la proprietes _feel de BeOS */
/****************************************/
#include "_wfeel.h"
#include <interface/MenuField.h>
#include <interface/PopUpMenu.h>
#include <interface/MenuItem.h>
#include <interface/MenuBar.h>

/**** constructeur ****/
_Wfeel::_Wfeel(entry_ref *entry)
: VPropertie("_wfeel",entry)
{
	// noms gerés
	BMessage	*managedName = ManagedNames();
	BPopUpMenu	*menu = NULL;
	
	// ne pas oublier de renseigner ces valeurs sinon l'addon ne pourras pas etre utilisée
	_wfeel = B_NORMAL_WINDOW_FEEL;

	// ajouter les noms gerés
	managedName->AddString(VPROPERTIE_ALTERN_NAME,"_wfeel");

	// creer le menu field pour le selection
	menu = new BPopUpMenu("MenuField");
	menu->AddItem(new BMenuItem("B_NORMAL_WINDOW_FEEL",NULL));
	menu->AddItem(new BMenuItem("B_MODAL_SUBSET_WINDOW_FEEL",NULL));
	menu->AddItem(new BMenuItem("B_MODAL_APP_WINDOW_FEEL",NULL));
	menu->AddItem(new BMenuItem("B_MODAL_ALL_WINDOW_FEEL",NULL));
	menu->AddItem(new BMenuItem("B_FLOATING_SUBSET_WINDOW_FEEL",NULL));
	menu->AddItem(new BMenuItem("B_FLOATING_APP_WINDOW_FEEL",NULL));
	menu->AddItem(new BMenuItem("B_FLOATING_ALL_WINDOW_FEEL",NULL));
	_wfeelMenuField = new BMenuField(BRect(0,0,400,25),"menu-field-feel","",menu);
	_wfeelMenuField->SetDivider(0);
}

/**** destructeur ****/
_Wfeel::~_Wfeel()
{
	// liberer la memoire
	delete _wfeelMenuField;
}

/**** function de clonage d'une propriete ****/
VPropertie* _Wfeel::InstantiateVPropertie(entry_ref *entry)
{
	// creer le MObject apres MeTOS va appeler la methode CreateObject() pour reelement
	// creer l'objet graphique (fenetre,vue etc...)
	return (new _Wfeel(entry));
}

/**** gerer l'affichage ****/
void _Wfeel::Draw(BView *owner,BRect bounds,bool complete)
{
	BString		feel;

	// recuperer la bonne chaine
	switch(_wfeel)
	{
	// normal
	case B_NORMAL_WINDOW_FEEL:
		feel = "B_NORMAL_WINDOW_FEEL";
		break;
	// modal
	case B_MODAL_SUBSET_WINDOW_FEEL:
		feel = "B_MODAL_SUBSET_WINDOW_FEEL";
		break;
	case B_MODAL_APP_WINDOW_FEEL:
		feel = "B_MODAL_APP_WINDOW_FEEL";
		break;
	case B_MODAL_ALL_WINDOW_FEEL:
		feel = "B_MODAL_ALL_WINDOW_FEEL";
		break;
	// floating
	case B_FLOATING_SUBSET_WINDOW_FEEL:
		feel = "B_FLOATING_SUBSET_WINDOW_FEEL";
		break;
	case B_FLOATING_APP_WINDOW_FEEL:
		feel = "B_FLOATING_APP_WINDOW_FEEL";
		break;
	case B_FLOATING_ALL_WINDOW_FEEL:
		feel = "B_FLOATING_ALL_WINDOW_FEEL";
		break;
	}

	// apres on va l'afficher
	owner->SetLowColor(owner->HighColor());
	owner->SetHighColor(0,0,0);
	owner->DrawString(feel.String(),BPoint(bounds.left+5,bounds.top+15));	
}

/**** recuperer la valeur qui nous interesse ****/
void _Wfeel::SetProperties(BMessage *properties)
{
	// verifier que la proprietes existe dans l'item
	if(properties!=NULL)
		properties->FindInt32(PropertiesName(),Index(),(int32 *)&_wfeel);
}

/**** renvoyer la bonne valeur ****/
void _Wfeel::Properties(BMessage *objectProperties)
{
	objectProperties->AddInt32(PropertiesName(),(int32)_wfeel);
}

/**** selection l'item pour editer la donnée ****/
void _Wfeel::SelectItem(BView *owner,BRect bounds)
{
	int32	index = 0;
	
	// selectionner le bon item
	switch(_wfeel)
	{
	// normal
	case B_NORMAL_WINDOW_FEEL:
		index = 0;
		break;
	// modal
	case B_MODAL_SUBSET_WINDOW_FEEL:
		index = 1;
		break;
	case B_MODAL_APP_WINDOW_FEEL:
		index = 2;
		break;
	case B_MODAL_ALL_WINDOW_FEEL:
		index = 3;
		break;
	// floating
	case B_FLOATING_SUBSET_WINDOW_FEEL:
		index = 4;
		break;
	case B_FLOATING_APP_WINDOW_FEEL:
		index = 5;
		break;
	case B_FLOATING_ALL_WINDOW_FEEL:
		index = 6;
		break;
	}
	_wfeelMenuField->Menu()->ItemAt(index)->SetMarked(true);

	// ajouter a la vue
	owner->AddChild(_wfeelMenuField);
	_wfeelMenuField->MoveTo(bounds.left+1,bounds.top);
	_wfeelMenuField->MakeFocus(true);
}

/**** deselection l'item ****/
void _Wfeel::UnSelectItem(BView *owner,BRect bounds)
{
	BMenu		*menu = NULL;
	int32		index;
	int32		newWfeel = B_NORMAL_WINDOW_FEEL;
	
	
	menu = _wfeelMenuField->Menu();
	index = menu->IndexOf(menu->FindMarked());
	
	// recuperer la constante en fonction de l'index selectionné
	switch(index)
	{
	case 0:
		newWfeel = B_NORMAL_WINDOW_FEEL;
		break;
	case 1:
		newWfeel = B_MODAL_SUBSET_WINDOW_FEEL;
		break;
	case 2:
		newWfeel = B_MODAL_APP_WINDOW_FEEL;
		break;
	case 3:
		newWfeel = B_MODAL_ALL_WINDOW_FEEL;
		break;
	case 4:
		newWfeel = B_FLOATING_SUBSET_WINDOW_FEEL;
		break;
	case 5:
		newWfeel = B_FLOATING_APP_WINDOW_FEEL;
		break;
	case 6:
		newWfeel = B_FLOATING_ALL_WINDOW_FEEL;
		break;
	}
	
	// valider le changement
	SetChange(newWfeel!=_wfeel);
	_wfeel = (window_feel)newWfeel;
	owner->RemoveChild(_wfeelMenuField);
}

/**** recuperer la valeur sour forme de chaine ****/
void _Wfeel::ValueAsString(BMessage *properties,BString &value,int32 index)
{
	int32		feel = 0;
	
	if(properties->FindInt32(PropertiesName(),index,(int32 *)&feel)==B_OK)
	{
		switch(feel)
		{
		// normal
		case B_NORMAL_WINDOW_FEEL:
			value = " B_NORMAL_WINDOW_FEEL";
			break;
		// modal
		case B_MODAL_SUBSET_WINDOW_FEEL:
			value = " B_MODAL_SUBSET_WINDOW_FEEL";
			break;
		case B_MODAL_APP_WINDOW_FEEL:
			value = " B_MODAL_APP_WINDOW_FEEL";
			break;
		case B_MODAL_ALL_WINDOW_FEEL:
			value = " B_MODAL_ALL_WINDOW_FEEL";
			break;
		// floating
		case B_FLOATING_SUBSET_WINDOW_FEEL:
			value = " B_FLOATING_SUBSET_WINDOW_FEEL";
			break;
		case B_FLOATING_APP_WINDOW_FEEL:
			value = " B_FLOATING_APP_WINDOW_FEEL";
			break;
		case B_FLOATING_ALL_WINDOW_FEEL:
			value = " B_FLOATING_ALL_WINDOW_FEEL";
			break;
		}
	}
}

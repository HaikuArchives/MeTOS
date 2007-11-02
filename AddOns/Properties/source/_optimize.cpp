/************************************/
/* Addon de la proprietes _optimize */
/************************************/
#include "_optimize.h"
#include <interface/MenuField.h>
#include <interface/PopUpMenu.h>
#include <interface/MenuItem.h>

/**** constructeur ****/
_Optimize::_Optimize(entry_ref *entry)
: VPropertie("_optimize",entry)
{
	// noms gerés
	BMessage	*managedName = ManagedNames();
	BPopUpMenu	*menu = NULL;
	
	// ne pas oublier de renseigner ces valeurs sinon l'addon ne pourras pas etre utilisée
	_optimize = 0;	// application

	// ajouter les noms gerés
	managedName->AddString(VPROPERTIE_ALTERN_NAME,"_optimize");

	// creer le menu field pour le selection
	menu = new BPopUpMenu("MenuField");
	menu->AddItem(new BMenuItem("NONE",NULL));
	menu->AddItem(new BMenuItem("SOME",NULL));
	menu->AddItem(new BMenuItem("FULL",NULL));
	_optimizeMenuField = new BMenuField(BRect(0,0,400,25),"menu-field-optimize","",menu);
	_optimizeMenuField->SetDivider(0);
}

/**** destructeur ****/
_Optimize::~_Optimize()
{
	// liberer la memoire
	delete _optimizeMenuField;
}

/**** function de clonage d'une propriete ****/
VPropertie* _Optimize::InstantiateVPropertie(entry_ref *entry)
{
	// creer le MObject apres MeTOS va appeler la methode CreateObject() pour reelement
	// creer l'objet graphique (fenetre,vue etc...)
	return (new _Optimize(entry));
}

/**** gerer l'affichage ****/
void _Optimize::Draw(BView *owner,BRect bounds,bool complete)
{
	BString		optimize;

	// recuperer la bonne chaine
	switch(_optimize)
	{
	case 0:
		optimize = "NONE";
		break;
	case 1:
		optimize = "SOME";
		break;
	case 2:
		optimize = "FULL";
	}

	// apres on va l'afficher
	owner->SetLowColor(owner->HighColor());
	owner->SetHighColor(0,0,0);
	owner->DrawString(optimize.String(),BPoint(bounds.left+5,bounds.top+15));	
}

/**** recuperer la valeur qui nous interesse ****/
void _Optimize::SetProperties(BMessage *properties)
{
	// verifier que la proprietes existe dans l'item
	if(properties!=NULL)
		properties->FindInt32(PropertiesName(),Index(),(int32 *)&_optimize);
}

/**** renvoyer la bonne valeur ****/
void _Optimize::Properties(BMessage *objectProperties)
{
	objectProperties->AddInt32(PropertiesName(),(int32)_optimize);
}

/**** selection l'item pour editer la donnée ****/
void _Optimize::SelectItem(BView *owner,BRect bounds)
{
	// selectionner le bon item
	_optimizeMenuField->Menu()->ItemAt(_optimize)->SetMarked(true);

	// ajouter a la vue
	owner->AddChild(_optimizeMenuField);
	_optimizeMenuField->MoveTo(bounds.left+1,bounds.top);
	_optimizeMenuField->MakeFocus(true);
}

/**** deselection l'item ****/
void _Optimize::UnSelectItem(BView *owner,BRect bounds)
{
	BMenu		*menu = NULL;
	int32		newOptimize = 0;
	
	// trouver la constante selectionnée
	menu = _optimizeMenuField->Menu();
	newOptimize = menu->IndexOf(menu->FindMarked());

	// valider le changement
	SetChange(newOptimize!=_optimize);
	_optimize = newOptimize;
	owner->RemoveChild(_optimizeMenuField);
}

/**** recuperer la valeur sour forme de chaine ****/
void _Optimize::ValueAsString(BMessage *properties,BString &value,int32 index)
{
	int32		optimize = 0;
	
	if(properties->FindInt32(PropertiesName(),index,(int32 *)&optimize)==B_OK)
	{
		switch(optimize)
		{
		case 0:
			value = "NONE";
			break;
		case 1:
			value = "SOME";
			break;
		case 2:
			value = "FULL";
			break;
		}
	}
}

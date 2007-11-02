/***************************************/
/* Addon de la proprietes BOOL de BeOS */
/***************************************/
#include "BOOL.h"
#include <interface/CheckBox.h>

/**** constructeur ****/
BOOL::BOOL(entry_ref *entry)
: VPropertie("BOOL",entry)
{
	// noms gerés
	BMessage	*managedName = ManagedNames();

	// ne pas oublier de renseigner ces valeurs sinon l'addon ne pourras pas etre utilisée
	_boolean = false;

	// ajouter les noms gerés
	managedName->AddString(VPROPERTIE_ALTERN_NAME,"BOOL");

	// creer le control d'edition pour l'utiliser
	_checkBox = new BCheckBox(BRect(0,0,10,10),"boolean-check","boolean",NULL);
	
	// on a besoin de rafraichir la liste des objets
	SetNeedRefresh();
}

/**** destructeur ****/
BOOL::~BOOL()
{
	delete _checkBox;
}

/**** function de clonage d'une propriete ****/
VPropertie* BOOL::InstantiateVPropertie(entry_ref *entry)
{
	// creer le MObject apres MeTOS va appeler la methode CreateObject() pour reelement
	// creer l'objet graphique (fenetre,vue etc...)
	return (new BOOL(entry));
}

/**** gerer l'affichage ****/
void BOOL::Draw(BView *owner,BRect bounds,bool complete)
{
	// apres on va l'afficher
	owner->SetLowColor(owner->HighColor());
	owner->SetHighColor(0,0,0);
	if(_boolean)
		owner->DrawString("true",BPoint(bounds.left+5,bounds.top+15));	
	else
		owner->DrawString("false",BPoint(bounds.left+5,bounds.top+15));	
}

/**** recuperer la valeur qui nous interesse ****/
void BOOL::SetProperties(BMessage *properties)
{
	// verifier que la proprietes existe dans l'item
	if(properties!=NULL)
		properties->FindBool(PropertiesName(),Index(),&_boolean);
}

/**** renvoyer la bonne valeur ****/
void BOOL::Properties(BMessage *objectProperties)
{
	objectProperties->AddBool(PropertiesName(),_boolean);
}

/**** selection l'item pour editer la donnée ****/
void BOOL::SelectItem(BView *owner,BRect bounds)
{
	owner->AddChild(_checkBox);
	_checkBox->ResizeTo(bounds.Width()-6,bounds.Height()-5);
	_checkBox->MoveTo(bounds.left+3,bounds.top+2);
	_checkBox->SetViewColor(_itemColor);
	_checkBox->SetLowColor(_itemColor);
	
	// renseigner le controle avec la proprietes
	_checkBox->SetValue(_boolean);
}

/**** deselection l'item ****/
void BOOL::UnSelectItem(BView *owner,BRect bounds)
{
	bool	newBoolean;
	
	newBoolean = _checkBox->Value();
	SetChange(newBoolean!=_boolean);
	_boolean = newBoolean;
	owner->RemoveChild(_checkBox);
}

/**** recuperer la valeur sour forme de chaine ****/
void BOOL::ValueAsString(BMessage *properties,BString &value,int32 index)
{
	bool		boolean;
	
	// entourer la chaine de "
	if(properties->FindBool(PropertiesName(),index,&boolean)==B_OK)
		if(boolean)
			value << "true";
		else
			value << "false";
}

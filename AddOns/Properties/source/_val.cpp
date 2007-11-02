/***************************************/
/* Addon de la proprietes _Val de BeOS */
/***************************************/
#include "_val.h"
#include <interface/CheckBox.h>

/**** constructeur ****/
_Val::_Val(entry_ref *entry)
: VPropertie("_val",entry)
{
	// noms gerés
	BMessage	*managedName = ManagedNames();

	// ne pas oublier de renseigner ces valeurs sinon l'addon ne pourras pas etre utilisée
	_value = B_CONTROL_OFF;

	// ajouter les noms gerés
	managedName->AddString(VPROPERTIE_ALTERN_NAME,"_val");

	// creer le control d'edition pour l'utiliser
	_checkBox = new BCheckBox(BRect(0,0,10,10),"value-check","value",NULL);
	
	// on a besoin de rafraichir la liste des objets
	SetNeedRefresh();
}

/**** destructeur ****/
_Val::~_Val()
{
	// liberer la memoire
	delete _checkBox;
}

/**** function de clonage d'une propriete ****/
VPropertie* _Val::InstantiateVPropertie(entry_ref *entry)
{
	// creer le MObject apres MeTOS va appeler la methode CreateObject() pour reelement
	// creer l'objet graphique (fenetre,vue etc...)
	return (new _Val(entry));
}

/**** gerer l'affichage ****/
void _Val::Draw(BView *owner,BRect bounds,bool complete)
{
	// apres on va l'afficher
	owner->SetLowColor(owner->HighColor());
	owner->SetHighColor(0,0,0);
	if(_value==B_CONTROL_OFF)
		owner->DrawString("B_CONTROL_OFF",BPoint(bounds.left+5,bounds.top+15));	
	else
		owner->DrawString("B_CONTROL_ON",BPoint(bounds.left+5,bounds.top+15));	
}

/**** recuperer la valeur qui nous interesse ****/
void _Val::SetProperties(BMessage *properties)
{
	if(properties!=NULL)
	{
		// verifier que la proprietes existe dans l'item
		properties->FindInt32(PropertiesName(),Index(),&_value);
	}
}

/**** renvoyer la bonne valeur ****/
void _Val::Properties(BMessage *objectProperties)
{
	objectProperties->AddInt32(PropertiesName(),_value);
}

/**** selection l'item pour editer la donnée ****/
void _Val::SelectItem(BView *owner,BRect bounds)
{
	owner->AddChild(_checkBox);
	_checkBox->ResizeTo(bounds.Width()-6,bounds.Height()-5);
	_checkBox->MoveTo(bounds.left+3,bounds.top+2);
	_checkBox->SetViewColor(_itemColor);
	_checkBox->SetLowColor(_itemColor);
	
	// renseigner le controle avec la proprietes
	_checkBox->SetValue(_value);
}

/**** deselection l'item ****/
void _Val::UnSelectItem(BView *owner,BRect bounds)
{
	int32	newValue;
	
	newValue = _checkBox->Value();
	SetChange(newValue!=_value);
	_value = newValue;
	owner->RemoveChild(_checkBox);
}

/**** recuperer la valeur sour forme de chaine ****/
void _Val::ValueAsString(BMessage *properties,BString &value,int32 index)
{
	int32		val;
	
	// entourer la chaine de "
	if(properties->FindInt32(PropertiesName(),index,&val)==B_OK)
		if(val==B_CONTROL_OFF)
			value << "B_CONTROL_OFF";
		else
			value << "B_CONTROL_ON";
}

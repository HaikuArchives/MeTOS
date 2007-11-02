/****************************************/
/* Addon de la proprietes FLOT de BeOS */
/****************************************/
#include "FLOT.h"
#include <interface/TextControl.h>
#include <stdio.h>

/**** constructeur ****/
FLOT::FLOT(entry_ref *entry)
: VPropertie("FLOT",entry)
{
	// noms gerés
	BMessage	*managedName = ManagedNames();

	// ne pas oublier de renseigner ces valeurs sinon l'addon ne pourras pas etre utilisée
	_float = 0x0;

	// ajouter les noms gerés
	managedName->AddString(VPROPERTIE_ALTERN_NAME,"FLOT");

	// creer le control d'edition pour l'utiliser
	_editor = new BTextControl(BRect(0,0,10,10),"flot-editor",NULL,"",NULL);
	
	// on a besoin de rafraichir la liste des objets
	SetNeedRefresh();
}

/**** destructeur ****/
FLOT::~FLOT()
{
	delete _editor;
}

/**** function de clonage d'une propriete ****/
VPropertie* FLOT::InstantiateVPropertie(entry_ref *entry)
{
	// creer le MObject apres MeTOS va appeler la methode CreateObject() pour reelement
	// creer l'objet graphique (fenetre,vue etc...)
	return (new FLOT(entry));
}

/**** gerer l'affichage ****/
void FLOT::Draw(BView *owner,BRect bounds,bool complete)
{
	BString		buffer;
	
	// apres on va l'afficher
	owner->SetLowColor(owner->HighColor());
	owner->SetHighColor(0,0,0);
	buffer << _float;
	owner->DrawString(buffer.String(),BPoint(bounds.left+5,bounds.top+15));	
}

/**** recuperer la valeur qui nous interesse ****/
void FLOT::SetProperties(BMessage *properties)
{
	// verifier que la proprietes existe dans l'item
	if(properties!=NULL)
		properties->FindFloat(PropertiesName(),Index(),&_float);
}

/**** renvoyer la bonne valeur ****/
void FLOT::Properties(BMessage *objectProperties)
{
	objectProperties->AddFloat(PropertiesName(),_float);
}

/**** selection l'item pour editer la donnée ****/
void FLOT::SelectItem(BView *owner,BRect bounds)
{
	BString		buffer;
	
	owner->AddChild(_editor);
	_editor->ResizeTo(bounds.Width()-6,bounds.Height()-5);
	_editor->MoveTo(bounds.left+3,bounds.top+2);
	// pour fixer un bug d'affichage
	_editor->ChildAt(0)->MoveTo(3,3);
	_editor->ChildAt(0)->ResizeTo(bounds.Width()-12,bounds.Height()-10);
	_editor->MakeFocus(true);
	
	// renseigner le controle avec la proprietes
	buffer << _float;
	_editor->SetText(buffer.String());
}

/**** deselection l'item ****/
void FLOT::UnSelectItem(BView *owner,BRect bounds)
{
	float	newfloat;
	
	sscanf(_editor->Text(),"%f",&newfloat);	
	SetChange(newfloat!=_float);
	_float = newfloat;
	owner->RemoveChild(_editor);
}

/**** recuperer la valeur sour forme de chaine ****/
void FLOT::ValueAsString(BMessage *properties,BString &value,int32 index)
{
	float		valfloat;
	
	// entourer la chaine de "
	if(properties->FindFloat(PropertiesName(),index,&valfloat)==B_OK)
		value << valfloat;
}

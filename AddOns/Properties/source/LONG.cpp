/****************************************/
/* Addon de la proprietes LONG de BeOS */
/****************************************/
#include "LONG.h"
#include <interface/TextControl.h>
#include <stdio.h>

/**** constructeur ****/
LONG::LONG(entry_ref *entry)
: VPropertie("LONG",entry)
{
	// noms gerés
	BMessage	*managedName = ManagedNames();

	// ne pas oublier de renseigner ces valeurs sinon l'addon ne pourras pas etre utilisée
	_long = 0x0;

	// ajouter les noms gerés le int32
	managedName->AddString(VPROPERTIE_ALTERN_NAME,"LONG");

	// creer le control d'edition pour l'utiliser
	_editor = new BTextControl(BRect(0,0,10,10),"long-editor",NULL,"",NULL);
	
	// on a besoin de rafraichir la liste des objets
	SetNeedRefresh();
}

/**** destructeur ****/
LONG::~LONG()
{
	delete _editor;
}

/**** function de clonage d'une propriete ****/
VPropertie* LONG::InstantiateVPropertie(entry_ref *entry)
{
	// creer le MObject apres MeTOS va appeler la methode CreateObject() pour reelement
	// creer l'objet graphique (fenetre,vue etc...)
	return (new LONG(entry));
}

/**** gerer l'affichage ****/
void LONG::Draw(BView *owner,BRect bounds,bool complete)
{
	BString		buffer;
	
	// apres on va l'afficher
	owner->SetLowColor(owner->HighColor());
	owner->SetHighColor(0,0,0);
	buffer << _long;
	owner->DrawString(buffer.String(),BPoint(bounds.left+5,bounds.top+15));	
}

/**** recuperer la valeur qui nous interesse ****/
void LONG::SetProperties(BMessage *properties)
{
	// verifier que la proprietes existe dans l'item
	if(properties!=NULL)
		properties->FindInt32(PropertiesName(),Index(),&_long);
}

/**** renvoyer la bonne valeur ****/
void LONG::Properties(BMessage *objectProperties)
{
	objectProperties->AddInt32(PropertiesName(),_long);
}

/**** selection l'item pour editer la donnée ****/
void LONG::SelectItem(BView *owner,BRect bounds)
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
	buffer << _long;
	_editor->SetText(buffer.String());
}

/**** deselection l'item ****/
void LONG::UnSelectItem(BView *owner,BRect bounds)
{
	int32		newLong;
	
	sscanf(_editor->Text(),"%ld",&newLong);	
	SetChange(newLong!=_long);
	_long = newLong;
	owner->RemoveChild(_editor);
}

/**** recuperer la valeur sour forme de chaine ****/
void LONG::ValueAsString(BMessage *properties,BString &value,int32 index)
{
	int32		valLong;
	
	// entourer la chaine de "
	if(properties->FindInt32(PropertiesName(),index,&valLong)==B_OK)
		value << valLong;
}

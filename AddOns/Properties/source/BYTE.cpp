/****************************************/
/* Addon de la proprietes BYTE de BeOS */
/****************************************/
#include "BYTE.h"
#include <interface/TextControl.h>
#include <stdio.h>

/**** constructeur ****/
BYTE::BYTE(entry_ref *entry)
: VPropertie("BYTE",entry)
{
	// noms gerés
	BMessage	*managedName = ManagedNames();

	// ne pas oublier de renseigner ces valeurs sinon l'addon ne pourras pas etre utilisée
	_byte = 0x0;

	// ajouter les noms gerés le int32
	managedName->AddString(VPROPERTIE_ALTERN_NAME,"BYTE");

	// creer le control d'edition pour l'utiliser
	_editor = new BTextControl(BRect(0,0,10,10),"byte-editor",NULL,"",NULL);
	
	// on a besoin de rafraichir la liste des objets
	SetNeedRefresh();
}

/**** destructeur ****/
BYTE::~BYTE()
{
	delete _editor;
}

/**** function de clonage d'une propriete ****/
VPropertie* BYTE::InstantiateVPropertie(entry_ref *entry)
{
	// creer le MObject apres MeTOS va appeler la methode CreateObject() pour reelement
	// creer l'objet graphique (fenetre,vue etc...)
	return (new BYTE(entry));
}

/**** gerer l'affichage ****/
void BYTE::Draw(BView *owner,BRect bounds,bool complete)
{
	BString		buffer;
	
	// apres on va l'afficher
	owner->SetLowColor(owner->HighColor());
	owner->SetHighColor(0,0,0);
	buffer << _byte;
	owner->DrawString(buffer.String(),BPoint(bounds.left+5,bounds.top+15));	
}

/**** recuperer la valeur qui nous interesse ****/
void BYTE::SetProperties(BMessage *properties)
{
	// verifier que la proprietes existe dans l'item
	if(properties!=NULL)
		properties->FindInt8(PropertiesName(),Index(),&_byte);
}

/**** renvoyer la bonne valeur ****/
void BYTE::Properties(BMessage *objectProperties)
{
	objectProperties->AddInt8(PropertiesName(),_byte);
}

/**** selection l'item pour editer la donnée ****/
void BYTE::SelectItem(BView *owner,BRect bounds)
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
	buffer << _byte;
	_editor->SetText(buffer.String());
}

/**** deselection l'item ****/
void BYTE::UnSelectItem(BView *owner,BRect bounds)
{
	char		tempchar;
	int8		newByte;
	
	sscanf(_editor->Text(),"%c",&tempchar);	
	newByte = (int8)tempchar;
	SetChange(newByte!=_byte);
	_byte = newByte;
	owner->RemoveChild(_editor);
}

/**** recuperer la valeur sour forme de chaine ****/
void BYTE::ValueAsString(BMessage *properties,BString &value,int32 index)
{
	int8		valByte;
	
	// entourer la chaine de "
	if(properties->FindInt8(PropertiesName(),index,&valByte)==B_OK)
		value << valByte;
}

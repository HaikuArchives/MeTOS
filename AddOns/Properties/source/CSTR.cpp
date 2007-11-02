/*****************************************/
/* Addon de la proprietes _frame de BeOS */
/*****************************************/
#include "CSTR.h"
#include <interface/TextControl.h>

/**** constructeur ****/
CSTR::CSTR(entry_ref *entry)
: VPropertie("CSTR",entry)
{
	// noms gerés
	BMessage	*managedName = ManagedNames();

	// ne pas oublier de renseigner ces valeurs sinon l'addon ne pourras pas etre utilisée
	_string = "Empty String";

	// ajouter les noms gerés
	managedName->AddString(VPROPERTIE_ALTERN_NAME,"CSTR");

	// creer le control d'edition pour l'utiliser
	_editor = new BTextControl(BRect(0,0,10,10),"string-editor",NULL,"",NULL);
	
	// on a besoin de rafraichir la liste des objets
	SetNeedRefresh();
}

/**** destructeur ****/
CSTR::~CSTR()
{
	delete _editor;
}

/**** function de clonage d'une propriete ****/
VPropertie* CSTR::InstantiateVPropertie(entry_ref *entry)
{
	// creer le MObject apres MeTOS va appeler la methode CreateObject() pour reelement
	// creer l'objet graphique (fenetre,vue etc...)
	return (new CSTR(entry));
}

/**** gerer l'affichage ****/
void CSTR::Draw(BView *owner,BRect bounds,bool complete)
{
	// apres on va l'afficher
	owner->SetLowColor(owner->HighColor());
	owner->SetHighColor(0,0,0);
	owner->DrawString(_string.String(),BPoint(bounds.left+5,bounds.top+15));	
}

/**** recuperer la valeur qui nous interesse ****/
void CSTR::SetProperties(BMessage *properties)
{
	// verifier que la proprietes existe dans l'item
	if(properties!=NULL)
		properties->FindString(PropertiesName(),Index(),&_string);
}

/**** renvoyer la bonne valeur ****/
void CSTR::Properties(BMessage *objectProperties)
{
	objectProperties->AddString(PropertiesName(),_string);
}

/**** selection l'item pour editer la donnée ****/
void CSTR::SelectItem(BView *owner,BRect bounds)
{
	owner->AddChild(_editor);
	_editor->ResizeTo(bounds.Width()-6,bounds.Height()-5);
	_editor->MoveTo(bounds.left+3,bounds.top+2);
	// pour fixer un bug d'affichage
	_editor->ChildAt(0)->MoveTo(3,3);
	_editor->ChildAt(0)->ResizeTo(bounds.Width()-12,bounds.Height()-10);
	_editor->MakeFocus(true);
	
	// renseigner le controle avec la proprietes
	_editor->SetText(_string.String());
}

/**** deselection l'item ****/
void CSTR::UnSelectItem(BView *owner,BRect bounds)
{
	BString		newString;
	
	newString = _editor->Text();
	SetChange(newString!=_string);
	_string = newString;
	owner->RemoveChild(_editor);
}

/**** recuperer la valeur sour forme de chaine ****/
void CSTR::ValueAsString(BMessage *properties,BString &value,int32 index)
{
	const char	*string;
	
	// entourer la chaine de "
	if(properties->FindString(PropertiesName(),index,&string)==B_OK)
		value << "\"" << string << "\"";
}

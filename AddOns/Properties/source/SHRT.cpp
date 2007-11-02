/****************************************/
/* Addon de la proprietes SHRT de BeOS */
/****************************************/
#include "SHRT.h"
#include <interface/TextControl.h>
#include <stdio.h>

/**** constructeur ****/
SHRT::SHRT(entry_ref *entry)
: VPropertie("SHRT",entry)
{
	// noms gerés
	BMessage	*managedName = ManagedNames();

	// ne pas oublier de renseigner ces valeurs sinon l'addon ne pourras pas etre utilisée
	_shrt = 0x0;

	// ajouter les noms gerés le int32
	managedName->AddString(VPROPERTIE_ALTERN_NAME,"SHRT");

	// creer le control d'edition pour l'utiliser
	_editor = new BTextControl(BRect(0,0,10,10),"shrt-editor",NULL,"",NULL);
	
	// on a besoin de rafraichir la liste des objets
	SetNeedRefresh();
}

/**** destructeur ****/
SHRT::~SHRT()
{
	delete _editor;
}

/**** function de clonage d'une propriete ****/
VPropertie* SHRT::InstantiateVPropertie(entry_ref *entry)
{
	// creer le MObject apres MeTOS va appeler la methode CreateObject() pour reelement
	// creer l'objet graphique (fenetre,vue etc...)
	return (new SHRT(entry));
}

/**** gerer l'affichage ****/
void SHRT::Draw(BView *owner,BRect bounds,bool complete)
{
	BString		buffer;
	
	// apres on va l'afficher
	owner->SetLowColor(owner->HighColor());
	owner->SetHighColor(0,0,0);
	buffer << _shrt;
	owner->DrawString(buffer.String(),BPoint(bounds.left+5,bounds.top+15));	
}

/**** recuperer la valeur qui nous interesse ****/
void SHRT::SetProperties(BMessage *properties)
{
	// verifier que la proprietes existe dans l'item
	if(properties!=NULL)
		properties->FindInt16(PropertiesName(),Index(),&_shrt);
}

/**** renvoyer la bonne valeur ****/
void SHRT::Properties(BMessage *objectProperties)
{
	objectProperties->AddInt16(PropertiesName(),_shrt);
}

/**** selection l'item pour editer la donnée ****/
void SHRT::SelectItem(BView *owner,BRect bounds)
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
	buffer << _shrt;
	_editor->SetText(buffer.String());
}

/**** deselection l'item ****/
void SHRT::UnSelectItem(BView *owner,BRect bounds)
{
	int32		templong;
	int16		newShrt;
	
	sscanf(_editor->Text(),"%ld",&templong);	
	newShrt = (int16)templong;
	SetChange(newShrt!=_shrt);
	_shrt = newShrt;
	owner->RemoveChild(_editor);
}

/**** recuperer la valeur sour forme de chaine ****/
void SHRT::ValueAsString(BMessage *properties,BString &value,int32 index)
{
	int16		valShrt;
	
	// entourer la chaine de "
	if(properties->FindInt16(PropertiesName(),index,&valShrt)==B_OK)
		value << valShrt;
}

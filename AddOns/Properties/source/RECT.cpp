/*******************************/
/* Addon du type BRect de BeOS */
/*******************************/
#include "RECT.h"

/**** constructeur ****/
RECT::RECT(entry_ref *entry)
: VPropertie("RECT",entry)
{
	// noms gerés
	BMessage	*managedName = ManagedNames();

	// valeur par defaut
	_rect = BRect(0,0,0,0);

	// ajouter les noms gerés
	managedName->AddString(VPROPERTIE_ALTERN_NAME,"RECT");
}

/**** function de clonage d'une propriete ****/
VPropertie* RECT::InstantiateVPropertie(entry_ref *entry)
{
	// creer le MObject apres MeTOS va appeler la methode CreateObject() pour reelement
	// creer l'objet graphique (fenetre,vue etc...)
	return (new RECT(entry));
}

/**** gerer l'affichage ****/
void RECT::Draw(BView *owner,BRect bounds,bool complete)
{
	BString		rectString("No Rect");

	// apres on va l'afficher
	rectString = "x : ";
	rectString << _rect.left << "  y : " << _rect.top;
	rectString << "  w : " << _rect.Width() << "  h : " << _rect.Height();

	owner->SetLowColor(owner->HighColor());
	owner->SetHighColor(0,0,0);
	owner->DrawString(rectString.String(),BPoint(bounds.left+5,bounds.top+15));	
}

/**** recuperer la valeur qui nous interesse ****/
void RECT::SetProperties(BMessage *properties)
{
	// verifier que la proprietes existe dans l'item
	if(properties!=NULL)
		properties->FindRect(PropertiesName(),Index(),&_rect);
}

/**** renvoyer la bonne valeur ****/
void RECT::Properties(BMessage *objectProperties)
{
}

/**** recuperer la valeur sour forme de chaine ****/
void RECT::ValueAsString(BMessage *properties,BString &value,int32 index)
{
	BRect		rect;
	
	// construire la chaine avec BRect()
	if(properties->FindRect(PropertiesName(),index,&rect)==B_OK)
		value << "BRect(" << rect.left << "," << rect.top << "," << rect.right << "," << rect.bottom << ")";
}

/*****************************************/
/* Addon de la proprietes BPoint de BeOS */
/*****************************************/
#include "BPNT.h"

/**** constructeur ****/
BPNT::BPNT(entry_ref *entry)
: VPropertie("BPNT",entry)
{
	// noms gerés
	BMessage	*managedName = ManagedNames();

	// valeur par defaut
	_point = BPoint(0,0);

	// ajouter les noms gerés
	managedName->AddString(VPROPERTIE_ALTERN_NAME,"BPNT");
}

/**** function de clonage d'une propriete ****/
VPropertie* BPNT::InstantiateVPropertie(entry_ref *entry)
{
	// creer le MObject apres MeTOS va appeler la methode CreateObject() pour reelement
	// creer l'objet graphique (fenetre,vue etc...)
	return (new BPNT(entry));
}

/**** gerer l'affichage ****/
void BPNT::Draw(BView *owner,BRect bounds,bool complete)
{
	BString		pointString("No Point");

	// apres on va l'afficher
	pointString = "x : ";
	pointString << _point.x << "  y : " << _point.y;

	owner->SetLowColor(owner->HighColor());
	owner->SetHighColor(0,0,0);
	owner->DrawString(pointString.String(),BPoint(bounds.left+5,bounds.top+15));	
}

/**** recuperer la valeur qui nous interesse ****/
void BPNT::SetProperties(BMessage *properties)
{
	// verifier que la proprietes existe dans l'item
	if(properties!=NULL)
		properties->FindPoint(PropertiesName(),Index(),&_point);
}

/**** renvoyer la bonne valeur ****/
void BPNT::Properties(BMessage *objectProperties)
{
}

/**** recuperer la valeur sour forme de chaine ****/
void BPNT::ValueAsString(BMessage *properties,BString &value,int32 index)
{
	BPoint		point;
	
	// construire la chaine avec BRect()
	if(properties->FindPoint(PropertiesName(),index,&point)==B_OK)
		value << "BPoint(" << point.x << "," << point.y << ")";
}

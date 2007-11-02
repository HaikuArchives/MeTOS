/*****************************************/
/* Addon de la proprietes _frame de BeOS */
/*****************************************/
#include "_frame.h"

/**** constructeur ****/
_Frame::_Frame(entry_ref *entry)
: VPropertie("_frame",entry)
{
	// noms gerés
	BMessage	*managedName = ManagedNames();

	// initialisation
	_frame = BRect(0,0,0,0);

	// ajouter les noms gerés
	managedName->AddString(VPROPERTIE_ALTERN_NAME,"_frame");
}

/**** function de clonage d'une propriete ****/
VPropertie* _Frame::InstantiateVPropertie(entry_ref *entry)
{
	// creer le MObject apres MeTOS va appeler la methode CreateObject() pour reelement
	// creer l'objet graphique (fenetre,vue etc...)
	return (new _Frame(entry));
}

/**** gerer l'affichage ****/
void _Frame::Draw(BView *owner,BRect bounds,bool complete)
{
	BString		frameString("No Frame");

	// apres on va l'afficher
	frameString = "x : ";
	frameString << _frame.left << "  y : " << _frame.top;
	frameString << "  w : " << _frame.Width() << "  h : " << _frame.Height();

	owner->SetLowColor(owner->HighColor());
	owner->SetHighColor(0,0,0);
	owner->DrawString(frameString.String(),BPoint(bounds.left+5,bounds.top+15));	
}

/**** recuperer la valeur qui nous interesse ****/
void _Frame::SetProperties(BMessage *properties)
{
	if(properties!=NULL)
	{
		// verifier que la proprietes existe dans l'item
		properties->FindRect(PropertiesName(),Index(),&_frame);
	}
}

/**** renvoyer la bonne valeur ****/
void _Frame::Properties(BMessage *objectProperties)
{
}

/**** recuperer la valeur sour forme de chaine ****/
void _Frame::ValueAsString(BMessage *properties,BString &value,int32 index)
{
	BRect		frame;
	
	// construire la chaine avec BRect()
	if(properties->FindRect(PropertiesName(),index,&frame)==B_OK)
		value << "BRect(" << frame.left << "," << frame.top << "," << frame.right << "," << frame.bottom << ")";
}

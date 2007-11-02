/************************************/
/* Addon proprietes du type BMesage */
/************************************/
#include "MSGG.h"
#include <interface/TextControl.h>

/**** constructeur ****/
MSGG::MSGG(entry_ref *entry)
: VPropertie("MSGG",entry)
{
	// noms gerés
	BMessage	*managedName = ManagedNames();

	// ne pas oublier de renseigner ces valeurs sinon l'addon ne pourras pas etre utilisée
	_message.what = 0;

	// ajouter les noms gerés
	managedName->AddString(VPROPERTIE_ALTERN_NAME,"MSGG");

	// creer le control d'edition pour l'utiliser
	_editor = new BTextControl(BRect(0,0,10,10),"msgg-editor",NULL,"",NULL);
	
	// on a besoin de rafraichir la liste des objets
	SetNeedRefresh();
}

/**** destructeur ****/
MSGG::~MSGG()
{
	delete _editor;
}

/**** function de clonage d'une propriete ****/
VPropertie* MSGG::InstantiateVPropertie(entry_ref *entry)
{
	// creer le MObject apres MeTOS va appeler la methode CreateObject() pour reelement
	// creer l'objet graphique (fenetre,vue etc...)
	return (new MSGG(entry));
}

/**** gerer l'affichage ****/
void MSGG::Draw(BView *owner,BRect bounds,bool complete)
{
	BString		buffer;
	
	// apres on va l'afficher
	ConvertWhatToString(_message.what,buffer);
	buffer.Prepend("what : '");
	buffer.Append("'");
	owner->SetLowColor(owner->HighColor());
	owner->SetHighColor(0,0,0);
	owner->DrawString(buffer.String(),BPoint(bounds.left+5,bounds.top+15));	
}

/**** recuperer la valeur qui nous interesse ****/
void MSGG::SetProperties(BMessage *properties)
{
	// verifier que la proprietes existe dans l'item
	if(properties!=NULL)
		properties->FindMessage(PropertiesName(),Index(),&_message);
}

/**** renvoyer la bonne valeur ****/
void MSGG::Properties(BMessage *objectProperties)
{
	objectProperties->AddMessage(PropertiesName(),&_message);
}

/**** selection l'item pour editer la donnée ****/
void MSGG::SelectItem(BView *owner,BRect bounds)
{
	BString		buffer;
	float		whatSize = owner->StringWidth("_WWWW_");
	
	owner->AddChild(_editor);
	_editor->ResizeTo(whatSize+6,bounds.Height()-5);
	_editor->MoveTo(bounds.left+3,bounds.top+2);
	// pour fixer un bug d'affichage
	_editor->ChildAt(0)->MoveTo(3,3);
	_editor->ChildAt(0)->ResizeTo(whatSize,bounds.Height()-10);
	_editor->MakeFocus(true);
	
	// renseigner le controle avec la proprietes
	ConvertWhatToString(_message.what,buffer);
	_editor->SetText(buffer.String());
}

/**** deselection l'item ****/
void MSGG::UnSelectItem(BView *owner,BRect bounds)
{
	BString		buffer;
	uint32		what;
	
	buffer = _editor->Text();
	switch(buffer.Length())
	{	
	case 0:
	case 4:
		{
			what = 0;
			// changer le message
			if(buffer.Length()>0)
			{
				((uint8 *)(&what))[3] = (uint8)(buffer[0]);
				((uint8 *)(&what))[2] = (uint8)(buffer[1]);
				((uint8 *)(&what))[1] = (uint8)(buffer[2]);
				((uint8 *)(&what))[0] = (uint8)(buffer[3]);
			}
			SetChange(what!=_message.what);
			_message.what = what;
		}
		break;
	default:
		{
			ConvertWhatToString(_message.what,buffer);
			_editor->SetText(buffer.String());
		}
	}
	owner->RemoveChild(_editor);	
}

/**** recuperer la valeur sour forme de chaine ****/
void MSGG::ValueAsString(BMessage *properties,BString &value,int32 index)
{
	BMessage	message;
	BString		buffer;
	
	// entourer la chaine de "
	if(properties->FindMessage(PropertiesName(),index,&message)==B_OK)
	{
		ConvertWhatToString(message.what,buffer);
		if(buffer=="Not Defined")
			value << "NULL";
		else
			value << " new BMessage('" << buffer << "')";
	}
}

/**** convertir un what de BMessage en chaine ****/
void MSGG::ConvertWhatToString(uint32 what,BString &string)
{
	// tenir compte du cas ou le what vaux 0
	if(what==0)
		string = "Not Defined";
	else
	{
		string = "";
		string << (char)(*(((uint8 *)&what) + 3));
		string << (char)(*(((uint8 *)&what) + 2));
		string << (char)(*(((uint8 *)&what) + 1));
		string << (char)(*((uint8 *)&what));	
	}
}

/************************************/
/* Addon de la proprietes _resource */
/************************************/
#include "_resource.h"
#include <storage/Directory.h>
#include <storage/Path.h>
#include <storage/Entry.h>

/**** constructeur ****/
_Resource::_Resource(entry_ref *entry)
: VPropertie("_resource",entry)
{
	// noms gerés
	BMessage	*managedName = ManagedNames();

	// ne pas oublier de renseigner ces valeurs sinon l'addon ne pourras pas etre utilisée
	_resource = "";
	
	// ajouter les noms gerés
	managedName->AddString(VPROPERTIE_ALTERN_NAME,"_resource");

	// creer le control d'edition pour l'utiliser
	_editor = new ResourceEditor();
}

/**** destructeur ****/
_Resource::~_Resource()
{
	// liberer la memoire prise par le bouton
	delete _editor;
}

/**** function de clonage d'une propriete ****/
VPropertie* _Resource::InstantiateVPropertie(entry_ref *entry)
{
	// creer le MObject apres MeTOS va appeler la methode CreateObject() pour reelement
	// creer l'objet graphique (fenetre,vue etc...)
	return (new _Resource(entry));
}

/**** gerer l'affichage ****/
void _Resource::Draw(BView *owner,BRect bounds,bool complete)
{
	// apres on va l'afficher
	owner->SetLowColor(owner->HighColor());
	owner->SetHighColor(0,0,0);
	owner->DrawString(_resource.String(),BPoint(bounds.left+5,bounds.top+15));	
}

/**** recuperer la valeur qui nous interesse ****/
void _Resource::SetProperties(BMessage *properties)
{
	if(properties!=NULL)
	{
		// verifier que la proprietes existe dans l'item
		properties->FindString(PropertiesName(),Index(),&_resource);
	}
}

/**** renvoyer la bonne valeur ****/
void _Resource::Properties(BMessage *objectProperties)
{
	objectProperties->AddString(PropertiesName(),_resource);
}

/**** selection l'item pour editer la donnée ****/
void _Resource::SelectItem(BView *owner,BRect bounds)
{
	owner->AddChild(_editor);
	// ajouter la vue pour recevoir le message
	_editor->ResizeTo(bounds.Width()-6,bounds.Height()-5);
	_editor->MoveTo(bounds.left+3,bounds.top+2);
	// pour fixer un bug d'affichage
	_editor->ChildAt(0)->MoveTo(3,3);
	_editor->ChildAt(0)->ResizeTo(bounds.Width()-12,bounds.Height()-10);
	_editor->MakeFocus(true);

	// renseigner le controle avec la proprietes
	_editor->SetText(_resource.String());
}

/**** deselection l'item ****/
void _Resource::UnSelectItem(BView *owner,BRect bounds)
{
	BString		newResource;
	
	newResource = _editor->Text();
	SetChange(_resource!=newResource);
	_resource = newResource;
	owner->RemoveChild(_editor);
}

/**** recuperer la valeur sour forme de chaine ****/
void _Resource::ValueAsString(BMessage *properties,BString &value,int32 index)
{
	const char *resource;
	
	// entourer la chaine de "
	if(properties->FindString(PropertiesName(),index,&resource)==B_OK)
		value << "\"" << resource << "\"";
}

/******************************************/
/* Bouton pour les messages du BFilePanel */
/******************************************/
ResourceEditor::ResourceEditor()
: BTextControl(BRect(0,0,10,10),"path-editor",NULL,"",NULL)
{
}

/**** gestion des message ****/
void ResourceEditor::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
	// drag and drop
	case 'DATA':
		{
			entry_ref	ref;
			BPath		path;
			
			// trouver le chemin
			message->FindRef("refs",&ref);
			path.SetTo(&ref);
			if(path.InitCheck()==B_OK)
				SetText(path.Path());
		}
		break;
	default:
		BView::MessageReceived(message);
	}
}

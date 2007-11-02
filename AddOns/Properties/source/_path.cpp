/********************************/
/* Addon de la proprietes _path */
/********************************/
#include "_path.h"
#include <storage/Directory.h>
#include <storage/Path.h>
#include <storage/Entry.h>

/**** constructeur ****/
_Path::_Path(entry_ref *entry)
: VPropertie("_path",entry)
{
	// noms gerés
	BMessage	*managedName = ManagedNames();

	// ne pas oublier de renseigner ces valeurs sinon l'addon ne pourras pas etre utilisée
	_path = "/boot/home/";
	
	// ajouter les noms gerés
	managedName->AddString(VPROPERTIE_ALTERN_NAME,"_path");

	// creer le control d'edition pour l'utiliser
	_editor = new PathEditor();
}

/**** destructeur ****/
_Path::~_Path()
{
	// liberer la memoire prise par le bouton
	delete _editor;
}

/**** function de clonage d'une propriete ****/
VPropertie* _Path::InstantiateVPropertie(entry_ref *entry)
{
	// creer le MObject apres MeTOS va appeler la methode CreateObject() pour reelement
	// creer l'objet graphique (fenetre,vue etc...)
	return (new _Path(entry));
}

/**** gerer l'affichage ****/
void _Path::Draw(BView *owner,BRect bounds,bool complete)
{
	// apres on va l'afficher
	owner->SetLowColor(owner->HighColor());
	owner->SetHighColor(0,0,0);
	owner->DrawString(_path.String(),BPoint(bounds.left+5,bounds.top+15));	
}

/**** recuperer la valeur qui nous interesse ****/
void _Path::SetProperties(BMessage *properties)
{
	if(properties!=NULL)
	{
		// verifier que la proprietes existe dans l'item
		properties->FindString(PropertiesName(),Index(),&_path);
	}
}

/**** renvoyer la bonne valeur ****/
void _Path::Properties(BMessage *objectProperties)
{
	objectProperties->AddString(PropertiesName(),_path);
}

/**** selection l'item pour editer la donnée ****/
void _Path::SelectItem(BView *owner,BRect bounds)
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
	_editor->SetText(_path.String());
}

/**** deselection l'item ****/
void _Path::UnSelectItem(BView *owner,BRect bounds)
{
	BString		newPath;
	
	newPath = _editor->Text();
	SetChange(_path!=newPath);
	_path = newPath;
	owner->RemoveChild(_editor);
}

/**** recuperer la valeur sour forme de chaine ****/
void _Path::ValueAsString(BMessage *properties,BString &value,int32 index)
{
	const char *path;
	
	// entourer la chaine de "
	if(properties->FindString(PropertiesName(),index,&path)==B_OK)
		value << "\"" << path << "\"";
}

/******************************************/
/* Bouton pour les messages du BFilePanel */
/******************************************/
PathEditor::PathEditor()
: BTextControl(BRect(0,0,10,10),"path-editor",NULL,"",NULL)
{
}

/**** gestion des message ****/
void PathEditor::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
	// drag and drop
	case 'DATA':
		{
			entry_ref	ref;
			BDirectory	directory;
			BPath		path;
			
			// trouver le chemin
			message->FindRef("refs",&ref);
			directory.SetTo(&ref);
			if(directory.InitCheck()==B_OK)
			{
				path.SetTo(&ref);
				SetText(path.Path());
			}
		}
		break;
	default:
		BView::MessageReceived(message);
	}
}

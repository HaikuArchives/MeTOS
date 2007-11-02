/*****************************************/
/* Addon de la proprietes _Fname de BeOS */
/*****************************************/
#include "_fname.h"
#include "MetosMessageDefs.h"
#include "VObject.h"
#include <interface/Font.h>
#include <interface/PopUpMenu.h>
#include <interface/Menu.h>
#include <interface/MenuItem.h>
#include <interface/MenuField.h>
#include <app/Application.h>

/**** constructeur ****/
_Fname::_Fname(entry_ref *entry)
: VPropertie("_fname",entry)
{
	BMessage		*managedName = ManagedNames();
	BPopUpMenu		*menu = NULL;
	font_family		family;
	int32			nbFamily;

	// pour cette proprietes on a 2 cas
	// on peut modifier le nom de la police ou son style,
	// car c'est le meme nom de donnée dans le message (nom index=0, style index=1)
	// ajouter les noms gerés
	managedName->AddString(VPROPERTIE_ALTERN_NAME,"_fname");

	// creer le menu des polices
	menu = new BPopUpMenu("Font");
	_fontMenu = new BMenuField(BRect(0,0,400,25),"menu-font","",menu);
	_fontMenu->SetDivider(0);
	nbFamily = count_font_families();
	for(int32 index=0;index<nbFamily;index++)
		if(get_font_family(index,&family)==B_OK)
			menu->AddItem(new BMenuItem(family,NULL));
	
	// et les styles
	// ce menu sera creer dynamiquement en fonction de la police selectionnée
	menu = new BPopUpMenu("FontStyle");
	_fontStyleMenu = new BMenuField(BRect(0,0,400,25),"menu-font-style","",menu);
	_fontStyleMenu->SetDivider(0);
	
	// ne pas oublier de renseigner ces valeurs sinon l'addon ne pourras pas etre utilisée
	_fname = "";
	_fStyle = "";

	// menu par defaut
	_selector = NULL;
}

/**** destructeur ****/
_Fname::~_Fname()
{
	delete _fontMenu;
	delete _fontStyleMenu;
}

/**** function de clonage d'une propriete ****/
VPropertie* _Fname::InstantiateVPropertie(entry_ref *entry)
{
	// creer le MObject apres MeTOS va appeler la methode CreateObject() pour reelement
	// creer l'objet graphique (fenetre,vue etc...)
	return (new _Fname(entry));
}

/**** gerer l'affichage ****/
void _Fname::Draw(BView *owner,BRect bounds,bool complete)
{
	// apres on va l'afficher
	owner->SetLowColor(owner->HighColor());
	owner->SetHighColor(0,0,0);
	owner->DrawString(_fname.String(),BPoint(bounds.left+5,bounds.top+15));	
}

/**** recuperer la valeur qui nous interesse ****/
void _Fname::SetProperties(BMessage *properties)
{
	if(properties!=NULL)
	{
		// verifier que la proprietes existe dans l'item
		// dans ce cas on garde Index() car ca peux etre le nom ou le style !
		properties->FindString(PropertiesName(),Index(),&_fname);

		// essayer de recuperer le style et le nom a chaque fois
		properties->FindString(PropertiesName(),0,&_fName);
		if(properties->FindString(PropertiesName(),1,&_fStyle)!=B_OK)
			_fStyle = "";
			
		// recuperer l'ID pour la mise a jour
		properties->FindInt32(VOBJECT_ID,(int32 *)&_objectID);
	}
}

/**** renvoyer la bonne valeur ****/
void _Fname::Properties(BMessage *objectProperties)
{
	// ajouter la nouvelle valeur
	objectProperties->AddString(PropertiesName(),_fname);
}

/**** selection l'item pour editer la donnée ****/
void _Fname::SelectItem(BView *owner,BRect bounds)
{
	switch(Index())
	{
	// nom de la police
	case 0:
		_selector = _fontMenu;
		break;
	// style
	case 1:
		_selector = _fontStyleMenu;
	// regenerer le menu des styles
	RebuildFontStyleMenu();		
		break;
	default:
		; // erreur
	}

	// selectionner la bon menu
	CheckMarcked();

	// ajouter a la vue
	owner->AddChild(_selector);
	_selector->MoveTo(bounds.left+1,bounds.top);
	_selector->MakeFocus(true);
}

/**** deselection l'item ****/
void _Fname::UnSelectItem(BView *owner,BRect bounds)
{
	BMenuItem	*markedItem = NULL;
	BString		fname;
	
	markedItem = _selector->Menu()->FindMarked();
	if(markedItem!=NULL)
		fname = markedItem->Label();
	
	// valider le changement
	SetChange(_fname!=fname);
	_fname = fname;
	owner->RemoveChild(_selector);

	// si on change la police, on doit verifier que le style existe
	// sinon on le modifie
	if(Index()==0)
		UpdateFontStyle();
}

/**** recuperer la valeur sour forme de chaine ****/
void _Fname::ValueAsString(BMessage *properties,BString &value,int32 index)
{
	const char		*string;
	
	// entourer la chaine de "
	if(properties->FindString(PropertiesName(),index,&string)==B_OK)
		value << "\"" << string << "\"";
}

/**** regenerer le menu des styles ****/
void _Fname::RebuildFontStyleMenu()
{
	BMenu			*menu = NULL;
	BMenuItem		*newItem = NULL;
	font_family		family;
	font_style		style;
	int32			nbStyle;
	int32			nbItems;
	
	// vider le menu
	menu = _fontStyleMenu->Menu();
	nbItems = menu->CountItems();
	for(int32 i=0;i<nbItems;i++)
		delete menu->RemoveItem((int32)0);
	
	// recopier le nom
	strcpy(family,_fName.String());

	// recuperer le nombre de style pour cette police
	nbStyle = count_font_styles(family);
	if(nbStyle>0)
	{
		// parcourir tous les styles de cette police
		// pour trouver si l'actuel existe
		for(int32 index=0;index<nbStyle;index++)
		{
			if(get_font_style(family,index,&style)==B_OK)
			{
				newItem = new BMenuItem(style,NULL);
				menu->AddItem(newItem);
				if(strcmp(style,_fStyle.String())==0)
					newItem->SetMarked(true);
			}
		}
	}
}

/**** mettre a jour le style ****/
void _Fname::UpdateFontStyle()
{
	BMessage		newProperties(M_OBJECT_CHANGE_MSG);
	font_family		family;
	font_style		style;
	int32			nbStyle;
	
	// recopier le nom
	strcpy(family,_fname.String());
	
	// recuperer le nombre de style pour cette police
	nbStyle = count_font_styles(family);
	if(nbStyle==0)
		_fStyle = "";
	else
	{
		int32	index;
		
		// parcourir tous les styles de cette police
		// pour trouver si l'actuel existe
		for(index=0;index<nbStyle;index++)
			if(get_font_style(family,index,&style)==B_OK && strcmp(style,_fStyle.String())==0)
				break;

		// verifier si on a trouvé
		// sinon on prend le premier de la liste
		if(index==nbStyle)
		{
			get_font_style(family,0,&style);
			_fStyle = style;
		}
	}

	// mettre a jour le style
	newProperties.AddInt32(M_OBJECT_SELECTED,_objectID);		
	newProperties.AddString("_fname",_fStyle);
	newProperties.AddInt32(M_PROPERTIES_INDEX,1);
	newProperties.AddBool(M_NEED_INVALIDATE,true);
	be_app_messenger.SendMessage(&newProperties);
}

/**** selectionner l'item de menu ****/
void _Fname::CheckMarcked()
{
	BMenu		*menu = NULL;
	int32		nbItems;
	
	// parcourir le menu
	menu = _selector->Menu();
	nbItems = menu->CountItems();
	for(int32 i=0;i<nbItems;i++)
		if(menu->ItemAt(i)!=NULL && _fname==((menu->ItemAt(i))->Label()))
			menu->ItemAt(i)->SetMarked(true);
}

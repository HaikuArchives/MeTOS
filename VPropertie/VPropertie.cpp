/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/

/*********************************************/
/* Classe qui sert a creer les addon pour    */
/* gerer les proprietes des objets graphique */
/*********************************************/
#include "VPropertie.h"
#include "VObject.h"
#include "VPreferenceFile.h"
#include "MetosGlobalDefs.h"
#include <interface/InterfaceDefs.h>
#include <interface/Bitmap.h>
#include <storage/AppFileInfo.h>
#include <storage/Path.h>
#include <interface/View.h>
#include <support/String.h>
#include <stdlib.h>

VPropertie::VPropertie(const char *name,entry_ref *entry)
: BListItem()
{
	int32	nameSize = 0;

	// initialiser les variables
	// par defaut on va pas toujours tout rafraichir
	_needRefresh = false;
	_index = 0;
	_icon = NULL;
	_hasChange = false;
	_enable = true;
	_sizePropertiesInfo = 180;
	_propertiesName = NULL;
	_managedNames.MakeEmpty();
	_addonName = NULL;
	
	// recopier le nom
	if(name!=NULL && (nameSize = strlen(name))>0)
	{
		// recopier avec le zero
		nameSize++;
		_addonName = (char *)malloc(nameSize);
		memcpy(_addonName,name,nameSize);
	}
	
	// initialiser le nom
	SetPropertiesName(name);
	
	// fixer l'entry si le ref est valide
	if(entry!=NULL)
		_entry.SetTo(entry);

	// l'icon
	SetIcon();
	
	// les preferences
	RefreshPreferences();
}

VPropertie::~VPropertie()
{
	// liberer la memoire prise par l'icone
	if(_propertiesName!=NULL)
		free(_propertiesName);

	// liberer la memoire  du nom
	if(_addonName!=NULL)
		free(_addonName);

	// liberer l'icon
	delete _icon;
}

/**** ne peut etre surchargé ca sert par exemple a ajouter des truc apres la creation ****/
void VPropertie::SetIndex(int32 index)
{
	// l'index
	_index = index;
}

/**** definir l'icon ****/
void VPropertie::SetIcon()
{
	// on doit toujours allouer l'icon
	_icon = new BBitmap(BRect(0,0,15,15 ), B_CMAP8);

	// verifier la validite de l'entree
	if(_entry.InitCheck()!=B_OK)
		return;

	BPath			path;
	BAppFileInfo	fileinfo;
	BFile			file(&_entry, B_READ_ONLY);
	
	// recuperer l'image de l'addon pour faire appel
	// ulterieurement au fonction externe de chaque AddOns
	// et le fichier pour l'icone
	_entry.GetPath(&path);
	fileinfo.SetTo(&file);	
	if(fileinfo.GetIcon(_icon, B_MINI_ICON) != B_OK)
		fileinfo.GetTrackerIcon(_icon, B_MINI_ICON);
}

void VPropertie::DrawItem(BView *owner,BRect bounds,bool complete)
{
	BString		index("index : ");
	rgb_color	background;
	
	// on doit ici afficher le nom de la proprietes
	// et desssiner le cadre le l'item
	// couleur de fond
	index << _index;

	// dessiner le fond
	if(IsSelected())
		owner->SetHighColor(_itemColor);
	else
		owner->SetHighColor(WHITE_COLOR);
	owner->SetLowColor(owner->HighColor());
	owner->FillRect(bounds);
	
	// couleur de fond
	background = owner->HighColor();

	// dessiner le cadre
	owner->SetHighColor(VIEW_COLOR);
	owner->StrokeLine(BPoint(bounds.left+_sizePropertiesInfo,bounds.top),BPoint(bounds.left+_sizePropertiesInfo,bounds.bottom));
	owner->StrokeLine(BPoint(bounds.left,bounds.bottom),BPoint(bounds.right,bounds.bottom));

	// afficher le nom de la proprieté
	owner->SetHighColor(BLACK_COLOR);
	_truncBuffer = _addonName;
	owner->TruncateString(&_truncBuffer,B_TRUNCATE_END,_sizePropertiesInfo-25);
	owner->DrawString(_truncBuffer.String(),BPoint(bounds.left+25,bounds.top+9));
	_truncBuffer = _propertiesName;
	owner->TruncateString(&_truncBuffer,B_TRUNCATE_END,_sizePropertiesInfo-25);
	owner->DrawString(_truncBuffer.String(),BPoint(bounds.left+25,bounds.top+20));
	_truncBuffer = index;
	owner->TruncateString(&_truncBuffer,B_TRUNCATE_END,_sizePropertiesInfo-125);
	owner->DrawString(_truncBuffer.String(),BPoint(bounds.left+125,bounds.top+20));
	
	// dessiner l'icon
	if(_icon!=NULL)
	{
		owner->SetDrawingMode(B_OP_ALPHA);
		owner->DrawBitmap(_icon,BPoint(bounds.left+4,bounds.top+4));
		owner->SetDrawingMode(B_OP_COPY);
	}
	
	// appel dessin utilisateur
	// si l'item n'est pas selectionné
	bounds.left += _sizePropertiesInfo;
	bounds.InsetBy(1,1);
	owner->SetHighColor(owner->LowColor());
	Draw(owner,bounds,complete);
	owner->SetHighColor(background);
}

/**** Selection de l'item ****/
void VPropertie::SelectItem(BView *owner,BRect bounds)
{ } // vide dans notre cas

/**** DeSelection de l'item ****/
void VPropertie::UnSelectItem(BView *owner,BRect bounds)
{ } // vide dans notre cas

/**** redimentionner ****/
void VPropertie::Update(BView *owner, const BFont *font)
{
	// fixer la taille
	SetHeight(23);
}

/**** rafraichir les preferences ****/
void VPropertie::RefreshPreferences()
{
	rgb_color tempColor = {200,200,255,255};
	_itemColor = g_ColorsPrefs->GetRGBColor(tempColor,"list-color");
}

/**** definir la propriete editée ****/
void VPropertie::SetPropertiesName(const char *value)
{
	// donner un nom correct
	if(value==NULL)
		return;
		
	size_t	nameSize;
	char	*newChar = NULL;
	
	nameSize = strlen(value);
	newChar = (char *)realloc(_propertiesName,nameSize + 1);
	if(newChar==NULL)
		return;
	
	_propertiesName = newChar;	
	memcpy(_propertiesName,value,nameSize);
	_propertiesName[nameSize] = '\0';
}

/**** verifier si ce nom est geré (ca peux etre un type) ****/
bool VPropertie::NameIsManaged(const char *name)
{
	const char		*managed = NULL;
	int32			index = 0;
	
	// on va chercher dans le message interne
	while(_managedNames.FindString(VPROPERTIE_ALTERN_NAME,index,&managed)==B_OK)
	{
		// verifier si le nom a ete trouvé
		if(strcmp(name,managed)==0)
			return true;
				
		// passer au suivant
		index++;
	}
	
	// pas trouvé	
	return false;
}

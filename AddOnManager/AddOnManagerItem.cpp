/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
/*******************************/
/* Item de la liste des AddOns */
/*******************************/
#include "AddOnManagerItem.h"
#include "MetosGlobalDefs.h"
#include "VPreferenceFile.h"
#include "MetosGlobalDefs.h"
#include "MetosApp.h"
#include "MetosGlobalDefs.h"
#include "MetosLanguagesDefs.h"
#include "VLanguage.h"

#include <storage/Entry.h>
#include <interface/Bitmap.h>
#include <storage/AppFileInfo.h>
#include <interface/View.h>
#include <storage/Path.h>

/**** Constructeur version addon ****/
AddOnManagerItem::AddOnManagerItem(entry_ref *entry,BMessage *properties,BMessage *settings)
: BListItem()
{
	BAppFileInfo	fileinfo;
	BFile			file(entry, B_READ_ONLY);
	BPath			path(entry);

	// recuperer l'image de l'addon pour faire appel
	// ulterieurement au fonction externe de chaque AddOns
	_properties = *properties;
	_settings = *settings;
	_name = path.Leaf();
	_lock = false;
	_available = true;
	_properties.FindInt32(VOBJECT_TYPE,(int32 *)&_type);
	_properties.FindString(VOBJECT_CLASS,&_className);
	_displayClassName = g_Languages->GetTranslationFor(L_ADDON_CLASS_NAME);
	_displayType = g_Languages->GetTranslationFor(L_ADDON_OBJECT_TYPE);

	// recuperer le fichier
	fileinfo.SetTo(&file);	

	// charger l'icon
	_icon = new BBitmap(BRect(0,0,31,31 ), B_CMAP8);
	if(fileinfo.GetIcon(_icon, B_LARGE_ICON) != B_OK)
		fileinfo.GetTrackerIcon(_icon, B_LARGE_ICON);
	
	// icon du cadena
	_lockIcon = g_MetosApp->GetBitmap("Lock");
		
	// c'est pas un superitem
	_isASuperItem = false;
	
	// preferences
	RefreshPreferences();
}

/**** Constructeur version superitem ****/
AddOnManagerItem::AddOnManagerItem(object_type type)
: BListItem()
{
	// initialisation
	_name = "";
	_lock = true;
	_available = true;
	_className = "";
	_type = type;
	_icon = NULL;

	// c'est un superitem
	_isASuperItem = true;

	// preferences
	RefreshPreferences();
}

/**** Destructeur ****/
AddOnManagerItem::~AddOnManagerItem()
{
	if(!_isASuperItem)
	{
		// effacer l'objet l'icone et l'entry
		delete _icon;
	}
}

/**** Dessin des Items ****/
void AddOnManagerItem::DrawItem(BView *owner,BRect bounds,bool complete)
{
	// recadrer
	bounds.left = 0;

	// couleur de fond
	owner->SetHighColor(WHITE_COLOR);
	if(_isASuperItem)
		owner->SetHighColor(LIGHT_2_COLOR);
	else
	{
		if(!_available || _lock)
			owner->SetHighColor(_unAvailableColor);
		else
			if(IsSelected())
				owner->SetHighColor(_itemColor);
	}
	owner->SetLowColor(owner->HighColor());
	owner->FillRect(bounds);

	// dessiner le cadre
	owner->SetHighColor(DARK_1_COLOR);
	owner->StrokeLine(BPoint(0,bounds.bottom),BPoint(bounds.right,bounds.bottom));
	
	if(!_isASuperItem)
	{
		BString			buffer;

		// morceau clair
		if(_available && !IsSelected())
		{
			owner->SetHighColor(LIGHT_2_COLOR);
			owner->FillRect(BRect(bounds.left,bounds.top,bounds.left+12,bounds.bottom-1));
		}
		
		// dessiner l'icon
		owner->SetDrawingMode(B_OP_ALPHA);
		owner->DrawBitmap(_icon,BPoint(bounds.left+16,bounds.top+1));
		owner->SetDrawingMode(B_OP_COPY);

		// afficher si il est bloqué ou pas
		if(_lock)
		{
			owner->SetDrawingMode(B_OP_ALPHA);
			owner->DrawBitmap(_lockIcon,BPoint(bounds.left+12,bounds.top));
			owner->SetDrawingMode(B_OP_COPY);
		}

		// afficher le nom de la classe l'objet et le type
		owner->SetHighColor(BLACK_COLOR);
		buffer = _displayClassName;
		buffer << _className;
		owner->DrawString(buffer.String(),BPoint(bounds.left+50,bounds.top+10));

		// en rouge le type si il est indisponible
		if(!_available)
			owner->SetHighColor(_unAvailableTextColor);
		
		buffer = _displayType;
		buffer << TypeName(_type);
		owner->DrawString(buffer.String(),BPoint(bounds.left+50,bounds.top+20));		
	}
	else
	{
		BRect	itemRect(bounds.left,bounds.top,bounds.left+bounds.Height(),bounds.top+bounds.Height());
		BRect	latchRect;

		latchRect = itemRect;
		latchRect.InsetBy(2,2);
		latchRect.left -=1;
		latchRect.top -=1;

		owner->SetHighColor(WHITE_COLOR);
		owner->StrokeLine(BPoint(latchRect.left,latchRect.top),BPoint(latchRect.right,latchRect.top));
		owner->StrokeLine(BPoint(latchRect.left,latchRect.top),BPoint(latchRect.left,latchRect.bottom));

		owner->SetHighColor(BLACK_COLOR);
		owner->StrokeLine(BPoint(latchRect.right,latchRect.top),BPoint(latchRect.right,latchRect.bottom));
		owner->StrokeLine(BPoint(latchRect.left,latchRect.bottom),BPoint(latchRect.right,latchRect.bottom));

		latchRect.InsetBy(1,1);
		owner->SetHighColor(VIEW_COLOR);
		owner->FillRect(latchRect);

		owner->SetHighColor(DARK_2_COLOR);
		owner->StrokeLine(BPoint(latchRect.right,latchRect.top),BPoint(latchRect.right,latchRect.bottom));
		owner->StrokeLine(BPoint(latchRect.left,latchRect.bottom),BPoint(latchRect.right,latchRect.bottom));

		latchRect.right -=1;
		latchRect.bottom -=1;
		owner->SetHighColor(BLACK_COLOR);
		owner->StrokeLine(BPoint(latchRect.left+2,latchRect.top+4),BPoint(latchRect.right-2,latchRect.top+4));
		if(IsExpanded())
			owner->StrokeLine(BPoint(latchRect.left+4,latchRect.top+2),BPoint(latchRect.left+4,latchRect.bottom-2));

		// afficher le nom de la classe l'objet et le type
		owner->SetHighColor(BLACK_COLOR);
		owner->DrawString(TypeName(_type).String(),BPoint(bounds.left+17,bounds.top+11));		
	}
}

/**** ajuster la taille de l'item ****/
void AddOnManagerItem::Update(BView *owner, const BFont *font)
{
	if(_isASuperItem)
		SetHeight(14);
	else
		SetHeight(34);
}

/**** retourner le type de base sous forme de chaine ****/
BString AddOnManagerItem::TypeName(int32 type)
{
	BString		name;

	switch(type)
	{
	// indefini
	case UNDEFINED_TYPE:
		name = g_Languages->GetTranslationFor(L_ADDON_UNDIFINED_TYPE);
		break;
	// projet, application
	case PROJECT_TYPE:
		name = "BProject";
		break;
	case APPLICATION_TYPE:
		name = "BApplication";
		break;
	// fenetre
	case WINDOW_TYPE:
		name = "BWindow";
		break;
	// vue
	case VIEW_TYPE:
		name = "BView";
		break;
	case TABVIEW_TYPE:
		name = "BTabView";
		break;
	case SCROLLVIEW_TYPE:
		name = "BScrollView";
		break;
	case LISTVIEW_TYPE:
		name = "BListView";
		break;
	case TEXTVIEW_TYPE:
		name = "BTextView";
		break;
	case BOX_TYPE:
		name = "BBox";
		break;
	case SCROLLBAR_TYPE:
		name = "BSCrollBar";
		break;
	case STRINGVIEW_TYPE:
		name = "BStringView";
		break;
	case STATUSBAR_TYPE:
		name = "BStatusBar";
		break;
	case MENUBAR_TYPE:
		name = "BMenuBar";
		break;
	case MENU_TYPE:
		name = "BMenu";
		break;
	case POPUPMENU_TYPE:
		name = "BPopUpMenu";
		break;
	// container
	case CONTAINER_TYPE:
		name = "Container";
		break;
	// controle
	case CONTROL_TYPE:
		name = "BControl";
		break;
	case BUTTON_TYPE:
		name = "BButton";
		break;
	case RODIOBUTTON_TYPE:
		name = "BRadioButton";
		break;
	case CHECKBOX_TYPE:
		name = "BCheckBox";
		break;
	case SLIDER_TYPE:
		name = "BSlider";
		break;
	case TEXTCONTROL_TYPE:
		name = "BTextControl";
		break;
	case MENUFIELD_TYPE:
		name = "BMenuField";
		break;
	// autre
	case MENUITEM_TYPE:
		name = "BMenuItem";	
		break;
	case LISTITEM_TYPE:
		name = "BListItem";
		break;
	case SHELF_TYPE:
		name = "BShelf";
		break;
	case BITMAP_TYPE:
		name = "Bitmap";
		break; 
	case OTHER_TYPE:
		name = g_Languages->GetTranslationFor(L_ADDON_OTHER_TYPE);
		break;	
	default:
		name = g_Languages->GetTranslationFor(L_ADDON_UNKNOW_TYPE);
	}

	return	name;
}

/**** bloquer ou pas la creation de nouvel objet ****/
void AddOnManagerItem::Lock(bool value)
{
	// assigner la valeur
	_lock = value;
	
	// puis modifier l'accesibilité
	if(_lock)
		_available = false;
}

/**** veifier si on peux creer cet object ****/
bool AddOnManagerItem::CheckDepencies(object_type type)
{
	// chercher dans notre liste de dependance
	int32		typeFind;
	int32		index = 0;
	
	// chercher si on a le bon type par rapport aux dependances
	while(_settings.FindInt32(VOBJECT_DEPEND_ON,index,&typeFind)==B_OK)
	{
		if(typeFind == type)
			return true;
		
		// prochaine données
		index++;
	}
	
	// pas trouvé
	return false;
}

/**** rafraichir les preferences ****/
void AddOnManagerItem::RefreshPreferences()
{
	rgb_color tempColor1 = {200,200,255,255};
	_itemColor = g_ColorsPrefs->GetRGBColor(tempColor1,"list-color");
	rgb_color tempColor2 = {230,230,230,255};
	_unAvailableColor = g_ColorsPrefs->GetRGBColor(tempColor2,"unAvailable-color");
	rgb_color tempColor3 = {230,0,0,255};
	_unAvailableTextColor = g_ColorsPrefs->GetRGBColor(tempColor3,"unAvailable-text-color");
}

/*********************/
/* MeTOS tools class */
/*********************/
#include "PaintToColor.h"
#include "MetosMessageDefs.h"
#include "VObject.h"
#include "GUIManager.h"
#include "VPreferenceFile.h"
#include "VPreference.h"
#include "CColorMenuItem.h"
#include <app/Application.h>
#include <interface/PopUpMenu.h>
#include <interface/MenuItem.h>
#include <interface/Window.h>

extern "C" __declspec(dllexport) VTool *instantiate_VTool(image_id addonId,BEntry *entry);

/**** function de creation de l'objet pour l'architecture Addon ****/
VTool *instantiate_VTool(image_id addonId,BEntry *entry)
{
	// creer le VTool apres MeTOS va ajouter le controle
	// puis charger l'image du ressource etc....
	return (new PaintToColor(addonId,entry));
}

/**** constructeur ****/
PaintToColor::PaintToColor(image_id addonId,BEntry *entry)
: VTool("PaintToColor",addonId,entry)
{
	// initialisation
	_application = BMessenger(be_app);
	_toolColor = (rgb_color){216,216,216,255};
	_colorMenu = NULL;

	// preferences
	RefreshPreferences();
}

/**** destructeur ****/
PaintToColor::~PaintToColor()
{
	// liberer la memoire
	delete _colorMenu;
}

/**** gestion des messages ****/
void PaintToColor::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
	// changer de couleur
	case PAINTTOCOLOR_SELECT_COLOR:
		ChangeColor(message);
		break;
	default:
		VTool::MessageReceived(message);
	}
}

/**** lancer l'action ****/
void PaintToColor::DoAction(BPoint *where)
{
	BList		*selectedObject = NULL;
	VObject		*object = NULL;
	BView		*view = NULL;
	int32		nbObjects;
	
	// il y a bien un objet selectionne
	selectedObject = g_GUIManagerView->ObjectList();
	if(selectedObject->CountItems()<2)
		return;
		
	// on deplace tous les objets
	nbObjects = selectedObject->CountItems();
	for(int32 i=1;i<nbObjects;i++)
	{
		object = (VObject *)(selectedObject->ItemAt(i));
		view = g_GUIManagerView->ViewFromObject(object);

		// on peux avoir NULL si on a une fenetre, une appli un projet ou autre
		if(!object->IsLock() && view!=NULL)
		{
			BMessage	newProperties(M_OBJECT_CHANGE_MSG);
			uint32		color;

			// transformer la couleur
 			((uint8 *)(&color))[3] = _toolColor.red;
 			((uint8 *)(&color))[2] = _toolColor.green;
			((uint8 *)(&color))[1] = _toolColor.blue;
 			((uint8 *)(&color))[0] = _toolColor.alpha;
			
			// envoyer le message de modif
			newProperties.AddBool(M_NEED_UNDO_ACTION,true);
			newProperties.AddPointer(M_HANDLER_SELECTED,(BHandler *)view);		
			newProperties.AddInt32(M_PROPERTIES_INDEX,2);
			newProperties.AddInt32("_color",color);
			_application.SendMessage(&newProperties);

			// vider le message
			newProperties.MakeEmpty();		
		}
	}
}

/**** bouton droit ****/
void PaintToColor::SelectTools(BPoint *where)
{
	// le popup doit biensure exister
	if(_colorMenu==NULL)
		return;
		
	// on va afficher le popup
	BWindow			*window = Window();
	BMessage		selection(PAINTTOCOLOR_SELECT_COLOR);
	CColorMenuItem	*item = NULL;
	
	// lancer le popup
	item = (CColorMenuItem *)(_colorMenu->Go(*where));
	if(item!=NULL && window!=NULL)
	{
		selection.AddInt32("index",_colorMenu->IndexOf(item));
		window->PostMessage(&selection,this);
	}
}

/**** changer la couleur courante ****/
void PaintToColor::ChangeColor(BMessage *message)
{
	int32		index = -1;
	
	// trouver l'index
	if(message->FindInt32("index",&index)!=B_OK && index>-1)
		return;
		
	// recuperer la couleur
	_toolColor = ((CColorMenuItem *)(_colorMenu->ItemAt(index)))->RectColor();
	Invalidate();
}

/**** dessin personnalisé ****/
void PaintToColor::DrawTool(BRect bounds)
{
	// dessin de l'icon
	VTool::DrawTool(bounds);
	
	// mettre le carré de couleur
	SetHighColor(_toolColor);
	FillRect(BRect(bounds.left+16,bounds.top+16,bounds.right-4,bounds.bottom-4));
}

/**** gestion des preferences ****/
void PaintToColor::RefreshPreferences()
{
	VPreferenceFile		prefsTools("PaintToColor",TOOLS_GROUP_PATH);
	int32				index;
	rgb_color			RGBColor;
	uint32				color;
	BString				label;
	
	// charger
	prefsTools.Load();
	
	// effacer l'ancien menu
	if(_colorMenu!=NULL)
		delete _colorMenu;
		
	// creer le menu a nouveau et ajouter la couleur
	// par defaut
	_colorMenu = new BPopUpMenu("color-menu");
	_colorMenu->AddItem(new CColorMenuItem((rgb_color){216,216,216,255},"r:216 g:216 b:216 a:255",NULL));	
	
	// ajouter les items	
	index = 0;
	while(prefsTools.FindInt32("color",index,(int32 *)&color)==B_OK)
	{
		// convertir la couleur
		UintToRGB(color,RGBColor);
		
		// creer le label
		label = "r:";
		label << (int32)RGBColor.red << " g:" << (int32)RGBColor.green << " b:" << (int32)RGBColor.blue << " a:" << (int32)RGBColor.alpha;
		
		// ajouter l'item
		_colorMenu->AddItem(new CColorMenuItem(RGBColor,label.String(),NULL));	
		index++;
	}
}

// =============================
// = Transformation de couleur =
// =============================

/**** transormer un uint32 en rgb_color ****/
void PaintToColor::UintToRGB(uint32 &color,rgb_color &rgbColor)
{
	 rgbColor.red = ((uint8 *)(&color))[3];
	 rgbColor.green = ((uint8 *)(&color))[2];
	 rgbColor.blue = ((uint8 *)(&color))[1];
	 rgbColor.alpha = ((uint8 *)(&color))[0];
}

/**** transormer un rgb_color en uint32 ****/
void PaintToColor::RGBToUint(rgb_color &rgbColor,uint32 &color)
{
	 ((uint8 *)(&color))[3] = rgbColor.red;
	 ((uint8 *)(&color))[2] = rgbColor.green;
	 ((uint8 *)(&color))[1] = rgbColor.blue;
	 ((uint8 *)(&color))[0] = rgbColor.alpha;
}

/*********************/
/* MeTOS tools class */
/*********************/
#include "AlignObjects.h"
#include "MetosMessageDefs.h"
#include "MetosGlobalDefs.h"
#include "VPreferenceFile.h"
#include "VObject.h"
#include "GUIManager.h"
#include <app/Application.h>
#include <interface/Window.h>
#include <interface/PopUpMenu.h>
#include <interface/MenuItem.h>
#include <interface/Bitmap.h>
#include <interface/View.h>
#include <storage/Entry.h>
#include <app/Roster.h>
#include <storage/Path.h>

extern "C" __declspec(dllexport) VTool *instantiate_VTool(image_id addonId,BEntry *entry);

/**** function de creation de l'objet pour l'architecture Addon ****/
VTool *instantiate_VTool(image_id addonId,BEntry *entry)
{
	// creer le VTool apres MeTOS va ajouter le controle
	// puis charger l'image du ressource etc....
	return (new AlignObjects(addonId,entry));
}

AlignObjects::AlignObjects(image_id addonId,BEntry *entry)
: VTool("AlignObjects",addonId,entry)
{
	// initialisation
	_displayedBitmap = NULL;
	_orientation = 0;
	_icon[0] = NULL;
	_icon[1] = NULL;
	_icon[2] = NULL;
	
	// messenger vers l'application
	_application = BMessenger(be_app);

	// menu popup
	_orientationMenu = new BPopUpMenu("orientation");
	_orientationMenu->AddItem(new BMenuItem("Top",NULL));
	_orientationMenu->AddItem(new BMenuItem("Bottom",NULL));
	_orientationMenu->AddItem(new BMenuItem("Left",NULL));
	_orientationMenu->AddItem(new BMenuItem("Right",NULL));
	
	// charger les images
	LoadBitmap();
	
	// charger les preferences
	RefreshPreferences();
}

/**** destructeur ****/
AlignObjects::~AlignObjects()
{
	// liberer la memoire
	delete _orientationMenu;
	delete _icon[0];
	delete _icon[1];
	delete _icon[2];
}

/**** gestion des messages ****/
void AlignObjects::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
	// changer de couleur
	case ALIGNOBECTS_ORIENTATION:
		ChangeOrientation(message);
		break;
	default:
		VTool::MessageReceived(message);
	}
}

/**** resizer les objets ****/
void AlignObjects::DoAction(BPoint *where)
{
	BMessage	newProperties(M_OBJECT_CHANGE_MSG);
	BList		*selectedObject = NULL;
	VObject		*object = NULL;
	BView		*view = NULL;
	int32		nbObjects;
	BRect		bounds;
	BRect		frame;
	BPoint		destination;
	float		x;
	float		y;
	
	// il y a bien au moins 2 objets selectionnés
	selectedObject = g_GUIManagerView->ObjectList();
	if(selectedObject->CountItems()<2)
		return;
		
	// recuperer la vue selectionnée (de l'objet)
	object = (VObject *)(selectedObject->ItemAt(0));
	view = g_GUIManagerView->ViewFromObject(object);
	if(view==NULL)
		return;
	
	// on doit avoir une vue
	if(view->LockLooper())
	{
		bounds = view->Bounds();
		view->ConvertToScreen(&bounds);
		view->UnlockLooper();

		// on deplace tous les objets
		nbObjects = selectedObject->CountItems();
		for(int32 i=1;i<nbObjects;i++)
		{
			object = (VObject *)(selectedObject->ItemAt(i));
			view = g_GUIManagerView->ViewFromObject(object);

			// on peux avoir NULL si on a une fenetre, une appli un projet ou autre
			if(!object->IsLock() && view!=NULL && view->Parent()!=NULL)
			{
				// recuperer les donnees
				if(view->LockLooper())
				{
					frame = view->Frame();			
					destination = view->ConvertFromScreen(bounds.LeftTop());
					view->ConvertToParent(&destination);
					view->UnlockLooper();
			
					// initialiser avec la position actuelle de l'objet
					x = frame.left;
					y = frame.top;
			
					// selon le cas on va calculer le rectangle de destination
					switch(_orientation)
					{
					// haut
					case 0:
						y = destination.y;
						break;
					// bas
					case 1:
						y = destination.y - (frame.Height() - bounds.Height());
						break;
					// gauche
					case 2:
						x = destination.x;
						break;
					// cas 3 droite
					default:
						x = destination.x - (frame.Width() - bounds.Width());
					}

					// redimentionner
					g_GUIManagerView->MoveObjectTo(object,x,y);
				
					// recuperer la position de l'objet
					if(view->LockLooper())
					{
						frame = view->Frame();			
						view->UnlockLooper();

						// envoyer le message de modif
						newProperties.AddBool(M_NEED_UNDO_ACTION,true);
						newProperties.AddPointer(M_HANDLER_SELECTED,(BHandler *)view);		
						newProperties.AddRect("_frame",frame);
						_application.SendMessage(&newProperties);

						// vider le message
						newProperties.MakeEmpty();		
					}
				}
			}
		}
	}
}

/**** selection de l'orientation ****/
void AlignObjects::SelectTools(BPoint *where)
{
	// on va afficher le popup
	BWindow			*window = Window();
	BMessage		selection(ALIGNOBECTS_ORIENTATION);
	BMenuItem		*item = NULL;
	
	// lancer le popup
	item = _orientationMenu->Go(*where);
	if(item!=NULL && window!=NULL)
	{
		selection.AddInt32("index",_orientationMenu->IndexOf(item));
		window->PostMessage(&selection,this);
	}
}

/**** dessin personnalisé ****/
void AlignObjects::DrawTool(BRect bounds)
{
	// dessin de l'icon (position top)
	if(_displayedBitmap==NULL)
		VTool::DrawTool(bounds);
	else
	{
		// dessin bitmap
		SetDrawingMode(B_OP_ALPHA);
		DrawBitmap(_displayedBitmap,BPoint(bounds.left+1,bounds.top+1));
		SetDrawingMode(B_OP_COPY);	
	}
}

/**** changer l'orientation ****/
void AlignObjects::ChangeOrientation(BMessage *message)
{
	int32		index = -1;
	
	// trouver l'index
	if(message->FindInt32("index",&index)!=B_OK && index>-1)
		return;

	// trouver l'image
	_displayedBitmap = NULL;
	if(index>0)
		_displayedBitmap = _icon[index-1];

	// renseigner l'orientation
	_orientation = index;
}

/**** charger les icons ****/
void AlignObjects::LoadBitmap()
{
	// on va se baser sur l'endroit ou se trouve l'application
	BString		appPath;
	BPath		path;
	BFile		file;
	BEntry		entry;
	app_info	info;

	// initialiser le fichier ressource
	// et le chemin de l'appli
	be_app->GetAppInfo(&info);
	path.SetTo(&(info.ref)); 
    path.GetParent(&path);
    appPath = path.Path();
	appPath << "/" << ADDON_FOLDER << "/" << ADDON_TOOLS_FOLDER << "/AlignObjects"; 
	file.SetTo(appPath.String(), B_READ_ONLY);
	_ressource.SetTo(&file);

	// recuperer les images
	_icon[0] = GetBitmap("AlignBottom");
	_icon[1] = GetBitmap("AlignLeft");
	_icon[2] = GetBitmap("AlignRight");
}

/**** trouver une image dans les ressources ****/
BBitmap *AlignObjects::GetBitmap(const char *name)
{
	BBitmap 	*bitmap = NULL;
	size_t 		len = 0;
	status_t 	error;	

	// charger depuis les ressources
	const void *data = _ressource.LoadResource('BBMP', name, &len);

	BMemoryIO stream(data, len);
	
	// charge l'image archivé
	BMessage archive;
	error = archive.Unflatten(&stream);
	if (error != B_OK)
		return NULL;

	// on va essayer de la recreer
	bitmap = new BBitmap(&archive);
	if(!bitmap)
		return NULL;

	// verifier que ca a marché
	if(bitmap->InitCheck() != B_OK)
	{
		delete bitmap;
		return NULL;
	}
	
	// tout c'est bien passé
	return bitmap;
}

/**** gestion des preferences ****/
void AlignObjects::RefreshPreferences()
{
	_stepGrid = g_MeTOSPrefs->GetInt32(10,"grid-step");
}

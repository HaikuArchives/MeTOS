/*********************/
/* MeTOS tools class */
/*********************/
#include "EqualSize.h"
#include "MetosMessageDefs.h"
#include "VPreferenceFile.h"
#include "VObject.h"
#include "GUIManager.h"
#include "MetosGlobalDefs.h"
#include <app/Application.h>
#include <interface/View.h>

extern "C" __declspec(dllexport) VTool *instantiate_VTool(image_id addonId,BEntry *entry);

/**** function de creation de l'objet pour l'architecture Addon ****/
VTool *instantiate_VTool(image_id addonId,BEntry *entry)
{
	// creer le VTool apres MeTOS va ajouter le controle
	// puis charger l'image du ressource etc....
	return (new EqualSize(addonId,entry));
}

EqualSize::EqualSize(image_id addonId,BEntry *entry)
: VTool("EqualSize",addonId,entry)
{
	// messenger vers l'application
	_application = BMessenger(be_app);
	
	// charger les preferences
	RefreshPreferences();
}

/**** resizer les objets ****/
void EqualSize::DoAction(BPoint *where)
{
	BMessage	newProperties(M_OBJECT_CHANGE_MSG);
	BList		*selectedObject = NULL;
	VObject		*object = NULL;
	BView		*view = NULL;
	int32		nbObjects;
	BRect		bounds;
	BRect		frame;
	
	// il y a bien un objet selectionne
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
		view->UnlockLooper();

		// on deplace tous les objets
		nbObjects = selectedObject->CountItems();
		for(int32 i=1;i<nbObjects;i++)
		{
			object = (VObject *)(selectedObject->ItemAt(i));
			view = g_GUIManagerView->ViewFromObject(object);

			// on peux avoir NULL si on a une fenetre, une appli un projet ou autre
			if(!object->IsLock() && view!=NULL)
			{
				// redimentionner
				g_GUIManagerView->ResizeObjectTo(object,bounds.Width(),bounds.Height());
				
				// recuperer la taille de l'objet
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

/**** gestion des preferences ****/
void EqualSize::RefreshPreferences()
{
	_stepGrid = g_MeTOSPrefs->GetInt32(10,"grid-step");
}

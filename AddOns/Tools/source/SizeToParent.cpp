/*********************/
/* MeTOS tools class */
/*********************/
#include "SizeToParent.h"
#include "MetosMessageDefs.h"
#include "VPreferenceFile.h"
#include "VObject.h"
#include "GUIManager.h"
#include "MetosGlobalDefs.h"
#include <app/Application.h>
#include <interface/View.h>
#include <stdio.h>

extern "C" __declspec(dllexport) VTool *instantiate_VTool(image_id addonId,BEntry *entry);

/**** function de creation de l'objet pour l'architecture Addon ****/
VTool *instantiate_VTool(image_id addonId,BEntry *entry)
{
	// creer le VTool apres MeTOS va ajouter le controle
	// puis charger l'image du ressource etc....
	return (new SizeToParent(addonId,entry));
}

SizeToParent::SizeToParent(image_id addonId,BEntry *entry)
: VTool("SizeToParent",addonId,entry)
{
	// messenger vers l'application
	_application = BMessenger(be_app);
	
	// charger les preferences
	RefreshPreferences();
}

/**** resizer les objets par rapport a leur parent ****/
void SizeToParent::DoAction(BPoint *where)
{
	BMessage	newProperties(M_OBJECT_CHANGE_MSG);
	BList		*selectedObject = NULL;
	VObject		*object = NULL;
	BView		*view = NULL;
	BView		*parentView = NULL;
	int32		nbObjects;
	BRect		parentFrame;
	BRect		frame;
	
	// il y a bien un objet selectionne
	selectedObject = g_GUIManagerView->ObjectList();
	if(selectedObject->CountItems()<2)
		return;
		
	// on rezize et deplace tous les objets
	nbObjects = selectedObject->CountItems();
	for(int32 i=1;i<nbObjects;i++)
	{
		object = (VObject *)(selectedObject->ItemAt(i));
		view = g_GUIManagerView->ViewFromObject(object);

		// on peux avoir NULL si on a une fenetre, une appli un projet ou autre
		if(!object->IsLock() && view!=NULL)
		{
			// le parent
			parentView = view->Parent();
			if(parentView==NULL)
				parentView = view;			

			// recuperer la taille du parent
			if(parentView->LockLooper())
			{
				parentFrame = parentView->Bounds();
				parentView->UnlockLooper();

				// redimentionner puis placer a 0.0 par rapport au parent
				g_GUIManagerView->ResizeObjectTo(object,parentFrame.Width(),parentFrame.Height());
				g_GUIManagerView->MoveObjectTo(object,0,0);

				// recuperer la taille de l'objet modifié
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
void SizeToParent::RefreshPreferences()
{
	_stepGrid = g_MeTOSPrefs->GetInt32(10,"grid-step");
}

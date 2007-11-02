/*********************/
/* MeTOS tools class */
/*********************/
#include "DeleteObject.h"
#include "MetosMessageDefs.h"
#include "VObject.h"
#include "GUIManager.h"
#include <app/Application.h>

extern "C" __declspec(dllexport) VTool *instantiate_VTool(image_id addonId,BEntry *entry);

/**** function de creation de l'objet pour l'architecture Addon ****/
VTool *instantiate_VTool(image_id addonId,BEntry *entry)
{
	// creer le VTool apres MeTOS va ajouter le controle
	// puis charger l'image du ressource etc....
	return (new DeleteObject(addonId,entry));
}

DeleteObject::DeleteObject(image_id addonId,BEntry *entry)
: VTool("DeleteObject",addonId,entry)
{
	_application = BMessenger(be_app);
}

void DeleteObject::DoAction(BPoint *where)
{
	BList		*selectedObject = NULL;

	// il y a bien un objet selectionne
	selectedObject = g_GUIManagerView->ObjectList();
	if(selectedObject->CountItems()>0)
		_application.SendMessage(M_DELETE_OBJECT_MSG);	
}

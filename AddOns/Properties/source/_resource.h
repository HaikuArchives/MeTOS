#ifndef __RESOURCE_H
#define __RESOURCE_H
/************************************/
/* Addon de la proprietes _resource */
/************************************/
#include "VPropertie.h"
#include <support/String.h>
#include <interface/TextControl.h>

// on a besoin d'une vue pour recevoir le message du file panel
class ResourceEditor : public BTextControl
{
public:
	ResourceEditor();
	
			void		MessageReceived(BMessage *message);
};

// classe principale
class _Resource : public VPropertie
{
public:
	_Resource(entry_ref *entry);
	~_Resource();

			VPropertie* InstantiateVPropertie(entry_ref *entry);						// clonage
			void		Draw(BView *owner,BRect bounds,bool complete = false);			// permet a l'utilisateur de dessiner ce qu'il veux
			void		SetProperties(BMessage *properties);							// acces aux proprietes
			void		Properties(BMessage *objectProperties);							// modifier les proprietes
			void		SelectItem(BView *owner,BRect bounds);							// selectionne l'item
			void		UnSelectItem(BView *owner,BRect bounds);						// deselectionne l'item
			void		ValueAsString(BMessage *properties,BString &value,int32 index);	// recuperer la valeur sour forme de chaine

private:
	BString				_resource;
	ResourceEditor		*_editor;
};

#endif

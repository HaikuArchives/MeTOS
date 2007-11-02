/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _OBJECTMANAGERSLISTVIEW_H
#define _OBJECTMANAGERSLISTVIEW_H
/*************************/
/* MeTOS list view class */
/*************************/
#include <interface/ListView.h>
#include <support/String.h>

class VObject;

class ObjectManagerListView : public BListView
{
public:
	ObjectManagerListView(BRect,const char *);
	virtual ~ObjectManagerListView();

	virtual void		MessageReceived(BMessage *);			// traitement des messages
	virtual void		MouseDown(BPoint );						// gestion souris
	virtual	void 		Draw(BRect updateRect);					// affichage les proprietes ou rien
	virtual	void		FrameResized(float width,float height);	// on redimentionne la vue
	
			void		SelectObject(VObject *select,bool shift);						// selectionner l'objet dans la liste
			bool		ObjectIsSelected(VObject *selected);							// trouver si un objet est selectionné
			VObject		*FindObjectFromHandler(BHandler *handler);						// trouver un objet a partir d'un handler
			void		AddVObject(VObject *newObject,VObject *dest,int32 level = 0);	// ajouter un objet a la liste
			int32		FindIndexOfObject(VObject *object);								// trouver l'index d'un objet
			VObject		*FindObjectByID(uint32 id);										// trouver l'objet par rapport a l'id
			int32		FindPlaceForNewObject(VObject *destination);					// trouver ou ajouter un objet
			status_t	ChangeLevel(int32 decal,VObject *object);						// changer le level des objets a partir de l'objet en parametres

protected:
	BString		_display;
};

#endif

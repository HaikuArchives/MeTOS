/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _UNDOMANAGERSLISTVIEW_H
#define _UNDOMANAGERSLISTVIEW_H
/*****************************/
/* MeTOS undo listview class */
/*****************************/

#include <interface/ListView.h>

class BPopUpMenu;

class UndoManagerListView : public BListView
{
public:
	UndoManagerListView(BRect,const char *);
	virtual ~UndoManagerListView();

	virtual void		MessageReceived(BMessage *);
	virtual void		MouseDown(BPoint );
			void		RestoreItem(int32 index = -1);						// Restorer l'action (faire le undo)
			void		DeleteItem(int32 index = -1);						// Effacer le undo de la liste

private:
	BPopUpMenu 			*_actionMenu;
	
			void	SelectItem(int32 index,uint32 button,BPoint &where);	// sectionner un item
			void	DeselectItem(int32 index);								// desectionner un item
			void	BuildMenu();											// construire le popup
			void	ExpandItem();											// etendre l'item (la taille)
};

#endif

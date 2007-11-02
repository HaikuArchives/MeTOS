/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _ADDONMANAGERSLISTVIEW_H
#define _ADDONMANAGERSLISTVIEW_H
/**********************************/
/* list des addons (objets) class */
/**********************************/
#include <interface/OutlineListView.h>

class BPopUpMenu;
class AddOnManagerItem;
class AddOnManagerWin;

class AddOnManagerListView : public BOutlineListView
{
public:
	AddOnManagerListView(BRect,const char *,AddOnManagerWin *support);
	virtual ~AddOnManagerListView();

	virtual void	MessageReceived(BMessage *);
	virtual void	MouseDown(BPoint );
	virtual	void	DrawLatch(BRect itemRect, int32 level, bool collapsed,bool highlighted, bool misTracked);	// le petit triangle dans notre cas on met rien !!

private:
	AddOnManagerWin		*_supportWindow;
	BPopUpMenu			*_option;

			void	DisplayPopUp(BPoint &);					// afficher le popup
			void	BuildMenuPoPup();						// creer le menu popoup
			void	BuildDragDrop();						// creer le message de drag & drop
			bool	FindDependencies(AddOnManagerItem *);	// trouver si on a les bonnes dependances
};

#endif

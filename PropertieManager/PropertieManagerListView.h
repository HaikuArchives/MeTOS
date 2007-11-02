/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _PROPERTIEMANAGERLISTVIEW_H
#define _PROPERTIEMANAGERLISTVIEW_H
/*************************/
/* MeTOS list view class */
/*************************/
#include <interface/ListView.h>
#include <support/String.h>

class CColumnView;
class PropertieManagerWin;

class PropertieManagerListView : public BListView
{
public:
	PropertieManagerListView(BRect,const char *,PropertieManagerWin *support);
	virtual ~PropertieManagerListView();

	virtual void	MessageReceived(BMessage *);
	virtual void	AttachedToWindow();						// attachement a la fenetre
	virtual	void	DetachedFromWindow();					// dettachement de la fenetre
	virtual void	MouseDown(BPoint );						// gestion de la souris
	virtual	void	WindowActivated(bool state);			// activation de la fenetre
	virtual	void 	Draw(BRect updateRect);					// affichage les proprietes ou rien
	virtual	void	FrameResized(float width,float height);	// on redimentionne la vue
			void	SelectItem();							// on selectionne un item de la liste
			void	UnSelectItem();							// on deselectionne un item	
			void	ManageKeyUp(BMessage *);				// gerer le clavier avant reception par un objet graphique d'un item

protected:
	PropertieManagerWin		*_supportWindow;
	CColumnView				*_columnView;
	BString					_display;
};

#endif

/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _CODEMANAGERSLISTVIEW_H
#define _CODEMANAGERSLISTVIEW_H
/*****************************/
/* MeTOS code listview class */
/*****************************/
#include <interface/ListView.h>
#include <support/String.h>

class CColumnView;
class BPopUpMenu;

class CPPCodeListView : public BListView
{
public:
	CPPCodeListView(BRect,const char *);
	~CPPCodeListView();

	virtual void		MessageReceived(BMessage *);				// gerer les messages
	virtual	void		AttachedToWindow();							// attachement a la fenetre
	virtual	void		MouseDown(BPoint where);					// clique souris
	virtual	void		Draw(BRect updateRect);						// afficher les items ou un message
	virtual void		FrameResized(float width,float height);		// on redimentionne la vue

protected:
	CColumnView		*_columnView;	// vue des colonnes	
	BPopUpMenu		*_popUpMenu;	// popup de selection du code a voir
	BString			_display;		// message affiché
	
			void		BuildPopUpMenu();			// creation du menu popup
};

#endif
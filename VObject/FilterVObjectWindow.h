/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _FILTERVOBJECTWINDOW_H
#define _FILTERVOBJECTWINDOW_H
/**********************/
/* WindowMessageFilter */
/**********************/
#include <app/MessageFilter.h>
#include <app/Messenger.h>

class FilterVObjectWindow : public BMessageFilter
{
public:
	FilterVObjectWindow();
	virtual ~FilterVObjectWindow();
	
	virtual filter_result Filter(BMessage* message, BHandler **target);

private:
			filter_result	KeyDown(BMessage *message);							// intercepter les touches Ctrl
			void			DropObject(BMessage *message,BHandler *target);		// on drag & drop addon d'objet
			filter_result	MouseDown(BMessage *message,BHandler *target);		// on clique sur la fenetre
			filter_result	WindowMoved(BMessage *message);						// on bouge la fenetre
			filter_result	WindowResized(BMessage *message);					// on resize la fenetre
			bool			WindowQuit(BMessage *message);						// on veux quitter la fenetre
};

#endif

/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _WINDOWSUPPORTVOBJECTVIEW_H
#define _WINDOWSUPPORTVOBJECTVIEW_H
/***************************************/
/* WindowSupportVObjectView view class */
/***************************************/
// c'est la vue que l'on ajoute a une fenetre
// pour gerer les objets
#include <interface/View.h>

class MetosApp;
class BList;
class FilterVObjectWindow;

class WindowSupportVObjectView : public BView
{
public:
	WindowSupportVObjectView(BRect frame);
	virtual ~WindowSupportVObjectView();

private:
	MetosApp			*_metosApp;
	FilterVObjectWindow	*_filter;
	rgb_color			_selection;
	rgb_color			_backSelection;
	int32				_useGrid;
	int32				_stepGrid;
	bool				_metosActivation;
	
			void		DrawBox(BView *);
	virtual	void		MessageReceived(BMessage *message);
	virtual	void		AttachedToWindow();
	virtual	void		DetachedFromWindow();
	virtual void		Draw(BRect frame);
	virtual void		DrawAfterChildren(BRect frame);
	virtual	void		WindowActivated(bool state);
			void		RefreshPreferences();
};

#endif

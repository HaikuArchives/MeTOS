/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _METOABOUTWIN_H
#define _METOABOUTWIN_H
/**************************************/
/* fenetre A propos de  l'application */
/**************************************/
#include <interface/Window.h>

class MetosAboutWin : public BWindow
{
public:
	MetosAboutWin(BRect);
	virtual	~MetosAboutWin();

	virtual bool		QuitRequested();		// fermer la fenetre
};

#endif


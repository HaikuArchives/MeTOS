/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _TIPSMANAGERWIN_H
#define _TIPSMANAGERWIN_H
/********************/
/* fenetre des Tips */
/********************/
#include <interface/Window.h>

// constantes
#define		VTIPS_CLOSE_WINDOW_MSG		'Vtcw'
#define		VTIPS_NEXT_TIPS_MSG			'Vntm'

class TipsManagerBox;
class BView;
class BCheckBox;
class BButton;

class TipsManagerWin : public BWindow
{
// fonctions
public:
	TipsManagerWin(BRect frame);
	virtual ~TipsManagerWin();

	virtual void		MessageReceived(BMessage *message);		// gestion des messages
	virtual	bool		QuitRequested();						// quitter la fenetre

protected:
	TipsManagerBox		*_tipsManagerBox;
	BView				*_supportView;
	BCheckBox			*_showAtStartUp;	
	BButton				*_closeWin;
	BButton				*_nextTip;
	
			BRect		GetFrame();			// ou placer la fenetre
			void		SavePreferences();	// sauver les preferences
};

#endif

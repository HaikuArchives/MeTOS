/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _TIPSMANAGERBOX_H
#define _TIPSMANAGERBOX_H
/****************/
/* Box des Tips */
/****************/
#include <interface/Box.h>
#include <support/List.h>
#include <support/String.h>

class BBitmap;
class BTextView;

class TipsManagerBox : public BBox
{
// fonctions
public:
	TipsManagerBox(BRect frame);
	virtual ~TipsManagerBox();

	virtual	void	MessageReceived(BMessage *message);		// gestion des messages
	virtual	void	Draw(BRect updateRect);					// dessin

protected:
	BBitmap		*_tipsIcon;			// icon
	BTextView	*_tipsText;			// text view pour aficher le tips
	BList		_tipsList;			// liste des tips
	BList		_displayedTips;		// tips deja affiché
	BString		_dayString;			// pti titre du jour
	
			status_t	LoadTips();				// charger l'ensemble des tips
			void		DisplayNextTips();		// afficher le prochain tips
			int32		RandomIndex();			// trouver un index aleatoirement
			void		CheckMeTOSTips();		// verifier si on a les tips MeTOS
			void		SetDayInfo();			// on va definir le jour et l'icon
};

#endif

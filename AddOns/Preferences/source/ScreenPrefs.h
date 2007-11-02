#ifndef _SCREENPREFS_H
#define _SCREENPREFS_H
/*****************************/
/* MeTOS preference class    */
/* pour la taille de l'ecran */
/*****************************/
#include "VPreference.h"
#include <Box.h>

class BMenuField;

// gestion des preferences de l'ecran
class ScreenPrefs : public VPreference
{
public:
	ScreenPrefs(BRect frame);

			void		AttachedToWindow();
			void		MessageReceived(BMessage *message);
			void		SavePrefs();						// sauver les preferences
	
protected:
	BMenuField		*_screenField;
	int32			_screenSize;

			void		SetScreenSize(BMessage *message);
};

// classe special pour dessiner un ecran
class ScreenBox : public BBox
{
public:
	ScreenBox(BRect frame,const char *name);
	
	virtual	void		Draw(BRect updateRect); 	// dessin de la vue

protected:
	BRect	_screenRect;
};

#endif
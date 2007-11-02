/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _METOSBACKGROUNDWIN_H
#define _METOSBACKGROUNDWIN_H
/***************************/
/* fenetre du fond d'ecran */
/***************************/
#include <interface/Window.h>
#include <support/String.h>

class BView;
class BBitmap;

class MetosBackGroundWin : public BWindow
{
// attributs
protected:

// fonctions
public:
	MetosBackGroundWin();
	virtual ~MetosBackGroundWin();

	virtual void	MessageReceived(BMessage *);

private:
	BString		_bitmapPath;		// chemin de l'image de fond
	rgb_color	_backgroundColor;	// couleur de fond
	BView		*_supportView;		// vue support
	BBitmap		*_background;		// image de fond si il y en a une

	BRect		ScreenFrame();							// determiner la taille de l'ecran
	void		ManageDragAndDrop(BMessage *message);	// gerer le drag&drop
	void		RefreshPreferences();					// rafraichir les preferences	
};

#endif

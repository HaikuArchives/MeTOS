/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _TOOLMANAGERWINDOW_H
#define _TOOLMANAGERWINDOW_H
/**********************/
/* fenetre des outils */
/**********************/
#include <interface/Window.h>

class BView;
class VObject;
class BList;
class BMessenger;

class ToolManagerWin : public BWindow
{
// fonctions
public:
	BView	*_toolsManagerView;

	ToolManagerWin(BRect);
	virtual ~ToolManagerWin();

	virtual void	MessageReceived(BMessage *);
	virtual bool	QuitRequested();
			void	LoadAddOns();							// charger l'ensemble des AddOns

	inline	BList	*ToolsList()		{ return _toolsList; }		// pour acceder a la liste des outils

private:
	BList		*_toolsList;		// liste des outils chargé (les addons en fait)
	int32		_nbPerLine;			// nombre d(outils par ligne
		
			void		UnloadAddOns();							// decharger les addons
			status_t	PlaceTools(int32 toolIndex);			// positionner l'outils
			void		CallTools(BMessage *message);			// appeler directement un outils
			void		RefreshPreferences();					// rafraichir les preferences
			void		BuildToolsNameList(BMessage *reply);	// creer la liste des noms des outils
			void		SavePreferences();						// sauver les preferences
};

#endif

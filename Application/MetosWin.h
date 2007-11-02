/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _METOSWINDOW_H
#define _METOSWINDOW_H
/****************************/
/* fenetre de l'application */
/****************************/
#include <interface/Window.h>

class BMenuBar;
class BRect;
class BPoint;
class BListView;
class BBitmap;
class BList;
class BMenu;

class MetosWin : public BWindow
{
// fonctions
public:
	MetosWin(BRect);
	virtual ~MetosWin();

	virtual void	MessageReceived(BMessage *);
	virtual bool	QuitRequested();
			void	ClearMemory();													// netoyer la memoire							
			void	AddGeneratorMenu(BList *generatorList);

	inline	int32		DebugLevel()				{ return _debugLevel; }			// recuperer le niveau de debug
	inline	BBitmap		*InfoBitmap(int32 index)	{ return _infoBitmap[index]; }	// recuperer l'image info

private:
	BListView			*_infoList;			// liste des messages
	BMenuBar			*_menuBar;			// menu de l'application
	BMenu				*_recentLoad;		// menu des elements recent
	BBitmap				*_infoBitmap[4];	// image pour l'item info
	int32				_debugLevel;		// niveau de debug
	int32				_nbMaxDebug;		// nombre max de message d'info
	int8				_maxRecent;			// nombre max de fichier dans les recents

	void		BuildMenu(BRect frame);					// construire le menu de l'application			
	void		BuildRecentMenu(BMenu *destination);	// construire le menu des elements recents
	void		AddProjectToRecent(BMessage *message);	// ajouter un fichier a la liste des fichiers recents
	void		OpenRecent(BMessage *message);			// ouvrir un fichier recent
	void		AddDebugMessage(BMessage *message);		// ajouter un message d'info
	void		RefreshPreferences();					// rafraichir les preferences
	void		SavePreferences();						// sauver les preferences
};

// variable global
extern BWindow				*g_MetosBackWin;

#endif

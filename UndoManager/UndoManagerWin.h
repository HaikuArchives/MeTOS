/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _UNDOMANAGERWINDOW_H
#define _UNDOMANAGERWINDOW_H
/******************************/
/* fenetre de gestion du undo */
/******************************/
#include <interface/Window.h>

class UndoManagerListView;
class BMessenger;
class CStatusView;

class UndoManagerWin : public BWindow
{
// fonctions
public:
	UndoManagerWin(BRect);
	virtual ~UndoManagerWin();

	virtual void		MessageReceived(BMessage *);
	virtual bool		QuitRequested();

			status_t	ArchiveUndo(BMessage *archive);		// archiver la liste des undo
			status_t	InstantiateUndo(BMessage *archive);	// recuperer les undo d'un projet
			void		RefreshDisplayNumberItems();		// mettre a jour le nombre d'items affiché
			void		ClearList();						// vider la liste

protected:
	BView					*_undoManagerView;
	UndoManagerListView		*_undoListView;
	CStatusView				*_statusView;
	// preferences
	int32					_undoMax;
	
			void		AddNewUndoAction(BMessage *message);	// ajouter un item
			void		RefreshPreferences();					// rafraichir les preferences
			void		SavePreferences();						// sauver les preferences
};

#endif

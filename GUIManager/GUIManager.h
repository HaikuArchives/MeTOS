/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _GUIMANAGER_H
#define _GUIMANAGER_H
/**************************/
/* MeTOS GUIManager class */
/**************************/
// cette classe est un Hadler pour la gestion des message
#include <interface/View.h>
#include <interface/Point.h>
#include <app/Messenger.h>

class BList;
class VObject;

// action possible sur un objet
enum action
{
	NO_ACTION		=-1,
	MOVE_OBJECT		= 0,
	RESIZE_OBJECT	= 1
};

// etat du curseur
enum cusrsor_state
{
	BE_STANDARD		= 0,
	MOVE_CURSOR		= 1,
	RESIZE_CURSOR	= 2
};

class GUIManager : public BView
{
public:
	GUIManager();
	virtual ~GUIManager();

	virtual	void		MouseUp(BPoint point);
	virtual	void		KeyDown(const char *bytes, int32 numBytes);
	virtual void		MouseMoved(BPoint point,uint32 transit,const BMessage *message);
	virtual void		MessageReceived(BMessage *message);
	
			bool		IsCurrent(BHandler *object);						// est-ce l'objet courant
			bool		IsSelected(BHandler *object);						// est-il dans la liste des selectionnes
			void		InvalidateAll();									// invalider tous les objets selectionnés
			void		RemoveAll();										// vider la liste (et rafraichir les objets)
			void		MoveObjectBy(BPoint &point);						// deplacer les objets selectionnés
			void		ResizeObjectBy(BPoint &point);						// redimentionner les objets selectionnés
			void		MoveObjectTo(VObject *object,float x,float y);		// deplacer un objet vers un rectangle donné
			void		ResizeObjectTo(VObject *object,float h,float v);	// redimentionner un objet a une taille donné
			BView		*ViewFromObject(VObject *object);					// retourne un pointer sur la vue de l'objet (pour une fenetre c'est la vue support)
			void		ClearMemory();										// effacer toute la memoire

	inline	action	Action()							{ return _action; }				// on est en train de faire quoi ?
	inline	BList	*ObjectList()						{ return _selectedObject; }		// liste des objets selectionnés

protected:
	BList			*_selectedObject;
	action			_action;
	BPoint			_actionPoint;
	float			_delta_x;
	float			_delta_y;
	cusrsor_state	_cursorState;
	bool			_firstAction;
	// preferences
	int32			_stepGrid;
	int32			_useGrid;
	int32			_snapGrid;
	int32			_resizeGrid;

			void		RefreshPreferences();					// rafraichir les preferences
			void		DefineCursorDisplay();					// definir l'affichage du curseur
			status_t	LeftTopCornerOfSelected(BPoint &point);	// recuperer le coin haut gauche du premier objet selectionné
			void		SelectObject(BMessage *data);			// selectionner graphiquement un objet
			void		SetAction(BMessage *action);			// savoir quoi faire
};

// gestionnaire des objets graphique en global
extern GUIManager  		*g_GUIManagerView;
extern BMessenger  		g_GUIManager;

#endif

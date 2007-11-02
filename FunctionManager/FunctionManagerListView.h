/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _FUNCTIONMANAGERLISTVIEW_H
#define _FUNCTIONMANAGERLISTVIEW_H
/*************************/
/* MeTOS list view class */
/*************************/
#include <interface/OutlineListView.h>
#include <support/List.h>
#include <support/String.h>

class CSClass;
class VObject;
class BPopUpMenu;
class FunctionItem;
class FunctionManagerItem;
class PropertieManagerWin;

class FunctionManagerListView : public BOutlineListView
{
public:
	FunctionManagerListView(BRect,const char *,PropertieManagerWin *support);
	virtual ~FunctionManagerListView();

	virtual	void	MessageReceived(BMessage *message);															// gestion des messages
	virtual	void	DrawLatch(BRect itemRect, int32 level, bool collapsed,bool highlighted, bool misTracked);	// le petit triangle dans notre cas on met rien !!
	virtual void	MouseDown(BPoint where);																	// gestion clique souris
	virtual	void 	Draw(BRect updateRect);																		// affichage les proprietes ou rien
	virtual	void	FrameResized(float width,float height);														// on redimentionne la vue
			void	DisplayMethodes(FunctionManagerItem *item,BMessage *selected);								// charger et afficher les metodes d'un objet 

protected:
	PropertieManagerWin		*_supportWindow;
	int32					_nbCheckedFunctions;
	BPopUpMenu				*_actionMenu;
	BString					_display;

			void	BuildActionMenu();							// creer le menu d'action de la vue
			void	SaveListAsDefault();						// sauver la liste des fonctions
			void	SelectFunction(FunctionItem *function);		// selectionner une fonction
			void	SelectBestConstructor();					// trouver le meilleur constructeur
			void	CheckSuperItem(FunctionItem *item);			// trouver si un item est selectionné dans un groupe
			void	CheckClassIsDerived();						// verifier et fixer si la classe doit etre derivé
			void	DragAndDropHeader(BMessage *message);		// gerer le drag&drop d'un fichier haeder
};

#endif

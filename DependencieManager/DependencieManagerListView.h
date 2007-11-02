/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _DEPENDENCIEMANAGERLISTVIEW_H
#define _DEPENDENCIEMANAGERLISTVIEW_H
/********************************************/
/* MeTOS dependencieManager list view class */
/********************************************/
#include <interface/ListView.h>
#include <support/List.h>
#include <support/String.h>

class PropertieManagerWin;
class DependencieManagerItem;
class CColumnView;
class BPopUpMenu;
class BTextControl;

class DependencieManagerListView : public BListView
{
public:
	DependencieManagerListView(BRect,const char *,PropertieManagerWin *support);
	virtual ~DependencieManagerListView();

	virtual	void			MessageReceived(BMessage *message);							// gestion des messages
	virtual	void			Draw(BRect updateRect);										// affichage les dependances ou rien
	virtual	void			MouseDown(BPoint where);									// gestion du clique souris
	virtual	void			FrameResized(float width,float height);						// on redimentionne la vue
			void			DisplayDependencies(DependencieManagerItem *dependencies);	// afficher les dependance de cet item
			void			SelectGenerator(const char *ID);							// forcer la selection d'un generateur
			void			RemoveGeneratorMenu();										// enlever le menu des generateurs pour eviter de l'effacer 2 fois

private:
	PropertieManagerWin		*_supportWindow;		// fenetre
	DependencieManagerItem	*_currentManagerItem;	// item courant
	CColumnView				*_columnView;			// vue des colonnes
	BPopUpMenu				*_options;				// menu des options
	BTextControl			*_editor;				// editeur de chemin ou de type Mime
	uint8					_selected;				// selection chemin ou type Mime
	BString					_display;				// message affiché

	virtual	void			AttachedToWindow();									// attachement a la fenetre
	virtual	void			DetachedFromWindow();								// detachement de la fenetre
			void			SelectItem(int32 index,uint32 buttons,float &x);	// selection d'un item
			void			UnselectItem();										// on deselectionne un item
			void			BuildOptionMenu();									// creer le menu popup des options
			void			DropFile(BMessage *message);						// drag&drop
			void			RemoveDependency();									// effacer un element
			void			SaveAsDefault();									// sauver la configuration actuelle
};

#endif

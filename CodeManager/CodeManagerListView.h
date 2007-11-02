/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _CODEMANAGERLISTVIEW_H
#define _CODEMANAGERLISTVIEW_H
/*************************/
/* MeTOS list view class */
/*************************/
#include <interface/ListView.h>
#include <support/List.h>
#include <support/String.h>

class PropertieManagerWin;
class CodeManagerItem;
class VPreferenceFile;
class BPopUpMenu;
class BString;

class CodeManagerListView : public BListView
{
public:
	CodeManagerListView(BRect,const char *,PropertieManagerWin *support);
	~CodeManagerListView();

	virtual void		MessageReceived(BMessage *message);			// gestion des messages
	virtual	void 		Draw(BRect updateRect);						// affichage les proprietes ou rien
	virtual	void		MouseDown(BPoint where);					// gestion du clique souris
	virtual	void		FrameResized(float width,float height);		// on redimentionne la vue

			void			LoadExternalCodeFile(BMessage *message);	// charger le fichier de code externe
			VPreferenceFile	*FindCodeFromClass(const char *className);	// trouver dans notre liste interne le message contenant les codes externes
			void			DisplayCode(const char *className);			// afficher les codes externes et creer les items
			void			AddExternalCodeItem(const char *code);		// ajouter un nouvel item de code
			void			RemoveExternalCodeItem(BMessage *message);	// effacer un item de code

protected:
	PropertieManagerWin		*_supportWindow;	// fenetre support
	BPopUpMenu				*_options;			// menu des options
	BList					_codeListCache;		// liste internes des fichiers codes externes en cache
	BList					_codeItemCache;		// liste des items de code en cache
	char					*_className;		// nom de la classe actuellement gerée
	BString					_display;			// message si il n'y a pas de code

			CodeManagerItem		*CreateCodeItem();					// recuperer un code item du cache ou en creer un nouveau
			void				EmptyCodeList();					// vider la liste des codes et transferer dans le cache
			void				SaveExternalCodeFiles();			// sauver les fichiers de code externe
			void				SetDefaultCode();					// definir l'item comme defaut
			void				SelectCodeFromList(int32 index);	// selectionner un code de la liste
			void				UnSelectCodeFromList();				// Deselectionner un code de la liste
			void				BuildOptionMenu();					// construire le pop-up
};

#endif

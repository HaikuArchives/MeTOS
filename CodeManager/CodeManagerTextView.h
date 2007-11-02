/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _CODEMANAGERTEXTVIEW_H
#define _CODEMANAGERTEXTVIEW_H
/********************************/
/* MeTOS code editor view class */
/********************************/
#include <interface/TextView.h>
#include <app/Message.h>
#include <support/String.h>
#include <support/List.h>

class PropertieManagerWin;
class BPopUpMenu;
class BMenu;

class CodeManagerTextView : public BTextView
{
public:
	CodeManagerTextView(BRect frame,const char *name,BRect textRect,PropertieManagerWin *support);
	virtual ~CodeManagerTextView();
	
	virtual void		MessageReceived(BMessage *message);			// gestion des messages
	virtual void		Draw(BRect updateRect);						// dessin personalisé
	virtual	void		MouseDown(BPoint where);					// gestion du clique souris
	virtual	void		KeyUp(const char *bytes, int32 numBytes);	// gerer le clavier

			void			DisplayCode(BMessage *objectCode);								// affichage du code
			void			BuildPropertiesMenu(BMessage *properties,BMessage *locked);		// creer le menu des proprietes
			void			SelectGenerator(const char *ID);								// forcer la selection d'un generateur
			void			RemoveGeneratorMenu();											// enlever le menu des generateurs pour eviter de l'effacer 2 fois

protected:
	PropertieManagerWin		*_supportWindow;		// fenetre support
	BPopUpMenu				*_options;				// menu des options
	BMenu					*_propertiesMenu;		// menu des proprietes
	BMenu					*_specialProperties;	// menu des proprietes speciales
	BString					_className;				// nom de la classe de l'objet edité
	BString					_display;				// message affiché
	bool					_isEmpty;				// savoir si c'est vide

	virtual	void			AttachedToWindow();						// attachement a la fenetre
	virtual	void			DetachedFromWindow();					// detachement de la fenetre
			void			BuildOptionMenu();						// creer le menu popup des options
			void			UpdateExternalCode();					// mise a jour du code externe
};

#endif

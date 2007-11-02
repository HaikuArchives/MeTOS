/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _CODEMANAGERWINDOW_H
#define _CODEMANAGERWINDOW_H
/***************************/
/* MeTOS CodeManager class */
/***************************/
#include <interface/Window.h>
#include <app/Messenger.h>
#include <app/Message.h>
#include <support/List.h>
#include <support/String.h>

class BView;
class BButton;
class BTabView;
class CPPCodeListView;
class BTextView;
class CColumnView;
class CPPCodeItem;
class VLanguage;

class CPPCodeWin : public BWindow
{
public:
	CPPCodeWin(BRect frame,VLanguage *language);
	~CPPCodeWin();
	
	virtual	void		Show();										// afficher la fenetre
	virtual	void		MessageReceived(BMessage *message);			// gestion des messages
	virtual bool		QuitRequested();							// fermer la fenetre
			void		ClearMemory();								// netoyer la memoire							

			BMessage	*Father(BMessage *child);					// recuperer l'objet pere
			BMessage	*Child(BMessage *father);					// recuperer l'objet fils
			status_t	FillObjectList(BMessage *data);				// recuperer la liste des objets
			status_t	GenerateCode();								// generer le code
			
	inline	void		Exit()			{ _exitGenerator = true; }		// quitter
	inline	VLanguage	*Language()		{ return _language; }			// recuperer le language
	
protected:
	BView					*_codeManagerView;		// vue support
	BTabView				*_tabView;				// tab view custom
	CPPCodeListView			*_codeList;				// liste du code
	BTextView				*_headerTextView;		// vue du code du header
	BTextView				*_sourceTextView;		// vue du code du source
	BTextView				*_makefileTextView;		// vue makefile
	BButton					*_startGenerator;		// demarrer/arreter la generation
	BButton					*_writeFiles;			// creation des fichiers
	CColumnView				*_columnView;			// colonne de la liste
	BString					_basePath;				// chemin de base pour la creation
	BString					_makefile;				// texte du makefile
	int32					_recursIndex;			// index pour la recursivité
	BMessage				_dependencies;			// dependances
	bool					_exitGenerator;			// fermer la fenetre
	VLanguage				*_language;				// language
	
			status_t	GenerateObjectCode(CPPCodeItem *father);					// fonction de generation du code
			void		GenerateMakefile();											// generer le source du makefile
			void		ClearObjectList();											// vider la liste de code
			void		WriteFiles();												// creer les fichiers et ecrire le code dedans
			void		WriteMakefile();											// ecrire le makefile
			void		DisplayCode(BMessage *message);								// afficher le code
			void		RefreshPreferences();										// rafraichir les preferences
			void		GetLibFromDependencies(BString &makefile,bool pathOnly);	// recuperer l'ensemble des librairies
};

// pointer globale
extern CPPCodeWin		*g_CPPCodeWin;

#endif
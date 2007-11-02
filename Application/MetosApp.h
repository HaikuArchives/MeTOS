/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _METOSAPP_H
#define _METOSAPP_H
/*****************************/
/* MeTOS Application en-tete */
/*****************************/
#include <app/Application.h>
#include <storage/Resources.h>
#include <storage/Path.h>
#include <support/List.h>

// pour le debuggage
#include <stdio.h>

class MetosWin;
class BBitmap;
class BFilePanel;
class BString;
class BCursor;
class VTranslator;
class VPreferenceFile;

// fenetres
class MetosWin;
class ObjectManagerWin;
class AddOnManagerWin;
class ToolManagerWin;
class UndoManagerWin;
class PreferenceManagerWin;
class PropertieManagerWin;
class MetosSplashScreen;
class TipsManagerWin;

// message
#define		METOS_DEBUG_MSG		'Mdeb'

// donnees
#define		VDEBUG_TYPE			"VDebug-type"
#define		VDEBUG_STRING		"VDebug-string"

enum debug_type
{
	VDEBUG_INFO		= 0,
	VDEBUG_WARNING	= 1,
	VDEBUG_ERROR	= 2,
	VDEBUG_DEBUG	= 3
};

class MetosApp : public BApplication
{
// fonctions
public:
	// fenetres
	MetosWin				*_metosWin;					// application
	ObjectManagerWin		*_objectManagerWin;			// objects
	AddOnManagerWin			*_addOnManagerWin;			// AddOns
	ToolManagerWin			*_toolManagerWin;			// tools
	UndoManagerWin			*_undoManagerWin;			// undo
	PreferenceManagerWin	*_preferenceManagerWin;		// preferences 
	PropertieManagerWin		*_propertieManagerWin;		// properties
	TipsManagerWin			*_tipsManagerWin;			// tips
	MetosSplashScreen		*_splashScreen;				// splash-screen
	BWindow					*_aboutWin;					// about

	MetosApp();
	virtual ~MetosApp();

	virtual void		MessageReceived(BMessage *message);
	virtual	bool		QuitRequested();
	virtual	void		RefsReceived(BMessage *message);								// reception d'un fichier (drag&drop ou ouvertue par double clique)
			status_t	SendDebbug(const char *message,debug_type type = VDEBUG_DEBUG);	// fonction globale d'envoi de message
			BBitmap 	*GetBitmap(const char *name);									// trouver une image dans les ressources
			BCursor 	*GetCursor(const char *name);									// trouver un curseur dans les ressources
			int32		CountAddOnFile(const char *path);								// compter les fichiers Addon dans un repertoire de l'appli
			BRect		GetCenterFrame(float dx,float dy);								// recuperer un rectangle centré

	inline	BPath			GetAddonsPath()		{ return _appPath; }			// recuperer le chemin d'un repertoire des addons
	inline	BList			*Translators()		{ return &_translatorList; }	// liste des translators
	inline	BList			*Generators()		{ return &_generatorList; }		// liste des generateurs de code

// attributs
protected:
	// autres
	BResources			_ressource;
	BPath				_appPath;
	bool				_quit;
	BFilePanel			*_loadPanel;
	BFilePanel			*_savePanel;
	int32				_backgroundState;
	BList				_translatorList;
	BList				_generatorList;
	BCursor				*_cursorSprite[3];
	
			void		LoadProject(BMessage *message);				// charger un projet
			void		SaveAs(BMessage *message);					// sauvegarder un projet sous un nom
			void		Save();										// sauvegarder le projet courant
			status_t	SaveProject(BString &selectedFile);			// fonction de sauvegarde
			status_t	CreateProjectArchive(BMessage *archive);	// creer l'archive du projet
			void		ShowAboutBox();								// fentre a propos
			void		RefreshPreferences();						// rafraichir les preferences
			status_t	CheckProjectArchive(BMessage *archive);		// verifier que les dependances existent (addons etc...)
			void		DisplayBackGround(int32 state);				// afficher un fond si il faut
			void		ModifySubset();								// modifier la fenetre mere des autres
			void		DefineCursorLook(BMessage *data);			// definir la forme du curseur
			void		SetScreenSize();							// definir la position des fenetres par rapport a la taille de l'ecran
			void		QuitApplication();							// quitter
			// translator
			void		LoadTranslators();							// charger les translators
			void		UnloadTranslators();						// decharger de la memoire les translators
			VTranslator	*FindTranslatorFromMIME(const char *mime);	// trouver un translator pour un type mime
			void		AddTranslatorToPanel();						// ajouter la selection du translator
			// generator
			void		LoadGenerators();							// charger les generateurs de code
			void		UnloadGenerators();							// decharger de la memoire les generateurs de code
			void		StartGenerators(BMessage *message);			// lancer le generateur selectionné
			void		GetGeneratorList(BMessage *reply);			// pour informer la fenetre des proprietes des generateurs existants
			// mise a jour
			void		CheckUpdate(BMessage *data);				// verifier si il y a une mise a jour
			void		LaunchHelp();								// afficher l'aide
			void		SupportMeTOS();								// lancer la page web PayPal
};

// pointer globale sur be_app apres cast
extern MetosApp 	*g_MetosApp;

// messengers en globale
extern BMessenger		g_MetosWin;
extern BMessenger		g_ToolManager;
extern BMessenger  		g_UndoManager;
extern BMessenger 		g_AddOnManager;
extern BMessenger		g_PreferenceManager;
extern BMessenger		g_PropertieManager;
extern BMessenger		g_ObjectManager;

#endif

/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _PROPERTIEMANAGERWINDOW_H
#define _PROPERTIEMANAGERWINDOW_H
/**************************/
/* fenetre des proprietes */
/**************************/
#include <interface/Window.h>
#include <support/List.h>
#include <app/Message.h>
#include <support/String.h>

// nombre de vue colonne pour le tableau
#define		NB_COLOUMN_LIST_VIEW		3

class VObject;
class BFile;
class VPropertie;
class PropertieManagerFilter;
class PropertieManagerListView;
class PropertieManagerItem;
class FunctionManagerListView;
class DependencieManagerListView;
class DependencieManagerItem;
class CodeManagerTextView;
class CodeManagerListView;
class PropertieManagerInfoView;
class BTabView;
class BView;
class BMessenger;
class CColumnView;
class FunctionManagerItem;
class CodeManagerItem;
class BMenu;

class PropertieManagerWin : public BWindow
{
// fonctions
public:
	PropertieManagerWin(BRect frame);
	virtual ~PropertieManagerWin();

virtual void DispatchMessage(BMessage *message, BHandler *handler);
	virtual void	MessageReceived(BMessage *message);
	virtual bool	QuitRequested();

			void					DisplayProperties(BMessage *propertiesObject);			// va choisir et ajouter les bon items
			void					RemoveAll();											// vide la liste de ces items
			bool					AddFunctionItem(BMessage *data);						// ajouter un item de fonction
			bool					AddDependencyItem(BMessage *data);						// ajouter un item de dependance
			status_t				PreGenerateFunctions(BMessage *data,VObject *object);	// generer la liste des fonctions avec type retour, parametres, etc...
			status_t				PreGenerateDependencies(BMessage *data,const char *ID);	// generer la liste des dependances pour un generateur donné
			void					ConvertValuesToString(VObject *object,BMessage *data);	// convertir les valeurs de proprietes d'un objet en String et les mettrent dans le Message
			FunctionManagerItem		*FunctionItemFromClass(const char *name);				// recuperer un item qui gere le nom de classe en parametre
			DependencieManagerItem	*DependenciesItemFromClass(const char *name);			// acceder au dependance d'un objet reperé par son nom de classe
			void					LoadAddOns();											// charger l'ensemble des AddOns
			void					ManageKey(BMessage *message);							// gestion des messages clavier

	inline	uint32			Object()				{ return _selectedObject; }			// retourne l'objet dont on affiche les proprietes (l'objet courant)
	inline	BMessage		*Properties()			{ return &_properties; }			// retourne les proprietes de l'objet courant
	inline	BMessage		*Settings()				{ return _settings; }				// retourne les settings de l'objet courant
	inline	BMessage		*Infos()				{ return _infos; }					// retourne les infos de l'objet courant
	inline	BString			*GeneratorID()			{ return &_generatorID; }			// generateur actuel
	inline	BMessage		*GeneratorList()		{ return &_generatorList; }			// liste des generateurs
	inline	BMenu			*GeneratorMenu()		{ return _generatorMenu; }			// menu des generateurs
	inline	CColumnView		*ColumnView(int8 index)	{ return _columnView[index]; }		// vue colonne

protected:
	PropertieManagerFilter		*_propertiesManagerFilter;			// filtre des touches claviers
	BView						*_propertiesManagerView;			// vue support
	PropertieManagerListView	*_propertiesList;					// list view des proprietes
	FunctionManagerListView		*_functionsList;					// list view des fonctions
	CodeManagerTextView			*_codeTextView;						// vue d'affichage du code externe
	CodeManagerListView			*_codeListView;						// liste des codes externes
	DependencieManagerListView	*_dependencieList;					// list view des librairies
	PropertieManagerInfoView	*_infoView;							// vue des infos
	BTabView					*_tabView;							// tab view custom
	BList						_addonList;							// liste des PropertieManagerItem
	BList						_itemsCache;						// liste des VProperties creer a l'utilisation c'est un cache en fait
	CColumnView					*_columnView[NB_COLOUMN_LIST_VIEW];	// vues colonne		
	BMessage					_properties;				// proprietes
	BMessage					*_settings;					// settings
	BMessage					*_infos;					// infos
	uint32						_selectedObject;			// ID de l'object dont on affiche les proprietes
	BList						_functionItemList;			// liste des liste de fonctions
	BList						_dependencyList;			// liste des dependances
	BMessage					_generatorList;				// liste des generateurs
	BString						_generatorID;				// generateur actuel
	BString						_defaultGeneratorID;		// generateur a utiliser par defaut
	BMenu						*_generatorMenu;			// menu des generateurs
	
			void					UnloadAddOns();														// decharger les AddOns
			status_t				UpdateProperties(BMessage message);									// mettre a jour une proprietes
			void					UpdatePropertiesDisplay(BMessage *message);							// mettre l'affichage une proprietes a jour
			VPropertie 				*CreateProperties(const char *name,const char *type,int32 index);	// Creer une propriete pour editer un "name" a un "index" donné
			VPropertie				*PropertiesFromName(const char *name);								// recuperer une propriete qui gere le nom souhaité
			void					CheckDerived(BMessage *data);										// verifier la derivation
			void					RefreshSystemLib();													// la CSLib systeme
			void					RefreshPreferences();												// rafraichir les preferences
			void					SavePreferences();													// sauver les preferences
			void					SelectGenerator(BMessage *message);									// selection d'un generateur
			void					CreateGeneratorList(BMessage *data);								// liste des generateurs
			void					BuildGeneratorMenu();												// creer le menu des generateur
			void					SetDefaultGeneratorMenu();											// changer le menu en fonction du generateur selectionné
			void					AddExternalCode();													// ajouter un item de code
			void					SelectExternalCode();												// selectionner un code externe
};

#endif

/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _OBJECTMANAGERWINDOW_H
#define _OBJECTMANAGERWINDOW_H
/****************************/
/* fenetre de l'application */
/****************************/
#include <interface/Window.h>
#include <app/Message.h>

class ObjectManagerListView;
class VObject;
class MProject;
class BList;
class BPopUpMenu;
class BString;
class BMessenger;
class BSeparatorItem;
class CColumnView;
class CStatusView;
class AddOnManagerItem;
class BListView;
class BMenu;

class ObjectManagerWin : public BWindow
{
// fonctions
public:
	ObjectManagerWin(BRect);
	virtual ~ObjectManagerWin();

	virtual void		MessageReceived(BMessage *message);
	virtual bool		QuitRequested();

			status_t	AddNewObject(VObject *newObject);						// ajouter graphiquement et a la liste un nouvel objet
			void		SelectObject(BMessage *datas);							// selection d'un objet dans la liste ou  a l'ecran
			status_t	CreateNewProject();										// creer un nouveau projet
			status_t	DeleteAll();
			status_t	FlattenAllObject(BMessage *archive);					// sauver tous les objets dans une archive (BMessage)
			status_t	UnflattenAllObject(BMessage *archive);					// creer les objets d'une archive
			status_t	PreGenerate(BMessage *archive,BString &generatorID);	// archivage dans un message toutes les infos pour pouvoir generer le code
			status_t	SavePath(BString &savePath);							// retourner le chemin de sauvegarde
			VObject		*ChildFrom(VObject *father);							// recuperer le premier enfant d'un objet
			void		RefreshSavePath(const char *path,const char *name);		// mettre a jour le chemin de sauvegarde
			int32		FindItemFromName(const char *name);						// trouver l'index d'un item par nom de classe	
			void		LoadAddOns();											// charger l'ensemble des AddOns

	inline	CColumnView	*ColumnView()	{ return _columnView; }					// recuperer un pointer sur la colonne view
	inline	BListView	*ObjectList()	{ return (BListView *)_objectList; }	// recuperer un pointer sur la liste des objets

private:
	BView					*_objectManagerView;
	ObjectManagerListView	*_objectList;
	BPopUpMenu 				*_options;
	BMenu					*_toolsMenu;
	CColumnView				*_columnView;
	CStatusView				*_statusView;
	BList					_addOnList;
	BList					_garbageList;				// liste des objets effacés
	BMessage				_toolsNameList;				// liste des noms des tools
	uint32					_selectedObject;			// ID de l'object dont on affiche les proprietes
			
			void		DisplaySpecialPropertiesMenu(VObject *selected,BPoint &where);					// afficher le menu des proprietes speciales
			void		SetLockMenu(VObject *object);													// menu de blockage de l'objet
			void		BuildOptionsMenu();																// creer le menu des actions
			void		BuildToolsMenu();																// creer le menu des outils
			status_t	ConvertHandlerToObject(BMessage *message);										// convertion
			void		UpdateProperties(BMessage message);												// mettre a jour les proprietes de l'objet
			void		LockObject(VObject *object);													// bloquer un objet
			status_t	ArchiveObject(VObject *object,BMessage *archive);								// archivage dans un message
			status_t	InstantiateObject(VObject *newObject,BMessage *archive,bool newName = true);	// creer un objet a partir d'une archive
			void		AddObjectMethode(VObject *newObject,VObject *destObject);						// ajout d'un objet de type Add
			status_t 	Delete();																		// effacer les objets selectionnés
			status_t 	DeleteObject(VObject *object,bool needUndo = true);								// effacer un objet
			void		SetObjectName(VObject *object,int32 index);										// fixer le nom de l'objet
			void		CutSelectedObject();															// couper les objets selectionnés
			void		CopySelectedObject();															// copier les objets selectionnés
			void		PasteSelectedObject(BPoint &where);												// coller les objets selectionnés
			void		SelectToolsFromMenu(int32 index);												// selection d'un outils par le menu tools
			void		DropAddon(BMessage *message);													// creer un nouvel objet a partir d'un drag & drop
			void		PlaceNewObjectIntoDropPoint(VObject *newObject,BPoint &point);					// fixer la propriete de positionnement de l'objet en fonction d'un point de drag&drop
			void		ClearList();																	// vider la liste des objets
			void		RestoreUndo(BMessage *data);													// restorer une proprietes ou un objet en fonction du message de Undo
			void		RefreshDisplayNumberItems();													// mettre a jour le nombre d'items affiché
			VObject		*ObjectFromID(uint32 id);														// trouver un objet a partir de son ID
			void		CheckFunction(BMessage *message);												// selection d'un item de fonction
			// preferences
			void		RefreshPreferences();															// rafraichir les preferences
			void		SavePreferences();																// sauver les preferences
			// gestion des addons
			VObject		*CreateAddOnObject(int32 item);													// creation d'un VObject
			void		UnloadAddOns();																	// decharger les AddOns
			void		AddAddonObject(AddOnManagerItem *newAddon);										// ajouter et placer dans la liste un addon
			void		SetObjectAddonSource(VObject *object,int32 index = -1);							// renseigner dans un VObject le nom de l'addon utilisé pour le creer
			void		LoadFunctionsFromAddons();														// lancer la creation des items de fonctions de chaques objets
			void		LoadDependenciesFromAddons();													// lancer la creation des items de dependances de chaques objets
			// code generator
			void		LoadExternalCodesForAddons();													// lancer la creation des items de code externe de chaques objets
			status_t	PreGenerateObject(VObject *object,BMessage *archive,BString &generatorID);		// archivage dans un message toutes les infos d'un objet pour pouvoir generer le code
			status_t	AddGenerationCode(VObject *object,BMessage *archive,BString &generatorID);		// ajouter le code correspondant au generateur
			void		UpdateExternalCode(BMessage *data);												// mettre a jour en interne le code pour un objet
			// garbage
			void		ClearGarbage();																	// vider la liste des objets effacés
			VObject		*FindObjectInGarbage(const char *addonSource);									// trouver un objet dans la liste garbage
};

#endif

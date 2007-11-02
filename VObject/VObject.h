/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _VOBJECTS_H
#define _VOBJECTS_H
/***********************/
/* MeTOS objects class */
/***********************/
// on utilise cette classe comme paserelle avec l'application
// on peut creer un VObject a partir d'un herité, il faut juste implementer
// l'acces/modification des proprietées qui dependent de l'objet lui meme
// et tres important la metode CreateObject() qui sert a reelement creer l'objet
#include <interface/InterfaceDefs.h>
#include <support/Archivable.h>
#include <support/String.h>

// definition des types de base pour le dynamic_cast
enum object_type
{
	UNDEFINED_TYPE		= -1,
	PROJECT_TYPE		= 0,
	APPLICATION_TYPE	= 1,
	// window base
	WINDOW_TYPE			= 10,
	// view base
	VIEW_TYPE			= 20,
	TABVIEW_TYPE		= 21,
	SCROLLVIEW_TYPE		= 22,
	LISTVIEW_TYPE		= 23,
	TEXTVIEW_TYPE		= 24,
	BOX_TYPE			= 25,
	SCROLLBAR_TYPE		= 26,
	STRINGVIEW_TYPE		= 27,
	STATUSBAR_TYPE		= 28,
	MENUBAR_TYPE		= 29,
	MENU_TYPE			= 30,
	POPUPMENU_TYPE		= 31,
	// control base
	CONTAINER_TYPE		= 45,
	CONTROL_TYPE		= 50,
	BUTTON_TYPE			= 51,
	RODIOBUTTON_TYPE	= 52,
	CHECKBOX_TYPE		= 53,
	SLIDER_TYPE			= 54,
	TEXTCONTROL_TYPE	= 55,
	MENUFIELD_TYPE		= 56,
	// other base
	MENUITEM_TYPE		= 80,
	LISTITEM_TYPE		= 85,
	SHELF_TYPE			= 90,
	BITMAP_TYPE			= 95,
	OTHER_TYPE			= 99
};

// des proprietes de bases
#define		VOBJECT_PROPERTIES_MSG			'Mpro'
#define		VOBJECT_SETTINGS_MSG			'Mset'
#define		VOBJECT_INFOS_MSG				'Minf'
#define		VOBJECT_LOCKED_PROPERTIES_MSG	'Mlpr'
#define		VOBJECT_NAME_AS_PROPERTIES_MSG	'Mnap'
#define		VOBJECT_VALUE_AS_STRING_MSG		'Mvas'
#define		VOBJECT_FUNCTION_LIST_MSG		'Mofl'
#define		VOBJECT_ARCHIVE_MSG				'Moar'
#define		VOBJECT_EXTERNAL_CODE_MSG		'Mecm'

// données d'un objet
#define		VOBJECT_ARCHIVE					"object-archive"
#define		VOBJECT_SETTINGS				"object-settings"
#define		VOBJECT_PROPERTIES				"object-properties"
#define		VOBJECT_INFOS					"object-infos"
#define		VOBJECT_LOCKED_PROPERTIES		"object-locked-properties"
#define		VOBJECT_NAME_AS_PROPERTIES		"object-name-as-properties"
#define		VOBJECT_VALUE_AS_STRING			"object-value-as-string"
#define		VOBJECT_EXTERNAL_CODE			"object-external-code"
#define		VOBJECT_CODE_INDEX				"object-code-index"
#define		VOBJECT_FUNCTIONLIST			"function-list"
#define		VOBJECT_FUNCTION_INDEX			"function-index"
#define		VOBJECT_ADDON_SOURCE			"addon-source"
#define		VOBJECT_BE_OBJECT				"be-object"
#define		VOBJECT_LEVEL					"level"
#define		VOBJECT_NAME					"_name"
#define		VOBJECT_CLASS					"class"
#define		VOBJECT_TYPE					"type"
#define		VOBJECT_ID						"id"
#define		VOBJECT_FATHER_ID				"father-id"
#define		VOBJECT_DERIVED_CLASS			"derived-class"

// proprietes speciales
#define		VOBJECT_MEMBER_NAME				"#member-name#"
#define		VOBJECT_CHILD_OBJECT			"#child-object#"
#define		VOBJECT_FATHER_OBJECT			"#father-object#"
#define		VOBJECT_LOCKED_INDEX			"#locked-index#"

// createur
#define		VOBJECT_VERSION					"version"
#define		VOBJECT_AUTHOR					"author"
#define		VOBJECT_ABOUT					"about"
#define		VOBJECT_EMAIL					"e-mail"
#define		VOBJECT_WWW						"www"

// interaction utilisateur
#define		VOBJECT_ONLY_ONE				"only-one"
#define		VOBJECT_DEPEND_ON				"depend-on"
#define		VOBJECT_IS_LOCKED				"is-locked"

// proprietes speciales
#define		VOBJECT_PROPERTIES_ENABLED		0x00000000
#define		VOBJECT_PROPERTIES_DISABLED		0x00000001
#define		VOBJECT_PROPERTIES_HIDDEN		0x00000003	// une caché est non-editable
#define		VOBJECT_PROPERTIES_SYSTEM		0x00000007	// systeme caché et non editable
#define		VOBJECT_PROPERTIES_NOTCONVERTED	0x00000008	// non-convertie elle est la juste pour la creation de l'objet

// calsses utilisées
class BMessage;
class BMenu;
class FunctionItem;

// definition de la classe
class VObject
{
public:
	BArchivable	*_object;	// pointer sur l'objet (reconnaisable grace au type de base)

	VObject();				// constructeur d'objets
	virtual	~VObject(); 	// destructeur

	virtual status_t	CreateObject(BMessage *archive) = 0; 				// creer l'objet de l'interface (obligatoire)
			status_t	AfterCreateObject();								// ne peut etre surchargé ca sert par exemple a ajouter des truc apres la creation (ex : BWindow ajout supportView)
	virtual	status_t	SetDefaultProperties(BMessage *properties) = 0;		// ajouter des proprietes par defaut pour la creation d'un controle (doit etre renseigné)
	virtual	status_t	SetProperties(BMessage *value,int32 index) = 0;		// modifier une des proprietes de l'objet
	virtual	BMenu		*BuildActionMenu();									// creer le menu d'action supplementaire
	virtual	void		ExecuteAction(BString &name,int32 index);			// executer l'action du menu de numero index ou de nom name
			void		GenerateId();										// generer l'id (unique)
			void		UpdateProperties(BMessage *data,int32 index);		// mettre a jour les données en interne
			void		ReplaceProperties(BMessage *data);					// remplacer les données en interne (TOUTES !!!)
			status_t	SetFunctions(BMessage *data);						// recuperer les fonctions selectionnées			
			void		LoadDefaultExternalCode(BMessage *data);			// charger le code externe si il existe (ceux par defaut)
			void 		SetParentId(VObject *father);						// fixer l'id du parent dans la hierarchie
			void		LockObject();										// bloquer l'objet
			void		UnlockObject();										// debloquer l'objet
			bool		LooperLock();										// bloquer le looper
			bool		LooperUnlock();										// le debloquer si c'est possible
			void		Invalidate();										// rafraichir l'affichage

	inline	BArchivable		*Object()				{ return _object; }
	inline	BMessage		*Properties()			{ return _properties; }
	inline	BMessage		*Settings()				{ return _settings; }
	inline	BMessage		*Infos()				{ return _infos; }
	inline	BMessage		*LockedProperties()		{ return _lockedProperties; }
	inline	BMessage		*NameAsProperties()		{ return _nameAsProperties; }
	inline	BMessage		*FunctionList()			{ return _functionList; }
	inline	VObject			*Father()				{ return _father; }
	inline	void			ResetObject()			{ _object = NULL; }
			bool			IsAttached();
			object_type		Type();
			uint32			Id();
			const char		*ClassName() const;
			int32			Level();
			uint32			ParentId();
			bool			IsLock();
			
protected:
	BMessage	*_settings;				// parametres du VObject
	BMessage	*_infos;				// informations
	BMessage	*_properties;			// proprietes de l'objet graphique
	BMessage	*_lockedProperties;		// liste des proprietes de l'objet non editable
	BMessage	*_nameAsProperties;		// liste de correspondance des nom de proprietes et du nom pour le constructeur (ex resizingMode et _resize_mode)
	BMessage	*_functionList;			// liste des fonctions a selectionner d'office
	VObject		*_father;				// c'est un pointer sur le parent (ca va servir pour la generation du code)
};

#endif

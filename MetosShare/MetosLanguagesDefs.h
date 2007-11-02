/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _METOSLANGUAGESDEFS_H
#define _METOSLANGUAGESDEFS_H
/****************************************/
/* Definition des variables global pour	*/
/* la communication entre l'application */
/* et les objets (controles etc ) 		*/
/****************************************/

// definition de constantes
#define		LANGUAGE_NO_TRANSLATION			"no translation !"
#define		LANGUAGE_SEPARATOR				'|'
#define		LANGUAGE_END_OF_LINE			'\n'

// definition des ID de languages
// menu de la fenetre principale
#define		L_MENU_FILE						"MENU_FILE"
#define		L_MENU_FILE_NEW_PROJECT			"MENU_FILE_NEW_PROJECT"
#define		L_MENU_FILE_OPEN_PROJECT		"MENU_FILE_OPEN_PROJECT"
#define		L_MENU_FILE_OPEN_RECENT_PROJECT	"MENU_FILE_OPEN_RECENT_PROJECT"
#define		L_MENU_FILE_SAVE_PROJECT		"MENU_FILE_SAVE_PROJECT"
#define		L_MENU_FILE_SAVE_AS				"MENU_FILE_SAVE_AS"
#define		L_MENU_GENERATE					"MENU_GENERATE"
#define		L_MENU_FILE_QUIT				"MENU_FILE_QUIT"
#define		L_MENU_EDIT						"MENU_EDIT"
#define		L_MENU_EDIT_UNDO				"MENU_EDIT_UNDO"
#define		L_MENU_EDIT_PREFERENCES			"MENU_EDIT_PREFERENCES"
#define		L_MENU_VIEW						"MENU_VIEW"
#define		L_MENU_VIEW_OBJECTS				"MENU_VIEW_OBJECTS"
#define		L_MENU_VIEW_PROPERTIES			"MENU_VIEW_PROPERTIES"
#define		L_MENU_VIEW_ADDONS				"MENU_VIEW_ADDONS"
#define		L_MENU_METOS_TIPS				"MENU_METOS_TIPS"
#define		L_MENU_METOS_UPDATE				"MENU_METOS_UPDATE"
#define		L_MENU_METOS_HELP				"MENU_METOS_HELP"
#define		L_MENU_METOS_DONATE				"MENU_METOS_DONATE"
#define		L_MENU_METOS_ABOUT				"MENU_METOS_ABOUT"

// fenetre des objets
#define		L_OBJECT_OBJECTS				"OBJECT_OBJECTS"
#define		L_OBJECT_GARBAGE				"OBJECT_GARBAGE"
#define		L_OBJECT_NAME					"OBJECT_NAME"
#define		L_OBJECT_LOCK					"OBJECT_LOCK"
#define		L_OBJECT_DERIVED				"OBJECT_DERIVED"
#define		L_OBJECT_ID						"OBJECT_ID"
#define		L_OBJECT_MESSAGE				"OBJECT_MESSAGE"

// menu option des objets
#define		L_MENU_OBJECT_OPTIONS			"MENU_OBJECT_OPTIONS"
#define		L_MENU_OBJECT_LOCK				"MENU_OBJECT_LOCK"
#define		L_MENU_OBJECT_CUT				"MENU_OBJECT_CUT"
#define		L_MENU_OBJECT_COPY				"MENU_OBJECT_COPY"
#define		L_MENU_OBJECT_PASTE				"MENU_OBJECT_PASTE"
#define		L_MENU_OBJECT_TOOLS				"MENU_OBJECT_TOOLS"

// addon manager
#define		L_ADDON_NEW						"ADDON_NEW"
#define		L_ADDON_CLASS_NAME				"ADDON_CLASS_NAME"
#define		L_ADDON_OBJECT_TYPE				"ADDON_OBJECT_TYPE"
#define		L_ADDON_UNDIFINED_TYPE			"ADDON_UNDIFINED_TYPE"
#define		L_ADDON_OTHER_TYPE				"ADDON_OTHER_TYPE"
#define		L_ADDON_UNKNOW_TYPE				"ADDON_UNDIFINED_TYPE"

// dependances
#define		L_DEPENDENCIES_MESSAGE			"DEPENDENCIES_MESSAGE"
#define		L_DEPENDENCIES_OPTIONS			"DEPENDENCIES_OPTIONS"
#define		L_DEPENDENCIES_SAVE_AS_DEFAULT	"DEPENDENCIES_SAVE_AS_DEFAULT"
#define		L_DEPENDENCIES_REMOVE			"DEPENDENCIES_REMOVE"

// code externe
#define		L_CODE_MESSAGE					"CODE_MESSAGE"
#define		L_MENU_CODE_ADD_PROPERTIES		"MENU_CODE_ADD_PROPERTIES"
#define		L_MENU_CODE_ADD_SPECIALS		"MENU_CODE_ADD_SPECIALS"
#define		L_MENU_CODE_UPDATE				"MENU_CODE_UPDATE"
#define		L_MENU_CODE_ADD_NEW_ITEM		"MENU_CODE_ADD_NEW_ITEM"
#define		L_MENU_CODE_SAVE_AS_DEFAULT		"MENU_CODE_SAVE_AS_DEFAULT"
#define		L_MENU_CODE_SAVE				"MENU_CODE_SAVE"
#define		L_MENU_CODE_LOAD				"MENU_CODE_LOAD"

// fonctions
#define		L_FUNCTIONS_MESSAGE				"FUNCTIONS_MESSAGE"
#define		L_FUNCTIONS_SAVE_AS_DEFAULT		"FUNCTIONS_SAVE_AS_DEFAULT"
#define		L_FUNCTIONS_BEST_CONSTRUCTOR	"FUNCTIONS_BEST_CONSTRUCTOR"

// Informations d'objet
#define		L_INFO_VERSION					"INFO_VERSION"
#define		L_INFO_AUTHOR					"INFO_AUTHOR"
#define		L_INFO_EMAIL					"INFO_EMAIL"
#define		L_INFO_LINK						"INFO_LINK"
#define		L_INFO_ABOUT					"INFO_ABOUT"
#define		L_INFO_OBJECT_ID				"INFO_OBJECT_ID"
#define		L_INFO_PARENT_ID				"INFO_PARENT_ID"
#define		L_INFO_CLASS_NAME				"INFO_CLASS_NAME"
#define		L_INFO_BE_OBJECT				"INFO_BE_OBJECT"
#define		L_INFO_DERIVED					"INFO_DERIVED"
#define		L_INFO_YES						"INFO_YES"
#define		L_INFO_NO						"INFO_NO"

// Proprietes
#define		L_PROPERTIES_MESSAGE			"PROPERTIES_MESSAGE"
#define		L_PROPERTIES_TITLE				"PROPERTIES_TITLE"
#define		L_PROPERTIES_PARAMETERS			"PROPERTIES_PARAMETERS"
#define		L_PROPERTIES_FUNCTIONS			"PROPERTIES_FUNCTIONS"
#define		L_PROPERTIES_EXTERNAL_CODE		"PROPERTIES_EXTERNAL_CODE"
#define		L_PROPERTIES_DEPENDENCIES		"PROPERTIES_DEPENDENCIES"
#define		L_PROPERTIES_INFORMATIONS		"PROPERTIES_INFORMATIONS"
#define		L_PROPERTIES_PROPERTIES			"PROPERTIES_PROPERTIES"
#define		L_PROPERTIES_VALUES				"PROPERTIES_VALUES"
#define		L_PROPERTIES_DEFAULT			"PROPERTIES_DEFAULT"
#define		L_PROPERTIES_CODE				"PROPERTIES_CODE"
#define		L_PROPERTIES_PATH				"PROPERTIES_PATH"
#define		L_PROPERTIES_TYPE				"PROPERTIES_TYPE"
#define		L_PROPERTIES_GENERATOR			"PROPERTIES_GENERATOR"

// preferences
#define		L_PREFERENCES_CODE_GENERATOR	"PREFERENCES_CODE_GENERATOR"
#define		L_PREFERENCES_TRANSLATOR		"PREFERENCES_TRANSLATOR"
#define		L_PREFERENCES_TOOLS				"PREFERENCES_TOOLS"
#define		L_PREFERENCES_OTHER				"PREFERENCES_OTHER"

// trucs et astuce
#define		L_TIPS_TITLE					"TIPS_TITLE"
#define		L_TIPS_NEXT						"TIPS_NEXT"
#define		L_TIPS_CLOSE					"TIPS_CLOSE"
#define		L_TIPS_SHOW_AT_STARTUP			"TIPS_SHOW_AT_STARTUP"

// Outils
#define		L_TOOLS_TITLE					"TOOLS_TITLE"

// Undo
#define		L_UNDO_TITLE					"UNDO_TITLE"
#define		L_UNDO_RESTORE					"UNDO_RESTORE"
#define		L_UNDO_DELETE					"UNDO_DELETE"
#define		L_UNDO_UNDOS					"UNDO_UNDOS"
#define		L_UNDO_OBJECT_CUT				"UNDO_OBJECT_CUT"
#define		L_UNDO_OBJECT_PASTE				"UNDO_OBJECT_PASTE"
#define		L_UNDO_OBJECT_DELETE			"UNDO_OBJECT_DELETE"
#define		L_UNDO_OBJECT_CHANGE			"UNDO_OBJECT_CHANGE"
#define		L_UNDO_OBJECT_UNDIFINED			"UNDO_OBJECT_UNDIFINED"
#define		L_UNDO_OBJECT_UNDIFINED			"UNDO_OBJECT_UNDIFINED"

#endif
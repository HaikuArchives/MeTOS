/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _METOSGLOBALDEFS_H
#define _METOSGLOBALDEFS_H
/****************************************/
/* Definition des variables global pour	*/
/* la communication entre l'application */
/* et les objets (controles etc ) 		*/
/****************************************/
#include <interface/InterfaceDefs.h>
#include <stdio.h>

// pour pouvoir declarer le pointer
class VPreferenceFile;
class VLanguage;

// variable pour quitter l'application
// et preferences
extern	bool				g_DoQuit;
extern	VPreferenceFile		*g_MeTOSPrefs;
extern	VPreferenceFile		*g_ColorsPrefs;
extern	VLanguage			*g_Languages;

// definition des noms de repertoires pour les Addons
#define			METOS_SETTINGS_FOLDER			"MeTOS"				
#define			METOS_TIPS_FOLDER				"Tips"
#define			METOS_UPGRADER_FOLDER			"Upgrader"
#define			METOS_UPGRADER_APPLICATION		"MeTOSUpgrader"
#define			METOS_LANGUAGES_FOLDER			"Languages"
#define			METOS_DOCUMENTATIONS_FOLDER		"Documentations"

#define			ADDON_FOLDER					"AddOns"
#define			ADDON_HEADERS_FOLDER			"Headers"
#define			ADDON_OBJECTS_FOLDER			"Objects"
#define			ADDON_EXTERNAL_CODES_FOLDER		"ExternalCodes"
#define			ADDON_DEPENDENCIES_FOLDER		"Dependencies"
#define			ADDON_PREFERENCES_FOLDER		"Preferences"
#define			ADDON_PROPERTIES_FOLDER			"Properties"
#define			ADDON_TOOLS_FOLDER				"Tools"
#define			ADDON_TRANSLATORS_FOLDER		"Translators"
#define			ADDON_GENERATORS_FOLDER			"CodeGenerators"
#define			ADDON_FUNCTIONS_FOLDER			"Functions"

// type MIME des headers
#define			METOS_HEADER_MIME_TYPE			"text/x-source-code"

// upgrader
#define		METOS_UPGRADER_URL					"http://perso.wanadoo.fr/cvincent/logiciels/upgrades/metos"
#define		METOS_UPGRADER_SCRIPT				"metos-upgrade"

// base de couleur
#define		BASE_COLOR		B_PANEL_BACKGROUND_COLOR

// couleur
#define		WHITE_COLOR		tint_color(ui_color(BASE_COLOR),B_LIGHTEN_MAX_TINT)
#define		LIGHT_2_COLOR	tint_color(ui_color(BASE_COLOR),B_LIGHTEN_2_TINT)
#define		LIGHT_1_COLOR	tint_color(ui_color(BASE_COLOR),B_LIGHTEN_1_TINT)
#define		VIEW_COLOR		tint_color(ui_color(BASE_COLOR),B_NO_TINT)
#define		DARK_1_COLOR	tint_color(ui_color(BASE_COLOR),B_DARKEN_1_TINT)
#define		DARK_2_COLOR	tint_color(ui_color(BASE_COLOR),B_DARKEN_2_TINT)
#define		DARK_3_COLOR	tint_color(ui_color(BASE_COLOR),B_DARKEN_3_TINT)
#define		DARK_4_COLOR	tint_color(ui_color(BASE_COLOR),B_DARKEN_4_TINT)
#define		BLACK_COLOR		tint_color(ui_color(BASE_COLOR),B_DARKEN_MAX_TINT)

// constantes des fonctions
#define		FUNCTION_MSG				'Fmsg'
#define		FUNCTION_ITEM				"function-item"
#define		FUNCTION_PARAMETERS_MSG		'Fprm'
#define		FUNCTION_PARAMETERS			"parameters"
#define		FUNCTION_PARAMETER_TYPE		"parameter-type"
#define		FUNCTION_PARAMETER_NAME		"parameter-name"
#define		FUNCTION_PARAMETER_ARGS		"parameter-args"
#define		FUNCTION_PARAMETER_VALUE	"parameter-value"

// dependances
#define		DEPENDENCIES_MSG		'Dpdm'
#define		DEPENDENCIES_DATA		"dependencies-data"
#define		DEPENDENCIES_PATH		"dependencies-path"
#define		DEPENDENCIES_MIME		"dependencies-MIME"

// constante d'edition des dependences
#define		DEPENDENCIES_PATH_SELECTED		0
#define		DEPENDENCIES_MIME_SELECTED		1

// taille ecran
#define		SCREEN_640_480				0
#define		SCREEN_800_600				1
#define		SCREEN_1024_768				2
#define		SCREEN_1152_864				3
#define		SCREEN_1280_1024			4
#define		SCREEN_1600_1200			5

// code externe
#define		EXTERNALCODE_MENULABEL_SIZE		20

// language
#define		METOS_LANGUAGES_TYPE			"language-type"


#endif

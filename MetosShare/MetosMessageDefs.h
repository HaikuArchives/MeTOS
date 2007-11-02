/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _METOSMESSAGEDEFS_H
#define _METOSMESSAGEDEFS_H

/****************************************/
/* Definition des messages et pour 		*/
/* la communication entre l'application */
/* et les objets (controles etc ) 		*/
/****************************************/

// definitions des messages
#define			SHOW_PROJECT_WIN_MSG			'Spwm'
#define			SHOW_PROPERTIES_WIN_MSG			'Sprm'
#define			SHOW_OBJECT_WIN_MSG				'Sowm'
#define			SHOW_PREFERENCES_WIN_MSG		'Sprf'
#define			SHOW_GENERATOR_MSG				'Sgew'
#define			SHOW_TIPS_WIN_MSG				'Stiw'
#define			SHOW_ABOUT_WIN_MSG				'Sawm'

// application & preferences
#define			B_LOAD_REQUESTED				'Blor'
#define			M_PREFERENCE_MSG				'Mprf'
#define			M_SAVE_PREFERENCE_MSG			'Mspf'
#define			M_CHECK_UPDATE_MSG				'Mchu'
#define			M_METOS_HELP_MSG				'Mhlp'
#define			M_METOS_DONATE_MSG				'Mdon'
#define			M_LANGUAGE_MSG					'Mlan'

// application & managers
#define			M_ACTIVE_WINDOW_MSG				'Macw'
#define			M_ADD_EXTERNAL_CODE_MSG			'Maex'
#define			M_ADD_RECENT_MSG				'Madr'
#define			M_CHECK_CLASS_DERIVED_MSG		'Mccd'
#define			M_CREATE_DEPENDENCY_MSG			'Mcdy'
#define			M_CREATE_FUNCTION_MSG			'Mcfi'
#define			M_CREATE_OBJECT_MSG				'Mcob'
#define			M_CREATE_ADDON_MSG				'Mcad'
#define			M_DELETE_OBJECT_MSG				'Mdob'
#define			M_DISPLAY_EXTERNAL_CODE_MSG		'Mdex'
#define			M_DISPLAY_PROPERTIES_MSG		'Mdip'
#define			M_GENERATOR_NAME_LIST_MSG		'Mgnl'
#define			M_GENERATE_CODE_MSG				'Mgcm'
#define			M_LOAD_EXTERNAL_CODE_MSG		'Mlex'
#define			M_LOAD_PROJECT_MSG				'Mlpr'
#define			M_NEW_PROJECT_MSG				'Mnpr'
#define			M_OBJECT_CHANGE_MSG				'Mobc'
#define			M_OPEN_RECENT_MSG				'Mopr'
#define			M_PROPERTIES_CHANGE_MSG			'Mprc'
#define			M_PROJECT_ARCHIVE_MSG			'Mprj'
#define			M_HEADER_DISPLAY_MSG			'Mhdp'
#define			M_REFRESH_OBJECTS_MSG			'Mrob'
#define			M_REFRESH_FUNCTION_LIST_MSG		'Mrfl'
#define			M_REFRESH_NUMBER_ITEMS_MSG		'Mrni'
#define			M_REMOVE_CODE_MSG				'Mrcm'
#define			M_REMOVE_DEPENDENCY_MSG			'Mrde'
#define			M_REPLY_MSG						'Mrpl'
#define			M_SOURCE_DISPLAY_MSG			'Msdp'
#define			M_SELECT_OBJECT_MSG				'Msob'
#define			M_SELECT_FUNCTION_MSG			'Msfm'
#define			M_SELECT_GENERATOR_MSG			'Msge'
#define			M_SELECT_EXTERNAL_CODE_MSG		'Msec'
#define			M_SET_ACTION_MSG				'Msac'
#define			M_SET_CURSOR_MSG				'Msac'
#define			M_SET_DEFAULT_EXTERNAL_CODE_MSG	'Msde'
#define			M_SAVE_PROJECT_MSG				'Mspr'
#define			M_SAVE_PROJECT_AS_MSG			'Mspa'
#define			M_SAVE_EXTERNAL_CODE_MSG		'Msae'
#define			M_SAVE_DEPENDENCIES_MSG			'Msde'
#define			M_TOOLS_NAME_LIST_MSG			'Mtnl'
#define			M_UPDATE_EXTERNAL_CODE_MSG		'Muec'
#define			M_WRITE_CODE_FILES_MSG			'Mwcf'

// mise a jour
#define			M_UPDATER_SCRIPT				"updater-script"
#define			M_UPDATER_VERSION				"updater-version"
#define			M_UPDATER_URL					"updater-URL"

// divers (graphique/proprietes)
#define			M_SAVEASDEFAULT_FUNTION_MSG		'Msad'
#define			M_FINDBEST_CONSTRUCTOR_MSG		'Mfbc'
#define			M_REFRESH_PROPERTIES_MSG		'Mrpr'
#define			M_NEED_REFRESH					"need-refresh"
#define			M_NEED_INVALIDATE				"need-invalidate"
#define 		M_PROPERTIES_INDEX				"properties-index"
#define			M_GENERATOR_ID					"generator-id"
#define			M_GENERATOR_NAME				"generator-name"
#define			M_IS_MODIFIED					"is-modified"
#define			M_DEFAULT_INDEX					"default-index"

// action copier coller etc....
#define			M_CUT_OBJECT_MSG				'Mcut'
#define			M_COPY_OBJECT_MSG				'Mcpy'
#define			M_PASTE_OBJECT_MSG				'Mpst'
#define			M_TOOLS_CALL_MSG				'Mtcl'

#define			M_TOOLS_INDEX					"tools-index"
#define			M_TOOLS_NAME					"tools-name"
#define			M_NEED_UNDO_ACTION				"need-undo-action"
#define			M_CURSOR_LOOK					"cursor-look"
#define			M_RECENT_PATH					"recent-path"

// selection d'un objet
#define			M_HANDLER_SELECTED				"selected-handler"
#define			M_OBJECT_SELECTED				"selected-object"
#define			M_OBJECT_POINTER				"pointer-object"
#define			M_SHIFT_SELECTION				"shift-selection"
#define			M_MOUSE_BUTTON					"mouse-button"
#define			M_POINT_SELECTED				"selected-point"
#define			M_ACTION_TYPE					"action-type"
#define			M_ADDON_ENTRY					"addon-entry"
#define			M_ADDON_LOCK					"addon-lock"

// drag & drop
#define			M_DROP_ADDON_MSG				'Mdad'
#define			M_DROP_POINT					"metos-drop-point"

// raccourci clavier
#define			M_NEW_PROJECT_SHORTCUT			'N'
#define			M_OPEN_PROJECT_SHORTCUT			'O'
#define			M_SAVE_PROJECT_SHORTCUT			'S'
#define			M_UNDO_SHORTCUT					'Z'

#endif

/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _METOSUNDODEFS_H
#define _METOSUNDODEFS_H
/*************************/
/* MeTOS Undo constantes */
/*************************/
// liste des actions sauver pour le undo

#define		UNDO_RESTORE_MSG				'Urst'
#define		UNDO_ACTION_MSG					'Uact'
#define		UNDO_ACTION_TYPE				"undo-action-type"
#define		UNDO_LIST_ITEM					"undo-list-item"

enum	undo_type
{
	UNDO_TYPE_UNDIFINED				= -1,		// pas d'action definie
	UNDO_CUT_OBJECT					= 0,		// action couper d'objet
	UNDO_PASTE_OBJECT				= 1,		// action coller
	UNDO_DELETE_OBJECT				= 2,		// effacer des objets
	// --- modifications de parametres ---
	// ce message contient :
	// - la proprietes qui a changé
	// - sa valeur
	// - l'ID de l'objet en question
	// - l'etat des proprietes de l'objet avant changement
	UNDO_CHANGE_PROPERTIES_OBJECT	= 3
};

#endif

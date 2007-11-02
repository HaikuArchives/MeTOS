#ifndef _CPPGENERATORPREFS_H
#define _CPPGENERATORPREFS_H
/**************************/
/* MeTOS preference class */
/* pour l'application     */
/**************************/

#include "VPreference.h"

#define		IDENTSTYLE_CHECK_MSG		'Idtm'
#define		OPEN_SELECTOR				'Opsl'
#define		PATH_SELECTION				'Pslt'

class VPreferenceFile;
class BTextControl;
class BCheckBox;
class BFilePanel;
class BButton;

// gestion des couleurs
class CPPGeneratorPrefs : public VPreference
{
public:
	CPPGeneratorPrefs(BRect frame);
	~CPPGeneratorPrefs();

			void		AttachedToWindow();						// attachement a la fenetre
			void		MessageReceived(BMessage *message);		// gestion des messages
			void		SavePrefs();							// sauver les preferences

protected:
	VPreferenceFile		*_prefFile;
	BTextControl		*_prependMemberString;		// chaine en entete de variable
	BCheckBox			*_indentStyle;				// indentation
	BCheckBox			*_writeMakeFile;			// creation du makefile
	BCheckBox			*_useEqualOnly;				// utiliser le = uniquement dans le makefile
	BTextControl		*_systemCSLibPath;			// chemin du fichier CS systeme
	BButton				*_selector;					// bouton pour ouvrir un selecteur de chemin
	BFilePanel			*_pathSelector;				// selecteur de chemin
	
			void		SetIndentLabel();		// fixer le label du style d'indentation
};

#endif
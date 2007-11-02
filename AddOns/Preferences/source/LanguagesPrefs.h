#ifndef _LANGUAGESPREFS_H
#define _LANGUAGESPREFS_H
/**************************/
/* MeTOS preference class */
/* pour la localisation   */
/**************************/
#include "VPreference.h"
#include <support/String.h>

class BMenuField;

// gestion des preferences generales
class LanguagesPrefs : public VPreference
{
public:
	LanguagesPrefs(BRect frame);

			void		AttachedToWindow();						// attachement a la fenêtre
			void		MessageReceived(BMessage *message);		// gestion des messages
			void		ChangeLanguage(BMessage *message);		// changer la langue
			void		SavePrefs();							// sauver les preferences

protected:
	BMenuField		*_languages;		// choix de language
	BString			_currentLanguage;	// language courant
	
			void		_build_menu();		// construire le menu avec les languages dispo	
};

#endif
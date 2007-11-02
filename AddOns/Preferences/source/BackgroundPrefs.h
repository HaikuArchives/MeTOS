#ifndef _BACKGROUNDPREFS_H
#define _BACKGROUNDPREFS_H
/**************************/
/* MeTOS preference class */
/* pour l'application     */
/**************************/

#include "VPreference.h"

#define		OPEN_SELECTOR				'Opsl'
#define		PATH_SELECTION				'Pslt'

class BCheckBox;
class BTextControl;
class BFilePanel;
class BButton;

// gestion des preferences generales
class BackgroundPrefs : public VPreference
{
public:
	BackgroundPrefs(BRect frame);

			void		AttachedToWindow();
			void		MessageReceived(BMessage *message);
			void		SavePrefs();		// sauver les preferences
	
protected:
	BCheckBox		*_useBackGround;
	BTextControl	*_bitmapPath;
	BButton			*_selector;					// bouton pour ouvrir un selecteur de chemin
	BFilePanel		*_pathSelector;				// selecteur de chemin
};

#endif
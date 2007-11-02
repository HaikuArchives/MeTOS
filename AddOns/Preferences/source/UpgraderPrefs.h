#ifndef _UPGRADERPREFS_H
#define _UPGRADERPREFS_H
/**************************/
/* MeTOS preference class */
/* pour l'upgrader        */
/**************************/
#include "VPreference.h"

class BTextControl;

// gestion des preferences generales
class UpgraderPrefs : public VPreference
{
public:
	UpgraderPrefs(BRect frame);

			void		SavePrefs();		// sauver les preferences
	
protected:
	BTextControl	*_upgraderURL;
	BTextControl	*_upgraderScript;
};

#endif
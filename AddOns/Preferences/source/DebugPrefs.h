#ifndef _METOSDEBUGPREFS_H
#define _METOSDEBUGPREFS_H
/**************************/
/* MeTOS preference class */
/* pour l'application     */
/**************************/

#include "VPreference.h"

class BMenuField;
class BTextControl;

// gestion du debug
class DebugPrefs : public VPreference
{
public:
	DebugPrefs(BRect frame);

			void		AttachedToWindow();
			void		MessageReceived(BMessage *message);
			void		SavePrefs();		// sauver les preferences

protected:
	BMenuField		*_debugLevelMenu;
	BTextControl	*_nbDebugMessage;	
	int32			_debugLevel;
	
			void		SetDebugLevel(BMessage *message);	// fixer le niveau de debuugage
};

#endif
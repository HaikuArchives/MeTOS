#ifndef _APPLICATIONPREFS_H
#define _APPLICATIONPREFS_H
/**************************/
/* MeTOS preference class */
/* pour l'application     */
/**************************/

#include "VPreference.h"

class BTextControl;
class BCheckBox;
class BColorControl;

// gestion des preferences generales
class ApplicationPrefs : public VPreference
{
public:
	ApplicationPrefs(BRect frame);

			void		AttachedToWindow();					// attachement a la fenetre
			void		MessageReceived(BMessage *message);	// gestion des messages
			void		SavePrefs();						// sauver les preferences
	
protected:
	// gui-manager
	BTextControl	*_gridStep;
	BCheckBox		*_useGrid;
	BCheckBox		*_snapToGrid;
	BCheckBox		*_resizeToGrid;
	// menu popup
	BCheckBox		*_useToolsPopup;
	BTextControl	*_nbToolsPerLine;
	// unddo
	BTextControl	*_nbUndos;
	// fichiers recent
	BTextControl	*_nbRecent;
};

#endif
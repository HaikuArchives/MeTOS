/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _VPREFERENCE_H
#define _VPREFERENCE_H
/***************************/
/* MeTOS preferences class */
/* Plugin pour le support  */
/* des preferences         */
/***************************/
#include <interface/ListItem.h>
#include "VPreferenceContainer.h"

// constantes
#define		VPREFERENCE_TITLE_HIGH		19
#define		L_PREFERENCE_NAME			"PREFERENCE_NAME"

// type de groupe principale
enum preference_group
{
	OTHER_GROUP				= -1,
	METOS_GROUP				= 0,
	PROJECT_GROUP			= 1,
	CODE_GENERATION_GROUP	= 2,
	TRANSLATOR_GROUP		= 3,
	TOOLS_GROUP				= 4
};

class VPreferenceFile;
class VLanguage;

// declaration de la classe
class VPreference : public BListItem, public VPreferenceContainer
{
public:
	VPreference(BRect frame,const char *name,preference_group group);
	virtual ~VPreference();

	virtual void				DrawItem(BView *owner,BRect bounds,bool complete = false);	// dessiner l'item
	virtual void				SavePrefs() = 0;											// sauver les preferences
			void				RefreshPreferences();										// rafraichir les prefs

	inline	preference_group	Group()			{ return _group; }							// recuperer le groupe

protected:
	preference_group	_group;
	VLanguage			*_language;		// language
	char				*_name;
	rgb_color			_itemColor;
};

#endif

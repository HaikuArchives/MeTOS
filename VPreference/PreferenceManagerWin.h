/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _PREFERENCEMANAGERWIN_H
#define _PREFERENCEMANAGERWIN_H
/***************************/
/* fenetre des preferences */
/***************************/
#include <interface/Window.h>
#include <support/List.h>

class BView;
class PreferenceManagerListView;
class BButton;

class PreferenceManagerWin : public BWindow
{
public:
	PreferenceManagerWin(BRect);
	virtual	~PreferenceManagerWin();

	virtual	void	MessageReceived(BMessage *message);	// gestion des messages
	virtual bool	QuitRequested();					// fermer la fenetre
			void	LoadAddOns();						// charger l'ensemble des AddOns Preferences

protected:
	BView						*_supportView;
	PreferenceManagerListView	*_groupListView;
	BList						_addOnList;
	BButton						*_saveButton;
	
			void	UnloadAddOns();				// decharger les AddOns Preferences
			void	SaveCurrentPreference();	// sauver les preferences courantes
			void	SavePreferences();			// sauver les preferences
};

#endif

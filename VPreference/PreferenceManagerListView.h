/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _PREFERENCEMANAGERSLISTVIEW_H
#define _PREFERENCEMANAGERSLISTVIEW_H
/***************************************/
/* list des addons (preferences) class */
/***************************************/
#include <interface/OutlineListView.h>

class VPreference;

class PreferenceManagerListView : public BOutlineListView
{
public:
	PreferenceManagerListView(BRect,const char *);
	virtual ~PreferenceManagerListView();

	virtual void	MessageReceived(BMessage *);		// gerer les messages
	virtual void	MouseDown(BPoint );					// clique souris

			bool	AddPreferences(BList *prefsList);	// ajouter des VPreferences
			void	ClearList();						// vider la liste et liberer la memoire
			void	SelectItem(int32 index);			// selectionner un nouvel item
			void	UnSelectItem();						// deselectionner l'ancien item selectionne
			void	SelectFirstItem();					// selectionner le premier item valide

protected:
			int32	FindItemPlace(VPreference *newItem);	// trouver l'emplacement de l'item;
			void	BuildList();							// ajouter les super-items

};

#endif

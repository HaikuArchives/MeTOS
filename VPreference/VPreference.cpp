/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
/***************************/
/* MeTOS preferences class */
/* Plugin support class    */
/* for preference          */
/***************************/
#include "VPreference.h"
#include "VPreferenceFile.h"
#include "MetosGlobalDefs.h"
#include "VLanguage.h"
#include <stdlib.h>

/**** constructeur ****/
VPreference::VPreference(BRect frame,const char *name,preference_group group)
: BListItem(1), VPreferenceContainer(frame,name)
{
	int32	nameSize = 0;

	// init
	_group = group;
	
	// charger le language, pour changer le nom de la vue
	_language = new VLanguage(name);
	
	// verifier
	// trouver le nom a positionner
	if(_language->InitCheck()==B_OK && _language->Load()==B_OK)
	{
		SetName(_language->GetTranslationFor(L_PREFERENCE_NAME));
		name = Name();
	}
	
	// initialisation
	nameSize = strlen(name) + 1;
	if(nameSize<2)
		return;

	// recopier le nom
	_name = (char *)malloc(strlen(name)+1);
	memcpy(_name,name,strlen(name)+1);

	// charger les prefs
	RefreshPreferences();
}

/**** destructeur ****/
VPreference::~VPreference()
{
	// detruire l'element de language
	delete _language;

	// liberer les chaines
	free(_name);
}

/**** dessin de l'item ****/
void VPreference::DrawItem(BView *owner,BRect bounds,bool complete)
{
	// dessiner le fond
	bounds.left = 0;
	if(IsSelected())
		owner->SetHighColor(_itemColor);
	else
		owner->SetHighColor(255,255,255);
	owner->SetLowColor(owner->HighColor());
	owner->FillRect(bounds);

	// dessiner le titre
	owner->SetHighColor(0,0,0);
	owner->DrawString(_name,BPoint(bounds.left+23,bounds.top+10));
}

/**** rafraichir les preferences ****/
void VPreference::RefreshPreferences()
{
	// recuperer la couleur
	rgb_color tempColor = {200,200,255,255};
	_itemColor = g_ColorsPrefs->GetRGBColor(tempColor,"list-color");
}

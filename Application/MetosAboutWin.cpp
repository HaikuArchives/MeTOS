/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#include "MetosAboutWin.h"
#include "VPreferenceFile.h"
#include "MetosAboutView.h"
#include "MetosApp.h"
#include "MetosLanguagesDefs.h"
#include "MetosGlobalDefs.h"
#include "VLanguage.h"

/**** constructor ****/
MetosAboutWin::MetosAboutWin(BRect frame)
: BWindow(frame,g_Languages->GetTranslationFor(L_MENU_METOS_ABOUT), B_TITLED_WINDOW_LOOK, B_FLOATING_SUBSET_WINDOW_FEEL , B_NOT_RESIZABLE | B_NOT_ZOOMABLE )
{
	MetosAboutView	*view = NULL;

	// fixer la vitesse du pulse
	SetPulseRate(250000);

	// ajouter la vue
	view = new MetosAboutView(Bounds());
	AddChild(view);
}

/**** destructor ****/
MetosAboutWin::~MetosAboutWin()
{
	VPreferenceFile		prefs("MeTOS");
	
	// charger les preferences
	prefs.Load();

	// sauver la position dans les preferences
	prefs.SetRect(Frame(),"about-rect");

	// sauver
	prefs.Save();
}

/**** fermeture de la fenetre ****/
bool MetosAboutWin::QuitRequested()
{
	// ok on a plus de fenetre about
	g_MetosApp->_aboutWin = NULL;

	// quitter la fenetre
	return true;
}

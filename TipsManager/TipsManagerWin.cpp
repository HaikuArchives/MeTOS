/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
/********************/
/* fenetre des Tips */
/********************/
#include "TipsManagerWin.h"
#include "TipsManagerBox.h"
#include "MetosGlobalDefs.h"
#include "VPreferenceFile.h"
#include "MetosApp.h"
#include "MetosLanguagesDefs.h"
#include "VLanguage.h"

#include <interface/View.h>
#include <interface/CheckBox.h>
#include <interface/Button.h>
#include <interface/Screen.h>

/**** constructeur ****/
TipsManagerWin::TipsManagerWin(BRect frame)
: BWindow(frame,g_Languages->GetTranslationFor(L_TIPS_TITLE), B_TITLED_WINDOW_LOOK, B_FLOATING_SUBSET_WINDOW_FEEL, B_WILL_ACCEPT_FIRST_CLICK | B_NOT_RESIZABLE | B_NOT_ZOOMABLE)
{
	BRect				listRect = Bounds();

	// vue support
	_supportView = new BView(listRect,"support-tips-view",B_FOLLOW_ALL_SIDES, B_WILL_DRAW);
	_supportView->SetViewColor(VIEW_COLOR);
	
	// box des tips
	listRect.InsetBy(10,10);
	listRect.right -= 80;
	listRect.top -= 5;
	listRect.bottom -= 15;
	_tipsManagerBox = new TipsManagerBox(listRect);
	_tipsManagerBox->SetLabel(g_Languages->GetTranslationFor(L_TIPS_TITLE));

	// boutons
	listRect = Bounds();
	listRect.left = listRect.right - 75;
	listRect.right -= 5;
	listRect.bottom = listRect.top + 30;
	listRect.top += 10;
	_closeWin = new BButton(listRect,"close-button",g_Languages->GetTranslationFor(L_TIPS_CLOSE),new BMessage(VTIPS_CLOSE_WINDOW_MSG));
	listRect.top += 30;
	listRect.bottom += 30;
	_nextTip = new BButton(listRect,"next-tip",g_Languages->GetTranslationFor(L_TIPS_NEXT),new BMessage(VTIPS_NEXT_TIPS_MSG));

	
	// show at startUp
	listRect = Bounds();
	listRect.top = listRect.bottom - 20;
	listRect.left = 10;
	listRect.right = 120;
	_showAtStartUp = new BCheckBox(listRect,"show-at-startup",g_Languages->GetTranslationFor(L_TIPS_SHOW_AT_STARTUP),NULL);
	_showAtStartUp->SetValue(g_MeTOSPrefs->GetInt32(B_CONTROL_ON,"show-at-startup"));
	
	_supportView->AddChild(_tipsManagerBox);
	_supportView->AddChild(_closeWin);
	_supportView->AddChild(_nextTip);
	_supportView->AddChild(_showAtStartUp);

	// attacher a la fenetre
	AddChild(_supportView);

	// fixer la cible du bouton next
	_nextTip->SetTarget(_tipsManagerBox);
}

/**** destructeur ****/
TipsManagerWin::~TipsManagerWin()
{
	// sauver les preferences
	SavePreferences();
}

/**** gestion des messages ****/
void TipsManagerWin::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
	// on ferme pas la fenetre on la cache
	case VTIPS_CLOSE_WINDOW_MSG:
		Hide();
	default:
		BWindow::MessageReceived(message);	
	}
}

/**** fermer la fenetre ****/
bool TipsManagerWin::QuitRequested()
{
	// quitter
	if(g_DoQuit)
		return (true);
	
	// doit-on quitter ou simplement cacher la fenetre
	// si non on reduit la fenetre si ca n'est pas deja fait
	if(!IsHidden())
		Hide();
	
	return	(false);
}

/**** sauver les preferences ****/
void TipsManagerWin::SavePreferences()
{
	// charger les preferences
	g_MeTOSPrefs->Load();
	g_MeTOSPrefs->SetInt32(_showAtStartUp->Value(),"show-at-startup");
	g_MeTOSPrefs->Save();
}

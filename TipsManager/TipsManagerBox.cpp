/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
/****************/
/* Box des Tips */
/****************/
#include "TipsManagerBox.h"
#include "TipsManagerWin.h"
#include "MetosSplashScreen.h"
#include "MetosMessageDefs.h"
#include "MetosGlobalDefs.h"
#include "MetosApp.h"
#include "VTips.h"

#include <storage/FindDirectory.h>
#include <interface/Bitmap.h>
#include <interface/TextView.h>
#include <storage/Entry.h>
#include <storage/Directory.h>
#include <stdlib.h>
#include <time.h>

/**** constructeur ****/
TipsManagerBox::TipsManagerBox(BRect frame)
: BBox(frame,"tips-manager-box")
{
	BRect	bounds = Bounds();
	
	// info du jour
	SetDayInfo();
	
	_tipsText = new BTextView(BRect(15,50,bounds.right-15,120),"tips-text",BRect(0,0,bounds.right-30,70),B_FOLLOW_LEFT_RIGHT,B_WILL_DRAW);
	_tipsText->MakeSelectable(false);
	_tipsText->MakeEditable(false);
	_tipsText->SetViewColor(VIEW_COLOR);
	AddChild(_tipsText);

	// lancer la verif des tips METOS
	CheckMeTOSTips();
	
	// charger les tips
	LoadTips();

	// afficher un tips
	DisplayNextTips();
}

/**** destructeur ****/
TipsManagerBox::~TipsManagerBox()
{
	int32	nbItems;

	// vider la liste des tips
	nbItems = _tipsList.CountItems();
	for(int32 index=0;index<nbItems;index++)
		delete (BString *)(_tipsList.RemoveItem((int32)0));

	// effacer l'image
	delete _tipsIcon;
}

/**** gestion des messages ****/
void TipsManagerBox::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
	// passer au tips suivant
	case VTIPS_NEXT_TIPS_MSG:
		DisplayNextTips();
		break;
	default:
		BBox::MessageReceived(message);
	}
}

/**** dessin ****/
void TipsManagerBox::Draw(BRect updateRect)
{
	// dessin d'origine
	BBox::Draw(updateRect);
	
	// l'icone
	if(_tipsIcon!=NULL)
	{
		SetDrawingMode(B_OP_ALPHA);
		DrawBitmap(_tipsIcon,BPoint(10,13));
		SetDrawingMode(B_OP_COPY);
	}
	
	// titre
	DrawString(_dayString.String(),BPoint(50,30));
}

// =========================
// = Titre et icon du jour =
// =========================

void TipsManagerBox::SetDayInfo()
{
	time_t	day_t;
	tm		day;

	// recuperer l'image pour les tips
	// mais cela depend du jour :-)
	_tipsIcon = NULL;
	day_t = time(0);
	day = *localtime(&day_t);

	// halloween
	if((day.tm_mon==9 && day.tm_mday>27) || (day.tm_mon==10 && day.tm_mday<4))
	{
		_dayString = "Happy Halloween...";
		_tipsIcon = g_MetosApp->GetBitmap("Halloween");
	}
	
	// mayday (May 1st - socialist holiday)
	if(day.tm_mon==5 && day.tm_mday==1)
	{
		_dayString = "Happy MayDay (social justice now)...";
		_tipsIcon = g_MetosApp->GetBitmap("TipsIcon");
	}
	
	// noel
	if((day.tm_mon==11 && day.tm_mday>27) || (day.tm_mon==0 && day.tm_mday<4))
	{
		_dayString = "Mery Christmas...";
		_tipsIcon = g_MetosApp->GetBitmap("Christmas");
	}
		
	// on a pas d'icon special
	if(_tipsIcon==NULL)
	{
		_dayString = "Tip of the day...";
		_tipsIcon = g_MetosApp->GetBitmap("TipsIcon");
	}
}

// ===================
// = Tips par defaut =
// ===================

/**** verifier si on a les tips MeTOS ****/
void TipsManagerBox::CheckMeTOSTips()
{
	VTips	metosTips("MeTOS");
	
	// si les tips de MeTOS n'existe pas on va les creer
	if(!metosTips.Exist())
	{
		metosTips.AddTips("try using the CTRL key when you click on an object.");
		metosTips.AddTips("Use the right-mouse button on objects to open a popup menu.");
		metosTips.AddTips("Make sure you select *at least* one suitable constructor(s) for each object you place on a view/window. - otherwise no suitable code can be generated.");
		metosTips.AddTips("When you select more than one function for an object, code will be generate to *Derive* from it - for instance, when choosing 1 consturcor and MessageReceived for a BWindow object, the code generated will include a class derived from BWindow with that constructor and MessageReceived function.");
		metosTips.AddTips("Open a pop-up menu in the function-list tab to Save this list as a default for all future objects of this class");
		metosTips.AddTips("You can use the Arrow Keys to move selected object(s).");
		metosTips.AddTips("Use [Shift] key to select more one object. (try this on the object TreeList as well)");
		metosTips.AddTips("Right Click on an object display the copy/cut/paste popup, and more options, Lock/Unlock, tools menu and such. also, note that objects might list their special properties in the popup menu in case they have such");
		metosTips.AddTips("Create a new object using drag&drop from the Addons list into the Objects list. Remove objects using the Trash icon or the Tools/Delete item from the popup menu.");
		metosTips.AddTips("Try creating your own default function lists for objects you work with frequently, i.e BWindow::MessageReceived and a Constructor, all BControls::1_Constructor(), BeApp::BeApp(sig) etc.");
		metosTips.AddTips("Make a template blank BeIDE project that includes libbe and possibly libroot, place it in \'/boot/home/config/settings/Tracker/Tracker New Templates\' and drop the generated sources into it. once that's done, all you need to do is build it to run your newly generated code easily and quickly!");
		// sauver
		metosTips.Save();
		
		// informer de la creation des tips MeTOS
		g_MetosApp->SendDebbug("Create default Tips.",VDEBUG_WARNING);		
	}
}

// ====================
// = Gestion des tips =
// ====================

/**** charger l'ensemble des tips ****/
status_t TipsManagerBox::LoadTips()
{
	BMessage	splashProgress(M_PROGRESSINCREASE_MSG);
	BMessage	splashInfo(M_PROGRESSSTART_MSG);
	const char	*buffer = NULL;
	BMessage	tips;
	BFile		tipsFile;
	status_t	state = B_OK;
   	status_t 	err = B_NO_ERROR;
	BPath		path;
	BString		tipsPath;
	BEntry		tipsEntry;
	BEntry		entry;
	int32		nbFiles;
	int32		index;

	// trouver si le repertoire Settings existe
	if(find_directory(B_USER_SETTINGS_DIRECTORY,&path)!=B_OK)
		path.SetTo("/boot/home/config/settings");

	// verifier si le repertoire Tips existe
	// sinon le creer
	tipsPath = path.Path();
	tipsPath << "/" << METOS_SETTINGS_FOLDER;
	tipsPath << "/" << METOS_TIPS_FOLDER;

	// initialiser le compteur du splash-screen
	BDirectory dir(tipsPath.String());
	nbFiles = 0;

	while(err == B_NO_ERROR)
	{
		err = dir.GetNextEntry((BEntry*)&entry, true);			
		if(entry.InitCheck()==B_NO_ERROR && entry.Exists())
			nbFiles++;		
	}
	splashInfo.AddInt32(M_TOTALPROGRESS,nbFiles);
	splashInfo.AddString(M_TEXTDISPLAY,"Load Tips...");
	BMessenger(g_MetosApp->_splashScreen).SendMessage(&splashInfo);
	
   	// charger les addons
	err = B_NO_ERROR;
	dir.SetTo(tipsPath.String());
	while(err == B_NO_ERROR)
	{
		err = dir.GetNextEntry((BEntry*)&entry, true);			
		if(entry.InitCheck()!=B_NO_ERROR)
			break;		

		if(entry.GetPath(&path)==B_NO_ERROR)
		{
			tipsFile.SetTo(&entry,B_READ_ONLY);
			if(tips.Unflatten(&tipsFile)==B_OK)
			{
				// ajouter les textes
				index = 0;
				while(tips.FindString(V_TIPS_TEXT,index,&buffer)==B_OK)
				{
					_tipsList.AddItem(new BString(buffer));
					index++;
				}
			}
			// informer le splash-screen
			BMessenger(g_MetosApp->_splashScreen).SendMessage(&splashProgress);
		}
	}
		
	return state;
}

/**** afficher le prochain tips ****/
void TipsManagerBox::DisplayNextTips()
{
	BString		*tipsString = NULL;
	int32		tipsIndex;
	
	// recuperer un index
	tipsIndex = RandomIndex();
	tipsString = (BString *)(_tipsList.ItemAt(tipsIndex));
	
	// afficher le texte
	if(tipsString!=NULL)
		_tipsText->SetText(tipsString->String());
}

/**** trouver un index aleatoirement ****/
int32 TipsManagerBox::RandomIndex()
{
	int32	rndNumber = 0;
	
	// recuperer un nombre aleatoire
	srandom(time(0));
	rndNumber = random() % _tipsList.CountItems();

	// ok resultat
	return rndNumber;
}

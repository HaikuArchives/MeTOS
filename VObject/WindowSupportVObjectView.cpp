/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
/***************************************/
/* WindowSupportVObjectView view class */
/***************************************/
// c'est la vue que l'on ajoute a une fenetre
// pour gerer les objets
#include "WindowSupportVObjectView.h"
#include "FilterVObjectWindow.h"
#include "GUIManager.h"
#include "MetosGlobalDefs.h"
#include "MetosMessageDefs.h"
#include "MetosApp.h"
#include "VPreferenceFile.h"
#include <app/Application.h>
#include <interface/Window.h>
#include <app/Message.h>
#include <support/List.h>
#include <interface/Control.h>

// debug
//#include <iostream.h>

/**** Constructeur ****/
WindowSupportVObjectView::WindowSupportVObjectView(BRect frame)
: BView(frame,"_support-VObject-view",B_FOLLOW_ALL_SIDES, B_WILL_DRAW | B_FRAME_EVENTS | B_DRAW_ON_CHILDREN | B_FULL_UPDATE_ON_RESIZE)
{
	_filter = NULL;

	// recuperer la couleur de l'interface
	rgb_color	menuColor = ui_color(B_MENU_BACKGROUND_COLOR);
	SetViewColor(menuColor);

	// charger les preferences
	RefreshPreferences();

	// pour l'activation
	_metosActivation = false;
}

/**** Destructeur ****/
WindowSupportVObjectView::~WindowSupportVObjectView()
{
}

/**** gestion des messages ****/
void WindowSupportVObjectView::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
	case M_ACTIVE_WINDOW_MSG:
		_metosActivation = true;
		Window()->Activate();
		break;
	case M_PREFERENCE_MSG:
		RefreshPreferences();
		Invalidate();
		break;
	default:
		BView::MessageReceived(message);
	}
}

/**** attachement a la fenetre ****/
void WindowSupportVObjectView::AttachedToWindow()
{
	// appel fonction defaut
	BView::AttachedToWindow();
	
	_filter = new FilterVObjectWindow();
	
	if(LockLooper())
	{
		Window()->AddCommonFilter(_filter);
		UnlockLooper();
	}
	else
		printf("Erreur : LockLooper() dans WindowSupportVObjectView::AttachedToWindow()\n");
}

/**** detacher la vue ****/
void WindowSupportVObjectView::DetachedFromWindow()
{	
	// enlever le BMessageFilter
	if(LockLooper())
	{
		if(Window()->RemoveCommonFilter(_filter))
			delete _filter;

		UnlockLooper();
	}
	else
		printf("Erreur : LockLooper() dans WindowSupportVObjectView::DetachedFromWindow()\n");
	
	// appel fonction defaut
	BView::DetachedFromWindow();
}

/**** dessin de la vue ****/
void WindowSupportVObjectView::Draw(BRect frame)
{
	// dessin d'origine
	BView::Draw(frame);

	// dessin de la grille
	if(_useGrid==B_CONTROL_ON)
	{
		// recuperer la taille
		BRect	bounds = Bounds();
		float	x,y;
		
		// puis dessiner tous les x et y donné par le pas
		SetHighColor(0,0,0);
		for(x=_stepGrid;x<bounds.right;x+=_stepGrid)
			for(y=_stepGrid;y<bounds.bottom;y+=_stepGrid)
				StrokeRect(BRect(x,y,x,y));
	}
}

/**** dessiner apres les controles ****/
void WindowSupportVObjectView::DrawAfterChildren(BRect frame)
{
	// dessin des contours des controles
	DrawBox(this);	
}

/**** Fonction de dessin des contours ****/
void WindowSupportVObjectView::DrawBox(BView *view)
{
	//si on a pas de suivant on quitte
	if(view == NULL)
		return;

	const char	*viewName = NULL;
	int32		viewCount = 0;
	bool		selection = false;

	viewCount = view->CountChildren();
	viewName = view->Name();
	// on ne dessine pas les vue cachés avec _ devant le nom
	if(viewName!=NULL && viewName[0]!='_')
	{
		BRect	viewBounds = view->Bounds();
			
		view->ConvertToScreen(&viewBounds);
		ConvertFromScreen(&viewBounds);

		SetHighColor(0,0,0,100);
		// c'est l'objet selectionné
		selection = g_GUIManagerView->IsCurrent((BHandler *)view);
		if(selection)
			SetHighColor(_selection);
		else
		{
			selection = g_GUIManagerView->IsSelected((BHandler *)view);
			if(selection)
				SetHighColor(_backSelection);
			else // pas selectionné
				SetHighColor(180,180,180);
		}					
		// le cadre
		SetDrawingMode(B_OP_ALPHA);
		StrokeLine(BPoint(viewBounds.left,viewBounds.top+6),BPoint(viewBounds.left,viewBounds.bottom-6));
		StrokeLine(BPoint(viewBounds.right,viewBounds.top+6),BPoint(viewBounds.right,viewBounds.bottom-8));
		StrokeLine(BPoint(viewBounds.left+6,viewBounds.top),BPoint(viewBounds.right-6,viewBounds.top));
		StrokeLine(BPoint(viewBounds.left+6,viewBounds.bottom),BPoint(viewBounds.right-8,viewBounds.bottom));

		if(selection)
		{
			// les zones pleines
			FillRect(BRect(viewBounds.left,viewBounds.top,viewBounds.left+4,viewBounds.top+4));	
			FillRect(BRect(viewBounds.left,viewBounds.bottom-4,viewBounds.left+4,viewBounds.bottom));	
			FillRect(BRect(viewBounds.right-4,viewBounds.top,viewBounds.right,viewBounds.top+4));	
			FillTriangle(BPoint(viewBounds.right,viewBounds.bottom-6),BPoint(viewBounds.right,viewBounds.bottom),BPoint(viewBounds.right-6,viewBounds.bottom));
		}
		
		// ca on dessine pas en alpha
		SetHighColor(90,90,90);
		SetDrawingMode(B_OP_COPY);
		StrokeRect(BRect(viewBounds.left-1,viewBounds.top-1,viewBounds.left+5,viewBounds.top+5));	
		StrokeRect(BRect(viewBounds.left-1,viewBounds.bottom-5,viewBounds.left+5,viewBounds.bottom+1));	
		StrokeRect(BRect(viewBounds.right-5,viewBounds.top-1,viewBounds.right+1,viewBounds.top+5));	
		StrokeTriangle(BPoint(viewBounds.right+1,viewBounds.bottom-8),BPoint(viewBounds.right+1,viewBounds.bottom+1),BPoint(viewBounds.right-8,viewBounds.bottom+1));
	}

	// faire les vues suivantes
	for(int32 i=0;i<viewCount;i++)
		DrawBox(view->ChildAt(i));	
}

/**** on clique sur la fenetre ****/
void WindowSupportVObjectView::WindowActivated(bool state)
{
	// on selectionne la fenetre
	if(state && !_metosActivation)
	{	
		BMessage	message(M_SELECT_OBJECT_MSG);
		BHandler	*selected = NULL;

		// si c'est la vue support on retourne la fenetre
		selected = ((BHandler *)Window());
		
		// definir l'action
		g_GUIManager.SendMessage(M_SET_ACTION_MSG);
		
		// envoyer la selection
		message.AddBool(M_SHIFT_SELECTION,false);
		message.AddPointer(M_HANDLER_SELECTED,selected);
		be_app_messenger.SendMessage(&message);
	}
	_metosActivation = false;
}

/**** rafraichir les preferences ****/
void WindowSupportVObjectView::RefreshPreferences()
{
	// etat de la grille
	_useGrid = g_MeTOSPrefs->GetInt32(B_CONTROL_OFF,"grid-used");
	_stepGrid = g_MeTOSPrefs->GetInt32(10,"grid-step");

	// charger les couleurs
	rgb_color tempColor1 = {0,255,0,100};
	_selection = g_ColorsPrefs->GetRGBColor(tempColor1,"selction-color");
	rgb_color tempColor2 = {0,155,0,100};
	_backSelection = g_ColorsPrefs->GetRGBColor(tempColor2,"selction-back-color");

	// definir l'alpha pour le dessin
	_selection.alpha = 100;
	_backSelection.alpha = 100;
}

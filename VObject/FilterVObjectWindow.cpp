/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
/**********************/
/* WindowMessageFilter */
/**********************/
#include "FilterVObjectWindow.h"
#include "MetosGlobalDefs.h"
#include "MetosMessageDefs.h"
#include "MetosUndoDefs.h"
#include "MetosApp.h"

#include <app/Application.h>
#include <app/Message.h>
#include <interface/Window.h>
#include <interface/View.h>
#include <app/Handler.h>
#include <support/String.h>


FilterVObjectWindow::FilterVObjectWindow()
: BMessageFilter(B_ANY_DELIVERY,B_LOCAL_SOURCE)
{
}

FilterVObjectWindow::~FilterVObjectWindow()
{
}
	
/**** Filtrer les messages de la fenetres ****/
filter_result FilterVObjectWindow::Filter(BMessage* message, BHandler **target)
{
	filter_result	result = B_SKIP_MESSAGE;		

	// bloquer
	if(Looper()->Lock())
	{
	// si on essai de fermer la fenetre, on la minimize
	switch(message->what)
	{
	// clavier (surtout touche control)
	case B_KEY_DOWN:
		result = KeyDown(message);
		break;
	case M_DROP_ADDON_MSG:
		DropObject(message,*target);
		break;
	case B_MOUSE_DOWN:
		result = MouseDown(message,*target);
		break;
	case B_QUIT_REQUESTED:
		if(WindowQuit(message))
			result = B_DISPATCH_MESSAGE;
		break;
	case B_WINDOW_MOVED:
		result = WindowMoved(message);
		break;
	case B_WINDOW_RESIZED:
		result = WindowResized(message);
		break;	
	case M_PREFERENCE_MSG:
	case M_ACTIVE_WINDOW_MSG:
		*target = ((BWindow *)Looper())->ChildAt(0);
		result = B_DISPATCH_MESSAGE;
		break;		
	default:
		result = B_DISPATCH_MESSAGE;
	}
	
		// debloquer
		Looper()->Unlock();
	}
	else
		printf("Erreur : Lock() dans FilterVObjectWindow::Filter()\n");
	
	return result;
}

/**** on intercepte les touches Controles ****/
filter_result FilterVObjectWindow::KeyDown(BMessage *message)
{
	int32			modifiers;
	int32			raw_char;
	uint32			what;
	
	// recuperer les infos
	if(message->FindInt32("modifiers",&modifiers)!=B_OK)
		return B_DISPATCH_MESSAGE;

	if(message->FindInt32("raw_char",&raw_char)!=B_OK)
		return B_DISPATCH_MESSAGE;

	// on doit utiliser Ctrl
	if(!((modifiers & B_CONTROL_KEY) || (modifiers & B_COMMAND_KEY)))
		return B_DISPATCH_MESSAGE;
	
	// mettre en majuscule la touche
	raw_char -= 32;
	switch(raw_char)
	{
	case M_NEW_PROJECT_SHORTCUT:
		what = M_NEW_PROJECT_MSG;
		break;
	case M_OPEN_PROJECT_SHORTCUT:
		what = M_LOAD_PROJECT_MSG;
		break;
	case M_SAVE_PROJECT_SHORTCUT:
		what = M_SAVE_PROJECT_MSG;
		break;
	case M_UNDO_SHORTCUT:
		what = UNDO_RESTORE_MSG;
		break;
	default:
		// si on ne le gere pas on sort ici
		return B_DISPATCH_MESSAGE;
	}
		
	// ok envoyer le message
	be_app_messenger.SendMessage(what);
	
	// ok retourne resultat
	return B_SKIP_MESSAGE;
}

/**** on drag & drop addon d'objet ****/
void FilterVObjectWindow::DropObject(BMessage *message,BHandler *target)
{
	// si on a un handler destination
	// normalementon en a un !
	// on le place dans le message, c'est le destinataire du drag & drop
	if(target!=NULL)
	{
		BHandler	*selectedHandler = target;
		BWindow		*window = ((BWindow *)Looper());
		BView		*supportView = NULL;
		BPoint		dropPoint;
		BPoint		dropOffset;

		// trouver d'ou on drop le point de drag&drop	
		// puis on va convertir dans l'echelle du BHandler destination
		message->FindPoint("_drop_point_",&dropPoint);
		message->FindPoint("_drop_offset_",&dropOffset);
		dropPoint.x -= dropOffset.x;
		dropPoint.y -= dropOffset.y;
		if(target->LockLooper())
		{
			((BView *)target)->ConvertFromScreen(&dropPoint);
			target->UnlockLooper();
		}
		else
			printf("Erreur : LockLooper() dans FilterVObjectWindow::DropObject()\n");
		
		// si c'est la vue support, on prend le pointer sur la fenetre
		if(window->Lock())
		{
			supportView = window->ChildAt(0);
			window->Unlock();
		}
		else
			printf("Erreur : Lock() dans FilterVObjectWindow::DropObject()\n");
		
		if(selectedHandler == ((BHandler *)supportView))
			selectedHandler = ((BHandler *)window);

		message->AddPoint(M_DROP_POINT,dropPoint);
		message->AddPointer(M_HANDLER_SELECTED,selectedHandler);

	}
	
	// envoyer a l'application
	be_app_messenger.SendMessage(message);
}

/**** clique sur un objet ****/
filter_result FilterVObjectWindow::MouseDown(BMessage *message,BHandler *target)
{
	BWindow		*window = NULL;
	BMessage	selected(M_SELECT_OBJECT_MSG);
	BHandler	*selectedHandler = NULL;
	BView		*supportView = NULL;
	BView		*parentView = NULL;
	BString		viewName;
	BPoint		where;
	bool		shift;
	int32		buttons;
	int32		modifier;

	// si on a la touche Ctrl d'enfoncé on laisse passer le clique
	if(message->FindInt32("modifiers",&modifier)==B_OK && ((modifier & B_CONTROL_KEY) || (modifier & B_COMMAND_KEY)))
		return B_DISPATCH_MESSAGE;
		
	window = ((BWindow *)Looper());
	// la fenetre doit etre active
	if(!(window->IsActive()))
		return B_DISPATCH_MESSAGE;

	// donner le point de clique au GUIManager
	shift = false;
	selectedHandler = target;
	if(selectedHandler!=NULL && message->FindPoint("where",&where)==B_OK)
	{

		// recuperer le bouton cliqué
		message->FindInt32("buttons",&buttons);
	
		if(selectedHandler->LockLooper())
		{
			((BView *)selectedHandler)->ConvertToScreen(&where);
			viewName = ((BView *)selectedHandler)->Name();
			parentView = ((BView *)selectedHandler)->Parent();
			selectedHandler->UnlockLooper();
		}
		else
			printf("Erreur : LockLooper() dans FilterVObjectWindow::MouseDown()\n");

		// multiple selection si on ne demande pas le popup
		if(buttons!=2)
			shift = modifiers() & B_SHIFT_KEY;
		
		// si le nom contient un '_' on selectionne la vue parente (si elle existe)
		if(parentView!=NULL && viewName[0]=='_')
			selectedHandler = ((BHandler *)parentView);

		// si c'est la vue support, on selectionne la fenetre
		if(window->Lock())
		{
			supportView = window->ChildAt(0);
			window->Unlock();
		}
		else
			printf("Erreur : Lock() dans FilterVObjectWindow::MouseDown()\n");
		
		if(selectedHandler == ((BHandler *)supportView))
			selectedHandler = ((BHandler *)window);

		// on dit que l'on selectionne un objet !
		selected.AddBool(M_SHIFT_SELECTION,shift);
		selected.AddPointer(M_HANDLER_SELECTED,selectedHandler);		
		selected.AddInt32(M_MOUSE_BUTTON,buttons);
		selected.AddPoint(M_POINT_SELECTED,where);

		// on fait une selection (peux etre vide)
		be_app_messenger.SendMessage(&selected);

		// on evite le renvoi
		return B_SKIP_MESSAGE;
	}

	// on le renvoi
	return B_DISPATCH_MESSAGE;
}

/**** La fenetre bouge ****/
filter_result FilterVObjectWindow::WindowMoved(BMessage *message)
{
	BPoint		where;

	// trouver les infos
	if(message->FindPoint("where",&where)==B_OK)
	{
		// recalculer le rectangle
		BMessage	newProperties(M_OBJECT_CHANGE_MSG);
		BRect		frame;
	
		// bouger la fenetre
		if(((BWindow *)Looper())->Lock())
		{
			((BWindow *)Looper())->MoveTo(where);
			frame = ((BWindow *)Looper())->Frame();
			((BWindow *)Looper())->Unlock();
		}
		else
			printf("Erreur : Lock() dans FilterVObjectWindow::WindowMoved()\n");

		newProperties.AddPointer(M_HANDLER_SELECTED,Looper());		
		newProperties.AddRect("_frame",frame);
		be_app_messenger.SendMessage(&newProperties);

		// on evite le renvoi
		return B_SKIP_MESSAGE;
	}
	// on le renvoi
	return B_DISPATCH_MESSAGE;
}

/**** La fenetre est redimentionnée ****/
filter_result FilterVObjectWindow::WindowResized(BMessage *message)
{
	int32		width;
	int32		height;
	status_t	state;

	// trouver les infos
	state = message->FindInt32("width",&width);
	state &= message->FindInt32("height",&height);
	if(state==B_OK)
	{
		// recalculer le rectangle
		BMessage	newProperties(M_OBJECT_CHANGE_MSG);
		BRect		frame;

		// redimentionner la fenetre
		if(((BWindow *)Looper())->Lock())
		{
			((BWindow *)Looper())->ResizeTo(width,height);
			frame = ((BWindow *)Looper())->Frame();
			((BWindow *)Looper())->Unlock();
		}
		else
			printf("Erreur : Lock() dans FilterVObjectWindow::WindowResized()\n");

		// calculer la nouvelle taille
		frame.right = frame.left + width;
		frame.bottom = frame.top + height;

		newProperties.AddPointer(M_HANDLER_SELECTED,Looper());		
		newProperties.AddRect("_frame",frame);
		be_app_messenger.SendMessage(&newProperties);

		// on evite le renvoi
		return B_SKIP_MESSAGE;
	}
	// on le renvoi
	return B_DISPATCH_MESSAGE;
}

/**** quitter la fenetre ****/
bool FilterVObjectWindow::WindowQuit(BMessage *message)
{
	// attention cette fonction n'est pas appelée
	// lors de la destruction de la fenetre
	if(!((BWindow *)Looper())->IsHidden())
		((BWindow *)Looper())->Hide();

	return g_DoQuit;
}

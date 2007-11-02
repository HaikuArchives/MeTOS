/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
/**************************/
/* MeTOS GUIManager class */
/**************************/
// cette classe est un Hadler pour la gestion des message
#include "GUIManager.h"
#include "MetosMessageDefs.h"
#include "MetosGlobalDefs.h"
#include "VPreferenceFile.h"
#include "VObject.h"
#include <app/Application.h>
#include <interface/Window.h>
#include <interface/View.h>
#include <support/List.h>
#include <app/Message.h>
#include <interface/Control.h>
#include <app/Cursor.h>


// variable pour quitter
bool				g_DoQuit = false;
VPreferenceFile		*g_MeTOSPrefs = NULL;
VPreferenceFile		*g_ColorsPrefs = NULL;

// gestionnaire des objets graphique
GUIManager  *g_GUIManagerView = NULL;
BMessenger  g_GUIManager;

/**** constructeur ****/
GUIManager::GUIManager()
: BView(BRect(0,0,0,0),"MeTOS-GUI-Manager",B_FOLLOW_NONE, B_WILL_DRAW)
{
	// initialiser les variables
	_action = NO_ACTION;
	_delta_x = 0;
	_delta_y = 0;
	_cursorState = BE_STANDARD;
	SetViewColor(WHITE_COLOR);
	_firstAction = true;
	
	// liste des objets selectionnés
	_selectedObject = new BList();

	// charger les preferences
	RefreshPreferences();

	// recuperer les evenement souris
	SetEventMask(B_POINTER_EVENTS | B_KEYBOARD_EVENTS,B_NO_POINTER_HISTORY);
}

/**** destructeur ****/
GUIManager::~GUIManager()
{
	// liberer la memoire
	delete 	_selectedObject;	
}

/**** effacer toute la memoire ****/
void GUIManager::ClearMemory()
{
	// pas d'action
	_action =  NO_ACTION;			

	// vider la liste
	_selectedObject->MakeEmpty();	
}

/**** gestion des messages ****/
void GUIManager::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
	// selectionner un objet
	// puis repondre au message
	case M_SELECT_OBJECT_MSG:
		SelectObject(message);
		break;
	// definir l'action
	case M_SET_ACTION_MSG:
		SetAction(message);
		break;
	// rafraichir les preferences
	case M_PREFERENCE_MSG:
		RefreshPreferences();
		break;
	default:
		BView::MessageReceived(message);
	}
}

// ========================
// = Gestion de la souris =
// ========================

/**** clique souris ****/
void GUIManager::MouseUp(BPoint point)
{
	// annuler toute action
	_action = NO_ACTION;
	_firstAction = true;
}

/**** mouvement de la souris ****/
void GUIManager::MouseMoved(BPoint point,uint32 transit,const BMessage *message)
{
	switch(_action)
	{
	case MOVE_OBJECT:
		MoveObjectBy(point);
		break;
	case RESIZE_OBJECT:
		ResizeObjectBy(point);
		break;
	// si on a un item de selectionné on doit peux etre modifier
	// la forme du curseur
	default:
		DefineCursorDisplay();
	}
}

// ======================
// = Gestion du clavier =
// ======================

/**** on va gerer les fleches etc... ****/
void GUIManager::KeyDown(const char *bytes, int32 numBytes)
{
	VObject		*object = NULL;
	BView		*view = NULL;
	float		step_x,step_y;

	switch(bytes[0])
	{
	// deplacement
	case B_LEFT_ARROW:
		step_x = -_stepGrid;
		step_y = 0;
		break;
	case B_RIGHT_ARROW:
		step_x = _stepGrid;
		step_y = 0;
		break;
	case B_UP_ARROW:
		step_x = 0;
		step_y = -_stepGrid;
		break;
	case B_DOWN_ARROW:
		step_x = 0;
		step_y = _stepGrid;
		break;
	default:
		BView::KeyDown(bytes,numBytes);
		return;
	}

	// il y a bien un objet selectionne
	if(_selectedObject->CountItems()<2)
		return;
		
	// recuperer la vue selectionnée (de l'objet)
	object = (VObject *)(_selectedObject->ItemAt(0));
	view = ViewFromObject(object);
	if(view==NULL)
		return;
	
	// verifier que la fenetre de l'object principale est active
	if(view->Window()!=NULL && !(view->Window()->IsActive()))
		return;

	// point de deplacement
	BPoint	point;
	
	// deplacement		
	if(LeftTopCornerOfSelected(point)==B_OK)
	{
		_delta_x = 0;
		_delta_y = 0;
		point.PrintToStream();
		point.x += step_x;
		point.y += step_y;
		MoveObjectBy(point);
	}
}

// ========================================
// = Informations sur les objets courants =
// ========================================

/**** c'est la selection courante ?? ****/
bool GUIManager::IsCurrent(BHandler *object)
{
	VObject	*current = NULL;
	
	// on verifie qu'il n'est pas deja selectionne
	if(_selectedObject->CountItems()<=0)
		return false;

	current = (VObject *)(_selectedObject->ItemAt(0));
	if((BHandler *)(current->Object()) == object)
		return true;

	return false;
}

/**** est-il selectionné ****/
bool GUIManager::IsSelected(BHandler *object)
{
	int32	nbObjects;
	VObject	*current = NULL;
	
	// on verifie qu'il n'est pas deja selectionne
	nbObjects = _selectedObject->CountItems();
	for(int32 i=1;i<nbObjects;i++)
	{
		current = (VObject *)(_selectedObject->ItemAt(i));
		if((BHandler *)(current->Object()) == object)
			return true;
	}
	return false;
}

/**** selectionner un objet ****/
void GUIManager::SelectObject(BMessage *data)
{
	VObject		*object = NULL;
	
	// on doit trouver le pointer sur l'objet
	if(data->FindPointer(M_OBJECT_SELECTED,(void **)&object)!=B_OK)
		object = NULL;

	// pas de pointer null
	if(object==NULL)
	{
		// enelever tous les objets
		RemoveAll();
		return;
	}
	
	VObject		*oldSelected = NULL;
	bool		shift = false;
	
	// trouver si c'est une selection multiple
	if(data->FindBool(M_SHIFT_SELECTION,&shift)!=B_OK)
		shift = false;
		
	// si c'est pas une selection multiple on doit effacer l'ancienne
	if(!shift)
		RemoveAll();
	
	// rafraichir l'etat du curseur
	DefineCursorDisplay();
	
	// le selectionner de toute facon
	oldSelected = (VObject *)(_selectedObject->RemoveItem((int32)0));
	if(oldSelected!=NULL)
		oldSelected->Invalidate();

	// nouvelle selection
	_selectedObject->AddItem(object,0);

	// verifier si il n'est pas deja dans la liste
	if(!IsSelected((BHandler *)(object->Object())))
		_selectedObject->AddItem(object);
	
	// rafraichir l'affichage
	if(object!=NULL)
		object->Invalidate();

	// rafraichir l'etat du curseur
	DefineCursorDisplay();
}

/**** trouver une vue d'un objet ****/
BView *GUIManager::ViewFromObject(VObject *object)
{
	BView	*result = NULL;
	
	switch(object->Type())
	{
	case UNDEFINED_TYPE:
		// erreur pas de type defini
		break;
	case PROJECT_TYPE:
	case APPLICATION_TYPE:
	case MENUITEM_TYPE:
	case LISTITEM_TYPE:
	case BITMAP_TYPE:
	case SHELF_TYPE:
		// pas pour ceux-la
		break;
	default:
		{
			result = dynamic_cast<BView *>(object->Object());
			// la vue doit etre attachée
			if(result!=NULL)
				if(result->Window()==NULL)
					return NULL;
		}
	}
	
	return result;
}

/**** recuperer le coin haut gauche du premier objet selectionné ****/
status_t GUIManager::LeftTopCornerOfSelected(BPoint &point)
{
	BView	*view = NULL;
	BRect	bounds;

	// il y a bien un objet selectionne
	if(_selectedObject->CountItems()<2)
		return B_ERROR;
		
	// recuperer la vue selectionnée (de l'objet)
	view = ViewFromObject((VObject *)(_selectedObject->ItemAt(0)));
	if(view==NULL)
		return B_ERROR;

	// convertir le point courant
	if(view->LockLooper())
	{
		bounds = view->Bounds();
		view->ConvertToScreen(&bounds);
		point = bounds.LeftTop();
		ConvertFromScreen(&point);
		view->UnlockLooper();

		// ok
		return B_OK;
	}
	else
		printf("Erreur : LockLooper() dans GUIManager::LeftTopCornerOfSelected()\n");
	
	return B_ERROR;
}

// ==========================
// = Action sur l'affichage =
// ==========================

/**** invalider tous les objets selectionnés ****/
void GUIManager::InvalidateAll()
{
	int32	nbObjects;
	VObject	*object = NULL;
	
	// on deplace tous les objets
	nbObjects = _selectedObject->CountItems();
	for(int32 i=1;i<nbObjects;i++)
	{
		object = (VObject *)(_selectedObject->ItemAt(i));
		if(object!=NULL)
			object->Invalidate();
	}
}

/**** tout effacer ****/
void GUIManager::RemoveAll()
{
	int32	nbObjects;
	VObject	*object = NULL;

	// on deplace tous les objets
	nbObjects = _selectedObject->CountItems();
	for(int32 i=1;i<nbObjects;i++)
	{
		object = (VObject *)(_selectedObject->RemoveItem(1));
		if(object!=NULL)
			object->Invalidate();
	}

	// vider la liste de toute facon
	_selectedObject->MakeEmpty();
	
	// rafraichir l'etat du curseur
	DefineCursorDisplay();

	// pas d'action
	_action =  NO_ACTION;			
}

// =========================================
// = Changement d'emplacement ou de taille =
// =========================================

/**** deplacer les objets de la liste ****/
void GUIManager::MoveObjectBy(BPoint &point)
{
	BMessage	newProperties(M_OBJECT_CHANGE_MSG);
	VObject		*object = NULL;
	BView		*view = NULL;
	BView		*supportView = NULL;
	int32		nbObjects;
	BRect		bounds,frame;
	BPoint		origin;
	float		step_x,step_y;
	int32		parentLevel;
	
	// il y a bien un objet selectionne
	if(_selectedObject->CountItems()<2)
		return;
		
	// recuperer la vue selectionnée (de l'objet)
	object = (VObject *)(_selectedObject->ItemAt(0));
	view = ViewFromObject(object);
	if(view==NULL)
		return;

	// le level du parent !!
	// en effet un objet doit etre deplacé que si il est au meme level
	parentLevel = object->Level();
	
	// convertir le point courant
	if(view->LockLooper())
	{
		ConvertToScreen(&point);

		// recuperer la vue support
		supportView = view->Window()->ChildAt(0);

		// on doit avoir une vue
		bounds = view->Bounds();
		view->ConvertToScreen(&bounds);
		view->UnlockLooper();
	}
	else
		printf("Erreur : LockLooper() dans GUIManager::MoveObjectBy()\n");

	// clacul nouvel emplacement
	// determine le pas de deplacement
	origin = bounds.LeftTop();
	step_x = (origin.x - (point.x - _delta_x)) * -1;
	step_y = (origin.y - (point.y - _delta_y)) * -1;

	// on deplace tous les objets
	nbObjects = _selectedObject->CountItems();
	for(int32 i=1;i<nbObjects;i++)
	{
		object = (VObject *)(_selectedObject->ItemAt(i));
		view = ViewFromObject(object);

		// on peux avoir NULL si on a une fenetre, une appli un projet ou autre
		if(!object->IsLock() && view!=NULL && object->Level()==parentLevel)
		{
			// recarder si on utilise la fonction snap grid
			if(_useGrid==B_CONTROL_ON && _snapGrid==B_CONTROL_ON)
			{
				// bloquer la vue
				if(view->LockLooper())
				{
					bounds = view->Bounds();
					view->ConvertToScreen(&bounds);
					supportView->ConvertFromScreen(&bounds);				
					origin = bounds.LeftTop();
					origin.x = (int32)((origin.x + step_x) / _stepGrid) * _stepGrid;
					origin.y = (int32)((origin.y + step_y) / _stepGrid) * _stepGrid;
					step_x = (bounds.left - origin.x) * -1;
					step_y = (bounds.top - origin.y) * -1;		
				
					// debloquer
					view->UnlockLooper();
				}
				else
					printf("Erreur : LockLooper() dans GUIManager::MoveObjectBy()\n");
			}

			// ne pas deplacer si la difference est nulle
			if(step_x!=0 || step_y!=0)
			{				
				// si c'est la premiere fois on doit envoyer un undo
				if(_firstAction)
					newProperties.AddBool(M_NEED_UNDO_ACTION,true);
				
				// recuperer la position de l'objet
				if(view->LockLooper())
				{
					frame = view->Frame();
					view->UnlockLooper();
				
					// deplacer le rectangle de l'objet
					frame.OffsetBy(step_x,step_y);

					// deplacer l'objet
					MoveObjectTo(object,frame.left,frame.top);
						
					// envoyer le message de modif
					newProperties.AddPointer(M_HANDLER_SELECTED,(BHandler *)view);		
					newProperties.AddRect("_frame",frame);
					be_app_messenger.SendMessage(&newProperties);
				}
				else
					printf("Erreur : LockLooper() dans GUIManager::MoveObjectBy()\n");

				// vider le message
				newProperties.MakeEmpty();				
			}
		}
	}
	
	// ce n'est plus la premiere fois
	_firstAction = false;
}

/**** deplacer un objet vers un rectangle donné ****/
void GUIManager::MoveObjectTo(VObject *object,float x, float y)
{
	BRect	bounds;
	BView	*view = NULL;
	BView	*parentView = NULL;
	BView	*supportView = NULL;

	// recuperer la vue de l'objet
	view = ViewFromObject(object);

	if(view!=NULL)
	{
		// recuperer la vue parente
		parentView = view->Parent();
		if(parentView==NULL)
			parentView = view;			

		// recuperer la vue support
		if(view->LockLooper())
		{
			supportView = view->Window()->ChildAt(0);

			// deplacer
			bounds = view->Frame();
			bounds.InsetBy(-_stepGrid-1,-_stepGrid-1);
			view->MoveTo(x,y);

			// rafraichir
			parentView->ConvertToScreen(&bounds);
			supportView->ConvertFromScreen(&bounds);
			supportView->Invalidate(bounds);
			view->UnlockLooper();
		}
		else
			printf("Erreur : LockLooper() dans GUIManager::MoveObjectTo()\n");
	}
}

/**** redimentionner les objets de la liste ****/
void GUIManager::ResizeObjectBy(BPoint &point)
{
	BMessage	newProperties(M_OBJECT_CHANGE_MSG);
	VObject		*object = NULL;
	BView		*view = NULL;
	BView		*supportView = NULL;
	int32		nbObjects;
	BRect		bounds,frame;
	BPoint		origin;
	float		step_x,step_y;
	
	// il y a bien un objet selectionne
	if(_selectedObject->CountItems()<2)
		return;
		
	// recuperer la vue selectionnée (de l'objet)
	object = (VObject *)(_selectedObject->ItemAt(0));
	view = ViewFromObject(object);
	if(view==NULL)
		return;

	// convertir le point courant
	if(view->LockLooper())
	{
		ConvertToScreen(&point);

		// recuperer la vue support
		supportView = view->Window()->ChildAt(0);
	
		// on doit avoir une vue
		bounds = view->Bounds();
		view->ConvertToScreen(&bounds);
		view->UnlockLooper();
	}
	else
		printf("Erreur : LockLooper() dans GUIManager::ResizeObjectBy()\n");
	
	// clacul nouvelle taille
	// determine le pas de deplacement
	origin = bounds.RightBottom();
	step_x = (origin.x - point.x) * -1;
	step_y = (origin.y - point.y) * -1;
	
	// on deplace tous les objets
	nbObjects = _selectedObject->CountItems();
	for(int32 i=1;i<nbObjects;i++)
	{
		object = (VObject *)(_selectedObject->ItemAt(i));
		view = ViewFromObject(object);
		
		// on peux avoir NULL si on a une fenetre, une appli un projet ou autre
		if(!object->IsLock() && view!=NULL)
		{
			
			// recarder si on utilise la fonction snap grid
			if(_useGrid==B_CONTROL_ON && _resizeGrid==B_CONTROL_ON)
			{
				// bloquer la vue
				if(view->LockLooper())
				{
					bounds = view->Bounds();
					view->ConvertToScreen(&bounds);
					supportView->ConvertFromScreen(&bounds);				
					origin = bounds.RightBottom();
					origin.x = (int32)((origin.x + step_x) / _stepGrid) * _stepGrid;
					origin.y = (int32)((origin.y + step_y) / _stepGrid) * _stepGrid;
					step_x = (bounds.right - origin.x) * -1;
					step_y = (bounds.bottom - origin.y) * -1;		

					// debloquer la vue
					view->UnlockLooper();
				}
				else
					printf("Erreur : LockLooper() dans GUIManager::ResizeObjectBy()\n");
			}
			
			// ne pas deplacer si la difference est nulle
			if(step_x!=0 || step_y!=0)
			{				
				// si c'est la premiere fois on doit envoyer un undo
				if(_firstAction)
					newProperties.AddBool(M_NEED_UNDO_ACTION,true);


				// recuperer la position de l'objet
				if(view->LockLooper())
				{
					frame = view->Frame();
					view->UnlockLooper();
				
					// deplacer le rectangle de l'objet
					frame.right += step_x;
					frame.bottom += step_y;

					// deplacer l'objet
					ResizeObjectTo(object,frame.Width(),frame.Height());

					// envoyer le message de modif
					newProperties.AddPointer(M_HANDLER_SELECTED,(BHandler *)view);		
					newProperties.AddRect("_frame",frame);
					be_app_messenger.SendMessage(&newProperties);
				}
				else
					printf("Erreur : LockLooper() dans GUIManager::ResizeObjectBy()\n");
				
				// vider le message
				newProperties.MakeEmpty();
			}
		}
	}

	// ce n'est plus la premiere fois
	_firstAction = false;
}

/**** redimentionner un objet a une taille donné ****/
void GUIManager::ResizeObjectTo(VObject *object,float h,float v)
{
	BRect	bounds;
	BView	*view = NULL;
	BView	*parentView = NULL;
	BView	*supportView = NULL;

	// recuperer la vue de l'objet
	view = ViewFromObject(object);

	if(view!=NULL)
	{
		// recuperer la vue parente
		parentView = view->Parent();
		if(parentView==NULL)
			parentView = view;			

		// recuperer la vue support
		if(view->LockLooper())
		{
			supportView = view->Window()->ChildAt(0);

			// deplacer
			bounds = view->Frame();
			bounds.InsetBy(-_stepGrid-1,-_stepGrid-1);
			view->ResizeTo(h,v);

			// rafraichir
			parentView->ConvertToScreen(&bounds);
			supportView->ConvertFromScreen(&bounds);
			supportView->Invalidate(bounds);
			view->UnlockLooper();
		}
		else
			printf("Erreur : LockLooper() dans GUIManager::ResizeObjectTo()\n");
	}
}

/**** on clique sur un rectangle d'un des objets de la liste ****/
void GUIManager::SetAction(BMessage *data)
{
	BRect	bounds;
	BRect	resizeRect;
	VObject	*object = NULL;
	BView	*view = NULL;
	BPoint	origin;
	BPoint	actionPoint;
	
	// trouver des donnees si il y en a
	if(data->FindPoint(M_POINT_SELECTED,&actionPoint)==B_OK)
		_actionPoint = actionPoint;

	// il y a bien un objet selectionne
	_action =  NO_ACTION;			
	if(_selectedObject->CountItems()<2)
		return;

	object = (VObject *)(_selectedObject->ItemAt(0));
	view = ViewFromObject(object);
	if(view==NULL)
		return;

	// on doit avoir une vue
	if(view->LockLooper())
	{
		bounds = view->Bounds();
		view->ConvertToScreen(&bounds);
		view->UnlockLooper();
	}
	else
		printf("Erreur : LockLooper() dans GUIManager::SetAction()\n");

	// determiner le delta entre le point de clique
	// et l'objet (pour le deplacement)
	origin = bounds.LeftTop();
	_delta_x = _actionPoint.x - origin.x;
	_delta_y = _actionPoint.y - origin.y;

	// recherche de l'action
	// par defaut on bouge
	if(bounds.Contains(_actionPoint) && !object->IsLock())
		_action = MOVE_OBJECT;
		
	// on resize, c'est le coin en bas a droite
	resizeRect.Set(bounds.right-15,bounds.bottom-15,bounds.right,bounds.bottom);
	if(resizeRect.Contains(_actionPoint) && !object->IsLock())
		_action = RESIZE_OBJECT;
}

// ================================
// = gestion du curseur de souris =
// ================================

/**** definir l'affichage du curseur ****/
void GUIManager::DefineCursorDisplay()
{
	// on doit au moins avoir un item de selectionné
	// si on est en trainde quitter on sort
	if(_selectedObject->CountItems()==0 || g_DoQuit)
		return;
	
	VObject	*selectedObject = NULL;
	BView	*view = NULL;

	// recuperer l'object selectionné
	selectedObject = (VObject *)(_selectedObject->ItemAt(0));
	view = dynamic_cast<BView *>(selectedObject->Object());
	if(view==NULL)
		return;

	// voir maintenant ou est la souris
	BRect			bounds;
	BRect			resizeRect;
	BPoint			where;
	uint32			buttons;
	cusrsor_state	oldState = _cursorState;

	// recuperer la position de la souris
	// on doit avoir une fenetre parente
	if(Window()==NULL)
		return;

	if(LockLooper())
	{
		GetMouse(&where,&buttons);
		ConvertToScreen(&where);
		UnlockLooper();
	}
	else
		printf("Erreur : LockLooper() dans GUIManager::DefineCursorDisplay()\n");

	// verifier si la souris passe dans une zone precise
	// de notre objet
	// par defaut on utilise e curseur standard
	_cursorState = BE_STANDARD;
	if(view->Window()!=NULL && view->Window()->IsActive())
	{
		if(view->LockLooper())
		{
			bounds = view->Bounds();
			if(view->Parent())
				view->ConvertToScreen(&bounds);
			view->UnlockLooper();	
		}
		else
			printf("Erreur : LockLooper() dans GUIManager::DefineCursorDisplay()\n");
	
		// verifier si on est dans l'objet (inscrit dans son rectangle)
		if(bounds.Contains(where))
		{
			// en premier lieu on bouge
			_cursorState = MOVE_CURSOR;
		
			// mais on peux etre dans le coine de redimentionnement
			resizeRect.Set(bounds.right-15,bounds.bottom-15,bounds.right,bounds.bottom);
			if(resizeRect.Contains(where))
				_cursorState = RESIZE_CURSOR;
		}
	}

	// si l'etat a changé
	if(_cursorState!=oldState)
	{
		BMessage	defineCursor(M_SET_CURSOR_MSG);
		
		defineCursor.AddInt32(M_CURSOR_LOOK,_cursorState);
		be_app_messenger.SendMessage(&defineCursor);
	}
}

// ===========================
// = gestion des preferences =
// ===========================

/**** rafraichir les preferences ****/
void GUIManager::RefreshPreferences()
{
	_useGrid = g_MeTOSPrefs->GetInt32(B_CONTROL_OFF,"grid-used");
	_snapGrid = g_MeTOSPrefs->GetInt32(B_CONTROL_OFF,"grid-snap");
	_resizeGrid = g_MeTOSPrefs->GetInt32(B_CONTROL_OFF,"grid-resize");
	_stepGrid = g_MeTOSPrefs->GetInt32(10,"grid-step");
}

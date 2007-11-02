/*****************************************/
/* Addon de la proprietes _flags de BeOS */
/*****************************************/
#include "_flags.h"
#include <interface/CheckBox.h>

// pour les flags
#include <interface/View.h>
#include <interface/Window.h>

/**** constructeur ****/
_Flags::_Flags(entry_ref *entry)
: VPropertie("_flags",entry)
{
	// noms gerés
	BMessage	*managedName = ManagedNames();

	// ajouter les noms gerés
	managedName->AddString(VPROPERTIE_ALTERN_NAME,"_flags");

	// creer les checkBox des flags des BViews
	_checkBWillDraw			= new BCheckBox(BRect(0,0,115,15),"bWillDraw","B_WILL_DRAW",NULL);
	_checkBPulseNeeded		= new BCheckBox(BRect(0,0,115,15),"bPulseNeeded","B_PULSE_NEEDED",NULL);
	_checkBFrameEvents		= new BCheckBox(BRect(0,0,115,15),"bFrameEvents","B_FRAME_EVENTS",NULL);
	_checkBFullUpdate		= new BCheckBox(BRect(0,0,180,15),"bFullUpdate","B_FULL_UPDATE_ON_RESIZE",NULL);
	_checkBNavigable		= new BCheckBox(BRect(0,0,115,15),"bNavigable","B_NAVIGABLE",NULL);
	_checkBNavigableJump	= new BCheckBox(BRect(0,0,130,15),"bNavigableJump","B_NAVIGABLE_JUMP",NULL);
	_checkBSubpixelPrecise	= new BCheckBox(BRect(0,0,130,15),"bSubpixelPrecise","B_SUBPIXEL_PRECISE",NULL);

	// creer les checkBox des flags des BWindow
	_checkBNotMovable		= new BCheckBox(BRect(0,0,115,15),"bNotMovable","B_NOT_MOVABLE",NULL);
	_checkBNotClosable		= new BCheckBox(BRect(0,0,115,15),"bNotClosable","B_NOT_CLOSABLE",NULL);
	_checkBNotZoomable		= new BCheckBox(BRect(0,0,115,15),"bNotZoomable","B_NOT_ZOOMABLE",NULL);
	_checkBNotMinimizable	= new BCheckBox(BRect(0,0,180,15),"bNotMinimizable","B_NOT_MINIMIZABLE",NULL);
	_checkBNotHResizable	= new BCheckBox(BRect(0,0,115,15),"bNotHResizable","B_NOT_H_RESIZABLE",NULL);
	_checkBNotVResizable	= new BCheckBox(BRect(0,0,130,15),"bNotVResizable","B_NOT_V_RESIZABLE",NULL);
	_checkBNotResizable		= new BCheckBox(BRect(0,0,130,15),"bNotResizable","B_NOT_RESIZABLE",NULL);
	_checkBOutlineResize	= new BCheckBox(BRect(0,0,130,15),"bOutlineResize","B_OUTLINE_RESIZE",NULL);
	_checkBWillAcceptFC		= new BCheckBox(BRect(0,0,170,15),"bWillAcceptFC","B_WILL_ACCEPT_FIRST_CLICK",NULL);
	_checkBAvoidFront		= new BCheckBox(BRect(0,0,130,15),"bAvoidFront","B_AVOID_FRONT",NULL);
	_checkBAvoidFocus		= new BCheckBox(BRect(0,0,130,15),"bAvoidFocus","B_AVOID_FOCUS",NULL);
	_checkBNoWorkspaceA		= new BCheckBox(BRect(0,0,180,15),"bNoWorkspaceA","B_NO_WORKSPACE_ACTIVATION",NULL);
	_checkBNotAnchoredOA	= new BCheckBox(BRect(0,0,190,15),"bNotAnchoredOA","B_NOT_ANCHORED_ON_ACTIVATE",NULL);
	_checkBAsynchronousC	= new BCheckBox(BRect(0,0,180,15),"bAsynchronousC","B_ASYNCHRONOUS_CONTROLS",NULL);
}

/**** denstructeur ****/
_Flags::~_Flags()
{
	// liberer la memoire
	// check pour la vue
	delete _checkBWillDraw;
	delete _checkBPulseNeeded;
	delete _checkBFrameEvents;
	delete _checkBFullUpdate;
	delete _checkBNavigable;
	delete _checkBNavigableJump;
	delete _checkBSubpixelPrecise;
	// check pour la fenetre
	delete	_checkBNotMovable;
	delete	_checkBNotClosable;
	delete	_checkBNotZoomable;
	delete	_checkBNotMinimizable;
	delete	_checkBNotHResizable;
	delete	_checkBNotVResizable;
	delete	_checkBNotResizable;
	delete	_checkBOutlineResize;
	delete	_checkBWillAcceptFC;
	delete	_checkBAvoidFront;
	delete	_checkBAvoidFocus;
	delete	_checkBNoWorkspaceA;
	delete	_checkBNotAnchoredOA;
	delete	_checkBAsynchronousC;
}

/**** function de clonage d'une propriete ****/
VPropertie* _Flags::InstantiateVPropertie(entry_ref *entry)
{
	// creer le MObject apres MeTOS va appeler la methode CreateObject() pour reelement
	// creer l'objet graphique (fenetre,vue etc...)
	return (new _Flags(entry));
}

/**** gerer l'affichage ****/
void _Flags::Draw(BView *owner,BRect bounds,bool complete)
{
	BString		flags("Flags : ");
	rgb_color	highColor;

	// sauver la couleur de fond
	highColor = owner->HighColor();

	flags << _flags;
	owner->SetLowColor(owner->HighColor());
	owner->SetHighColor(0,0,0);
	owner->DrawString(flags.String(),BPoint(bounds.left+5,bounds.top+15));	
	
	// restaurer
	owner->SetHighColor(highColor);
}

/**** recuperer la valeur qui nous interesse ****/
void _Flags::SetProperties(BMessage *properties)
{
	if(properties!=NULL)
	{
		// verifier que la proprietes existe dans l'item
		properties->FindInt32(PropertiesName(),Index(),&_flags);
		properties->FindInt32(VOBJECT_TYPE,&_type);		
	}
}

/**** renvoyer la bonne valeur ****/
void _Flags::Properties(BMessage *objectProperties)
{
	// mettre a jour le flags
	objectProperties->AddInt32(PropertiesName(),_flags);
}

/**** selection l'item pour editer la donnée ****/
void _Flags::SelectItem(BView *owner,BRect bounds)
{
	switch(_type)
	{
	// cas d'une fenetre
	case WINDOW_TYPE:
		{
			// selectionner les checkbox en fonction de l'etat du flag
			_checkBNotMovable->SetValue(_flags & B_NOT_MOVABLE);
			_checkBNotClosable->SetValue(_flags & B_NOT_CLOSABLE);
			_checkBNotZoomable->SetValue(_flags & B_NOT_ZOOMABLE);
			_checkBNotMinimizable->SetValue(_flags & B_NOT_MINIMIZABLE);
			_checkBNotHResizable->SetValue(_flags & B_NOT_H_RESIZABLE);
			_checkBNotVResizable->SetValue(_flags & B_NOT_V_RESIZABLE);
			_checkBNotResizable->SetValue(_flags & B_NOT_RESIZABLE);
			_checkBOutlineResize->SetValue(_flags & B_OUTLINE_RESIZE);
			_checkBWillAcceptFC->SetValue(_flags & B_WILL_ACCEPT_FIRST_CLICK);
			_checkBAvoidFront->SetValue(_flags & B_AVOID_FRONT);
			_checkBAvoidFocus->SetValue(_flags & B_AVOID_FOCUS);
			_checkBNoWorkspaceA->SetValue(_flags & B_NO_WORKSPACE_ACTIVATION);
			_checkBNotAnchoredOA->SetValue(_flags & B_NOT_ANCHORED_ON_ACTIVATE);
			_checkBAsynchronousC->SetValue(_flags & B_ASYNCHRONOUS_CONTROLS);

			// ajouter les checkBox
			AddCheckBox(owner,_checkBNotMovable,bounds.left+3,bounds.bottom);
			AddCheckBox(owner,_checkBNotClosable,bounds.left+3,bounds.bottom+15);
			AddCheckBox(owner,_checkBNotZoomable,bounds.left+3,bounds.bottom+30);
			AddCheckBox(owner,_checkBNotMinimizable,bounds.left+3,bounds.bottom+45);
			AddCheckBox(owner,_checkBNotHResizable,bounds.left+3,bounds.bottom+60);
			AddCheckBox(owner,_checkBNotVResizable,bounds.left+3,bounds.bottom+75);
			AddCheckBox(owner,_checkBNotResizable,bounds.left+3,bounds.bottom+90);
			AddCheckBox(owner,_checkBOutlineResize,bounds.left+140,bounds.bottom);
			AddCheckBox(owner,_checkBWillAcceptFC,bounds.left+140,bounds.bottom+15);
			AddCheckBox(owner,_checkBAvoidFront,bounds.left+140,bounds.bottom+30);
			AddCheckBox(owner,_checkBAvoidFocus,bounds.left+140,bounds.bottom+45);
			AddCheckBox(owner,_checkBNoWorkspaceA,bounds.left+140,bounds.bottom+60);
			AddCheckBox(owner,_checkBNotAnchoredOA,bounds.left+140,bounds.bottom+75);
			AddCheckBox(owner,_checkBAsynchronousC,bounds.left+140,bounds.bottom+90);
			
			// fixer la taille
			_newSize = 115;
		}
		break;
	default:
		{
			// fixer les valeurs
			_checkBWillDraw->SetValue(_flags & B_WILL_DRAW);
			_checkBPulseNeeded->SetValue(_flags & B_PULSE_NEEDED);
			_checkBFrameEvents->SetValue(_flags & B_FRAME_EVENTS);
			_checkBFullUpdate->SetValue(_flags & B_FULL_UPDATE_ON_RESIZE);
			_checkBNavigable->SetValue(_flags & B_NAVIGABLE);
			_checkBNavigableJump->SetValue(_flags & B_NAVIGABLE_JUMP);
			_checkBSubpixelPrecise->SetValue(_flags & B_SUBPIXEL_PRECISE);
			
			// ajouter les checkBox
			AddCheckBox(owner,_checkBWillDraw,bounds.left+3,bounds.bottom);
			AddCheckBox(owner,_checkBPulseNeeded,bounds.left+3,bounds.bottom+15);
			AddCheckBox(owner,_checkBFrameEvents,bounds.left+3,bounds.bottom+30);
			AddCheckBox(owner,_checkBFullUpdate,bounds.left+3,bounds.bottom+45);
			AddCheckBox(owner,_checkBNavigable,bounds.left+120,bounds.bottom);
			AddCheckBox(owner,_checkBNavigableJump,bounds.left+120,bounds.bottom+15);
			AddCheckBox(owner,_checkBSubpixelPrecise,bounds.left+120,bounds.bottom+30);
			
			// fixer la taille
			_newSize = 65;
		}
	}

	// ajuster la taille
	SetHeight(bounds.Height() + _newSize);
}

/**** deselection l'item ****/
void _Flags::UnSelectItem(BView *owner,BRect bounds)
{
	int32		newFlags = 0;
	
	// ajuster la taille
	SetHeight(bounds.Height() - _newSize);

	switch(_type)
	{
	// cas d'une fenetre
	case WINDOW_TYPE:
		{
			// mettre a jour les valeurs
			if(_checkBNotMovable->Value())
				newFlags |= B_NOT_MOVABLE;
			if(_checkBNotClosable->Value())
				newFlags |= B_NOT_CLOSABLE;
			if(_checkBNotZoomable->Value())
				newFlags |= B_NOT_ZOOMABLE;
			if(_checkBNotMinimizable->Value())
				newFlags |= B_NOT_MINIMIZABLE;
			if(_checkBNotHResizable->Value())
				newFlags |= B_NOT_H_RESIZABLE;
			if(_checkBNotVResizable->Value())
				newFlags |= B_NOT_V_RESIZABLE;
			if(_checkBNotResizable->Value())
				newFlags |= B_NOT_RESIZABLE;
			if(_checkBOutlineResize->Value())
				newFlags |= B_OUTLINE_RESIZE;
			if(_checkBWillAcceptFC->Value())
				newFlags |= B_WILL_ACCEPT_FIRST_CLICK;
			if(_checkBAvoidFront->Value())
				newFlags |= B_AVOID_FRONT;
			if(_checkBAvoidFocus->Value())
				newFlags |= B_AVOID_FOCUS;
			if(_checkBNoWorkspaceA->Value())
				newFlags |= B_NO_WORKSPACE_ACTIVATION;
			if(_checkBNotAnchoredOA->Value())
				newFlags |= B_NOT_ANCHORED_ON_ACTIVATE;
			if(_checkBAsynchronousC->Value())
				newFlags |= B_ASYNCHRONOUS_CONTROLS;

			// enlever les checkBox
			owner->RemoveChild(_checkBNotMovable);
			owner->RemoveChild(_checkBNotClosable);
			owner->RemoveChild(_checkBNotZoomable);
			owner->RemoveChild(_checkBNotMinimizable);
			owner->RemoveChild(_checkBNotHResizable);
			owner->RemoveChild(_checkBNotVResizable);
			owner->RemoveChild(_checkBNotResizable);
			owner->RemoveChild(_checkBOutlineResize);
			owner->RemoveChild(_checkBWillAcceptFC);
			owner->RemoveChild(_checkBAvoidFront);
			owner->RemoveChild(_checkBAvoidFocus);
			owner->RemoveChild(_checkBNoWorkspaceA);
			owner->RemoveChild(_checkBNotAnchoredOA);
			owner->RemoveChild(_checkBAsynchronousC);
		}
		break;
	default:
		{
			if(_checkBWillDraw->Value())
				newFlags |= B_WILL_DRAW;
			if(_checkBPulseNeeded->Value())
				newFlags |= B_PULSE_NEEDED;
			if(_checkBFrameEvents->Value())
				newFlags |= B_FRAME_EVENTS;
			if(_checkBFullUpdate->Value())
				newFlags |= B_FULL_UPDATE_ON_RESIZE;
			if(_checkBNavigable->Value())
				newFlags |= B_NAVIGABLE;
			if(_checkBNavigableJump->Value())
				newFlags |= B_NAVIGABLE_JUMP;
			if(_checkBSubpixelPrecise->Value())
				newFlags |= B_SUBPIXEL_PRECISE;

			// enlever les checkBox
			owner->RemoveChild(_checkBWillDraw);
			owner->RemoveChild(_checkBPulseNeeded);
			owner->RemoveChild(_checkBFrameEvents);
			owner->RemoveChild(_checkBFullUpdate);
			owner->RemoveChild(_checkBNavigable);
			owner->RemoveChild(_checkBNavigableJump);
			owner->RemoveChild(_checkBSubpixelPrecise);
		}
	}
	
	// mettre a jour
	SetChange(newFlags!=_flags);
	_flags = newFlags;
}

/**** ajouter la checkBox a l'ecran ****/
void _Flags::AddCheckBox(BView *owner,BCheckBox *checkBox,float x,float y)
{
	// ajouter la chackBox et modifier la couleur de fond
	owner->AddChild(checkBox);
	checkBox->SetViewColor(_itemColor);
	checkBox->SetLowColor(_itemColor);
	checkBox->MoveTo(x,y);
}

/**** recuperer la valeur sour forme de chaine ****/
void _Flags::ValueAsString(BMessage *properties,BString &value,int32 index)
{
	int32		flags;
	int32		type;
	
	// construire la chaine avec les Flags
	if(properties->FindInt32(PropertiesName(),index,&flags)==B_OK)
	{
		// ensuite c'est fonction du type d'objet
		properties->FindInt32(VOBJECT_TYPE,&type);

		switch(type)
		{
		// cas d'une fenetre
		case WINDOW_TYPE:
			{
				if(flags & B_NOT_MOVABLE)
					value << " | B_NOT_MOVABLE";
				if(flags & B_NOT_CLOSABLE)
					value << " | B_NOT_CLOSABLE";
				if(flags & B_NOT_ZOOMABLE)
					value << " | B_NOT_ZOOMABLE";
				if(flags & B_NOT_MINIMIZABLE)
					value << " | B_NOT_MINIMIZABLE";
				if(flags & B_NOT_H_RESIZABLE)
					value << " | B_NOT_H_RESIZABLE";
				if(flags & B_NOT_V_RESIZABLE)
					value << " | B_NOT_V_RESIZABLE";
				if(flags & B_NOT_RESIZABLE)
					value << " | B_NOT_RESIZABLE";
				if(flags & B_OUTLINE_RESIZE)
					value << " | B_OUTLINE_RESIZE";
				if(flags & B_WILL_ACCEPT_FIRST_CLICK)
					value << " | B_WILL_ACCEPT_FIRST_CLICK";
				if(flags & B_AVOID_FRONT)
					value << " | B_AVOID_FRONT";
				if(flags & B_AVOID_FOCUS)
					value << " | B_AVOID_FOCUS";
				if(flags & B_NO_WORKSPACE_ACTIVATION)
					value << " | B_NO_WORKSPACE_ACTIVATION";
				if(flags & B_NOT_ANCHORED_ON_ACTIVATE)
					value << " | B_NOT_ANCHORED_ON_ACTIVATE";
				if(flags & B_ASYNCHRONOUS_CONTROLS)		
					value << " | B_ASYNCHRONOUS_CONTROLS";				
			}
			break;
		default:
			{
				if(flags & B_WILL_DRAW)
					value << " | B_WILL_DRAW";
				if(flags & B_PULSE_NEEDED)
					value << " | B_PULSE_NEEDED";
				if(flags & B_FRAME_EVENTS)
					value << " | B_FRAME_EVENTS";
				if(flags & B_FULL_UPDATE_ON_RESIZE)
					value << " | B_FULL_UPDATE_ON_RESIZE";
				if(flags & B_NAVIGABLE)
					value << " | B_NAVIGABLE";
				if(flags & B_NAVIGABLE_JUMP)
					value << " | B_NAVIGABLE_JUMP";
				if(flags & B_SUBPIXEL_PRECISE)			
					value << " | B_SUBPIXEL_PRECISE";
			}
		}
	
		// enlever le " | " en debut de chaine
		if(value.Length()>0)
			value.Remove(0,3);
	}
}

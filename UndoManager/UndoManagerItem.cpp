/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
/*******************************/
/* MeTOS UndoManagerItem class */
/*******************************/
#include "UndoManagerItem.h"
#include "MetosMessageDefs.h"
#include "MetosGlobalDefs.h"
#include "MetosUndoDefs.h"
#include "VPreferenceFile.h"
#include "ObjectManagerWin.h"
#include "MetosApp.h"
#include "MetosLanguagesDefs.h"
#include "VLanguage.h"

#include <app/Message.h>
#include <app/Messenger.h>
#include <interface/View.h>
#include <interface/Font.h>

/**** constructeur ****/
UndoManagerItem::UndoManagerItem(BMessage *info)
: BListItem()
{
	// recuperer le message qui contient
	// l'action qui a ete faite avec ces données
	_infos = info;

	// taille par defaut
	_itemHeight = ITEM_HEIGHT;
	
	// extraire les infos du message
	InitInfo();

	// extraire les données
	ExtractDatas();
	
	// charger les preferences
	RefreshPreferences();
}

/**** destructeur ****/
UndoManagerItem::~UndoManagerItem()
{
	int32	nbString;

	// liberer la liste des noms
	nbString = _nameLine.CountItems();
	for(int32 i=0;i<nbString;i++)
		delete (BString *)(_nameLine.RemoveItem((int32)0));

	// liberer la liste des infos
	nbString = _infoLine.CountItems();
	for(int32 i=0;i<nbString;i++)
		delete (BString *)(_infoLine.RemoveItem((int32)0));

	// liberer la memoire
	delete _infos;
}

/**** dessin ****/
void UndoManagerItem::DrawItem(BView *owner,BRect bounds,bool complete)
{
	// dessiner le cadre
	owner->SetHighColor(LIGHT_2_COLOR);
	owner->StrokeLine(BPoint(bounds.left,bounds.top),BPoint(bounds.right,bounds.top));	
	owner->StrokeLine(BPoint(bounds.left,bounds.top),BPoint(bounds.left,bounds.bottom));	

	owner->SetHighColor(DARK_2_COLOR);
	owner->StrokeLine(BPoint(bounds.left,bounds.bottom),BPoint(bounds.right,bounds.bottom));	
	owner->StrokeLine(BPoint(bounds.right,bounds.top+1),BPoint(bounds.right,bounds.bottom));	
	
	owner->SetHighColor(DARK_1_COLOR);
	owner->StrokeLine(BPoint(bounds.left+1,bounds.bottom-1),BPoint(bounds.right-1,bounds.bottom-1));	

	bounds.InsetBy(1,1);
	// remplir avec la couleur de fond
	if(IsSelected())
		owner->SetHighColor(_selectionColor);
	else
		owner->SetHighColor(_itemColor);
	owner->FillRect(bounds);

	// l'info
	owner->SetLowColor(owner->HighColor());
	owner->SetHighColor(0,0,0);
	owner->DrawString(_typeName.String(),BPoint(bounds.left+5,bounds.top+9));

	// est-on en mode etendu
	if(Height()>ITEM_HEIGHT)
	{
		int32		nbInfoLines;
		BString		*name = NULL;
		BString		*info = NULL;
		float		position;
		float		delta;
		
		delta = 10;
		nbInfoLines = _nameLine.CountItems();
		for(int32 i=0;i<nbInfoLines;i++)
		{
			// determiner la position du text
			position = bounds.top + (i+1)*12;
			if(i==nbInfoLines-1)
				delta = 5;
			
			// recuperer les chaines a afficher
			name = (BString *)(_nameLine.ItemAt(i));
			info = (BString *)(_infoLine.ItemAt(i));
			
			owner->SetHighColor(BLACK_COLOR);
			owner->StrokeLine(BPoint(bounds.left+7,position),BPoint(bounds.left+7,position+delta),B_MIXED_COLORS);	
			owner->StrokeLine(BPoint(bounds.left+7,position+6),BPoint(bounds.left+13,position+6),B_MIXED_COLORS);	
			owner->DrawString(name->String(),BPoint(bounds.left+17,position+8));
			owner->DrawString(info->String(),BPoint(bounds.left+17+_maxNameSize+5,position+8));
		}
	}
}

/**** gerer la taille de l'item ****/
void UndoManagerItem::Update(BView *owner, const BFont *font)
{
	// changer la taille
	SetHeight(ITEM_HEIGHT);
}

/**** trouver le type de undo et l'action ****/
void UndoManagerItem::InitInfo()
{
	int32	type;
	
	// cette fonction va essayer d'extraire les infos
	// contenu dans le message _infos pour afficher au mieux
	// des details sur le undo
	if(_infos->FindInt32(UNDO_ACTION_TYPE,&type)!=B_OK)
		type = UNDO_TYPE_UNDIFINED;
	
	// en fonction du type on regarde ce qu'on peux faire
	switch(type)
	{
		case UNDO_CUT_OBJECT:
			_typeName = g_Languages->GetTranslationFor(L_UNDO_OBJECT_CUT);
			break;
		case UNDO_PASTE_OBJECT:
			_typeName = g_Languages->GetTranslationFor(L_UNDO_OBJECT_PASTE);
			break;
		case UNDO_DELETE_OBJECT:
			_typeName = g_Languages->GetTranslationFor(L_UNDO_OBJECT_DELETE);
			break;
		case UNDO_CHANGE_PROPERTIES_OBJECT:
			_typeName = g_Languages->GetTranslationFor(L_UNDO_OBJECT_CHANGE);
			break;
		case UNDO_TYPE_UNDIFINED:
		default:
			// non definit
			_typeName = g_Languages->GetTranslationFor(L_UNDO_OBJECT_UNDIFINED);
	}
}

/**** extraire les info du messages ****/
void UndoManagerItem::ExtractDatas()
{
	// on va extraire les infos du message
	// et remplir la liste interne avec des BString a afficher
	int32			index;
	int32			nbItemsFind;
	char			*nameFound;
	uint32			type; 
	int32			count;
	BString			*name = NULL;
	BString			*info = NULL;
	BFont			font(be_plain_font);
	float			nameSize;
	

	// trouver les donnees
	index = 0;
	nbItemsFind = 0;
	while(_infos->GetInfo(B_ANY_TYPE, index, &nameFound, &type, &count)==B_OK)
	{
		// verfifier pour tous les items de meme nom
		for(int32 nbValue=0; nbValue<count; nbValue++)
		{
			// recuperer les nom
			name = new BString(nameFound);
			_nameLine.AddItem(name);
		
			// retenir la taille max des noms
			nameSize = font.StringWidth(nameFound);
			if(nameSize>_maxNameSize);
				_maxNameSize = nameSize;
		
			// recuperer l'info sur les données
			info = new BString(": ");
			switch(type)
			{
				// recuperer un boolean
				case B_BOOL_TYPE:
					{
						bool	value;
					
						_infos->FindBool(nameFound,nbValue,&value);
						if(value)
							*info << "True";
						else
							*info << "False";
					}
					break;
				// recuperer un entier 8bit
				case B_INT8_TYPE:
					{
						int8	value;

						_infos->FindInt8(nameFound,nbValue,&value);
						*info << (int32)value;
					}
					break;
				// recuperer un entier 16bit
				case B_INT16_TYPE:
					{
						int16	value;

						_infos->FindInt16(nameFound,nbValue,&value);
						*info << (int32)value;
					}
					break;
				// recuperer un entier 32bit
				case B_INT32_TYPE:
					{
						int32	value;

						_infos->FindInt32(nameFound,nbValue,&value);
						*info << (int32)value;
					}
					break;
				// recuperer un entier 64bit
				case B_INT64_TYPE:
					{
						int64	value;

						_infos->FindInt64(nameFound,nbValue,&value);
						*info << (int64)value;
					}
					break;
				// recuperer un float
				case B_FLOAT_TYPE:
					{
						float	value;

						_infos->FindFloat(nameFound,nbValue,&value);
						*info << value;
					}
					break;
				// recuperer un double
				case B_DOUBLE_TYPE:
					{
						double	value;

						_infos->FindDouble(nameFound,nbValue,&value);
						*info << (int64)value;
					}
					break;
				// recuperer une chaine
				case B_STRING_TYPE:
					{
						BString	value;

						_infos->FindString(nameFound,nbValue,&value);
						*info << value;
					}
					break;
				// recuperer un BPoint
				case B_POINT_TYPE:
					{
						BPoint	value;

						_infos->FindPoint(nameFound,nbValue,&value);
						*info << "x : ";
						*info << value.x;
						*info << "y : ";
						*info << value.y;
					}
					break;
				// recuperer un BRect
				case B_RECT_TYPE:
					{
						BRect	value;

						_infos->FindRect(nameFound,nbValue,&value);
						*info << "x : ";
						*info << value.left;
						*info << "y : ";
						*info << value.top;
						*info << "w : ";
						*info << value.right - value.left;
						*info << "h : ";
						*info << value.bottom - value.top;
					}
					break;
				case B_MESSAGE_TYPE:
					{
						BMessage	message;
						int32		what;
						
						_infos->FindMessage(nameFound,nbValue,&message);
						what = message.what;
						*info << (char)(*(((uint8 *)&what) + 3));
						*info << (char)(*(((uint8 *)&what) + 2));
						*info << (char)(*(((uint8 *)&what) + 1));
						*info << (char)(*((uint8 *)&what));
					}
					break;
				case B_REF_TYPE:
				case B_MESSENGER_TYPE:
				case B_POINTER_TYPE:
				default:
				{
					info->Append(g_Languages->GetTranslationFor(L_UNDO_OBJECT_UNDIFINED));
				}
			}
			
			// ajouter l'info sur les données
			_infoLine.AddItem(info);
		
			// nombre d'items totals (attention a ceux de meme nom)
			nbItemsFind++;
		
		}
		// passer au suivant
		index++;
	}
	
	// fixer la taille etendu
	_itemHeight = (nbItemsFind * 12) + 12;
}

/**** restorer l'action ****/
void UndoManagerItem::Restore()
{
	int32	type;
	
	// cette fonction va essayer d'extraire les infos
	// contenu dans le message _infos pour afficher au mieux
	// des details sur le undo
	if(_infos->FindInt32(UNDO_ACTION_TYPE,&type)!=B_OK)
		type = UNDO_TYPE_UNDIFINED;
	
	// en fonction du type on regarde ce qu'on peux faire
	switch(type)
	{
		case UNDO_CUT_OBJECT:
		case UNDO_PASTE_OBJECT:
			break;
		case UNDO_DELETE_OBJECT:
		case UNDO_CHANGE_PROPERTIES_OBJECT:
			g_ObjectManager.SendMessage(_infos);
			break;
		case UNDO_TYPE_UNDIFINED:
		default:
			// non definit
			_typeName = g_Languages->GetTranslationFor(L_UNDO_OBJECT_UNDIFINED);
	}
}

/**** rafraichir les preferences ****/
void UndoManagerItem::RefreshPreferences()
{
	rgb_color tempColor1 = VIEW_COLOR;
	_itemColor = g_ColorsPrefs->GetRGBColor(tempColor1,"undo-item-color");	
	rgb_color tempColor2 = {200,200,255,255};
	_selectionColor = g_ColorsPrefs->GetRGBColor(tempColor2,"list-color");
}

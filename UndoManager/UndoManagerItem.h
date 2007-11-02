/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _UNDOMANAGERSITEM_H
#define _UNDOMANAGERSITEM_H
/*******************************/
/* MeTOS UndoManagerItem class */
/*******************************/
#include <interface/ListItem.h>
#include <support/String.h>
#include <support/List.h>
#include <interface/InterfaceDefs.h>

// constantes
#define		ITEM_HEIGHT					15

class UndoManagerItem : public BListItem
{
public:
	UndoManagerItem(BMessage *info);
	virtual ~UndoManagerItem();
	
	virtual void	DrawItem(BView *owner,BRect bounds,bool complete = false);
	virtual void	Update(BView *owner, const BFont *font);

			void	Restore();								// restorer l'action
			void	RefreshPreferences();					// rafraichir les preferences
			
	inline	int32		ItemHeight()		{ return _itemHeight; }	// donner la taille de l'item
	inline	BMessage	*Infos()			{ return _infos; }		// retourner les données

private:
	BMessage	*_infos;
	BString		_typeName;
	int32		_itemHeight;
	float		_maxNameSize;
	// ligne d'infos dans l'item
	BList		_nameLine;
	BList		_infoLine;
	// preferences
	rgb_color	_selectionColor;
	rgb_color	_itemColor;
	
			void	InitInfo();		// initialiser certains parametres d'info (titre etc...)
			void	ExtractDatas();	// extraire les info du messages	
};

#endif

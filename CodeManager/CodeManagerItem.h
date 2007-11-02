/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _CODEMANAGERITEM_H
#define _CODEMANAGERITEM_H
/*********************************/
/* MeTOS code manager item class */
/*********************************/
#include "VPreferenceFile.h"
#include <interface/ListItem.h>

class CColumnView;
class BBitmap;

class CodeManagerItem : public BListItem
{
public:
	CodeManagerItem();
	CodeManagerItem(const char *code,const char *generator,bool isDefault = false);
	virtual ~CodeManagerItem();

	virtual	void		DrawItem(BView *owner,BRect bounds,bool complete);	// dessin de l'item
	virtual void		Update(BView *owner, const BFont *font);			// changer la taille
			void		SetCode(const char *code);							// definir le code
			void		SetGenerator(const char *generatorID);				// definir le generateur
			void		SetDefault(bool isDefault);							// definir par defaut
			void		RefreshPreferences();								// charger les preferences

	inline	const char	*Code()			{ return _code; }		// acces au code
	inline	const char	*Generator()	{ return _generator; }	// acces au generateur pour ce code
	inline	bool		IsDefault()		{ return _isDefault; }	// savoir si c'est l'item par defaut
			
protected:
	CColumnView	*_columnView;	// vue colonne
	char		*_code;			// le code source
	char		*_generator;	// generateur de ce code
	bool		_isDefault;		// est-ce le code par defaut
	BBitmap		*_checkIcon;	// icon pour l'item par defaut
	rgb_color	_itemColor;		// couleur de selection

			void		InitData();		// initialiser des données obligatoire
};

#endif

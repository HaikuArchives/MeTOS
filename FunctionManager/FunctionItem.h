/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _FUNCTIONITEM_H
#define _FUNCTIONITEM_H
/********************/
/* MeTOS view class */
/********************/
#include <interface/ListItem.h>
#include <support/String.h>
#include <app/Message.h>
#include <interface/InterfaceDefs.h>

class BBitmap;

class FunctionItem : public BListItem
{
public:
	FunctionItem(const char *returnType,const char *name,int16 protection);
	FunctionItem(BMessage *archive);
	virtual ~FunctionItem();

	virtual void		DrawItem(BView *owner,BRect bounds,bool complete = false);
	virtual void		Update(BView *owner, const BFont *font);
			void		AddParameter(const char *type,const char *name,const char *value,bool firstArg);		// ajouter les parametres
			void		RefreshPreferences();
			status_t	Archive(BMessage *archive);

	inline	BString		&ReturnType()					{ return _returnType; }			// donner le type de retour
	inline	BString		&MethodeName()					{ return _name; }				// donner le nom de la fonction
	inline	BString		&Arguments()					{ return _args; }				// donner les arguments de la fonction
	inline	BMessage	*Parameters()					{ return &_parameters; }		// donner les parametres de la fonction
	inline	void		SetCheckBitmap(BBitmap *value)	{ _checkFunction = value; }		// renseigner l'image du check
	inline	void		SetSuperItem(bool value)		{ _isASuperItem = value; }		// informer que c'est un superitem
	inline	void		SetChecked(bool value)			{ _isChecked = value; }			// informer que l'element est "cheked"
	inline	void		SetConstructor(bool value)		{ _isAConstructor = value; }	// fixer si c'est un constructeur ou pas
	inline	bool		IsASuperItem()					{ return _isASuperItem; }		// verifier si l'element un super item
	inline	bool		IsChecked()						{ return _isChecked; }			// verifier si l'element est "cheked"
	inline	bool		IsAConstructor()				{ return _isAConstructor; }		// c'est un constructeur
	
private:
	BString		_returnType;
	BString		_name;
	BString		_args;
	BMessage	_parameters;
	int16		_protection;
	bool		_isASuperItem;
	bool		_isChecked;
	bool		_isAConstructor;
	BBitmap		*_checkFunction;
	rgb_color	_itemColor;
	
			void		InitData();			// initialiser les parametres
};

#endif

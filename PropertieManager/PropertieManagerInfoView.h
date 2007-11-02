/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _PROPERTIEMANAGERINFOVIEW_H
#define _PROPERTIEMANAGERINFOVIEW_H
/*************************/
/* MeTOS info view class */
/*************************/
#include <interface/View.h>
#include <support/String.h>

class VObject;
class BTextView;
class CUrlView;
class BBitmap;
class PropertieManagerWin;

class PropertieManagerInfoView : public BView
{
public:
	PropertieManagerInfoView(BRect frame,const char *name,PropertieManagerWin *support);
	virtual ~PropertieManagerInfoView();

	virtual void	MessageReceived(BMessage *message);		// reception des messages
	virtual	void 	Draw(BRect updateRect);					// affichage des parametres de l'objet courant
	
			void	SetCurrentObject(uint32 selection);		// fixer l'objet courant
			void	RefreshValues();						// rafraichir les valeurs

protected:
	uint32					_currentObject;
	PropertieManagerWin		*_supportWindow;
	// a propros
	BString			_version;
	BString			_author;
	CUrlView		*_mail;
	CUrlView		*_www;
	BTextView		*_about;
	BBitmap			*_icon;
	
	BString			_id;
	BString			_parentId;
	BString			_className;
	BString			_isBeOject;
	BString			_derivedClass;
};

#endif

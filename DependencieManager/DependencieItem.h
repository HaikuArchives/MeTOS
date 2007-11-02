/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _DEPENDENCIEITEM_H
#define _DEPENDENCIEITEM_H
/**************************/
/* MeTOS Dependencie item */
/**************************/
#include <interface/ListItem.h>
#include <interface/InterfaceDefs.h>
#include <support/String.h>
#include <storage/Path.h>

class BBitmap;
class CColumnView;

class DependencieItem : public BListItem
{
public:
	DependencieItem(const char *filepath,const char *generator,const char *MIMEtype);
	~DependencieItem();

	virtual	void		DrawItem(BView *owner,BRect bounds,bool complete);	// dessin de l'item
	virtual void		Update(BView *owner, const BFont *font);			// changer la taille
			void		SetPath(const char *path);							// definir le chemin
			void		SetMIME(const char *typeMIME);						// definir le type MIME
			void		RefreshPreferences();								// charger les preferences

	inline	char		*GeneratorID()		{ return _generatorID; }		// recuperer le generateur geré
	inline	const char	*Path()				{ return _path.Path(); }		// recuperer le chemin
	inline	const char	*MIME()				{ return _typeMIME; }			// recuperer le type MIME

private:
	CColumnView		*_columnView;	// vue colonne
	char			*_generatorID;	// id du generator de cet item
	char			_typeMIME[255];	// le type mime
	status_t		_state;			// etat de la creation du BPath
	BPath			_path;			// le type
	BBitmap			*_icon;			// icon du fichier
	BString			_truncBuffer;	// pour l'affichage
	rgb_color		_itemColor;		// couleur de selection
};

#endif

#ifndef _METOSGENERATORPREFS_H
#define _METOSGENERATORPREFS_H
/**************************/
/* MeTOS preference class */
/* pour l'application     */
/**************************/
#include "VPreference.h"
#include <ListItem.h>
#include <String.h>

#define		SELECT_METOS_GENERATOR		'Smge'

class BListView;
class BMessage;

// parametrages du generateur a utiliser
class GeneratorPrefs : public VPreference
{
public:
	GeneratorPrefs(BRect frame);
	virtual	~GeneratorPrefs();

			void		AttachedToWindow();
			void		MessageReceived(BMessage *message);
			void		SavePrefs();		// sauver les preferences

protected:
	BListView		*_generatorListView;	
	
			void		SelectDefaultGenerator();				// selection du generateur
			void		AddGeneratorsItem(BMessage *message);	// recuperer la liste des generateurs
			void		ClearList();							// vider la liste
};

// item de liste
class GeneratorListItem : public BListItem
{
public:
	GeneratorListItem(const char *id,const char *name);

			void		DrawItem(BView *owner,BRect bounds,bool complete);	// dessin
	
	inline	BString		ID()					{ return _generatorID; }	// recuperer l'id
	inline	bool		IsDefault()				{ return _default; }		// le selectionné
	inline	void		SetDefault(bool value)	{ _default = value; }		// selection de l'item
	
protected:
	BString		_generatorID;		// identifiant du generateur
	BString		_generatorName;		// nom du generateur
 	BString		_buffer;			// pour affichage
	bool		_default;			// celui qui est defini par defaut
};

#endif
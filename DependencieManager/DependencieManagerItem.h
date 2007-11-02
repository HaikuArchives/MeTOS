/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _DEPENDENCIEMANAGERITEM_H
#define _DEPENDENCIEMANAGERITEM_H
/**********************************/
/* MeTOS Dependencie manager item */
/**********************************/
#include <support/String.h>
#include <support/List.h>
#include <storage/Entry.h>

class BMessage;
class DependencieItem;

class DependencieManagerItem
{
public:
	DependencieManagerItem(BMessage *data);
	~DependencieManagerItem();

			DependencieItem		*AddDependency(entry_ref *ref,const char *generator);	// ajouter une dependance
			status_t			LoadDependency();										// charger le contenu du fichier 
			status_t			SaveDependency(BMessage *generatorList);				// sauver dans un fichier l'etat des dependances
			status_t			PreGenerateDependencies(BMessage *data,const char *ID);	// generer la liste des dependances pour un generateur donné

	inline	BString				ClassName()			{ return _className; }				// nom de la classe geréee
	inline	BList				*Dependencies()		{ return &_depenciesList; }			// liste interne des items

private:
	BString		_className;			// nom de la classe gerée
	BList		_depenciesList;		// liste des dependances
};

#endif

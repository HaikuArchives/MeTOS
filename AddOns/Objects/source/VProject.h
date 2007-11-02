#ifndef _VPROJECT_H
#define _VPROJECT_H
// on utilise un message pour gerer les projets
// car on en utilise deja pour la proprietes des objets
// donc c'est plus facile pour sauver et relire les données
 
#include "VObject.h"

class BMessage;

class VProject:	public VObject
{
public:
	VProject();
	virtual ~VProject();

	virtual status_t	CreateObject(BMessage *archive); 			// creer l'objet de l'interface (obligatoire)
	virtual	status_t	SetDefaultProperties(BMessage *properties);	// ajouter des proprietes par defaut pour la creation d'un controle (doit etre renseigné)
	virtual	status_t	SetProperties(BMessage *value,int32 index);	// modifier une des proprietes de l'objet
};

#endif
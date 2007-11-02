/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _VLANGUAGE_H
#define _VLANGUAGE_H
/*****************************************/
/* MeTOS Language class for localisation */
/*****************************************/
#include <app/Message.h>
#include <support/String.h>

// definition de la classe
class VLanguage
{
public:
	VLanguage(const char *filename);
	virtual	~VLanguage();

			status_t	Load();								// charger les données du fichiers
			const char *GetTranslationFor(const char *id);	// recuperer la traduction
			status_t	InitCheck();						// verifier l'etat d'initialisation

protected:
	BMessage	*_translations;		// message qui contient les traductions
	BString		_filename;			// nom du fichier ou trouver les traduction
	status_t	_state;				// etat de la consrtuction
};

#endif

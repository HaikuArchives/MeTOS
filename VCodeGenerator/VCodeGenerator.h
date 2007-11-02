/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _VCODEGENERATOR_H
#define _VCODEGENERATOR_H
/*****************************************/
/* MeTOS CodeGenerator file format class */
/*****************************************/
#include <interface/ListItem.h>
#include <support/String.h>
#include <kernel/image.h>
#include <app/Message.h>

// constantes
#define		L_GENERATOR_NAME			"GENERATOR_NAME"

class BBitmap;
class BEntry;
class VLanguage;

class VCodeGenerator : public BListItem
{
public:
	VCodeGenerator(const char *name,image_id addonId,BEntry *entry);
	virtual ~VCodeGenerator();

	virtual	void		StartGenerator(BMessage *archive) = 0;		// appeler le generateur
	virtual const char	*DisplayLabel() = 0;						// ce qui faut afficher

	inline	image_id	AddonId()			{ return _addonId; }		// l'identifiant de l'image de l'addon
	inline	const char	*Name()				{ return _name; }			// le nom
	inline	BString		ID()				{ return _generatorID; }	// recuperer l'ID
	inline	BMessage	*Parameters()		{ return &_parameters; }	// liste des parametres de dependance

protected:
	image_id			_addonId;		// image ID le l'addon
	BBitmap				*_miniIcon;		// icon mini
	char				*_name;			// nom du generateur
	BString				_generatorID;	// identifiant du generateur
	BMessage			_parameters;	// parametres possible pour le dependances
	VLanguage			*_language;		// language

			void		DrawItem(BView *owner,BRect bounds,bool complete = false);	// dessin de l'addon
};

#endif

/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _VTRANSLATOR_H
#define _VTRANSLATOR_H
/**************************************/
/* MeTOS Translator file format class */
/**************************************/
#include <interface/ListItem.h>
#include <support/String.h>
#include <kernel/image.h>

// type du translator Lecture/Ecriture
enum translator_type
{
	VTRANSLATOR_WRITE_ONLY	= 0,
	VTRANSLATOR_READ_ONLY	= 1,
	VTRANSLATOR_READWRITE	= 2
};

class BBitmap;
class BMessage;
class BEntry;
class BFile;

class VTranslator :	public BListItem
{
public:
	VTranslator(image_id addonId,BEntry *entry,translator_type type);
	virtual ~VTranslator();
	
	virtual	status_t	LoadFile(BFile *file,BMessage *data) = 0;		// lecture
	virtual	status_t	SaveFile(BFile *file,BMessage *data) = 0;		// ecriture
			status_t	RegisterMIMEType(const char *filePath);			// fixer le type MIME du fichier
			status_t	CheckMIMEType();								// verifier si le type MIME existe

	inline	image_id			AddonId()		{ return _addonId; }			// l'identifiant de l'image de l'addon
	inline	BString				MIMEType()		{ return _mimeType; }			// recuperer le type MIME gerer
	inline	BString				Name()			{ return _displayedName; }		// recuperer le nom affiché
	inline	BString				Description()	{ return _mimeDescription; }	// description du type MIME
	inline	BString				Extension()		{ return _extension; }			// recuperer l'extension
	inline	translator_type		Type()			{ return _type; }				// type du translator

protected:
	image_id			_addonId;			// image ID le l'addon
	BString				_mimeType;			// type mime gerer en lecture/ecriture
	BString				_extension;			// extension du type MIME
	BString				_displayedName;		// name display in the Selection in the OpenPanel
	BString				_mimeDescription;	// description du type MIME
	BBitmap				*_miniIcon;			// icon mini
	BBitmap				*_largeIcon;		// icon large
	translator_type		_type;				// type du translator
	
			void		DrawItem(BView *owner,BRect bounds,bool complete = false);	// dessin de l'addon
};

#endif

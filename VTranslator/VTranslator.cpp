/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
/**************************************/
/* MeTOS Translator file format class */
/**************************************/

#include "VTranslator.h"
#include "MetosGlobalDefs.h"
#include "MetosAppDefs.h"

#include <app/Message.h>
#include <storage/Entry.h>
#include <interface/Bitmap.h>
#include <storage/AppFileInfo.h>

/**** constructeur ****/
VTranslator::VTranslator(image_id addonId,BEntry *entry,translator_type type)
: BListItem()
{
	BAppFileInfo	fileinfo;
	BFile			file(entry, B_READ_ONLY);

	// initialiser
	_addonId = addonId;
	_type = type;
	_mimeType = "";
	_extension = "";
	_displayedName = "";

	// recuperer le fichier
	fileinfo.SetTo(&file);	

	// charger l'icon mini
	_miniIcon = new BBitmap(BRect(0,0,15,15), B_CMAP8);
	if(fileinfo.GetIcon(_miniIcon, B_MINI_ICON) != B_OK)
		fileinfo.GetTrackerIcon(_miniIcon, B_MINI_ICON);

	// charger l'icon mini
	_largeIcon = new BBitmap(BRect(0,0,31,31), B_CMAP8);
	if(fileinfo.GetIcon(_largeIcon, B_LARGE_ICON) != B_OK)
		fileinfo.GetTrackerIcon(_largeIcon, B_LARGE_ICON);
}

/**** destructeur ****/
VTranslator::~VTranslator()
{
	// liberer la memoire
	delete _miniIcon;
	delete _largeIcon;
}

/**** dessin de l'item ****/
void VTranslator::DrawItem(BView *owner,BRect bounds,bool complete)
{
}

/**** verifier l'installation du type MIME ****/
status_t VTranslator::CheckMIMEType()
{
	if(_mimeType.Length()==0 || _extension.Length()==0)
		return B_ERROR;
	
	BMimeType	MeTOSFileType(_mimeType.String());
	
	// verifier la validite du type MIME
	if(MeTOSFileType.IsValid())
	{
		// est-il installé, si non il faut l'installer
		if(!(MeTOSFileType.IsInstalled()))
		{
			// definir les icones du type
			// l'application prefere
			// l'extension de type de fichier supporté
			BMessage	extension;
		
			if(_largeIcon==NULL || _miniIcon==NULL)
				return B_ERROR;
			
			extension.AddString("extensions",_extension);
			MeTOSFileType.SetIcon(_largeIcon,B_LARGE_ICON);
			MeTOSFileType.SetIcon(_miniIcon,B_MINI_ICON);			
			MeTOSFileType.SetPreferredApp(APPLICATION_SIGNATURE);
			MeTOSFileType.SetFileExtensions(&extension);
			MeTOSFileType.SetShortDescription(_mimeDescription.String());
			
			// l'installer
			if(MeTOSFileType.Install()!=B_OK)
				return B_ERROR;			
		}
	}

	// ok
	return B_OK;
}

// fixer le type MIME du fichier
status_t VTranslator::RegisterMIMEType(const char *filePath)
{
	BMimeType	translatorMIME(_mimeType.String());

	// verifier la validité du type MIME
	if(!translatorMIME.IsValid())
		return B_ERROR;

	// renseigner le MIME Type
	BNode		fileNode(filePath);
	BNodeInfo	nodeInfo(&fileNode);

	if(nodeInfo.InitCheck()!=B_OK)
		return B_ERROR;
		
	// fixer le type
	nodeInfo.SetType(_mimeType.String());

	// ok tout est bon
	return B_OK;
}

/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author.					   */
/***********************************************/
/**************************************/
/* MeTOS Translator file format class */
/**************************************/
#include "MeTOSProjectFile.h"
#include "MetosAppDefs.h"
#include "MetosGlobalDefs.h"
#include <app/Message.h>
#include <storage/Entry.h>
#include <storage/File.h>

extern "C" __declspec(dllexport) VTranslator	*instantiate_VTranslator(image_id addonId,BEntry *entry);

/**** Function to create the MeTOS Addon ****/
VTranslator	*instantiate_VTranslator(image_id addonId,BEntry *entry)
{
	// create and return the Addon Translator
	return (new MeTOSProjectFile(addonId,entry));
}

/**** constructor ****/
MeTOSProjectFile::MeTOSProjectFile(image_id addonId,BEntry *entry)
: VTranslator(addonId,entry,VTRANSLATOR_READWRITE)
{
	// here define the mime type the translator can load
	// and the file extension
	_mimeType = FILE_MIME_TYPE;
	_extension = FILE_EXTENSION;
	_displayedName = "MeTOS Project";
	_mimeDescription = "MeTOS original Project FileType.";
}

/**** Load ****/
status_t MeTOSProjectFile::LoadFile(BFile *file,BMessage *data)
{
	// here write the code to add value to the message
	return data->Unflatten(file);
}

/**** Save ****/
status_t MeTOSProjectFile::SaveFile(BFile *file,BMessage *data)
{
	// here write the code to save value from message
	return data->Flatten(file);
}

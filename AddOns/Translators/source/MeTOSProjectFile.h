/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author.					   */
/***********************************************/
#ifndef _METOSPROJECTFILE_H
#define _METOSPROJECTFILE_H
/**************************************/
/* MeTOS Translator file format class */
/**************************************/
#include "VTranslator.h"
#include <kernel/image.h>

class BMessage;
class BFile;

class MeTOSProjectFile : public VTranslator
{
public:
	MeTOSProjectFile(image_id addonId,BEntry *entry);
	
	status_t	LoadFile(BFile *file,BMessage *data);		// lecture
	status_t	SaveFile(BFile *file,BMessage *data);		// ecriture
};

#endif

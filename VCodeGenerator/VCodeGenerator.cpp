/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
/*****************************************/
/* MeTOS CodeGenerator file format class */
/*****************************************/
#include "VCodeGenerator.h"
#include "MetosGlobalDefs.h"
#include "VLanguage.h"

#include <storage/Entry.h>
#include <interface/Bitmap.h>
#include <interface/View.h>
#include <storage/AppFileInfo.h>
#include <stdlib.h>

/**** constructeur ****/
VCodeGenerator::VCodeGenerator(const char *name,image_id addonId,BEntry *entry)
: BListItem()
{
	BAppFileInfo	fileinfo;
	BFile			file(entry, B_READ_ONLY);
	int32			nameSize = 0;

	// initialiser
	_addonId = addonId;
	_generatorID = "";

	// recuperer le fichier
	fileinfo.SetTo(&file);	

	// charger l'icon mini
	_miniIcon = new BBitmap(BRect(0,0,15,15), B_CMAP8);
	if(fileinfo.GetIcon(_miniIcon, B_MINI_ICON) != B_OK)
		fileinfo.GetTrackerIcon(_miniIcon, B_MINI_ICON);

	// charger le language, pour changer le nom de la vue
	_language = new VLanguage(name);

	// verifier
	// trouver le nom a positionner
	if(_language->InitCheck()==B_OK && _language->Load()==B_OK)
		name = _language->GetTranslationFor(L_GENERATOR_NAME);

	// initialisation
	nameSize = strlen(name) + 1;
	if(nameSize<2)
		return;

	// recopier le nom
	_name = (char *)malloc(strlen(name)+1);
	memcpy(_name,name,strlen(name)+1);
}

/**** destructeur ****/
VCodeGenerator::~VCodeGenerator()
{
	// detruire l'element de language
	delete _language;

	// liberer les chaines
	free(_name);

	// liberer la memoire
	delete _miniIcon;
}


/**** dessin de l'item ****/
void VCodeGenerator::DrawItem(BView *owner,BRect bounds,bool complete)
{}
/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
/*************************************/
/* MeTOS PASCAL Code Generator Addon */
/*************************************/
#include "BePascalCodeGenerator.h"
#include "VCodeGenerator.h"
#include "VPreferenceFile.h"

#include <Message.h>
#include <Entry.h>

extern "C" __declspec(dllexport) VCodeGenerator	*instantiate_VCodeGenerator(image_id addonId,BEntry *entry);

/**** Function to create the MeTOS Addon ****/
VCodeGenerator	*instantiate_VCodeGenerator(image_id addonId,BEntry *entry)
{
	// create and return the Addon Code Generator
	return (new BePascalCodeGenerator(addonId,entry));
}

/**** constructeur ****/
BePascalCodeGenerator::BePascalCodeGenerator(image_id addonId,BEntry *entry)
: VCodeGenerator("BePascalGenerator",addonId,entry)
{
	// fixer l'ID
	_generatorID = "BEPASCAL";
}

/**** destructeur ****/
BePascalCodeGenerator::~BePascalCodeGenerator()
{
}

/**** appeler le generateur ****/
void BePascalCodeGenerator::StartGenerator(BMessage *archive)
{
	archive->PrintToStream();
}
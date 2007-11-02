/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
/**********************************/
/* MeTOS C++ Code Generator Addon */
/**********************************/
#include "CPPCodeWin.h"
#include "VCodeGenerator.h"
#include "CPPCodeGenerator.h"
#include "VPreferenceFile.h"
#include "MetosGlobalDefs.h"
#include "VLanguage.h"

#include <app/Message.h>
#include <storage/Entry.h>

extern "C" __declspec(dllexport) VCodeGenerator	*instantiate_VCodeGenerator(image_id addonId,BEntry *entry);

/**** Function to create the MeTOS Addon ****/
VCodeGenerator	*instantiate_VCodeGenerator(image_id addonId,BEntry *entry)
{
	// create and return the Addon Code Generator
	return (new CPPCodeGenerator(addonId,entry));
}

/**** constructeur ****/
CPPCodeGenerator::CPPCodeGenerator(image_id addonId,BEntry *entry)
: VCodeGenerator("CPPCodeGenerator",addonId,entry)
{
	// identifiant du generateur
	_generatorID = "CPP";

	// creation de la fenetre
	_CPPCodeWin = new CPPCodeWin(g_MeTOSPrefs->GetRect(BRect(200,300,600,600),"cpp-code-generator-rect"),_language);
	
	// definir le nom de la fenetre
	_CPPCodeWin->SetTitle(_language->GetTranslationFor(L_GENERATOR_NAME));
}

/**** destructeur ****/
CPPCodeGenerator::~CPPCodeGenerator()
{
	// demander a quitter la fenetre
	_CPPCodeWin->Exit();
	
	// quitter la fenetre
	if(_CPPCodeWin->Lock())
		_CPPCodeWin->Quit();
}

/**** appeler le generateur ****/
void CPPCodeGenerator::StartGenerator(BMessage *archive)
{
	// passer les données
	if(_CPPCodeWin->Lock())
	{
		_CPPCodeWin->FillObjectList(archive);
		_CPPCodeWin->Unlock();
	}
	
	// afficher la fenetre
	_CPPCodeWin->Show();
}
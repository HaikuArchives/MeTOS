/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
/********************/
/* MeTOS tips class */
/********************/
#include "VTips.h"
#include "MetosGlobalDefs.h"

#include <storage/FindDirectory.h>
#include <storage/Directory.h>
#include <storage/File.h>
#include <storage/Path.h>

/**** constructeur ****/
VTips::VTips(const char *name)
: BMessage(V_TIPS_MSG)
{
	BPath		path;
	BEntry		tipsPathEntry;
	
	// trouver si le repertoire Settings existe
	if(find_directory(B_USER_SETTINGS_DIRECTORY,&path)!=B_OK)
		path.SetTo("/boot/home/config/settings");

	// le repertoire settings de MeTOS
	_filePath = path.Path();
	_filePath << "/" << METOS_SETTINGS_FOLDER;
	tipsPathEntry.SetTo(_filePath.String());
	if(!(tipsPathEntry.Exists()))
		create_directory(_filePath.String(),00777);

	// verifier si le repertoire Tips existe
	// sinon le creer
	_filePath << "/" << METOS_TIPS_FOLDER;
	tipsPathEntry.SetTo(_filePath.String());
	if(!(tipsPathEntry.Exists()))
		create_directory(_filePath.String(),00777);

	// le chemin complet
	_filePath << "/";
	_filePath << name;
	_filePath << ".tips";
	
	// l'Entry sur le fichier
	_tipsFile.SetTo(_filePath.String());
}

/**** destructeur ****/
VTips::~VTips()
{
}

/**** le fichier existe ou pas ****/
bool VTips::Exist()
{
	// verifier si le fichier existe
	return _tipsFile.Exists();
}

/**** ajouter un tips ****/
status_t VTips::AddTips(const char *newTips)
{	
	// ajout du tips
	return BMessage::AddString(V_TIPS_TEXT,newTips);
}

/**** sauver les tips ****/
status_t VTips::Save()
{	
	BFile		file;

	if(file.SetTo(&_tipsFile,B_CREATE_FILE|B_ERASE_FILE|B_WRITE_ONLY)!=B_OK)
		return B_ERROR;
	
	// sauver si le message n'est pas etre vide
	if(BMessage::IsEmpty())
		return B_ERROR;
	
	if(BMessage::Flatten(&file)!=B_OK)
		return B_ERROR;
	
	return B_OK;
}

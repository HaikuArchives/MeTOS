/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
/****************************/
/* M preferences file class */
/****************************/
#include "VPreferenceFile.h"
#include "MetosMessageDefs.h"
#include "MetosGlobalDefs.h"
#include <storage/FindDirectory.h>
#include <storage/Directory.h>
#include <storage/Path.h>
#include <storage/Entry.h>
#include <storage/File.h>
#include <interface/Control.h>
#include <support/String.h>
#include <stdlib.h>

/**** constructeur ****/
VPreferenceFile::VPreferenceFile(const char *fileName,const char *extendedPath)
: BMessage(M_PREFERENCE_MSG)
{
	BPath		path;
	BString		filePath;
	int32		filePathSize;
	BEntry		settingPathEntry;
	
	// par defaut on a pas reussi
	filePathSize = 0;
	_filePath = NULL;
	_state = B_ERROR;

	// trouver si le repertoire Settings existe
	if(find_directory(B_USER_SETTINGS_DIRECTORY,&path)!=B_OK)
		path.SetTo("/boot/home/config/settings");

	// verifier si le repertoire MeTOS existe
	// sinon le creer
	filePath = path.Path();
	filePath << "/" << METOS_SETTINGS_FOLDER;
	settingPathEntry.SetTo(filePath.String());
	if(!(settingPathEntry.Exists()))
		create_directory(filePath.String(),00777);

	// on est dans un sous repertoire
	if(extendedPath!=NULL)
	{
		BString		extendedPathString;
		
		extendedPathString = extendedPath;
		if(extendedPathString.Length()>0)
		{
			filePath << "/";
			filePath << extendedPath;
			settingPathEntry.SetTo(filePath.String());
			if(!(settingPathEntry.Exists()))
				create_directory(filePath.String(),00777);
		}
	}
	
	// ajouter le nom du fichier
	filePath << "/";
	filePath << fileName;
	filePath << ".prefs";
	
	// recopier le chemin
	filePathSize = filePath.Length() + 1;
	_filePath = (char *)malloc(filePathSize);
	memcpy(_filePath,filePath.LockBuffer(filePathSize),filePathSize);
	filePath.UnlockBuffer();

	// verifier l'existance du fichier
	_state = B_OK;
}

/**** destructeur ****/
VPreferenceFile::~VPreferenceFile()
{
	// liberer la memoire
	if(_filePath!=NULL)
		free(_filePath);
}

/**** verifier si le fichier existe ****/
bool VPreferenceFile::FileExist()
{
	// l'initialisation doit etre correcte
	if(_state!=B_OK)
		return false;

	// verifier maintenant l'existance du fichier
	BEntry		fileEntry;
	
	fileEntry.SetTo(_filePath);
	return fileEntry.Exists();
}

// ===================================
// = Lecture et ecriture des données =
// ===================================

/**** sauvegarder les prefs ****/
status_t VPreferenceFile::Save()
{	
	if(_state!=B_OK)
		return B_ERROR;

	BFile		file(_filePath,B_CREATE_FILE|B_ERASE_FILE|B_WRITE_ONLY);
	if(file.InitCheck()!=B_OK)
		return B_ERROR;
	
	// sauver si le message n'est pas etre vide
	if(IsEmpty())
		return B_ERROR;
	
	if(BMessage::Flatten(&file)!=B_OK)
		return B_ERROR;
	
	return B_OK;
}

/**** charger les prefs ****/
status_t VPreferenceFile::Load()
{
	if(_state!=B_OK)
		return B_ERROR;

	BFile		file(_filePath,B_READ_ONLY);
	if(file.InitCheck()!=B_OK)
		return B_ERROR;
	
	// charger les donnees dans le message
	BMessage::MakeEmpty();
	if(BMessage::Unflatten(&file)!=B_OK)
		return B_ERROR;
	
	return B_OK;
}

// ============================
// = Recuperation des données =
// ============================

/**** recuperer un BRect ****/
BRect VPreferenceFile::GetRect(BRect defaultValue,const char *name,int32 index)
{
	BRect	result;
	
	if(BMessage::FindRect(name,index,&result)!=B_OK)
		result = defaultValue;
		
	return result;
}

/**** recuperer un int8 ****/
int8 VPreferenceFile::GetInt8(int8 defaultValue,const char *name,int32 index)
{
	int8	result;
	
	if(BMessage::FindInt8(name,index,&result)!=B_OK)
		result = defaultValue;

	return result;
}

/**** recuperer un int32 ****/
int32 VPreferenceFile::GetInt32(int32 defaultValue,const char *name,int32 index)
{
	int32	result;
	
	if(BMessage::FindInt32(name,index,&result)!=B_OK)
		result = defaultValue;

	return result;
}

/**** recuperer un boolean ****/
bool VPreferenceFile::GetBool(bool defaultValue,const char *name,int32 index)
{
	bool	result;
	
	if(BMessage::FindBool(name,index,&result)!=B_OK)
		result = defaultValue;

	return result;
}

/**** recuperer un couleur ****/
rgb_color VPreferenceFile::GetRGBColor(rgb_color defaultValue,const char *name,int32 index = 0)
{
	rgb_color	result;

	if(BMessage::FindInt32(name,index,(int32 *)&result)!=B_OK)
		result = defaultValue;

	return result;
}

/**** recuperer une chaine ****/
BString VPreferenceFile::GetString(BString defaultValue,const char *name,int32 index)
{
	BString		result;
	
	if(BMessage::FindString(name,index,&result)!=B_OK)
		result = defaultValue;
		
	return result;
}

// ============================
// = Modification des données =
// ============================

/**** modifier un rectangle ****/
void VPreferenceFile::SetRect(BRect value,const char *name,int32 index)
{
	if(BMessage::ReplaceRect(name,index,value)!=B_OK)
		BMessage::AddRect(name,value);
}

/**** modifier un int8 ****/
void VPreferenceFile::SetInt8(int8 value,const char *name,int32 index)
{
	if(BMessage::ReplaceInt8(name,index,value)!=B_OK)
		BMessage::AddInt8(name,value);
}

/**** modifier un int32 ****/
void VPreferenceFile::SetInt32(int32 value,const char *name,int32 index)
{
	if(BMessage::ReplaceInt32(name,index,value)!=B_OK)
		BMessage::AddInt32(name,value);
}

/**** modifier un boolean ****/
void VPreferenceFile::SetBool(bool value,const char *name,int32 index)
{
	if(BMessage::ReplaceBool(name,index,value)!=B_OK)
		BMessage::AddBool(name,value);
}

/**** modifier une couleur ****/
void VPreferenceFile::SetRGBColor(rgb_color value,const char *name,int32 index)
{
	if(BMessage::ReplaceInt32(name,index,*(int32 *)&value)!=B_OK)
		BMessage::AddInt32(name,*(int32 *)&value);
}

/**** modifier une chaine ****/
void VPreferenceFile::SetString(BString value,const char *name,int32 index)
{
	if(BMessage::ReplaceString(name,index,value)!=B_OK)
		BMessage::AddString(name,value);
}

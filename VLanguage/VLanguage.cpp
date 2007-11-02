/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
/*****************************************/
/* MeTOS Language class for localisation */
/*****************************************/
#include "VLanguage.h"
#include "MetosGlobalDefs.h"
#include "MetosMessageDefs.h"
#include "MetosLanguagesDefs.h"
#include "VPreferenceFile.h"

#include <storage/File.h>
#include <storage/Path.h>
#include <app/Roster.h>
#include <app/Application.h>


/**** constructeur ****/
VLanguage::VLanguage(const char *filename)
{
	// par defaut c'est pas bon
	_state = B_ERROR;

	// verifier le nom de fichier
	if(filename==NULL)
		return;

	// creer le message porteur des traductions
	_translations = new BMessage(M_LANGUAGE_MSG);
	_filename = filename;

	// ok
	_state = B_OK;
}

/**** destructeur ****/
VLanguage::~VLanguage()
{
	// liberer la memoire
	if(_translations!=NULL)
		delete _translations;
}

/**** charger les données du fichiers ****/
status_t VLanguage::Load()
{
	// verifier l'etat de construction
	if(InitCheck()!=B_OK)
		return B_ERROR;

	// on doit avoir un pointeur d'application
	if(be_app==NULL)
		return B_ERROR;		

	BFile		file;
	BEntry		entry;
	app_info	info;
	BPath		appPath;
	BPath		languagePath;
	BString		languageFile;

	// initialiser le fichier ressource
	// et le chemin de l'appli
	be_app->GetAppInfo(&info);
	file.SetTo(&(info.ref), B_READ_ONLY);
	appPath.SetTo(&(info.ref)); 
    appPath.GetParent(&appPath);
	
	// on va construire le chemin
	languageFile.SetTo(appPath.Path());
	languageFile << "/";
	languageFile << ADDON_FOLDER;
	languageFile << "/";
	languageFile << METOS_LANGUAGES_FOLDER;
	languageFile << "/";
	languageFile << _filename;
	languageFile << ".";
	languageFile << g_MeTOSPrefs->GetString("English",METOS_LANGUAGES_TYPE);

	languagePath.SetTo(languageFile.String());
	if(languagePath.InitCheck()!=B_OK)
		return B_ERROR;
	
	// puis lire du fichier texte les IDs et la traduction qui va avec
	// si la trduction n'est pas la on gardera LANGUAGE_NO_TRANSLATION
	BFile		textFile(languageFile.String(),B_READ_ONLY);
	char		letter;
	char		id[255];
	char		word[4096];
	char		*buffer = NULL;
	short		index;
	
	// verifier la validité du fichier
	if(textFile.InitCheck()!=B_OK)
		return B_ERROR;

	// on va en premier recuperer un ID
	index = 0;
	buffer = id;

	// charger la traduction
	textFile.Seek(0,SEEK_SET);
	while(textFile.Read(&letter,1)==1)
	{
		// voir en fonction de la lettre que l'on recupere
		switch(letter)
		{
		// fin de ligne et donc de traduction
		// on ajoute l'ID et la traduction
		case LANGUAGE_END_OF_LINE:
			buffer[index] = '\0';
			_translations->AddString(id,word);
			id[0] = '\0';
			word[0] = '\0';
			index = 0;
			buffer = id;
			break;
		// separateur, fin de l'ID
		case LANGUAGE_SEPARATOR:
			buffer[index] = '\0';
			index = 0;
			buffer = word;
			break;
		default:
			buffer[index] = letter;
			index++;
		}
	}
		
	// ok tout c'est bien passé
	return B_OK;
}

/**** recuperer la traduction ****/
const char *VLanguage::GetTranslationFor(const char *id)
{
	const char	*translation = NULL;
	
	// on va essayer de trouver la correspondance dans le message
	if(_translations!=NULL && _translations->FindString(id,&translation)==B_OK)
		return translation;
	
	// on a rien trouvé
	return LANGUAGE_NO_TRANSLATION;
}

/**** verifier l'etat d'initialisation ****/
status_t VLanguage::InitCheck()
{	return _state;	}


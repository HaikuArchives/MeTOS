/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
/**********************************/
/* MeTOS Dependencie manager item */
/**********************************/
// les dependance sont les liens necessaire pour la compilation
// par exemeple les lib fichier *.so ou les headers *.h en cpp
// on doit les geré pour chaque type de langage !
// ces preferences seront sauver dans le repertoire "Dependencies"
// des settings
#include "DependencieManagerItem.h"
#include "DependencieItem.h"
#include "VPreferenceFile.h"
#include "MetosMessageDefs.h"
#include "MetosGlobalDefs.h"
#include "VObject.h"
#include <app/Message.h>
#include <storage/Path.h>

/**** constructeur ****/
DependencieManagerItem::DependencieManagerItem(BMessage *data)
{
	// recuperer le nom de la classe
	if(data->FindString(VOBJECT_CLASS,&_className)!=B_OK)
		return;

	// charger le contenu du fichier
	LoadDependency();
}

/**** destructeur ****/
DependencieManagerItem::~DependencieManagerItem()
{
	int32	nbItems;

	// vider la liste des dependances
	nbItems = _depenciesList.CountItems() - 1;
	for(int32 index = nbItems;index>=0;index--)
		delete (DependencieItem *)_depenciesList.RemoveItem(index);
}

// ==========================
// = Gestion des dependance =
// ==========================

/**** ajouter une dependance ****/
DependencieItem *DependencieManagerItem::AddDependency(entry_ref *ref,const char *generator)
{
	DependencieItem		*newItem = NULL;
	BPath				path(ref);

	// le fait de mettre NULL pour le type MIME va forcer a le trouver
	// a partir du chemin
	newItem =  new DependencieItem(path.Path(),generator,NULL);
	if(_depenciesList.AddItem(newItem))
		return newItem;

	// probleme a l'ajout
	return NULL;
}

// ============================
// = Chargement et sauvegarde =
// ============================

/**** charger le contenu du fichier ****/
status_t DependencieManagerItem::LoadDependency()
{
	// recuperer le nom de la classe
	// et le fichier de preference
	VPreferenceFile		prefsFile(_className.String(),ADDON_DEPENDENCIES_FOLDER);

	// verifier si le fichier existe
	if(!(prefsFile.FileExist()))
		return B_ERROR;

	// charger le fichier
	if(prefsFile.Load()!=B_OK)
		return B_ERROR;

	// importer les dependances
	DependencieItem		*newItem = NULL;
	BMessage			dependency(DEPENDENCIES_MSG);
	char				*generatorID = NULL;
	char 				*filepath = NULL;
	char				*typeMime = NULL;
	int32				index = 0;
	int32				indexDep = 0;

	while(prefsFile.FindMessage(DEPENDENCIES_DATA,index,&dependency)==B_OK)
	{
		// trouver le nom du generateur
		if(dependency.FindString(M_GENERATOR_ID,(const char **)&generatorID)==B_OK)
		{
			// reinitialiser
			indexDep = 0;

			// on doit bien trouver tous les elements
			while(dependency.FindString(DEPENDENCIES_PATH,indexDep,(const char **)&filepath)==B_OK)
			{
				// essayer de trouver si le type MIME est avec
				if(dependency.FindString(DEPENDENCIES_MIME,indexDep,(const char **)&typeMime)!=B_OK)
					typeMime = NULL;
					
				// ok on a tout on va creer le nouvel item
				newItem = new DependencieItem(filepath,generatorID,typeMime);
				_depenciesList.AddItem(newItem);

				// prochaine dependance
				indexDep++;
			}
		}
		
		// prochain generateur
		index++;
	}	
	
	// ok tout c'est bien passé
	return B_OK;
}

/**** sauver dans un fichier l'etat des dependances ****/
status_t DependencieManagerItem::SaveDependency(BMessage *generatorList)
{
	int32				nbItems = 0;

	// verifier que la liste n'est pas vide
	nbItems = _depenciesList.CountItems();
	if(nbItems==0)
		return B_ERROR;

	// on va ajouter les dependance une par une selon leurs
	// generateurs dans un message puis sauver dans le repertoire de config
	// recuperer le nom de la classe
	// et le fichier de preference
	VPreferenceFile		prefsFile(_className.String(),ADDON_DEPENDENCIES_FOLDER);
	DependencieItem		*current = NULL;
	BMessage			dependency(DEPENDENCIES_MSG);
	BString				generatorID;
	int32				index = 0;

	// vider le fichier avant
	prefsFile.MakeEmpty();

	// ajouter les données
	// pour chaque generateur
	while(generatorList->FindString(M_GENERATOR_ID,index,&generatorID)==B_OK)
	{
		// preparer le nouveau message
		dependency.MakeEmpty();
		dependency.AddString(M_GENERATOR_ID,generatorID);
		
		// ajouter la suite
		for(int32 i=0;i<nbItems;i++)
		{
			current = (DependencieItem *)(_depenciesList.ItemAt(i));
			if(current!=NULL && strcmp(generatorID.String(),current->GeneratorID())==0)
			{
				// ajouter le chemin et le type MIME
				dependency.AddString(DEPENDENCIES_PATH,current->Path());
				dependency.AddString(DEPENDENCIES_MIME,current->MIME());
			}
		}

		// ajouter si on a au moins un element
		if(dependency.CountNames(B_STRING_TYPE)>1)
			prefsFile.AddMessage(DEPENDENCIES_DATA,&dependency);
		
		// generateur suivant
		index++;
	}
		
	// puis sauver et retourner le resultat
	// de la sauvegarde
	return prefsFile.Save();
}

/**** generer la liste des dependances pour un generateur donné ****/
status_t DependencieManagerItem::PreGenerateDependencies(BMessage *data,const char *ID)
{
	// on va parcourir la liste des items de dependance
	// et recuperer ces dernieres si elle correspondent
	// au generateur passé en parametre
	DependencieItem		*current = NULL;
	status_t			state = B_OK;
	int32				nbItems = 0;

	// verifier que la liste n'est pas vide
	nbItems = _depenciesList.CountItems();
	if(nbItems==0)
		return B_ERROR;

	// ajouter la suite
	for(int32 i=0;i<nbItems;i++)
	{
		current = (DependencieItem *)(_depenciesList.ItemAt(i));
		if(current!=NULL && strcmp(ID,current->GeneratorID())==0)
			state &= data->AddString(DEPENDENCIES_PATH,current->Path());
	}
	
	// etat de retour
	return state;
}

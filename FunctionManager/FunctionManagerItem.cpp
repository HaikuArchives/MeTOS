/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
/*************************************/
/* MeTOS FunctionManagerItem class */
/*************************************/
#include "FunctionManagerItem.h"
#include "MetosMessageDefs.h"
#include "MetosGlobalDefs.h"
#include "CSLibrary.h"
#include "CSClass.h"
#include "CSMethod.h"
#include "CSVariable.h"
#include "FunctionItem.h"
#include "VObject.h"
#include "VPreferenceFile.h"
#include "VPreference.h"
#include "MetosApp.h"
#include <app/Application.h>
#include <interface/Bitmap.h>
#include <support/DataIO.h>
#include <app/Roster.h>
#include <storage/Path.h>
#include <storage/FindDirectory.h>

/**** constructeur ****/
FunctionManagerItem::FunctionManagerItem(BMessage *data)
{
	// on ne garde surtout pas le pointer
	// car on ne le gere pas ici mais on recupere
	// des infos a partir de ce dernier, ca evite un grosse copie
	// de message
	VObject		*object = NULL;
	
	// recuperer les infos sur l'objet
	data->FindRef(M_ADDON_ENTRY,&_addonRef);
	data->FindPointer(M_OBJECT_POINTER,(void **)&object);

	_className = object->ClassName();
	_properties = *(object->Properties());
	_settings = *(object->Settings());
	_infos = *(object->Infos());
	_lockedProperties = *(object->LockedProperties());
	_nameAsProperties = *(object->NameAsProperties());

	// initialiser
	_settings.FindBool(VOBJECT_BE_OBJECT,&_BeObject);
	_systemFile = NULL;
	_bufferCSData = NULL;

	// l'image
	_checkBitmap = g_MetosApp->GetBitmap("CheckFunction");

	// charger la liste des fonctions
	LoadFunctionsList();
}

/**** destructeur ****/
FunctionManagerItem::~FunctionManagerItem()
{
	// vider la liste des fonctions
	EmptyFunctionList();

	// liberer la bitmap
	delete _checkBitmap;
}

/**** vider la liste des fonctions ****/
void FunctionManagerItem::EmptyFunctionList()
{
	int32			nbItem;

	// liberer les items de la liste
	nbItem = _functionList.CountItems();
	for(int32 index=0;index<nbItem;index++)
		delete (FunctionItem *)(_functionList.RemoveItem((int32)0));			
}

/**** charger la liste des fonctions depuis le fichier ****/
status_t FunctionManagerItem::LoadFunctionsList()
{
	// cette fonction va essayer de trouver une liste de fonctions dans le fichiers
	// qui est dans les prefs de MeTOS, si c'est pas le cas on va essayer de trouver
	// dans le fichier CSSysteme pour recreer cette liste et la sauvegarder
	VPreferenceFile		functionsFile(_className.String(),ADDON_FUNCTIONS_FOLDER);
	status_t			state = B_OK;
	
	// verifier si le fichier existe
	if(functionsFile.FileExist() && functionsFile.Load()==B_OK)
	{
		BMessage			function(FUNCTION_MSG);

		// on va generer les items de fonction en rapport aux données
		// recuperer du fichier
		for(int32 index=0;functionsFile.FindMessage(FUNCTION_ITEM,index,&function)==B_OK;index++)
			if(!function.IsEmpty())
				SortMethodes(new FunctionItem(&function));
	}

	// sinon si on a pas d'element on va essayer de trouver les fonctions
	// l'appel de FindFromHeaderFile avec un parametre NULL
	// lance la recherche dans le fichier CS systeme
	if(_functionList.CountItems()==0)
		state &= FindFromHeaderFile(NULL);
	
	// ok tout est bon on sort
	return state;
}

/**** sauver la liste des fonctions ****/
status_t FunctionManagerItem::SaveFunctionsList()
{
	VPreferenceFile		functionsFile(_className.String(),ADDON_FUNCTIONS_FOLDER);
	status_t			state = B_OK;
	int32				nbItems = 0;

	// enregistrer les parametres
	// si biensure on a des fonctions a enregistrer
	nbItems = _functionList.CountItems();
	if(nbItems<=0)
		return state;

	FunctionItem	*functionItem = NULL;
		
	// parcourir la liste
	for(int32 index=0;index<nbItems;index++)
	{
		functionItem = (FunctionItem *)(_functionList.ItemAt(index));
		if(functionItem!=NULL && !functionItem->IsASuperItem())
			state &= functionItem->Archive(&functionsFile);	
	}	
	
	// ecrire sur disque
	state &= functionsFile.Save();
	
	// retour du resultat
	return state;
}

/**** drag&drop d'un fichier header ****/
status_t FunctionManagerItem::FindFromHeaderFile(const char *file)
{
	CSLibrary	*objectLib = NULL;
	status_t	state = B_OK;

	// recuperer les fonctions du fichier
	objectLib = GetCSLibObject(file);
	if(objectLib==NULL)
		state = B_ERROR;
	else
	{
		// vider la liste
		EmptyFunctionList();
		
		// chercher les fonctions
		state &= FindCSClass(objectLib);
	
		// apres ca on va chercher le meilleur constructeur
		// et sauver si le fichier n'existe pas encore
		CheckBestConstructor();	

		// liberer la lib
		delete objectLib;
	}

	// liberer le fichier CSSysteme si il existe
	if(_systemFile!=NULL)
		delete _systemFile;

	// pareil pour le buffer de données
	if(_bufferCSData!=NULL)
		delete _bufferCSData;
	
	// ok tout c'est bien passé
	return state;
}

/**** trouver la CSClass ****/
status_t FunctionManagerItem::FindCSClass(CSLibrary *objectLib)
{
	// cette fonction est applée si on a pas de fichier qui contient la liste
	// des fonction, ou si on drag&drop un fichier pour recreer cette liste
	// donc a la fin du traitement on va enregisterer la liste
	CSClass		*objectClass = NULL;
	
	// recuperer la CSClass
	objectClass = objectLib->GetClass(_className.String());
	
	// chercher les metodes et les trier
	if(objectClass!=NULL)
	{
		BString		debug;

		// trouver les fonctions puis detruire l'objet CSClass
		// qui a servi pour recuperer les fonctions a l'aide de la CSLib
		FindMethodes(objectClass);
		delete objectClass;

		// informer du nombre de fonctions trouvé
		debug = "";
		debug << _functionList.CountItems();
		debug << " functions finded for " << _className;
		g_MetosApp->SendDebbug(debug.String(),VDEBUG_DEBUG);
			
		// si on a des fonctions dans notre liste on sauve tout ca dans un fichier
		if(_functionList.CountItems()>0 && SaveFunctionsList()!=B_OK)
		{
			// on a pas reussi a enregistrer le fichier
			debug = "Error saving functions for ";
			debug << _className;
			g_MetosApp->SendDebbug(debug.String(),VDEBUG_ERROR);				
		}	
	}

	return B_OK;
}

/**** recuperer un objet CSLib ****/
CSLibrary *FunctionManagerItem::GetCSLibObject(const char *headerFile)
{
	CSLibrary	*objectLib = NULL;

	// si ca n'est pas un objet Be on doit parser le header
	// passé en parametre
	if(!_BeObject || headerFile!=NULL)
	{
		BString		headerName;

		// verifier si on donne un fichier header en parametre
		if(headerFile!=NULL)
			headerName = headerFile;
		else
		{
			BPath		path;
			app_info	info;
		
	 	  	// trouver le chemin de l'application
    		be_app->GetAppInfo(&info); 
    		BEntry entry(&info.ref); 
   			entry.GetPath(&path); 
	    	path.GetParent(&path);

			headerName << path.Path() << "/" << ADDON_FOLDER << "/";
			headerName << ADDON_HEADERS_FOLDER;
			headerName << "/";
			headerName << _className.String() << ".h";
		}
		
		// construire le buffer pour le parsing
		_bufferCSData = new BMallocIO();
		objectLib = new CSLibrary(_bufferCSData);
		objectLib->Parse(headerName.String());
	}
	
	// on a pas trouvé de fichier header a parser
	if(objectLib==NULL)
	{
		// ici on va essayer de trouver la classe dans le fichier CS systeme
		VPreferenceFile		prefsGenerator("Generator",CODE_GENERATION_GROUP_PATH);
		BString				libPath;
		BPath				settingPath;

		// charger la CSLib systeme dans le repertoire standard
		find_directory(B_USER_SETTINGS_DIRECTORY,&settingPath);
		libPath.SetTo(settingPath.Path());
		libPath << "/" << METOS_SETTINGS_FOLDER;
		libPath = prefsGenerator.GetString(libPath,"CS-system-path");
		libPath << "/SystemHeaders.cs";
		
		_systemFile = new BFile(libPath.String(),B_READ_ONLY);
		if(_systemFile->InitCheck()==B_OK)
			objectLib = new CSLibrary(_systemFile);
	}
	
	// retourné ce qui a ete trouvé
	return objectLib;
}

/**** renseigner la classe ****/
void FunctionManagerItem::FindMethodes(CSClass *objectClass)
{
	CSMethod		*method = NULL;
	CSVariable		*argument = NULL;
	FunctionItem	*function;
	int16			protection;
	BString			type;
	BString			name;
	BString			argsType;
	BString			argsName;
	BString			argsValue;
	BString			className;
	bool			firstArg = true;
	
	// recuperer le nom de la classe
	className = objectClass->GetName();
	
	// on va parcourir les méthodes
	method = objectClass->GetFirstMethod(CSMT_PUBLIC);
	while (method != NULL)
	{		
		// initialiser la chaine
		name.SetTo("");

		// le nom de la méthode
		name = method->GetName();

		if(method->IsVirtual() || name==className)
		{
			// initialiser la chaine
			type.SetTo("");

			// verifier si c'est un const
			if(method->IsReturnTypeConst())
				type << "const ";
		
			// type de retour
			type << method->GetReturnTypeName() << " ";
		
			// les '*'
			unsigned int pointerLevel = method->GetReturnPointerLevel();
			for (unsigned int i=0; i<pointerLevel; i++)
				type << "*";

			// gerer les references
			if(method->IsReturnTypeRef())
				type << "&";
		
			// recuperer la protection public/protected/private
			protection = method->GetZone();

			// nouvelle fonction
			function = new FunctionItem(type.String(),name.String(),protection);

			// verifier si c'est un constructeur
			if(name.FindFirst(className)==0)
				function->SetConstructor(true);

			// les arguments
			firstArg = true;
			argument = method->GetFirstArgument();
			while (argument != NULL)
			{
				// initialiser la chaine
				argsType.SetTo("");
			
				// verifier si c'est un const
				if(argument->IsConst())
					argsType << "const ";
			
				// recuperer le type
				argsType << argument->GetTypeName();

				// les '*'
				pointerLevel = argument->GetPointerLevel();
				for (unsigned int i=0; i<pointerLevel; i++)
					argsType << " *";

				// gerer les references
				if(argument->IsRef())
					argsType << " &";
			
				// le nom de l'argument, s'il est là
				argsName = argument->GetName();
			
				// la valeur par défault, si elle est là
				argsValue.SetTo("");
				if(argument->GetDefaultValue()!=NULL)
					argsValue = argument->GetDefaultValue();

				// ajouter les arguments
				function->AddParameter(argsType.String(),argsName.String(),argsValue.String(),firstArg);

				// c'est plus le premier
				firstArg = false;
			
				delete argument;
				argument = method->GetNextArgument();
			}
			// ajouter l'item a la liste
			SortMethodes(function);			
		}
		
		delete method;
		method = objectClass->GetNextMethod();
	}	
}

// =============================
// = Gestion des FunctionItems =
// =============================

/**** ajouter un superItem si on a plusieurs meme fonction ****/
void FunctionManagerItem::SortMethodes(FunctionItem *function)
{
	FunctionItem		*current = NULL;
	int32			nbItem;
	int32			index;
	BString			searchName;

	// definir la bitmap on le fait ici
	// car SortMethodes est toujours appele
	// pour chaque nouvel item
	function->SetCheckBitmap(_checkBitmap);

	// verifier si on a deja un item du meme nom
	searchName = function->MethodeName();
	nbItem = _functionList.CountItems();
	for(index = 0;index<nbItem;index++)
	{
		current = (FunctionItem *)(_functionList.ItemAt(index));
		if(searchName == current->MethodeName())
			break;
	}	

	if(index<nbItem)
	{
		// si c'est un super item on ajoute pas
		if(!current->IsASuperItem())
		{
			current = new FunctionItem("",searchName.String(),0);
			current->SetCheckBitmap(_checkBitmap);
			current->SetSuperItem(true);
			_functionList.AddItem(current,index);			
		}
	}

	// puis ajouter l'item
	_functionList.AddItem(function);
}

/**** verifier si on a bien un fichier par defaut (avec un constructeur) ****/
void FunctionManagerItem::CheckBestConstructor()
{
	// on doit avoir une liste avec des elements
	if(_functionList.IsEmpty())
		return;
	
	// avant tout on va verifier que le fichier si il existe,
	// il ne contient pas deja un constructeur, car ce n'est pas
	// alors la peine de rechercher le meilleur
	VPreferenceFile		prefsFile(_className.String(),ADDON_OBJECTS_FOLDER);
	bool				startFind = false;
	
	startFind = !(prefsFile.FileExist());
	if(!startFind)
	{
		FunctionItem	*current = NULL;
		int32			index = 0;
		int32			i = 0;
		
		// charger le fichier et verifier qu'on a un constructeur
		// de selectionné
		startFind = true;
		prefsFile.Load();
		
		while(startFind && prefsFile.FindInt32(VOBJECT_FUNCTION_INDEX,i,&index)==B_OK)
		{
			// verifier que c'est un constructeur
			current = (FunctionItem *)(_functionList.ItemAt(index));
			if(current!=NULL && current->IsAConstructor())
				startFind = false;
			
			// passer au suivant
			i++;
		}
	}
	
	// voir si on doit charcher
	if(startFind)
	{
		FunctionItem	*bestConstructor = NULL;
	
		// le trouver puis le selectionner
		// et sauver la liste
		bestConstructor = FindBestConstructor();
		if(bestConstructor!=NULL)
		{
			FunctionItem	*function = NULL;
			int32			nbFunctions;
			
			// chercher le super item correspondant
			nbFunctions = _functionList.CountItems();
			for(int32 index=0;index<nbFunctions;index++)
			{
				// trouver les constructeurs
				function = (FunctionItem *)(_functionList.ItemAt(index));		
				if(function->IsASuperItem() && function->MethodeName()==_className)
					break;

				// annuler pou passer au suivant
				function = NULL;
			}
			
			// cocher ces items 
			if(function!=NULL)
			{
				function->SetChecked(true);
				bestConstructor->SetChecked(true);
			
				SaveListAsDefault();
			}
		}
	}
}

/**** trouver le meilleur constructeur ****/
FunctionItem *FunctionManagerItem::FindBestConstructor()
{
	BMessage		*parameters = NULL;
	FunctionItem	*function = NULL;
	FunctionItem	*bestConstructor = NULL;
	int32			nbFunctions;
	int32			maxSameNameFinded = 0;
	int32			sameNameFinded = 0;
	BString			parameterName;
	type_code		typeFound = 0;
	int32			countFound = 0;
	int32			indexName = 0;
	bool			complete;
	
	// chercher dans la liste des fonctions
	nbFunctions = _functionList.CountItems();
	for(int32 index=0;index<nbFunctions;index++)
	{
		// trouver les constructeurs
		function = (FunctionItem *)(_functionList.ItemAt(index));		
		
		// verifier le constructeur
		if(function!=NULL && function->IsAConstructor())
		{
			// initialiser
			indexName = 0;
			sameNameFinded = 0;
			complete = true;
			
			// comparer par rapport aux nom des parametres
			parameters = function->Parameters();
			if(parameters!=NULL)
			{
				// trouver les nom des parametres de ce constructeur
				while(parameters->FindString(FUNCTION_PARAMETER_NAME,indexName,&parameterName)==B_OK)
				{
					// trouver la correspondance
					_nameAsProperties.FindString(parameterName.String(),&parameterName);
					if(_properties.GetInfo(parameterName.String(),&typeFound,&countFound)==B_OK)
						sameNameFinded++;
					else
						complete = false;

					// passer au suivant
					indexName++;
				}
					
				// garder le meilleur constructeur
				if(sameNameFinded>maxSameNameFinded && complete)
					bestConstructor = function;
			}			
		}
	}
		
	// donner le resultat
	return bestConstructor;
}

/**** sauver la liste par defaut ****/
void FunctionManagerItem::SaveListAsDefault()
{
	VPreferenceFile		prefsFile(_className.String(),ADDON_OBJECTS_FOLDER);
	FunctionItem		*current = NULL;
	int32				nbItem;
	
	// retenir les fonctions selectionnées
	nbItem = _functionList.CountItems();
	for(int32 index=0;index<nbItem;index++)
	{
		current = (FunctionItem *)(_functionList.ItemAt(index));
		if(current!=NULL && current->IsChecked())
			prefsFile.AddInt32(VOBJECT_FUNCTION_INDEX,index);
	}

	// sauver le fichier
	prefsFile.Save();
}

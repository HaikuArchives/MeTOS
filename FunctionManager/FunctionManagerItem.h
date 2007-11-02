/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _FUNCTIONMANAGERITEM_H
#define _FUNCTIONMANAGERITEM_H
/*************************************/
/* MeTOS FunctionManagerItem class */
/*************************************/
#include <support/String.h>
#include <support/List.h>
#include <app/Message.h>
#include <storage/Entry.h>

class CSClass;
class CSLibrary;
class BMallocIO;
class FunctionItem;
class VObject;
class BBitmap;

class FunctionManagerItem
{
public:
	FunctionManagerItem(BMessage *data);
	virtual ~FunctionManagerItem();

			status_t		FindFromHeaderFile(const char *file);	// drag&drop d'un fichier header 
			status_t		FindCSClass(CSLibrary *objectLib);		// trouver la classe pour creer la liste des fonctions
			FunctionItem	*FindBestConstructor();					// trouver le meilleur constructeur
			void			SaveListAsDefault();					// sauver la liste par defaut

	inline	BString		ClassName()			{ return _className; }			// recuperer le nom de la classe gerée
	inline	BList		*FunctionList()		{ return &_functionList; }		// liste des FunctionItems
	inline	BMessage	*Settings()			{ return &_settings; }			// les settings
	inline	BMessage	*Infos()			{ return &_infos; }				// les infos
	inline	BMessage	*LockedProperties()	{ return &_lockedProperties; }	// les proprietes bloquées
	inline	BMessage	*NameAsProperties()	{ return &_nameAsProperties; }	// correspondance non-proprietes
	inline	entry_ref	*AddOnRef()			{ return &_addonRef; }			// pour recuperer l'icon

protected:
	entry_ref	_addonRef;			// entry_ref sur l'addon (c'est pour l'icon)
	BBitmap		*_checkBitmap;		// image du check vert
	BString		_className;			// nom de la classe
	bool		_BeObject;			// objet de l'API BeOS
	BList		_functionList;		// liste des fonctions de l'objet recuperer avec la CSLib
	BFile		*_systemFile;		// fichier systeme pour les objet de type Be
	BMallocIO	*_bufferCSData;		// buffer pour les objets non Be
	// infos sur l'objet
	BMessage	_properties;		// les proprietes
	BMessage	_settings;			// les settings
	BMessage	_infos;				// les infos
	BMessage	_lockedProperties;	// les proprietes bloquées
	BMessage	_nameAsProperties;	// noms reel / nom proprietes

			void		EmptyFunctionList();							// vider la liste des fonctions
			status_t	LoadFunctionsList();							// charger la liste des fonctions depuis le fichier
			status_t	SaveFunctionsList();							// sauver la liste des fonctions
			CSLibrary	*GetCSLibObject(const char *headerFile = NULL);	// recuperer un objet CSLib pour parser un header si il est en parametre
			void		CheckBestConstructor();							// verifier si on a bien un fichier par defaut (avec un constructeur)
			void		FindMethodes(CSClass *objectClass);				// recuperer les metodes de la CSClass
			void		SortMethodes(FunctionItem *function);			// ajouter un superItem si on a plusieurs meme fonction
};

#endif

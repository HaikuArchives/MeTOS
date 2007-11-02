/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _CPPCODEITEM_H
#define _CPPCODEITEM_H
/****************************/
/* MeTOS MCPPCodeItem class */
/****************************/
#include <interface/ListItem.h>
#include <support/List.h>
#include <support/String.h>
#include <interface/InterfaceDefs.h>
#include <app/Message.h>

class VObject;
class BFile;

class CPPCodeItem : public BListItem
{
public:
	CPPCodeItem(BMessage &object);
	~CPPCodeItem();

	virtual void		DrawItem(BView *owner,BRect bounds,bool complete = false);
	virtual void		Update(BView *owner, const BFont *font);

			void		Generate();							// generer le code
			void		CreateFiles();						// creer les fichiers (*.h et *.cpp)
			void		CreateClass();						// creation de la classe
			void		CreateClassList();					// creer la liste des noms de classe
			void		CreatApplication();					// dans le cas d'une application on ajoute la fonction main
			void		AddObject(BMessage *object);		// ajouter la creation d'unobjet dans ce fichier
			void		AddInfo();							// ajouter l'information sur le fichier
			void		AddInclude();						// ajouter un #include
			void		AddClass();							// ajout des instructions class
			void		AddFunctions();						// ajouter les fonctions membres
			void		AddAlocations();					// ajouter la creation des variables
			void		AddExternalCode();					// ajouter le code externe en plus
			void		AddParametersValues(BMessage *);	// ajouter les valeurs des parametres de creation
			void		AddChild();							// ajouter les instructions AddChild, Show etc ...
			void		AddMembers();						// ajouter les varaibles membres
			BString		MemberName(BMessage *object);		// retourne le nom de la variable d'un objet
			void		ClearList();						// vider les listes

	inline	BMessage	*Object()						{ return &_baseObject; }			// prorietes de l'objet
	inline	const char	*HeaderCode()					{ return _headerCode.String(); }	// recuperer le code du header
	inline	const char	*SourceCode()					{ return _sourceCode.String(); }	// recuperer le code du header
	inline	const char	*FileName()						{ return _filesName.String(); }		// nom du fichier generé
	inline	status_t	InitCheck()						{ return _state; }					// verifier si il a ete bien creer
	inline	bool		Herited()						{ return _herited; }				// savoir si l'element est herité (donc creation de fichier)
	inline	void		SetColumnRect(float value)		{ _columnSize = value; }			// fixer la taille de la colonne
	inline	void		SetBasePath(const char *path)	{ _basePath = path; }				// chemin ou ecrire les fichiers

protected:
	// données relative a un objet
	BMessage	_settings;
	BMessage	_properties;
	BMessage	_infos;
	BMessage	_lockProperties;
	BMessage	_nameAsProperties;
	BMessage	_valueAsString;
	BString		_externalCode;
	BMessage	_functionList;
	BString		_display;				// pour le message affiché
	
	BMessage	_baseObject;			// objet de base
	status_t	_state;					// etat de creation de l'item
	BList		_objectList;			// ajouter a ce fichier un objet non herité !
	BList		_classList;				// liste des noms de classe
	BFile		*_header;				// fichier header le *.h
	BFile		*_source;				// fichier source le *.cpp
	BString		_filesName;				// nom des fichiers resultat
	BString		_basePath;				// chemin des fichiers
	float		_columnSize;			// taille de la colonne
	BString		_headerCode;			// le code generer du header
	BString		_sourceCode;			// le code generer du source
	bool		_herited;				// si l'objet est herité !
	// settings
	rgb_color	_itemColor;				// couleur de selection
	BString		_prependStringMember;	// chaine a ajouter en entete de variable membre
	int32		_indentStyle;			// style de l'indentation
	int32		_writeMakefile;			// ecriture du makefile
	
			void		RefreshPreferences();									// rafraichir les preferences
			void		CompleteString(BString &string,char c,int32	size);		// completeter la chaine avec un caractere
};

#endif
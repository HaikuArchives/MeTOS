/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
/***************************/
/* MeTOS CPPCodeItem class */
/***************************/
#include "CPPCodeItem.h"
#include "CPPCodeWin.h"
#include "VObject.h"
#include "MetosGlobalDefs.h"
#include "VPreference.h"
#include "VPreferenceFile.h"
#include "VLanguage.h"

#include <app/Message.h>
#include <storage/File.h>
#include <interface/View.h>
#include <storage/Entry.h>
#include <storage/Directory.h>
#include <interface/Control.h>
#include <time.h>

#include <stdio.h>

/**** constructeur ****/
CPPCodeItem::CPPCodeItem(BMessage &object)
: BListItem()
{
	// initialiser
	_baseObject = object;
	_state = B_OK;
	_header = NULL;
	_source = NULL;
	_headerCode = "";
	_sourceCode = "";
	_basePath = "";

	// message
	_display = (g_CPPCodeWin->Language())->GetTranslationFor("CPP_ITEM_MESSAGE");

	// recuperer les settings, les proprietes, les infos, les proprietes bloquées, la correspondance nom propreites
	_state &= _baseObject.FindMessage(VOBJECT_SETTINGS,&_settings);
	_state &= _baseObject.FindMessage(VOBJECT_PROPERTIES,&_properties);
	_state &= _baseObject.FindMessage(VOBJECT_INFOS,&_infos);
	_state &= _baseObject.FindMessage(VOBJECT_LOCKED_PROPERTIES,&_lockProperties);
	_state &= _baseObject.FindMessage(VOBJECT_NAME_AS_PROPERTIES,&_nameAsProperties);
	_state &= _baseObject.FindMessage(VOBJECT_VALUE_AS_STRING,&_valueAsString);
	_state &= _baseObject.FindString(VOBJECT_EXTERNAL_CODE,&_externalCode);

	// recuperer les fonctions qui sont selectionnées avec les parametres etc...
	// puis l'ajouter a notre archive
	_state &= _baseObject.FindMessage(VOBJECT_FUNCTIONLIST,&_functionList);

	// objet courant
	if(_properties.FindBool(VOBJECT_DERIVED_CLASS,&_herited)!=B_OK)
		_herited = false;

	// charger les prefs
	RefreshPreferences();
}

/**** destructeur ****/
CPPCodeItem::~CPPCodeItem()
{
	// vider la liste des classes
	ClearList();
	
	// liberer la memoire
	if(_header!=NULL)
		delete _header;
	if(_source!=NULL)
		delete _source;
}

/**** affichage ****/
void CPPCodeItem::DrawItem(BView *owner,BRect bounds,bool complete)
{
	BString		buffer;

	// recuperer des infos
	_properties.FindString(VOBJECT_NAME,&buffer);

	// couleur de fond
	if(IsSelected())
		owner->SetHighColor(200,200,255);
	else
		owner->SetHighColor(255,255,255);
	owner->SetLowColor(owner->HighColor());
	owner->FillRect(bounds);
	owner->SetHighColor(BLACK_COLOR);

	// nom de l'objet
	owner->DrawString(buffer.String(),BPoint(10,bounds.top+11));

	// afficher le nom des fichiers
	if(_herited)
	{
		buffer = _basePath;
		buffer << "/" << _filesName << ".h";
		owner->DrawString(buffer.String(),BPoint(_columnSize+10,bounds.bottom-15));
		buffer = _basePath;
		buffer << "/" << _filesName << ".cpp";
		owner->DrawString(buffer.String(),BPoint(_columnSize+10,bounds.bottom-3));
	}
	else
	{
		// sinon on dit qu'il n'y a pas de fichier
		owner->DrawString(_display.String(),BPoint(_columnSize+10,bounds.bottom-3));
	}
	
	// dessiner le cadre
	owner->SetHighColor(VIEW_COLOR);
	owner->StrokeLine(BPoint(_columnSize,bounds.top),BPoint(_columnSize,bounds.bottom));
	owner->StrokeLine(BPoint(0,bounds.bottom),BPoint(bounds.right,bounds.bottom));
}

/**** Changer la taille ****/
void CPPCodeItem::Update(BView *owner, const BFont *font)
{
	// fixer la taille des items
	if(_herited)
		SetHeight(25);
	else
		SetHeight(15);
}

/**** generer le code ****/
void CPPCodeItem::Generate()
{
	// vider le source et le header
	_headerCode = "";
	_sourceCode = "";

	// recuperer des infos
	_properties.FindString(VOBJECT_NAME,&_filesName);

	// creer le header et le source
	CreateClassList();
	CreateClass();
}

/**** vider les listes ****/
void CPPCodeItem::ClearList()
{
	int32	nbItem;
	
	// vider la liste des nom de classe utilisé
	nbItem = _classList.CountItems();
	for(int32 index=0;index<nbItem;index++)
		delete (BString *)(_classList.RemoveItem((int32)0));

	// vider la liste des objets
	_objectList.MakeEmpty();
}

/**** creer les fichiers ****/
void CPPCodeItem::CreateFiles()
{
	// cela ne marche que pour les objets derivé
	if(!_herited)
		return;

	BString		header;
	BString		source;
	BString		path;
	BEntry		pathEntry;

	// le chemin destination
	path << _basePath << "/";

	// verifier que le chemin existe surtout le repertoire projet
	pathEntry.SetTo(path.String());
	if(!(pathEntry.Exists()))
		create_directory(path.String(),00777);
	
	// recuperer des infos
	_properties.FindString(VOBJECT_NAME,&_filesName);
	
	// creer les fichiers
	header << path << _filesName << ".h";
	source << path << _filesName << ".cpp";
	_header = new BFile(header.String(),B_CREATE_FILE|B_ERASE_FILE|B_READ_WRITE);
	_source = new BFile(source.String(),B_CREATE_FILE|B_ERASE_FILE|B_READ_WRITE);

	// ecrire les données dedans
	_header->Write(_headerCode.String(),_headerCode.Length());
	_source->Write(_sourceCode.String(),_sourceCode.Length());
}

// ================================
// = gestion des données internes =
// ================================

/**** creation de la liste des classes utilisées ****/
void CPPCodeItem::CreateClassList()
{
	BMessage	*object = NULL;
	BMessage	settings;
	BMessage	properties;
	BString		buffer;
	int32		nbObject;
	int32		nameIndex;
	int32		nbName;
	bool		findName;
	bool		derived;
	bool		beObject;
	
	// ajouter les instruction class Nom;
	nbObject = _objectList.CountItems();
	for(int32 index=0;index<nbObject;index++)
	{
		// on considere que l'on a pas ce nom
		findName = false;
		
		object = (BMessage *)(_objectList.ItemAt(index));
		object->FindMessage(VOBJECT_SETTINGS,&settings);
		object->FindMessage(VOBJECT_PROPERTIES,&properties);		
		if(settings.FindBool(VOBJECT_BE_OBJECT,&beObject)!=B_OK)
			beObject = false;
		properties.FindBool(VOBJECT_DERIVED_CLASS,&derived);
		
		// tenir compte des classes derivées
		if(derived)
			properties.FindString(VOBJECT_NAME,&buffer);
		else
			properties.FindString(VOBJECT_CLASS,&buffer);
		
		// c'est pour montrer que c'est un objet Be
		if(beObject & !derived)
			buffer.Prepend("#");

		nbName = _classList.CountItems();
		for(nameIndex=0;nameIndex<nbName;nameIndex++)
		{
			if(buffer==*(BString *)(_classList.ItemAt(nameIndex)))
			{
				findName = true;
				break;
			}
		}
		
		if(!findName)	
			_classList.AddItem(new BString(buffer));
	}
}

// ============================
// = Creation du code complet =
// ============================

/**** creation de la classe (du header) ****/
void CPPCodeItem::CreateClass()
{
	BString		className;
	BString		headerDefineCode;
	BString		buffer;
	
	// debut du #endif
	_properties.FindString(VOBJECT_NAME,&buffer);
	headerDefineCode << "_" << buffer << "_H\n";
	headerDefineCode.ToUpper();
	_headerCode << "#ifndef " << headerDefineCode;
	_headerCode << "#define " << headerDefineCode;

	// info + perso
	AddInfo();

	// les #include
	AddInclude();

	// ajouter des instructions class
	AddClass();

	// entete de classe
	_properties.FindString(VOBJECT_CLASS,&className);
	_headerCode << "class " << buffer << " : public " << className << "\n";
	_headerCode << "{\n";
	_headerCode << "public:\n";
	AddFunctions();
	
	// ajouter les variables membres
	_headerCode << "protected:\n";
	AddMembers();
	
	// fermer la declaration de classe
	_headerCode << "};\n";
	_headerCode << "\n";

	// fin du #endif
	_headerCode << "#endif\n";
}

/**** dans le cas d'une application on ajoute la fonction main ****/
void CPPCodeItem::CreatApplication()
{
	// on va creer ici le code cpp contenant la fonction main
	// qui lance l'application
	BString		buffer;

	// les infos sur la classe
	_properties.FindString(VOBJECT_NAME,&buffer);
	
	_sourceCode << "/**** main function ****/\n";
	_sourceCode << "int main(void)";

	// gerer le style d'ecriture
	if(_indentStyle==B_CONTROL_ON)
		_sourceCode << "\n{\n";
	else
		_sourceCode << " {\n";			

	_sourceCode << "\t" << buffer << " application(";
	
	// ajouter les parametres de la creation de l'objet application
	AddParametersValues(&_baseObject);

	_sourceCode << ");\n";
	_sourceCode << "\n";
	_sourceCode << "\t// Start Application\n";
	_sourceCode << "\tapplication.Run();\n";
	_sourceCode << "\n";
	_sourceCode << "\treturn 0;\n";
	_sourceCode << "}\n";
}

// ==================================================
// = metodes d'ajout des differentes partie du code =
// ==================================================

/**** ajouter la creation d'un objet dans ce fichier ****/
// par exemple un bouton stadard non surchargé
void CPPCodeItem::AddObject(BMessage *object)
{
	// ajouter l'item a la liste
	_objectList.AddItem(object);
}

/**** ajouter l'information sur le fichier ****/
void CPPCodeItem::AddInfo()
{
	// c'est la qu'on ajoute que c'est fait avec MeTOS
	// au fichier *.cpp : la date etc...
	BString		info;
	BString		buffer;
	BString		infoLine;
	BString		dateLine;
	BString		borderLine;
	int32		maxSize;
	time_t		today_t;
	tm			today_tm;

	// les infos sur la classe
	_properties.FindString(VOBJECT_NAME,&buffer);
	infoLine << " " << buffer << " based on ";
	_properties.FindString(VOBJECT_CLASS,&buffer);
	infoLine << buffer << " class ";
	
	// date de generation
	today_t = time(0);
	today_tm = *gmtime(&today_t);
	dateLine = " Created : ";
	dateLine << today_tm.tm_mday << "/" << today_tm.tm_mon + 1 << "/" << today_tm.tm_year + 1900;
	dateLine << " - ";
	dateLine << today_tm.tm_hour << ":" << today_tm.tm_min << ":" << today_tm.tm_sec << " ";
	
	// trouver la plus grande taille
	maxSize = infoLine.Length();
	if(maxSize < dateLine.Length())
		maxSize = dateLine.Length();
	
	// limite haut et bas du rectangle
	// on doit enlever le /* et */ et le retour a la ligne
	borderLine.SetTo('*',maxSize);
	borderLine.Prepend("/*");
	borderLine << "*/\n";

	// haut
	info << borderLine;
	
	// info et date completé
	CompleteString(infoLine,' ',maxSize);
	info << "/*" << infoLine << "*/\n";
	CompleteString(dateLine,' ',maxSize);
	info << "/*" << dateLine << "*/\n";

	// generer par MeTOS
	buffer = " Code Generated by MeTOS ";
	CompleteString(buffer,' ',maxSize);
	info << "/*" << buffer << "*/\n";

	// bas
	info << borderLine;
	info << "\n";

	// ajouter au header et au source
	_headerCode << info;
	_sourceCode << info;
}

/**** ajouter un #include ****/
void CPPCodeItem::AddInclude()
{
	// on ajoute les includes
	// au source (fichiers *.cpp)
	BString		buffer;
	bool		beObject;
	
	// ajouter l'include de la classe de base
	_properties.FindString(VOBJECT_CLASS,&buffer);

	// si c'est un objet Be on doit enlever le B pour l'include
	if(_settings.FindBool(VOBJECT_BE_OBJECT,&beObject)!=B_OK)
		beObject = false;

	_headerCode << "#include ";
	if(beObject)
	{
		buffer.Remove(0,1);
		_headerCode << "<" << buffer << ".h>\n";
	}
	else
		_headerCode << "\"" << buffer << ".h\"\n";
	_headerCode << "\n";

	// entete de classe derivée
	_properties.FindString(VOBJECT_NAME,&buffer);

	_sourceCode << "#include \"" << buffer << ".h\"\n";
	_sourceCode << "\n";

	// on va se baser sur la liste des classes utilisées
	for(int32 index=0;index<_classList.CountItems();index++)
	{
		// recuperer la classe
		buffer = *(BString *)(_classList.ItemAt(index));
		
		// si c'est une Be on enleve le B
		// puis on ajoute
		if(buffer[0]=='#')
		{
			buffer.Remove(0,2);
			_sourceCode << "#include <" << buffer << ".h>\n";
		}
		else
			_sourceCode << "#include \"" << buffer << ".h\"\n";
	}
	_sourceCode << "\n";		
}

/**** gestion des noms de classe ****/
void CPPCodeItem::AddClass()
{
	// on ajoute dans le header
	// les noms de classe des objets qui seront creer
	// dans le source
	BString		*name = NULL;
	int32		nbName;
	
	// ajouter les instruction class Nom;
	nbName = _classList.CountItems();
	for(int32 index=0;index<nbName;index++)
	{
		name = (BString *)(_classList.ItemAt(index));
		if((*name)[0]=='#')
			name->Remove(0,1);
		_headerCode << "class " << *name << ";\n";
	}
	_headerCode << "\n";
}

/**** ajouter les fonctions membres ****/
void CPPCodeItem::AddFunctions()
{
	// ajout des fonctions selectionnees dans les parametres
	// ainsi que le constructeur
	// mais tout ca dans le header et en meme temps dans le source
	BMessage	parameters;
	BString		objectName;
	BString		className;
	BString		type;
	BString		buffer;
	int32		nbFunctions;
	BString		methodName;
	BString		arguments;
	bool		isAConstructor = false;
	bool		isADestructor = false;
	bool		firstTime = true;
	int32		index = 0;
	object_type	objectType;
		
	// entete de classe derivée
	_properties.FindString(VOBJECT_NAME,&objectName);

	_headerCode << "\t// publics functions\n";

	// dans le cas d'une application on ajoute la fonction main
	_properties.FindInt32(VOBJECT_TYPE,(int32 *)&objectType);
	if(objectType==APPLICATION_TYPE)
		CreatApplication();

	// recuperer les fonctions a deriver
	nbFunctions = 0;
	while(_functionList.FindString(FUNCTION_PARAMETER_NAME,nbFunctions,&methodName)==B_OK)
	{
		_functionList.FindString(FUNCTION_PARAMETER_TYPE,nbFunctions,&type);
		_functionList.FindString(FUNCTION_PARAMETER_ARGS,nbFunctions,&arguments);
		_functionList.FindMessage(FUNCTION_PARAMETERS,nbFunctions,&parameters);

		// verifier si c'est un constructeur ou un destructeur
		_properties.FindString(VOBJECT_CLASS,&className);
		isAConstructor = (methodName == className);
		isADestructor =  (methodName[0]=='~');
			
		_headerCode << "\t";
			
		// commentaire
		if(type.Length()>0)
		{
			_headerCode << "virtual\t" << type << "\t";
			_sourceCode << "/***** " << methodName << " ****/\n";
			_headerCode << methodName << "(" << arguments << ");\n";
			_sourceCode << type << objectName << "::" << methodName << "(";
		}
		else
		{
			if(isADestructor)
				_sourceCode << "/***** Destructor ****/\n";
			else
				_sourceCode << "/***** Constructor ****/\n";
			
			if(isADestructor)
				_headerCode << "~";
			
			_headerCode << objectName << "(" << arguments << ");\n";
				_sourceCode << type << objectName << "::";
			if(isADestructor)
				_sourceCode << "~";
			_sourceCode << objectName << "(";
		}

		// nom de la fonction avec ces parametres
		index = 0;
		while(parameters.FindString(FUNCTION_PARAMETER_TYPE,index,&buffer)==B_OK)
		{
			if(index!=0)
				_sourceCode << ",";	
			
			_sourceCode << buffer;
			if(buffer.FindFirst('*')==B_ERROR)
				_sourceCode << " ";
			parameters.FindString(FUNCTION_PARAMETER_NAME,index,&buffer);
			_sourceCode << buffer;

			// au suivant
			index++;
		}							
		_sourceCode << ")";	
			
		// c'est un constructeur
		if(isAConstructor)
		{			
			index = 0;
			_sourceCode << "\n";	
			_sourceCode << ": " << className << "(";
			while(parameters.FindString(FUNCTION_PARAMETER_NAME,index,&buffer)==B_OK)
			{
				if(index!=0)
					_sourceCode << ",";	

				_sourceCode << buffer;
					
				// au suivant
				index++;
			}				
			_sourceCode << ")";	
		}
		
		// ouvrir la fonction
		if(_indentStyle==B_CONTROL_ON)
			_sourceCode << "\n";
		_sourceCode << "{\n";
		if(isAConstructor && firstTime)
		{
			// ajouter le creation des controles
			AddAlocations();

			// Ajout du code externe
			AddExternalCode();
			
			// ajouter l'ajout
			AddChild();
			firstTime = false;
		}
		
		// fermer la fonction
		_sourceCode << "}\n";
		_sourceCode << "\n";

		// passer a la fonction suivante
		nbFunctions++;
	}
	_headerCode << "\n";
}

/**** creation des objets ****/
void CPPCodeItem::AddAlocations()
{
	BMessage	*object = NULL;
	BMessage	properties;
	BString		memberName;
	BString		buffer;
	int32		nbObject;
	bool		derived;
	
	// petit commentaire
	_sourceCode << "\t// Allocate object\n";
	
	// pour chaque objet
	// mais on parcour la liste a l'envers car on doit parfois
	// deja avoir un objet fils pour le parent comme par exemple
	// pour la BScrollView
	nbObject = _objectList.CountItems() -1;
	for(int32 index=nbObject;index>=0;index--)
	{
		// objet courant
		object = (BMessage *)(_objectList.ItemAt(index));
		object->FindMessage(VOBJECT_PROPERTIES,&properties);
		properties.FindBool(VOBJECT_DERIVED_CLASS,&derived);
		
		// recuperer le nom de la variable
		memberName = MemberName(object);		
		
		// on se base sur le type derivée ou pas !!!
		_sourceCode << "\t" << _prependStringMember << memberName << " = new ";
		properties.FindString(VOBJECT_CLASS,&buffer);
		if(derived)
			properties.FindString(VOBJECT_NAME,&buffer); 					
		_sourceCode << buffer << "(";
		
		// ajouter les valeurs de la ligne de parametre
		AddParametersValues(object);

		// fermer la parentheses
		_sourceCode << ");\n";
	}
}

/**** ajouter le code externe en plus ****/
void CPPCodeItem::AddExternalCode()
{
	BMessage	*object = NULL;
	BMessage	*father = NULL;
	BMessage	*child = NULL;
	BMessage	stringParameters;
	BString		memberName;
	BString		value;
	BString		buffer;
	char		*name;
	int32		count;
	uint32		dataType;
	int32		nbObject;
	int32		index;
	int32		indexObject;
	bool		isFirstTime;

	// pour chaque objet
	// mais on parcour la liste a l'envers car on doit parfois
	// deja avoir un objet fils pour le parent comme par exemple
	// pour la BScrollView
	isFirstTime = true;
	nbObject = _objectList.CountItems() -1;
	for(indexObject=nbObject;indexObject>=0;indexObject--)
	{

		// objet courant
		object = (BMessage *)(_objectList.ItemAt(indexObject));

		// recuperer les valeures des proprietes en chaines
		object->FindMessage(VOBJECT_VALUE_AS_STRING,&stringParameters);
		if(object->FindString(VOBJECT_EXTERNAL_CODE,&buffer)==B_OK)
		{
			// si c'est la premiere fois on va ecrire l'entete
			if(isFirstTime)
			{
				_sourceCode << "\n";
				_sourceCode << "\t// Additional Code\n";
				isFirstTime = false;
			}
		
			// recuperer le nom de la variable
			memberName = _prependStringMember;
			memberName << MemberName(object);		
			buffer.ReplaceAll(VOBJECT_MEMBER_NAME,memberName.String());

			// et le nom de la variable parent
			if(g_CPPCodeWin->Lock())
			{
				father = g_CPPCodeWin->Father(object);
				g_CPPCodeWin->Unlock();
			}
			
			if(father!=NULL)
			{
				memberName = _prependStringMember;
				memberName << MemberName(father);		
				buffer.ReplaceAll(VOBJECT_FATHER_OBJECT,memberName.String());
			}

			// et le nom de la variable enfant
			if(g_CPPCodeWin->Lock())
			{
				child = g_CPPCodeWin->Child(object);
				g_CPPCodeWin->Unlock();
			}
			
			if(child!=NULL)
			{
				memberName = _prependStringMember;
				memberName << MemberName(child);		
				buffer.ReplaceAll(VOBJECT_CHILD_OBJECT,memberName.String());
			}
	
			// on va remplacer dans le code les proprietes par
			// leurs valeurs reelles
			for(int32 i=0;stringParameters.GetInfo(B_STRING_TYPE,i,&name,&dataType,&count) == B_OK;i++)
			{
				// on peut avoir plusieurs valeurs du meme nom
				for(index=0;index<count;index++)
				{
					// rcomposer le nom present dans le code externe
					memberName = "#";
					if(index>0)
					{
						memberName << index;
						memberName << ":";
					}
					memberName << name;
					memberName << "#";
		
					// on va pour chaque nom et et nombre de nom remplacer dans le code externe
					// ca correspondance
					if(stringParameters.FindString(name,index,&value)==B_OK)
						buffer.ReplaceAll(memberName.String(),value.String());
				}
			}
			// ajouter une tabulation au debut
			// puis apres chaque nouvelle ligne donc apres des "\n"
			buffer.Prepend("\t");
			buffer.ReplaceAll("\n","\n\t");

			// ajouter le code
			_sourceCode << buffer;

			// un retour a la ligne en fin
			_sourceCode << "\n";
		}
	}
}

/**** ajouter les valeurs des parametres de creation ****/
void CPPCodeItem::AddParametersValues(BMessage *object)
{
	BMessage		stringParameters;
	BMessage		functionList;
	BMessage		properties;
	int32			nbFunctions;
	BString			className;
	BString			methodName;
	BString			type;
	
	BMessage		*child = NULL;
	BMessage		nameAsProperties;
	BMessage		parameters;
	int32			indexParameters;
	BString			nameParameters;
	BString			buffer;
	bool			haveConstructor;
	
	// voir pour toutes les fonctions
	nbFunctions = 0;
	haveConstructor = false;
	object->FindMessage(VOBJECT_FUNCTIONLIST,&functionList);
	object->FindMessage(VOBJECT_PROPERTIES,&properties);	
	while(functionList.FindString(FUNCTION_PARAMETER_NAME,nbFunctions,&methodName)==B_OK)
	{
		properties.FindString(VOBJECT_CLASS,&className);
		functionList.FindString(FUNCTION_PARAMETER_TYPE,nbFunctions,&type);
		if(haveConstructor = (type.Length()==0 && methodName==className))
			break;
			
		// sinon au suivant
		nbFunctions++;
	}
				
	if(!haveConstructor)
	{
		_sourceCode << "/* No Constructor ! */";
		return;
	}
		
	// construire la chaine de parametres
	object->FindMessage(VOBJECT_VALUE_AS_STRING,&stringParameters);
			
	// correspondance des parametres et des valeurs
	indexParameters = 0;
	functionList.FindMessage(FUNCTION_PARAMETERS,nbFunctions,&parameters);
	object->FindMessage(VOBJECT_NAME_AS_PROPERTIES,&nameAsProperties);
	while(parameters.FindString(FUNCTION_PARAMETER_NAME,indexParameters,&buffer)==B_OK)
	{
		// on doit trouver la correspondance du nom reel
		if(nameAsProperties.FindString(buffer.String(),&nameParameters)!=B_OK)
			buffer = "/* No Value ! */";

		// on doit aussi avoir sa valeur
		// mais on peux aussi trouver une valeur par defaut
		if(stringParameters.FindString(nameParameters.String(),&buffer)!=B_OK || buffer.Length()==0)
			if(parameters.FindString(FUNCTION_PARAMETER_VALUE,indexParameters,&buffer)!=B_OK || buffer.Length()==0)
				buffer = "/* No Value ! */";
				
		// on demande le pointer sur la premiere vue enfant
		if(nameParameters==VOBJECT_CHILD_OBJECT)
		{
			if(g_CPPCodeWin->Lock())
			{
				child = g_CPPCodeWin->Child(object);
				g_CPPCodeWin->Unlock();
				
				// recuperer le nom de variable de l'objet enfant
				buffer = MemberName(child);
				buffer.Prepend(_prependStringMember);
			}
		}

		// on demande le pointer sur le parent
		if(nameParameters==VOBJECT_FATHER_OBJECT)
		{
			// recuperer le nom de variable de l'objet enfant
			if(g_CPPCodeWin->Lock())
			{
				buffer = MemberName(g_CPPCodeWin->Father(object));
				g_CPPCodeWin->Unlock();
				buffer.Prepend(_prependStringMember);
			}
		}

		if(indexParameters!=0)
			_sourceCode << ",";	

		_sourceCode << buffer;				

		// au suivant
		indexParameters++;
	}				
}

/**** ajouter les instructions AddChild, Show etc ... ****/
void CPPCodeItem::AddChild()
{
	BMessage	properties;
	BMessage	*object = NULL;
	BMessage	*father = NULL;
	BString		className;
	BString		parentClassName;
	BString		buffer;
	int32		nbObject;
	object_type	type;
	object_type	typeFather;
	
	_sourceCode << "\n";
	_sourceCode << "\t// Adding object\n";

	// on va faire ca pour chaque objet
	nbObject = _objectList.CountItems();
	for(int32 index=0;index<nbObject;index++)
	{
		// objet courant
		object = (BMessage *)(_objectList.ItemAt(index));

		// recuperer le nom de la variable
		className =  _prependStringMember;
		className << MemberName(object);

		// et le nom de la variable parent
		if(g_CPPCodeWin->Lock())
		{
			father = g_CPPCodeWin->Father(object);
			g_CPPCodeWin->Unlock();
		}
		
		if(father!=NULL)
		{
			BMessage	propertiesFather;

			parentClassName = MemberName(father);
			father->FindMessage(VOBJECT_PROPERTIES,&propertiesFather);
			propertiesFather.FindInt32(VOBJECT_TYPE,(int32 *)&typeFather);
		}
		// si le parent est l'objet en cours on n'utilise pas sa
		// variable
		buffer.SetTo("");
		if(father!=&_baseObject)
			buffer << _prependStringMember << parentClassName << "->";

		object->FindMessage(VOBJECT_PROPERTIES,&properties);
		properties.FindInt32(VOBJECT_TYPE,(int32 *)&type);
		switch(type)
		{
		case UNDEFINED_TYPE:
			_sourceCode << "/* Object type not define ! */\n";
			break;
		case PROJECT_TYPE:
		case APPLICATION_TYPE:
			break;
		case WINDOW_TYPE:
				// pour les fenetre c'est un show
				_sourceCode << "\t" << className << "->Show();\n";
			break;
		case MENU_TYPE:
			// on ajoute pas le menu au BMenuField
			// c'est deja fait par le constructeur
			if(typeFather==MENUFIELD_TYPE)
				break;
		case MENUITEM_TYPE:
				_sourceCode << "\t" << buffer << "AddItem(" << className << ");\n";
			break;
		case BITMAP_TYPE:
		case SHELF_TYPE:
			break;
		default:
			{
				switch(typeFather)
				{
				case TABVIEW_TYPE:
					// pour les tabview on ajoute un onglet
					_sourceCode << "\t" << buffer << "AddTab(" << className << ");\n";
					break;
				case SCROLLVIEW_TYPE:
					// on ajoute pas d'autre vue a une ScrollView
					// c'est deja fait par le constructeur
					break;
				default:
					// sinon c'est un AddChild standard
					_sourceCode << "\t" << buffer << "AddChild(" << className << ");\n";
				}
			}
		}
	}
}

/**** ajouter les varaibles membres ****/
void CPPCodeItem::AddMembers()
{
	BMessage	*object = NULL;
	BMessage	properties;
	BString		buffer;
	int32		nbObject;
	bool		derived;
	
	_headerCode << "\t// protected members\n";

	// des variables membres
	nbObject = _objectList.CountItems();
	for(int32 index=0;index<nbObject;index++)
	{
		object = (BMessage *)(_objectList.ItemAt(index));
		object->FindMessage(VOBJECT_PROPERTIES,&properties);
		properties.FindBool(VOBJECT_DERIVED_CLASS,&derived);
		
		properties.FindString(VOBJECT_CLASS,&buffer); 
		if(derived)
			properties.FindString(VOBJECT_NAME,&buffer); 
		_headerCode << "\t" << buffer << "\t\t*" << _prependStringMember << MemberName(object) << ";\n";
	}
	_headerCode << "\n";
}

/**** recuperer le nom de variable d'un objet ****/
BString CPPCodeItem::MemberName(BMessage *object)
{
	BMessage	properties;
	BString		buffer;

	object->FindMessage(VOBJECT_PROPERTIES,&properties);
	properties.FindString(VOBJECT_NAME,&buffer);
	buffer.ToLower();
	
	return buffer;
}

// ====================
// = Autres fonctions =
// ====================

/**** completeter la chaine avec un caractere ****/
void CPPCodeItem::CompleteString(BString &string,char c,int32	size)
{
	BString		buffer;
	int32		stringSize;
	
	stringSize = string.Length();
	if(size-stringSize <= 0)
		return;
		
	buffer.SetTo(c,size-stringSize);
	string << buffer;
}

/**** rafraichir les preferences ****/
void CPPCodeItem::RefreshPreferences()
{
	VPreferenceFile		prefsMeTOS("MeTOS");
	VPreferenceFile		prefsGenerator("CPPGenerator",CODE_GENERATION_GROUP_PATH);
	
	// charger
	prefsMeTOS.Load();
	prefsGenerator.Load();

	_itemColor = prefsMeTOS.GetRGBColor((rgb_color){200,200,255,255},"list-color");	
	_prependStringMember = prefsGenerator.GetString("_","prepend-string-member");
	_indentStyle = prefsGenerator.GetInt32(B_CONTROL_ON,"indent-style");
	_writeMakefile = prefsGenerator.GetInt32(B_CONTROL_OFF,"write-makefile");
}
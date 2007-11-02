/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#include "CPPCodeWin.h"
#include "MetosMessageDefs.h"
#include "MetosGlobalDefs.h"
#include "CPPCodeListView.h"
#include "VPreferenceFile.h"
#include "VObject.h"
#include "CPPCodeItem.h"
#include "CTabView.h"
#include "CColumnView.h"
#include "CColumnItem.h"
#include "VLanguage.h"

#include <interface/Screen.h>
#include <interface/View.h>
#include <interface/Button.h>
#include <interface/ListView.h>
#include <interface/ScrollView.h>
#include <interface/TextView.h>
#include <storage/File.h>
#include <storage/Node.h>
#include <storage/NodeInfo.h>
#include <storage/Path.h>

// pointer globale
CPPCodeWin		*g_CPPCodeWin = NULL;

/**** Constructeur ****/
CPPCodeWin::CPPCodeWin(BRect frame,VLanguage *language)
: BWindow(frame,"Code Genarator", B_TITLED_WINDOW_LOOK, B_FLOATING_SUBSET_WINDOW_FEEL , B_NOT_ZOOMABLE | B_FRAME_EVENTS)
{	
	BRect			bounds = Bounds();
	BRect			listRect = bounds;
	BRect			textRect = bounds;
	BScrollView		*scrollView = NULL;
	
	// pointer globale
	g_CPPCodeWin = this;
	_exitGenerator = false;
	_language = language;
	
	// les onglets
	listRect = bounds;
	listRect.InsetBy(2,2);
	listRect.bottom -= 34;
	_tabView = new CTabView(listRect,"generation-tab-view",B_WIDTH_FROM_LABEL);

	listRect.bottom -= 26;
	listRect.right -= 19;

	// ajouter la liste des proprietes
	_codeList = new CPPCodeListView(listRect,"code-list-view");
	scrollView = new BScrollView(_language->GetTranslationFor("CPP_FILES_LIST"),_codeList,B_FOLLOW_ALL_SIDES,B_WILL_DRAW,false,true);
	_tabView->AddTab(scrollView);

	// ajouter les colonnes
	_columnView = new CColumnView("column-files-view",NULL,false,true);
	_columnView->AddItem(new CColumnItem("object",_language->GetTranslationFor("CPP_OBJECT_NAME")),100);
	_columnView->AddItem(new CColumnItem("files",_language->GetTranslationFor("CPP_FILES")));
	scrollView->AddChild(_columnView);	
	
	textRect.right *= 3;	
	// vue du code du header
	_headerTextView = new BTextView(listRect,"header-text-view",textRect,new BFont(be_fixed_font),NULL,B_FOLLOW_ALL_SIDES,B_WILL_DRAW);
	scrollView = new BScrollView(_language->GetTranslationFor("CPP_HEADER_CODE"),_headerTextView,B_FOLLOW_ALL_SIDES,B_WILL_DRAW,true,true);
	_tabView->AddTab(scrollView);

	// vue du code du source
	_sourceTextView = new BTextView(listRect,"source-text-view",textRect,new BFont(be_fixed_font),NULL,B_FOLLOW_ALL_SIDES,B_WILL_DRAW);
	scrollView = new BScrollView(_language->GetTranslationFor("CPP_SOURCE_CODE"),_sourceTextView,B_FOLLOW_ALL_SIDES,B_WILL_DRAW,true,true);
	_tabView->AddTab(scrollView);

	// initialiser la vue makefile
	_makefileTextView = NULL;

	// fixer la couleur de la vue
	// construire la vue principale
	_codeManagerView = new BView(bounds,"codeManager-view",B_FOLLOW_ALL_SIDES,B_WILL_DRAW);
	_codeManagerView->SetViewColor(VIEW_COLOR);	

	// bouton d'arret de generation
	_startGenerator = new BButton(BRect(0,0,50,25),"stop-generator",_language->GetTranslationFor("CPP_START"),new BMessage(M_GENERATE_CODE_MSG),B_FOLLOW_RIGHT | B_FOLLOW_BOTTOM);
	_startGenerator->MoveBy(bounds.right-60,bounds.bottom-30);

	// bouton de creation des fichiers
	_writeFiles = new BButton(BRect(0,0,75,25),"write-code-files",_language->GetTranslationFor("CPP_WRITE_FILES"),new BMessage(M_WRITE_CODE_FILES_MSG),B_FOLLOW_RIGHT | B_FOLLOW_BOTTOM);
	_writeFiles->MoveBy(bounds.right-140,bounds.bottom-30);
	_writeFiles->SetEnabled(false);

	// ajouter a la fenetre
	_codeManagerView->AddChild(_tabView);
	_codeManagerView->AddChild(_startGenerator);
	_codeManagerView->AddChild(_writeFiles);
	AddChild(_codeManagerView);

	// preferences
	RefreshPreferences();
}

/**** Destructeur ****/
CPPCodeWin::~CPPCodeWin()
{
	// ne pas oublier de virer l'onglet makefile
	// si il n'est pas deja virer
	if(_tabView->CountTabs()>3)
		delete _tabView->RemoveTab(3);
}

/**** netoyer la memoire ****/
void CPPCodeWin::ClearMemory()
{
	// vider la liste
	ClearObjectList();
}

/**** gestion des messages ****/
void CPPCodeWin::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
	// lancer la generation
	case M_GENERATE_CODE_MSG:
		GenerateCode();
		break;
	// creer les fichiers
	case M_WRITE_CODE_FILES_MSG:
		WriteFiles();
		break;
	// afficher le contenu du MCodeItem
	case M_SOURCE_DISPLAY_MSG:
	case M_HEADER_DISPLAY_MSG:
		DisplayCode(message);
		break;
	// rafraichir les preferences
	case M_PREFERENCE_MSG:
		RefreshPreferences();
	default:
		BWindow::MessageReceived(message);
	}
}

/**** Si on quitte on ferme les fenetre du projet ****/
bool CPPCodeWin::QuitRequested()
{
	// sauver la position dans les preferences
	g_MeTOSPrefs->SetRect(Frame(),"codeManager-rect");
	g_MeTOSPrefs->Save();

	// quitter
	if(_exitGenerator)
		return (true);
	
	// doit-on quitter ou simplement cacher la fenetre
	// si non on reduit la fenetre si ca n'est pas deja fait
	if(!IsHidden())
		Hide();

	return	(false);
}

/**** afficher la fenetre ****/
void CPPCodeWin::Show()
{
	BScreen		screen;
	BRect		screenFrame;	
	BRect		bounds;

	// centrer la fenetre
	bounds = Bounds();
	screenFrame = screen.Frame();
	screenFrame.right = (screenFrame.right - bounds.Width()) / 2;
	screenFrame.bottom = (screenFrame.bottom - bounds.Height()) / 2;
	MoveTo(screenFrame.right,screenFrame.bottom);

	// fonction parente
	BWindow::Show();
}

/**** afficher le code ****/
void CPPCodeWin::DisplayCode(BMessage *message)
{
	int32	selected;
	
	// on doit avoir un item de selectionné
	selected = _codeList->CurrentSelection();
	if(selected<0)
		return;
	
	// on recupere l'item
	CPPCodeItem	*current = NULL;
	
	current = (CPPCodeItem *)(_codeList->ItemAt(selected));
	if(current==NULL)
		return;
	
	// remplir les textview avec le code generé
	_headerTextView->SetText(current->HeaderCode());
	_sourceTextView->SetText(current->SourceCode());

	if(message->what==M_HEADER_DISPLAY_MSG)
		_tabView->Select(1);
	else
		_tabView->Select(2);	
}

// ====================
// = Liste des objets =
// ====================

/**** recuperer la liste des objets ****/
status_t CPPCodeWin::FillObjectList(BMessage *data)
{
	BMessage	objectArchive(VOBJECT_ARCHIVE_MSG);
	int32		nbObject;
	status_t	state = B_OK;
	
	// vider la liste si elle est remplie
	ClearObjectList();	

	// extraire du message les infos
	nbObject = 0;
	while(data->FindMessage(VOBJECT_ARCHIVE,nbObject,&objectArchive)==B_OK)
	{
		if(!_codeList->AddItem(new CPPCodeItem(objectArchive)))
			state &= B_ERROR;
			
		// passer au suivant
		nbObject++;
	}
	
	// on doit au moins avoir plus de 2 objets
	// le projet, l'application ou la fenetre !
	if(nbObject<2)
		state &= B_ERROR;
		
	// recuperer les dependances
	state &= data->FindMessage(DEPENDENCIES_DATA,&_dependencies);
		
	return state;
}

/**** vider la liste des item de code ****/
void CPPCodeWin::ClearObjectList()
{
	int32	nbItems;
	
	// liberer la memoire
	nbItems = _codeList->CountItems();
	for(int32 index=0;index<nbItems;index++)
		delete (CPPCodeItem *)(_codeList->RemoveItem((int32)0));
}

// ==========================
// = Generation du code C++ =
// ==========================

/**** Generation du code C++ ****/
status_t CPPCodeWin::GenerateCode()
{	
	BMessage		*project = NULL;
	BMessage		properties;
	BString			projectName;
	object_type		type;

	// rechercher les infos dans le projet de base
	project = ((CPPCodeItem *)(_codeList->ItemAt(0)))->Object();
	project->FindMessage(VOBJECT_PROPERTIES,&properties);
	if(properties.FindInt32(VOBJECT_TYPE,(int32 *)&type)!=B_OK)
		return B_ERROR;
		
	if(type!=PROJECT_TYPE)
		return B_ERROR;
	
	// on doit trouver le chemin de base
	if(properties.FindString("_path",&_basePath)!=B_OK)
		return B_ERROR;

	if(properties.FindString(VOBJECT_NAME,&projectName)!=B_OK)
		return B_ERROR;

	// virer du nom l'extension
	// puis definir le chemin pour les fichiers
	projectName.RemoveFirst(".metos");
	_basePath << "/";
	_basePath << projectName;

	// creer les items de code qui vont generer le source
	// commencer a 1 (apres le projet) et aller a unde plus pour generer le dernier
	// item derivé	
	_startGenerator->SetLabel(_language->GetTranslationFor("CPP_STOP"));
	_recursIndex = 2;
	// on commence au premier level pour eviter le projet
	GenerateObjectCode((CPPCodeItem *)(_codeList->ItemAt(1)));
	_startGenerator->SetLabel(_language->GetTranslationFor("CPP_START"));
	_writeFiles->SetEnabled(true);
	
	// generer le makefile
	GenerateMakefile();
	
	// rafraichir l'affichage
	_codeList->Invalidate();
	
	// tout c'est bien passé
	return B_OK;
}

/**** fonction de generation ****/
status_t CPPCodeWin::GenerateObjectCode(CPPCodeItem *father)
{
	// on doit avoir un parent
	if(father==NULL)
		return B_ERROR;

	// recuperer les infos sur l'objet
	CPPCodeItem		*current = NULL;
	BMessage		*object = NULL;
	BMessage		properties;
	status_t		state = B_OK;
	int32			level;
	int32			fatherLevel;
	bool			derivedClass;

	// vider les anciennes données
	father->ClearList();
	
	// mettre a jour le chemin destination des fichiers
	father->SetBasePath(_basePath.String());

	// recuperer les données du parent
	object = father->Object();
	object->FindMessage(VOBJECT_PROPERTIES,&properties);
	properties.FindInt32(VOBJECT_LEVEL,&fatherLevel);

	// continuer tant que l'ona des enfants
	while(_recursIndex<_codeList->CountItems())
	{		
		// l'element courant
		current = (CPPCodeItem *)(_codeList->ItemAt(_recursIndex));
		if(current==NULL)
			return B_OK;

		// recuperer les données de l'element courant
		object = current->Object();
		object->FindMessage(VOBJECT_PROPERTIES,&properties);
		properties.FindBool(VOBJECT_DERIVED_CLASS,&derivedClass);
		properties.FindInt32(VOBJECT_LEVEL,&level);
		
		// on sort si on a un level plus petit que le parent
		// ou egale sinon ca merde si 2 classe de suite sont derivées
		if(level<=fatherLevel)
			break;

		// au suivant
		_recursIndex++;
		
		father->AddObject(current->Object());
		if(derivedClass)			
			state &= GenerateObjectCode(current);
		
	}		
	
	// generer
	father->Generate();

	// on quitte normalement
	return state;
}

/**** creer les fichiers et ecrire le code dedans ****/
void CPPCodeWin::WriteFiles()
{
	VPreferenceFile		prefFile("CPPGenerator",CODE_GENERATION_GROUP_PATH);
	int32				nbItems;
	
	// faire ca pour chaque items
	nbItems = _codeList->CountItems();
	for(int32 index=0;index<nbItems;index++)
		((CPPCodeItem *)(_codeList->ItemAt(index)))->CreateFiles();

	// ecrire le makefile avec si c'est demandé
	prefFile.Load();
	if(prefFile.GetInt32(B_CONTROL_OFF,"write-makefile")==B_CONTROL_ON)
		WriteMakefile();
}

// ==========================
// = Generation du makefile =
// ==========================

// generer le source du makefile
void CPPCodeWin::GenerateMakefile()
{
	VPreferenceFile		prefFile("CPPGenerator",CODE_GENERATION_GROUP_PATH);

	// verifier si on veux un makefile
	prefFile.Load();
	if(prefFile.GetInt32(B_CONTROL_OFF,"write-makefile")==B_CONTROL_OFF)
		return;

	// ok on doit le generer
	BMessage	*project = NULL;
	BMessage	valueAsString;
	int32		nbItems;
	BString		buffer;
	BString		equal;

	// rechercher les infos dans le projet de base
	project = ((CPPCodeItem *)(_codeList->ItemAt(0)))->Object();
	if(project->FindMessage(VOBJECT_VALUE_AS_STRING,&valueAsString)!=B_OK)
		return;

	// type de egale
	equal = "= : ";
	if(prefFile.GetInt32(B_CONTROL_ON,"use-equal-in-makefile")==B_CONTROL_ON)
		equal = "= ";

	// nombre d'items dans la liste
	nbItems = _codeList->CountItems();
	
	// ecrire le makefile
	_makefile =  "##    MeTOS Generated Makefile v1.0    ##\n";
	_makefile << "## based on BeOS Generic Makefile v2.2 ##\n";
	_makefile << "\n";	
	_makefile << "# specify the name of the binary\n";
	
	// on ajoute ./../ pour que ce soit generer dans le repertoire
	// parent de x.86
	valueAsString.FindString("_binary_name",&buffer);
	buffer.RemoveAll("\"");
	_makefile << "NAME" << equal << "./../" << buffer << "\n";	

	_makefile << "\n";	
	_makefile << "# specify the type of binary\n";
	_makefile << "#	APP:	Application\n";
	_makefile << "#	SHARED:	Shared library or add-on\n";
	_makefile << "#	STATIC:	Static library archive\n";
	_makefile << "#	DRIVER: Kernel Driver\n";

	// type de binaire
	valueAsString.FindString("_type_binary",&buffer);
	_makefile << "TYPE" << equal << buffer;

	_makefile << "\n\n";
	_makefile << "#	specify the source files to use\n";
	_makefile << "#	full paths or paths relative to the makefile can be included\n";
	_makefile << "# 	all files, regardless of directory, will have their object\n";
	_makefile << "#	files created in the common object directory.\n";
	_makefile << "#	Note that this means this makefile will not work correctly\n";
	_makefile << "#	if two source files with the same name (source.c or source.cpp)\n";
	_makefile << "#	are included from different directories.  Also note that spaces\n";
	_makefile << "#	in folder names do not work well with this makefile.\n";
	_makefile << "SRCS" << equal;
	
	// se baser sur les items de la liste pour savoir quoi ajouter
	// comme fichiers sources
	nbItems = _codeList->CountItems();
	for(int32 index=0;index<nbItems;index++)
		if(((CPPCodeItem *)(_codeList->ItemAt(index)))->Herited())
			_makefile << " ./" << ((CPPCodeItem *)(_codeList->ItemAt(index)))->FileName() << ".cpp";

	_makefile << "\n\n";
	_makefile << "#	specify the resource files to use\n";
	_makefile << "#	full path or a relative path to the resource file can be used.\n";

	// fichier resource
	valueAsString.FindString("_resource",&buffer);
	buffer.RemoveAll("\"");
	_makefile << "RSRCS" << equal << buffer << "\n";	

	_makefile << "\n\n";
	_makefile << "#	specify additional libraries to link against\n";
	_makefile << "#	there are two acceptable forms of library specifications\n";
	_makefile << "#	-	if your library follows the naming pattern of:\n";
	_makefile << "#		libXXX.so or libXXX.a you can simply specify XXX\n";
	_makefile << "#		library: libbe.so entry: be\n";
	_makefile << "#		\n";
	_makefile << "#	- 	if your library does not follow the standard library\n";
	_makefile << "#		naming scheme you need to specify the path to the library\n";
	_makefile << "#		and it's name\n";
	_makefile << "#		library: my_lib.a entry: my_lib.a or path/my_lib.a\n";

	// ajouter les librairies
	// on ajoute de toute facon la lib "libbe.so"
	_makefile << "LIBS" << equal << "be ";
	GetLibFromDependencies(_makefile,false);

	_makefile << "\n\n";
	_makefile << "#	specify additional paths to directories following the standard\n";
	_makefile << "#	libXXX.so or libXXX.a naming scheme.  You can specify full paths\n";
	_makefile << "#	or paths relative to the makefile.  The paths included may not\n";
	_makefile << "#	be recursive, so include all of the paths where libraries can\n";
	_makefile << "#	be found.  Directories where source files are found are\n";
	_makefile << "#	automatically included.\n";
	_makefile << "LIBPATHS" << equal;

	// ajouter les chemin des librairies
	GetLibFromDependencies(_makefile,true);

	_makefile << "\n\n";
	_makefile << "#	additional paths to look for system headers\n";
	_makefile << "#	thes use the form: #include <header>\n";
	_makefile << "#	source file directories are NOT auto-included here\n";
	_makefile << "SYSTEM_INCLUDE_PATHS" << equal << "\n";
	_makefile << "\n";
	_makefile << "#	additional paths to look for local headers\n";
	_makefile << "#	thes use the form: #include \"header\"\n";
	_makefile << "#	source file directories are automatically included\n";
	_makefile << "LOCAL_INCLUDE_PATHS" << equal << "\n";
	_makefile << "\n";
	_makefile << "#	specify the level of optimization that you desire\n";
	_makefile << "#	NONE, SOME, FULL\n";
	_makefile << "OPTIMIZE" << equal << "";
	_makefile << "\n\n";
	_makefile << "#	specify any preprocessor symbols to be defined.  The symbols will not\n";
	_makefile << "#	have their values set automatically; you must supply the value (if any)\n";
	_makefile << "#	to use.  For example, setting DEFINES to \"DEBUG=1\" will cause the\n";
	_makefile << "#	compiler option \"-DDEBUG=1\" to be used.  Setting DEFINES to \"DEBUG\"\n";
	_makefile << "#	would pass \"-DDEBUG\" on the compiler's command line.\n";

	// defines
	valueAsString.FindString("_defines",&buffer);
	buffer.RemoveAll("\"");
	_makefile << "DEFINES" << equal << buffer;

	_makefile << "\n\n";
	_makefile << "#	specify special warning levels\n";
	_makefile << "#	if unspecified default warnings will be used\n";
	_makefile << "#	NONE = supress all warnings\n";
	_makefile << "#	ALL = enable all warnings\n";

	// erreurs de compil et de link
	valueAsString.FindString("_warnings",&buffer);
	if(strcmp(buffer.String(),"true")==0)
		_makefile << "WARNINGS" << equal << "ALL";
	else
		_makefile << "WARNINGS" << equal << "NONE";

	_makefile << "\n\n";
	_makefile << "#	specify whether image symbols will be created\n";
	_makefile << "#	so that stack crawls in the debugger are meaningful\n";
	_makefile << "#	if TRUE symbols will be created\n";

	// erreurs de compil et de link
	valueAsString.FindString("_symbols",&buffer);
	buffer.ToUpper();
	_makefile << "SYMBOLS" << equal << buffer;

	_makefile << "\n\n";
	_makefile << "#	specify debug settings\n";
	_makefile << "#	if TRUE will allow application to be run from a source-level\n";
	_makefile << "#	debugger.  Note that this will disable all optimzation.\n";

	// debugger
	valueAsString.FindString("_debugger",&buffer);
	buffer.ToUpper();
	_makefile << "DEBUGGER" << equal << buffer;

	_makefile << "\n\n";
	_makefile << "#	specify additional compiler flags for all files\n";

	// flags de compilation
	valueAsString.FindString("_compiler_flags",&buffer);
	buffer.RemoveAll("\"");
	_makefile << "COMPILER_FLAGS" << equal << buffer;

	_makefile << "\n\n";
	_makefile << "#	specify additional linker flags\n";

	// flags d'edition des liens
	valueAsString.FindString("_linker_flags",&buffer);
	buffer.RemoveAll("\"");
	_makefile << "LINKER_FLAGS" << equal << buffer;

	_makefile << "\n\n";
	_makefile << "#	specify the version of this particular item\n";
	_makefile << "#	(for example, -app 3 4 0 d 0 -short 340 -long \"340 \"`echo -n -e '\302\251'`\"1999 GNU GPL\") \n";
	_makefile << "#	This may also be specified in a resource.\n";

	// version de l'application
	valueAsString.FindString("_app_version",&buffer);
	buffer.RemoveAll("\"");
	_makefile << "APP_VERSION" << equal << buffer;

	_makefile << "\n\n";
	_makefile << "#	(for TYPE == DRIVER only) Specify desired location of driver in the /dev\n";
	_makefile << "#	hierarchy. Used by the driverinstall rule. E.g., DRIVER_PATH = video/usb will\n";
	_makefile << "#	instruct the driverinstall rule to place a symlink to your driver's binary in\n";
	_makefile << "#	~/add-ons/kernel/drivers/dev/video/usb, so that your driver will appear at\n";
	_makefile << "#	/dev/video/usb when loaded. Default is \"misc\".\n";
	_makefile << "DRIVER_PATH" << equal << "\n";
	_makefile << "\n";
	_makefile << "## include the makefile-engine\n";
	_makefile << "include $(BUILDHOME)/etc/makefile-engine\n";
	
	// assigner a la vue
	// verifier l'existance de la vue
	if(_makefileTextView!=NULL)
		_makefileTextView->SetText(_makefile.String());
}

/**** ecrire le makefile ****/
void CPPCodeWin::WriteMakefile()
{
	BFile		file;
	BString		fileName;

	// ecrire le fichier
	fileName = _basePath << "/makefile";

	// on ne va pas verifier que le chemin
	// la generation de fichier la fait
	file.SetTo(fileName.String(),B_CREATE_FILE|B_ERASE_FILE|B_READ_WRITE);
	if(file.InitCheck()!=B_OK)
		return;

	// ecrire le tout, mais avant on va verifier si la vue existe
	// dans ce cas on va reprendre le texte a l'interieur
	if(_makefileTextView!=NULL && _makefileTextView->TextLength()>0)
		_makefile.SetTo(_makefileTextView->Text());
	
	// ecrire dans le fichier
	file.Write(_makefile.String(),_makefile.Length());
}

// =============================
// = Recherches sur les objets =
// =============================

/**** recuperer l'objet pere ****/
BMessage *CPPCodeWin::Father(BMessage *child)
{
	BMessage	*current = NULL;
	BMessage	properties;
	int32		nbItems;
	int32		fatherID;
	int32		currentID;
	
	if(child->FindMessage(VOBJECT_PROPERTIES,&properties)!=B_OK)
		return NULL;
	
	if(properties.FindInt32(VOBJECT_FATHER_ID,&fatherID)!=B_OK)
		return NULL;
	
	// chercher dans la liste
	nbItems = _codeList->CountItems();
	for(int32 index=0;index<nbItems;index++)
	{
		current = ((CPPCodeItem *)(_codeList->ItemAt(index)))->Object();
		if(current!=NULL)
		{
			if(current->FindMessage(VOBJECT_PROPERTIES,&properties)!=B_OK)
				return NULL;
	
			if(properties.FindInt32(VOBJECT_ID,&currentID)!=B_OK)
				return NULL;
			
			if(currentID == fatherID)
				return current;
		}
	}
	return NULL;
}

/**** recuperer l'objet fils ****/
BMessage *CPPCodeWin::Child(BMessage *father)
{
	BMessage	*current = NULL;
	BMessage	properties;
	int32		nbItems;
	int32		childID;
	int32		currentFatherID;
	
	if(father->FindMessage(VOBJECT_PROPERTIES,&properties)!=B_OK)
		return NULL;
	
	if(properties.FindInt32(VOBJECT_ID,&childID)!=B_OK)
		return NULL;
	
	// chercher dans la liste
	nbItems = _codeList->CountItems();
	for(int32 index=0;index<nbItems;index++)
	{
		current = ((CPPCodeItem *)(_codeList->ItemAt(index)))->Object();
		if(current!=NULL)
		{
			if(current->FindMessage(VOBJECT_PROPERTIES,&properties)!=B_OK)
				return NULL;
	
			if(properties.FindInt32(VOBJECT_FATHER_ID,&currentFatherID)!=B_OK)
				return NULL;
			
			if(currentFatherID == childID)
				return current;
		}
	}
	return NULL;
}

/**** rafraichir les preferences ****/
void CPPCodeWin::RefreshPreferences()
{
	VPreferenceFile		prefFile("CPPGenerator",CODE_GENERATION_GROUP_PATH);
	
	// ecrire le makefile avec si c'est demandé
	// et ajouter l'onglet si il n'est pas deja la
	// sinon on le vire
	prefFile.Load();
	if(_tabView->CountTabs()<=3 && prefFile.GetInt32(B_CONTROL_OFF,"write-makefile")==B_CONTROL_ON)
	{
		BRect			listRect = Bounds();
		BRect			textRect = listRect;
		BScrollView		*scrollView = NULL;

		// les onglets
		listRect.InsetBy(2,2);
		listRect.bottom -= 34;
		textRect.right *= 3;

		// vue pour le makefile si il est selectionner dans les options
		if(Lock())
		{
			_makefileTextView = new BTextView(listRect,"makefile-text-view",textRect,new BFont(be_fixed_font),NULL,B_FOLLOW_ALL_SIDES,B_WILL_DRAW);
			scrollView = new BScrollView(_language->GetTranslationFor("CPP_MAKEFILE"),_makefileTextView,B_FOLLOW_ALL_SIDES,B_WILL_DRAW,true,true);
			_tabView->AddTab(scrollView);
			_tabView->Invalidate();
			Unlock();
		}
	}
	else
	{
		// verifier si la tab est la
		if(_tabView->CountTabs()>3 && prefFile.GetInt32(B_CONTROL_OFF,"write-makefile")==B_CONTROL_OFF)
		{
			// effacer puis reinitialiser le compteur
			delete _tabView->RemoveTab(3);
			_makefileTextView = NULL;
		}
	}
}

// ===========================
// = Gestion des dependances =
// ===========================

/***** recuperer l'ensemble des librairies ****/
void CPPCodeWin::GetLibFromDependencies(BString &makefile,bool pathOnly)
{
	const char	*path = NULL;
	char		typeMime[256];
	BNodeInfo	nodeInfo;
	BNode		node;
	BPath		pathFile;
	BPath		pathParent;
	int32		index = 0;
	BString		buffer;

	// on va rechercher dans la liste des dependances
	// et conparer le type MIME avec celui demandé
	while(_dependencies.FindString(DEPENDENCIES_PATH,index,&path)==B_OK)
	{
		node.SetTo(path);
		if(node.InitCheck()==B_OK)
		{
			nodeInfo.SetTo(&node);
			nodeInfo.GetType(typeMime);
			if(strcmp(typeMime,"application/x-vnd.Be-elfexecutable")==0)
			{
				pathFile.SetTo(path);
				if(pathOnly)
				{
					// dans le cas du chemin uniquement le fait de
					// recuperer le parent, retire le fichier
					pathFile.GetParent(&pathParent);
					makefile << " " << pathParent.Path();
				}
				else
				{
					// dans le cas du fichier il faut enlever le "lib" en debut
					// du nomm si il y en a un
					buffer.SetTo(pathFile.Leaf());
					if(buffer.IFindFirst("lib")==0)
						buffer.RemoveFirst("lib");
					buffer.RemoveLast(".so");
					makefile << " " << buffer;				
				}
			}
		}
		
		// passer au suivant
		index++;
	}
}


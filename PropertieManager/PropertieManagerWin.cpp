/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
/**************************************************/
/* Fenetre qui affiche les proprietes d'un objets */
/**************************************************/
#include "MetosApp.h"
#include "PropertieManagerWin.h"
#include "PropertieManagerListView.h"
#include "FunctionManagerListView.h"
#include "DependencieManagerListView.h"
#include "DependencieManagerItem.h"
#include "CodeManagerTextView.h"
#include "CodeManagerListView.h"
#include "CodeManagerItem.h"
#include "PropertieManagerInfoView.h"
#include "PropertieManagerItem.h"
#include "CTabView.h"
#include "ObjectManagerWin.h"
#include "MetosSplashScreen.h"
#include "GUIManager.h"
#include "VPropertie.h"
#include "MetosMessageDefs.h"
#include "MetosGlobalDefs.h"
#include "CColumnView.h"
#include "CColumnItem.h"
#include "VObject.h"
#include "FunctionItem.h"
#include "DependencieManagerItem.h"
#include "VPreferenceFile.h"
#include "VPreference.h"
#include "FunctionManagerItem.h"
#include "CSLibrary.h"
#include "MetosLanguagesDefs.h"
#include "VLanguage.h"

#include <app/Roster.h>
#include <storage/Path.h>
#include <storage/NodeInfo.h>
#include <storage/Directory.h>
#include <storage/FindDirectory.h>
#include <interface/ListView.h>
#include <storage/File.h>
#include <interface/ScrollView.h>
#include <app/Messenger.h>
#include <interface/Menu.h>
#include <interface/MenuItem.h>

/*** constructeur de la fenetre des proprietes ****/
PropertieManagerWin::PropertieManagerWin(BRect frame)
: BWindow(frame,g_Languages->GetTranslationFor(L_PROPERTIES_TITLE), B_DOCUMENT_WINDOW_LOOK, B_FLOATING_SUBSET_WINDOW_FEEL, B_WILL_ACCEPT_FIRST_CLICK | B_NOT_ZOOMABLE)
{
	BScrollView			*scrollView = NULL;
	BView				*supportView = NULL;
	BRect				bounds = Bounds();
	BRect				listRect;
	BRect				textRect;
	BRect				codeRect;

	// pas d'objet courant
	_selectedObject = (uint32)NULL;
	_defaultGeneratorID.SetTo("");
	_generatorID.SetTo("");
	_settings = NULL;
	_infos = NULL;
	
	// creer le menu des generateurs
	_generatorMenu = new BMenu(g_Languages->GetTranslationFor(L_PROPERTIES_GENERATOR));
	_generatorMenu->SetRadioMode(true);

	listRect = bounds;
	listRect.InsetBy(2,2);
	listRect.bottom -= 14;

	// les onglets
	_tabView = new CTabView(listRect,"properties-tab-view",B_WIDTH_FROM_LABEL);

	listRect.bottom -= 26;
	listRect.right -= 19;
	
	// ajouter la liste des proprietes
	_propertiesList = new PropertieManagerListView(listRect,"navigation-properties-list",this);
	scrollView = new BScrollView(g_Languages->GetTranslationFor(L_PROPERTIES_PARAMETERS),_propertiesList,B_FOLLOW_ALL_SIDES,B_WILL_DRAW,false,true);
	_tabView->AddTab(scrollView);

	// ajouter les colonnes
	_columnView[0] = new CColumnView("column-properties-view",g_MetosApp->GetCursor("HeightResize"),false,true);
	_columnView[0]->AddItem(new CColumnItem("properties",g_Languages->GetTranslationFor(L_PROPERTIES_PROPERTIES)),g_MeTOSPrefs->GetInt32(168,"propertie-manager-column-list"));
	_columnView[0]->AddItem(new CColumnItem("values",g_Languages->GetTranslationFor(L_PROPERTIES_VALUES)));
	scrollView->AddChild(_columnView[0]);	

	// ajouter la liste des fonctions
	_functionsList = new FunctionManagerListView(listRect,"navigation-functions-list",this);
	scrollView = new BScrollView(g_Languages->GetTranslationFor(L_PROPERTIES_FUNCTIONS),_functionsList,B_FOLLOW_ALL_SIDES,B_WILL_DRAW,true,true);
	scrollView->SetViewColor(VIEW_COLOR);
	_tabView->AddTab(scrollView);

	// ajouter la vue du code externe et la liste des codes disponibles
	// on va utiliser une vue support pour ajouter les 2
	supportView = new BView(listRect,g_Languages->GetTranslationFor(L_PROPERTIES_EXTERNAL_CODE),B_FOLLOW_ALL_SIDES,B_WILL_DRAW);
	supportView->SetViewColor(VIEW_COLOR);
	codeRect = listRect;
	codeRect.right -= 20;
	codeRect.bottom /= 2;
	codeRect.bottom -= 20;
	textRect = codeRect;
	textRect.right *= 3;
	_codeTextView = new CodeManagerTextView(codeRect,"external-code-management",textRect,this);
	scrollView = new BScrollView("Additional Code Text",_codeTextView,B_FOLLOW_LEFT_RIGHT,B_WILL_DRAW,true,true);
	scrollView->SetViewColor(VIEW_COLOR);
	supportView->AddChild(scrollView);
	codeRect = listRect;
	codeRect.right -= 20;
	codeRect.top = listRect.bottom / 2;
	codeRect.bottom -= 6;
	_codeListView = new CodeManagerListView(codeRect,"external-list-code-management",this);
	scrollView = new BScrollView("Additional Code List",_codeListView,B_FOLLOW_ALL_SIDES,B_WILL_DRAW,false,true);
	scrollView->SetViewColor(VIEW_COLOR);
	supportView->AddChild(scrollView);
	_tabView->AddTab(supportView);
	
	// ajouter les colonnes
	_columnView[1] = new CColumnView("column-external-code-view",g_MetosApp->GetCursor("HeightResize"),false,true);
	_columnView[1]->AddItem(new CColumnItem("default",g_Languages->GetTranslationFor(L_PROPERTIES_DEFAULT)),g_MeTOSPrefs->GetInt32(63,"external-code-column-list"));
	_columnView[1]->AddItem(new CColumnItem("code",g_Languages->GetTranslationFor(L_PROPERTIES_CODE)));
	scrollView->AddChild(_columnView[1]);	


	// ajouter la liste des lib
	_dependencieList = new DependencieManagerListView(listRect,"navigation-dependencies-list",this);
	scrollView = new BScrollView(g_Languages->GetTranslationFor(L_PROPERTIES_DEPENDENCIES),_dependencieList,B_FOLLOW_ALL_SIDES,B_WILL_DRAW,true,true);
	scrollView->SetViewColor(VIEW_COLOR);
	_tabView->AddTab(scrollView);	

	// ajouter les colonnes
	_columnView[2] = new CColumnView("column-dependencies-view",g_MetosApp->GetCursor("HeightResize"),false,true);
	_columnView[2]->AddItem(new CColumnItem("path",g_Languages->GetTranslationFor(L_PROPERTIES_PATH)),g_MeTOSPrefs->GetInt32(168,"dependencies-manager-column-list"));
	_columnView[2]->AddItem(new CColumnItem("type",g_Languages->GetTranslationFor(L_PROPERTIES_TYPE)));
	scrollView->AddChild(_columnView[2]);	
	
	// ajouter la vue des infos
	_infoView = new PropertieManagerInfoView(bounds,g_Languages->GetTranslationFor(L_PROPERTIES_INFORMATIONS),this);
	_tabView->AddTab(_infoView);

	// construire la vue principale
	_propertiesManagerView = new BView(bounds,"Properties-Manager-view",B_FOLLOW_ALL_SIDES, B_WILL_DRAW | B_FRAME_EVENTS);
	_propertiesManagerView->SetViewColor(VIEW_COLOR);

	// ajouter les onglets
	_propertiesManagerView->AddChild(_tabView);
	AddChild(_propertiesManagerView);

	// verifier la lib CSSysteme
	RefreshSystemLib();

	// demander la liste des generateurs a la fenetre
	be_app_messenger.SendMessage(M_GENERATOR_NAME_LIST_MSG,this);
}

/*** destructeur de la fenetre MeTOS Application ****/
PropertieManagerWin::~PropertieManagerWin()
{
	int32	nbItem;

	// sauver les preferences
	SavePreferences();

	// effacer le menu des generateurs
	_codeTextView->RemoveGeneratorMenu();
	_dependencieList->RemoveGeneratorMenu();
	delete _generatorMenu;
	
	// vider la liste des infos
	_infoView->SetCurrentObject((uint32)NULL);
	
	// vider la liste des fonctions
	_functionsList->MakeEmpty();

	// vider la liste des proprietes si on selectionne pas d'objet
	_propertiesList->MakeEmpty();
	
	// les listes des colonnes
	for(int32 indexColumn=0;indexColumn<NB_COLOUMN_LIST_VIEW;indexColumn++)
	{
		nbItem = _columnView[indexColumn]->CountItems() - 1;
		for(int32 index = nbItem;index>=0;index--)
			delete _columnView[indexColumn]->RemoveItem(index);
	}
	// liberer la liste des fonctions
	nbItem = _functionItemList.CountItems() - 1;
	for(int32 index = nbItem;index>=0;index--)
		delete (FunctionManagerItem *)(_functionItemList.RemoveItem(index));

	// liberer la liste des proprietes
	nbItem = _itemsCache.CountItems() - 1;
	for(int32 index = nbItem;index>=0;index--)
		delete (VPropertie *)(_itemsCache.RemoveItem(index));

	// decharger les addons
	UnloadAddOns();
}

/**** gestion des messages ****/
void PropertieManagerWin::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
	// renvoyer le mouvement de la roulette de la souris a l'onglet active
	case B_MOUSE_WHEEL_CHANGED:
		{
			BView	*selectedView = NULL;
			
			selectedView = (_tabView->TabAt(_tabView->Selection()))->View();
			if(dynamic_cast<BScrollView *>(selectedView))
				BMessenger(selectedView->ChildAt(0)).SendMessage(message);
		}
		break;
	// afficher les proprietes
	case M_DISPLAY_PROPERTIES_MSG:
		DisplayProperties(message);
		break;
	// rafraichir des proprietes
	case M_OBJECT_CHANGE_MSG:
		UpdatePropertiesDisplay(message);
		break;
	// modifier des proprietes
	case M_PROPERTIES_CHANGE_MSG:
		if(UpdateProperties(*message)==B_OK)
			g_ObjectManager.SendMessage(message);
		break;
	// effacer les objets selectionnés
	case M_DELETE_OBJECT_MSG:
		RemoveAll();
		break;
	// creer un item de fonction
	case M_CREATE_FUNCTION_MSG:
		AddFunctionItem(message);
		break;
	// creer un item de dependance
	case M_CREATE_DEPENDENCY_MSG:
		AddDependencyItem(message);
		break;
	// ajouter un item de code externe
	case M_ADD_EXTERNAL_CODE_MSG:
		AddExternalCode();
		break;
	// selection d'un code externe de notre liste de code
	case M_SELECT_EXTERNAL_CODE_MSG:
		SelectExternalCode();
		break;
	// demande si la classe est derivé
	case M_CHECK_CLASS_DERIVED_MSG:
		CheckDerived(message);
		message->SendReply(M_REPLY_MSG);
		break;
	// charger le fichier de code externe d'une classe donnée
	case M_LOAD_EXTERNAL_CODE_MSG:
		_codeListView->LoadExternalCodeFile(message);
		break;
	// preferences
	case M_PREFERENCE_MSG:
		RefreshPreferences();
		_propertiesList->Invalidate();
		break;
	// on clique dans la liste des fonctions
	case M_REFRESH_FUNCTION_LIST_MSG:
		// nettoyer la liste des fonctions
		_infoView->RefreshValues();
		break;
	// selection d'un generateur
	case M_SELECT_GENERATOR_MSG:
		SelectGenerator(message);
		break;
	// liste des generateurs
	case M_GENERATOR_NAME_LIST_MSG:
		CreateGeneratorList(message);
		break;
	default:
		BWindow::MessageReceived(message);	
	}
}

/**** Si on quitte on ferme les fenetre du projet ****/
bool PropertieManagerWin::QuitRequested()
{
	// quitter
	if(g_DoQuit)
		return (true);
	
	// doit-on quitter ou simplement reduire la fenetre
	// si non on reduit la fenetre si ca n'est pas deja fait
	if(!IsHidden())
		Hide();
	
	return	(false);
}

// ======================
// = Gestion du clavier =
// ======================

void PropertieManagerWin::DispatchMessage(BMessage *message, BHandler *handler)
{
	// si c'est des touches clavier on va verifier si elles nous interessent
	switch(message->what)
	{
	// clavier
	case B_KEY_DOWN:
	case B_KEY_UP:
		{
			int32	raw_char = 0;
			
			// recuperer le caractere
			if(message->FindInt32("raw_char",&raw_char)==B_OK)
			{
				// tenir compte des touches Tab et Esc
				switch(raw_char)
				{
				case B_ESCAPE:
				case B_TAB:
					ManageKey(message);
					return;
				}
			}
		}
	}

	// par defaut on fait rien
	BWindow::DispatchMessage(message,handler);
}

/**** gestion des messages clavier ****/
void PropertieManagerWin::ManageKey(BMessage *message)
{
	// on ne traite que le keyup
	if(message->what!=B_KEY_UP)
		return;
	
	// on va appeler le traitement des touches en fonction de la vue active
	switch(_tabView->Selection())
	{
	// liste des proprietes
	case 0:
		_propertiesList->ManageKeyUp(message);
		break;
	default:
		; // rien
	}	
}

// ======================
// = Gestion des Addons =
// ======================

/**** charger l'ensemble des AddOns VPropertie ****/
void PropertieManagerWin::LoadAddOns()
{
	void					(*instantiate_VProperties)(BList *,entry_ref *);
	BList					list;
	image_id				addonId;
   	status_t 				err = B_NO_ERROR;
	app_info 				info; 
    BPath 					path;
	BString					message;	
	int32					nbFiles;
	BMessage				splashInfo(M_PROGRESSSTART_MSG);
	BMessage				splashProgress(M_PROGRESSINCREASE_MSG);
	BEntry					entry;
	entry_ref				ref;
	
	// initialiser le compteur du splash-screen
	nbFiles = g_MetosApp->CountAddOnFile(ADDON_PROPERTIES_FOLDER);
	splashInfo.AddInt32(M_TOTALPROGRESS,nbFiles);
	splashInfo.AddString(M_TEXTDISPLAY,"Load Properties AddOns...");
	BMessenger(g_MetosApp->_splashScreen).SendMessage(&splashInfo);

   	// trouver le chemin des addons
    path = g_MetosApp->GetAddonsPath();
	path.Append(ADDON_PROPERTIES_FOLDER);
	BDirectory dir(path.Path());

   	//load all effects
	while( err == B_NO_ERROR )
	{
		err = dir.GetNextEntry((BEntry*)&entry,false);			
		if( entry.InitCheck() != B_NO_ERROR )
			break;
		
		if( entry.GetPath(&path) == B_NO_ERROR )
		{
			addonId = load_add_on(path.Path());
			if(addonId>= 0)
			{
				// on regarde si on arrive a trouver la metode de creation de l'objet
				if(get_image_symbol(addonId,"instantiate_VProperties", B_SYMBOL_TYPE_TEXT,(void **)&instantiate_VProperties) == B_OK)
				{
					// on va creer un objet et le garder de coté
					// on va ensuite l'ajouter puis l'enlever en fonction du besoin
					// et l'effacer definitivement quand on quitte l'application
					entry.GetRef(&ref);
					(*instantiate_VProperties)(&list,&ref);
					if(list.CountItems()>0)
						_itemsCache.AddList(&list);
						
					// vider la liste
					list.MakeEmpty();
					
					// ajouter un item qui contient l'addonID
					_addonList.AddItem(new PropertieManagerItem(addonId,&entry));

					// informer le splash-screen
					BMessenger(g_MetosApp->_splashScreen).SendMessage(&splashProgress);

					// informer que tout c'est bien passé
					message = "Properties : ";	
					message << path.Path() << " Loaded";
					g_MetosApp->SendDebbug(message.String(),VDEBUG_DEBUG);					
				}
				else
					unload_add_on(addonId);
			}
		}
	}
	// informer que tout c'est bien passé
	g_MetosApp->SendDebbug("Properties Addons Loading finish",VDEBUG_INFO);
}

/**** decharger les AddOns ****/
void PropertieManagerWin::UnloadAddOns()
{
	// liberer la memoire prise par les items
	PropertieManagerItem	*propertiesItem = NULL;
	int32					nbItem = 0;
	image_id				image;

	// liberer la liste des addons
	nbItem = _addonList.CountItems() - 1;
	for(int32 item = nbItem;item>=0;item--)
	{
		propertiesItem = (PropertieManagerItem *)(_addonList.RemoveItem(item));
		image = propertiesItem->AddonId();				
		delete propertiesItem;
		if(unload_add_on(image)!=B_OK)
			printf("Error unloading Propertie Addon\n");
	}
}

/**** fonction qui choisi en fonction du type les items a ajouter a la liste ****/
void PropertieManagerWin::DisplayProperties(BMessage *propertiesObject)
{
	// on va charcher dans les nom des valeurs dans le BMessage
	// et regarder si on a le meme dans notre liste interne (nom de l'addon)
	// si oui on l'ajoute alors a la liste
	DependencieManagerItem	*dependenciesItem = NULL;
	FunctionManagerItem		*functionsItem = NULL;
   	VPropertie				*currentItem = NULL; 
	uint32					selected = (uint32)NULL;
	uint32					oldSelected = (uint32)NULL;
	BMessage				*lockedProperties;
	BMessage				functions;
	BMessage				externalCode;
	BString					className;
	char					*name = NULL;
	char					type[5];
	BString					fileName;
	uint32					dataType; 
	int32					count; 
	BString					message;
	int32					nbitemintern;
	bool					findItem = false;
	int32					stateProperties = -1;
	status_t				state = B_OK;
	BString					indexLockedName;
	int32					nbIndex = 0;
	int16					indexLocked = 0;
	bool					findIndex = false;
	VObject					*object = NULL;

	// recuperer le pointer
	// le fait de repondre au message permet de proteger l'acces au pointer
	// on ne le garde pas, on recupere tout de suite les infos
	state &= propertiesObject->FindPointer(M_OBJECT_SELECTED,(void **)&object);
	
	// recuperer les infos
	// si le pointeur est valide
	if(state==B_OK && object!=NULL)
	{
		selected = object->Id();
		_properties = *(object->Properties());
		functions = *(object->FunctionList());

		// liste des fonctions
		_properties.FindString(VOBJECT_CLASS,&className);
		functionsItem = FunctionItemFromClass(className.String());
		
		// dependance
		dependenciesItem = DependenciesItemFromClass(className.String());
	}

	// verifier qu'on a tout trouvé
	if((state!=B_OK && object!=NULL) || functionsItem==NULL || selected==(uint32)NULL)
		RemoveAll();
	else
	{
		// proprietées bloquées et autres ...
		lockedProperties = functionsItem->LockedProperties();
		_settings = functionsItem->Settings();
		_infos = functionsItem->Infos();
	
		// verfifier que c'est pas le meme objet
		if(_selectedObject != selected)
		{		
			// vider les listes
			// retenir l'ancien objet selectionne
			oldSelected = _selectedObject;
			RemoveAll();

			// fixer l'ID
			_selectedObject = selected;
			// trouver les proprietes
			for(int32 i = 0;_properties.GetInfo(B_ANY_TYPE, i, &name, &dataType, &count) == B_OK;i++)
			{
				// recuperer le types (4 lettres)
				type[0]= (dataType & 0xFF000000) >> 24;
				type[1]= (dataType & 0x00FF0000) >> 16;
				type[2]= (dataType & 0x0000FF00) >> 8;
				type[3]= (dataType & 0x000000FF);
				type[4]='\0';

				// initialiser pour la recherche de proprietes bloquees
				indexLockedName = name;
				indexLockedName << VOBJECT_LOCKED_INDEX;

				// chercher dans la liste interne
				for(int32 indexValue = 0;indexValue<count;indexValue++)
				{
					// verifier que l'on peux afficher cette proprietes
					// on ne doit pas trouver alors son nom dans la liste des bloquées
					if(lockedProperties->FindInt32(name,&stateProperties)!=B_OK)
						stateProperties = VOBJECT_PROPERTIES_ENABLED;
					else
					{
						// on a trouvé une info sur le blockage de propriete
						// on va verifier si on specifie un index ou plusieur index
						// si on en trouve pas on laisse l'etat comme il est
						lockedProperties->GetInfo(indexLockedName.String(),&dataType,&nbIndex);
						if(nbIndex>0)
						{
							// chercher les indexs
							findIndex = false;
							for(int32 indexSearch=0;(!findIndex && lockedProperties->FindInt16(indexLockedName.String(),indexSearch,&indexLocked)==B_OK);indexSearch++)
								findIndex = (indexLocked==indexValue);
					
							// si on a le nom dans les bloquée mais pas avec l'index courant
							// on laisse la valeur d'origine
							if(!findIndex)
								stateProperties = VOBJECT_PROPERTIES_ENABLED;
						}
					}			

					// doit-on finalement afficher la propriete
					if(stateProperties == VOBJECT_PROPERTIES_ENABLED || stateProperties == VOBJECT_PROPERTIES_DISABLED)
					{
						// voir en debug ce qui a ete trouvé
						message = "Properties ";	
						message << name << " i= " << indexValue << " type= " << type << " find in the Properties " ;

						// parcourir le cache (la liste) des addons creer
						// pour essayer de trouver si une correspond
						nbitemintern = _itemsCache.CountItems();
						for(int32 j = 0;j<nbitemintern;j++)
						{
							// on recupere la VPropertie du cache
							// on va verifier son index qui doit correspondre a celui que l'on cherche
							// et elle doit gerer le nom ou le type recherché
							// ces infos sont dans le message de la VPropertie
							currentItem = (VPropertie *)(_itemsCache.ItemAt(j));

							// determiner si on peux prendre cet item
							findItem = (indexValue == currentItem->Index());
							findItem &= (strcmp(currentItem->AddonName(),name)==0);
							findItem |= (strcmp(currentItem->AddonName(),type)==0 && strcmp(currentItem->PropertiesName(),name)==0);
						
							// on doit avoir le meme nom et le meme index
							if(findItem)
							{
								_propertiesList->AddItem(currentItem);
								currentItem->SetProperties(&_properties);
						
								message << " Added to the list";
								break;
							}
						
							// mettre a zero le pointer sinon on risque de prendre
							// le dernier trouvé
							currentItem = NULL;
						}

						// si on a pas trouvé d'addon on va essayer d'en creer une
						// pour rendre possible l'edition de ca propriete
						if(!findItem)
						{
							currentItem = CreateProperties(name,type,indexValue);
							if(currentItem!=NULL)
							{
								// ajouter au proprietes affichées
								_propertiesList->AddItem(currentItem);
								currentItem->SetPropertiesName(name);
								currentItem->SetProperties(&_properties);
							}
							else
							{
								BString					message;
							
								// on a rien trouvé
								message = "No Properties Addon named : ";
								message << name;
								message << " Finded";
								g_MetosApp->SendDebbug(message.String(),VDEBUG_DEBUG);
							}	
						}
					
						// verifier si on doit enlever l'edition de l'item !
						if(currentItem!=NULL)
						currentItem->SetEnable(!(stateProperties & VOBJECT_PROPERTIES_DISABLED));
					
						// envoyer l'info
						g_MetosApp->SendDebbug(message.String(),VDEBUG_DEBUG);		
					}				
				}
			}	
			// demander d'affficher les fonctions
			// est selectionner l'objet
			_functionsList->DisplayMethodes(functionsItem,&functions);
			_infoView->SetCurrentObject(selected);

			// creation du menu de proprietes
			_properties.FindMessage(VOBJECT_EXTERNAL_CODE,&externalCode);
			_codeTextView->BuildPropertiesMenu(&_properties,lockedProperties);
			
			// affichage du code externe pour la generation
			_codeTextView->DisplayCode(&externalCode);
			_codeListView->DisplayCode(className.String());

			// les depeendances
			_dependencieList->DisplayDependencies(dependenciesItem);
		}
	}
	// renvoyer la reponse pour liberer le pointeur
	propertiesObject->SendReply(M_REPLY_MSG);
}

/**** Creer une propriete pour editer un "name" ou un "type" a un "index" donné ****/
VPropertie *PropertieManagerWin::CreateProperties(const char *name,const char *type,int32 index)
{
	VPropertie		*searchItem = NULL;
	VPropertie		*newPropertiesItem = NULL;
	entry_ref		ref;

	// ajouter a la liste interne
	if((searchItem = PropertiesFromName(name))==NULL && (searchItem = PropertiesFromName(type))==NULL)
		return NULL;

	// creer une nouvelle proprietes d'index different
	(searchItem->Entry())->GetRef(&ref);
	newPropertiesItem = searchItem->InstantiateVPropertie(&ref);
	if(newPropertiesItem!=NULL)
	{
		// mettre a jour l'addon proprietes
		// et l'ajouter dans le cache des addons
		if(Lock())
		{
			newPropertiesItem->SetIndex(index);
			_itemsCache.AddItem(newPropertiesItem);
			Unlock();
		}
		else
			g_MetosApp->SendDebbug("Error : Lock() in PropertieManagerWin::CreateProperties()",VDEBUG_ERROR);
		
		// on retourne la nouvelle proprietes
		return	newPropertiesItem;
	}

	// proprietes non ceree
	return NULL;
}

/**** recuperer litem en fonction du nom ****/
VPropertie *PropertieManagerWin::PropertiesFromName(const char *name)
{
	VPropertie		*current = NULL;
	int32			nbItems = 0;
	int32			index = 0;
	
	// on va rechercher dans le cache si on
	// trouve une propritete qui gere le nom passé en parametre
	nbItems = _itemsCache.CountItems();
	for(index = 0;index<nbItems;index++)
	{
		// on va chercher par l'intermediaire de la metode de la VPropertie
		current = (VPropertie *)(_itemsCache.ItemAt(index));
		if(current!=NULL && current->NameIsManaged(name))
			return current;
	}
	
	// pas trouvé	
	return NULL;
}

/**** enlever les items de la liste ****/
void PropertieManagerWin::RemoveAll()
{
	// vider la liste des proprietes si on selectionne pas d'objet
	// la liste des fonctions et l'information
	_propertiesList->UnSelectItem();
	_propertiesList->MakeEmpty();
	
	// effacer les fonctions de la liste
	_functionsList->MakeEmpty();	

	// vue info
	_infoView->SetCurrentObject((uint32)NULL);

	// code externe
	_codeTextView->BuildPropertiesMenu(NULL,NULL);
//	_codeTextView->SetExternalCode(NULL);

	// dependance
	_dependencieList->DisplayDependencies(NULL);

	// annuler l'ID
	_selectedObject = (uint32)NULL;
}

/**** mettre a jour une proprietes ****/
status_t PropertieManagerWin::UpdateProperties(BMessage message)
{
	const void		*buffer;
	char			*name;
	uint32			type; 
	int32			count;
	ssize_t			size;
	int32			propertiesIndex = 0;

	// trouver l'index de la proprietes
	message.FindInt32(M_PROPERTIES_INDEX,&propertiesIndex);

	// on va mettre a jour les proprietes en interne
	// ne garder que la proprietes a mettre a jour
	message.RemoveName(M_OBJECT_SELECTED);
	message.RemoveName(M_PROPERTIES_INDEX);
	message.RemoveName(M_NEED_REFRESH);
	message.RemoveName(M_NEED_INVALIDATE);
	message.RemoveName(M_NEED_UNDO_ACTION);

	// recuperer la valeur a mettre a jour
	if(message.GetInfo(B_ANY_TYPE, 0, &name, &type, &count)!=B_OK)
		return B_ERROR;

	// remplacer en interne
	message.FindData(name,type,&buffer,&size);
	_properties.ReplaceData(name,type,propertiesIndex,buffer,size);
	
	// ok ca a marché
	return B_OK;
}

/**** mettre l'affichage d'une proprietes a jour ****/
void PropertieManagerWin::UpdatePropertiesDisplay(BMessage *message)
{
	uint32		object = (uint32)NULL;

	// trouver l'objet
	message->FindInt32(M_OBJECT_SELECTED,(int32 *)&object);

	if(object==(uint32)NULL)
		return;

	// verifier que c'est pour le bon objet
	if(object != _selectedObject)
		return;
			
	// sinon rafraichir la bonne proprietes
	// vider la liste
   	VPropertie		*currentItem = NULL; 
	const void		*buffer;
	BRect			itemFrame;
	int32			nbItem;
	BString			itemName;
	BString			searchName;
	char			*name;
	uint32			type; 
	int32			count;
	ssize_t			size;
	int32			propertiesIndex = 0;
	
	// trouver l'index de la proprietes
	message->FindInt32(M_PROPERTIES_INDEX,&propertiesIndex);

	// ne garder que la proprietes a mettre a jour
	message->RemoveName(M_OBJECT_SELECTED);
	message->RemoveName(M_PROPERTIES_INDEX);
	message->RemoveName(M_NEED_REFRESH);
	message->RemoveName(M_NEED_INVALIDATE);
	message->RemoveName(M_NEED_UNDO_ACTION);

	// recuperer la valeur a mettre a jour
	if(message->GetInfo(B_ANY_TYPE, 0, &name, &type, &count)!=B_OK)
		return;

	// trouver les proprietes
	if(Lock())
	{
		nbItem = _propertiesList->CountItems();
		searchName.SetTo(name);
		
		// chercher dans la liste
		for(int32 j = 0;j<nbItem;j++)
		{
			currentItem = (VPropertie *)(_propertiesList->ItemAt(j));
			itemName.SetTo(currentItem->PropertiesName());
			if(itemName == searchName && currentItem->Index()==propertiesIndex)
			{
				// remplacer en interne
				message->FindData(itemName.String(),type,&buffer,&size);
				_properties.ReplaceData(itemName.String(),type,propertiesIndex,buffer,size);
				
				// mettre a jour l'affichage
				currentItem->SetProperties(&_properties);
				itemFrame = _propertiesList->ItemFrame(j);
				itemFrame.InsetBy(1,1);
				itemFrame.left += currentItem->SizeProperties();
				_propertiesList->Invalidate(itemFrame);
				break;
			}
		}
		Unlock();
	}
	else
		g_MetosApp->SendDebbug("Error : Lock() in PropertieManagerWin::UpdatePropertiesDisplay()",VDEBUG_ERROR);
}

// ====================================
// = Convertion de valeurs en chaines =
// ====================================

/**** convertir les valeurs de proprietes d'un objet en String et les mettrent dans le Message ****/
void PropertieManagerWin::ConvertValuesToString(VObject *object,BMessage *data)
{
	BMessage		*properties = NULL;
	BMessage		*lockedProperties = NULL;
	char			*name;
	char			type[5];
	type_code		dataType; 
	int32			count;
	int32			stateProperties = -1;
	int32			nbitemintern;
	VPropertie		*currentItem = NULL;
	BString			addonName;
	BString			propertiesName;
	bool			findItem = false;
	BString			indexLockedName;
	int32			nbIndex = 0;
	int16			indexLocked = 0;
	bool			findIndex = false;
	
	// on va regareder toutes les proprietes de l'objet
	// sauf ceux non visibles c'est souvent des proprietés internes
	properties = object->Properties();
	lockedProperties = object->LockedProperties();

	// ne tenir compte que des proprietes d'indice 0
	for(int32 i = 0;properties->GetInfo(B_ANY_TYPE, i, &name, &dataType, &count) == B_OK;i++)
	{		
		// recuperer le types (4 lettres)
		type[0]= (dataType & 0xFF000000) >> 24;
		type[1]= (dataType & 0x00FF0000) >> 16;
		type[2]= (dataType & 0x0000FF00) >> 8;
		type[3]= (dataType & 0x000000FF);
		type[4]='\0';
	
		// initialiser pour la recherche de proprietes bloquees
		indexLockedName = name;
		indexLockedName << VOBJECT_LOCKED_INDEX;

		// chercher dans la liste interne
		for(int32 indexValue = 0;indexValue<count;indexValue++)
		{
			// verifier que l'on peux afficher cette proprietes
			if(lockedProperties->FindInt32(name,&stateProperties)!=B_OK)
				stateProperties = VOBJECT_PROPERTIES_ENABLED;
			else
			{
				// on a trouvé une info sur le blockage de propriete
				// on va verifier si on specifie un index
				// si on en trouve pas on laisse tout ou on bloque tout
				lockedProperties->GetInfo(indexLockedName.String(),&dataType,&nbIndex);
				if(nbIndex>0)
				{
					// chercher les indexs
					findIndex = false;
					for(int32 indexSearch=0;(!findIndex && lockedProperties->FindInt16(indexLockedName.String(),indexSearch,&indexLocked)==B_OK);indexSearch++)
						findIndex = (indexLocked==indexValue);
			
					// si on autorise on annule la precedente valeure pour cet index
					if(!findIndex)
						stateProperties = VOBJECT_PROPERTIES_ENABLED;
				}
			}			

			// doit-on finalement convertir la propriete
			// on va convertir sauf si elle a le flag notconvert
			if(!(stateProperties & VOBJECT_PROPERTIES_NOTCONVERTED))
			{
				// parcourir le cache (la liste) des addons creer
				// pour essayer de trouver si une correspond
				nbitemintern = _itemsCache.CountItems();
				for(int32 j = 0;j<nbitemintern;j++)
				{
					currentItem = (VPropertie *)(_itemsCache.ItemAt(j));
					addonName.SetTo(currentItem->AddonName());
					propertiesName.SetTo(currentItem->PropertiesName());
					findItem = (addonName == name || (addonName == type && propertiesName == name));

					// on doit avoir le meme nom et le meme index
					if(findItem)
						break;
			
					// remettre a NULL
					currentItem = NULL;
				}
			
				// creer la propriete
				if(!findItem)
					currentItem = CreateProperties(name,type,0);

				// normalement arrivé ici on doit avoir une propriete valide
				// on va alors convertir nos parametres
				if(currentItem!=NULL)
				{
					BString			value;

					// si on vient de creer la propriete le nom n'a pas ete defini
					if(!findItem)
						currentItem->SetPropertiesName(name);

					// attention si on a plusieurs index
					// c'est gerer par la boucle indexValue mais il faut en tenir compte
					currentItem->ValueAsString(properties,value,indexValue);
					if(value.Length()>0)
						data->AddString(name,value);
				}
			}
		}
	}
}

// ===========================================
// = Recreer le fichier CS de la lib systeme =
// ===========================================

/**** Rafraichir la lib systeme ****/
void PropertieManagerWin::RefreshSystemLib()
{
	VPreferenceFile		prefsGenerator("Generator",CODE_GENERATION_GROUP_PATH);
	BMessage			splashInfo(M_PROGRESSSTART_MSG);
	BString				optionPath;
	BPath				settingPath;
	CSLibrary			*systemLibrary = NULL;
	BFile				systemFile;
	bool				existFile;

	// initialiser le compteur du splash-screen
	splashInfo.AddInt32(M_TOTALPROGRESS,3);
	splashInfo.AddString(M_TEXTDISPLAY,"Find the CS system file...");
	BMessenger(g_MetosApp->_splashScreen).SendMessage(&splashInfo);

	// si le repertoire n'existe pas on va le creer
	find_directory(B_USER_SETTINGS_DIRECTORY,&settingPath);
	optionPath.SetTo(settingPath.Path());
	optionPath << "/" << METOS_SETTINGS_FOLDER;
	optionPath = prefsGenerator.GetString(optionPath,"CS-system-path");
	optionPath << "/SystemHeaders.cs";
	
	// informer le splash-screen
	BMessenger(g_MetosApp->_splashScreen).SendMessage(M_PROGRESSINCREASE_MSG);

	// verifier si le fichier existe
	// si oui on va juste l'ouvrir et verifier la lib, sinon on va l'ecraser
	systemFile.SetTo(optionPath.String(),B_READ_WRITE);
	existFile = (systemFile.InitCheck()==B_OK);
	if(!existFile)
		systemFile.SetTo(optionPath.String(),B_READ_WRITE|B_CREATE_FILE|B_ERASE_FILE);
	
	// le fichier doit etre valide ici
	if(systemFile.InitCheck()!=B_OK)
	{
		// informer que l'on a pas reussit a creer le fichier
		g_MetosApp->SendDebbug("Can't create the file SystemHeaders.cs here /boot/home/config/settings/MeTOS",VDEBUG_WARNING);	
		
		return;
	}

	// informer le splash-screen
	BMessenger(g_MetosApp->_splashScreen).SendMessage(M_PROGRESSINCREASE_MSG);

	// renseigner le MIME Type
	BNode		fileNode(optionPath.String());
	BNodeInfo	nodeInfo(&fileNode);

	if(nodeInfo.InitCheck()==B_OK)
		nodeInfo.SetType("application/x-vnd.CKJ-Daixiwen-CSFile");

	// verifier la validité du cs systeme
	systemLibrary = new CSLibrary(&systemFile);
	if(systemLibrary->InitCheck()!=B_OK || !existFile)
	{
		// vider
		systemLibrary->Empty();
		
		// informer que l'on va lancer le parsing
		g_MetosApp->SendDebbug("Parsing /boot/develop/headers/be to create SystemHeaders.cs file",VDEBUG_WARNING);	

		// parser a nouveau
		systemLibrary->Parse("/boot/develop/headers/be",true);
	}
	
	// sauvegarder le fichier
	delete systemLibrary;

	// informer le splash-screen
	BMessenger(g_MetosApp->_splashScreen).SendMessage(M_PROGRESSINCREASE_MSG);
}

// ========================================================
// = Gestion des FunctionItems et des FunctionManagerItem =
// ========================================================

/**** ajouter un item de fonction ****/
bool PropertieManagerWin::AddFunctionItem(BMessage *data)
{
	if(data->IsEmpty())
		return false;
	
	// ajouter le nouvel item
	FunctionManagerItem		*newItem = NULL;	
	
	newItem = new FunctionManagerItem(data);
	return _functionItemList.AddItem(newItem);
}

/**** recuperer un item qui gere le nom de classe en parametre ****/
FunctionManagerItem * PropertieManagerWin::FunctionItemFromClass(const char *name)
{
	FunctionManagerItem	*current = NULL;
	int32				nbFunctionItem;
	
	nbFunctionItem = _functionItemList.CountItems() - 1;
	for(int32 index=nbFunctionItem;index>=0;index--)
	{
		// chercher dans la liste le nom de la classe
		current = (FunctionManagerItem *)(_functionItemList.ItemAt(index));	
		if(current!=NULL && current->ClassName() == name)
			return current;
	}
	
	// on a pas trouvé
	return NULL;
}

/**** generer la liste des fonctions avec type retour, parametres, etc... ****/
status_t PropertieManagerWin::PreGenerateFunctions(BMessage *data,VObject *object)
{
	// la liste de fonction doit exister pour la sauver
	FunctionManagerItem		*functionList = NULL;
	BMessage				*properties = NULL;
	status_t				state = B_OK;
	BString					className;
	
	// trouver l'item affiché
	properties = object->Properties();
	if(properties->FindString(VOBJECT_CLASS,&className)!=B_OK)
		return B_ERROR;
	
	functionList = FunctionItemFromClass(className.String());
	if(functionList==NULL)
		return B_ERROR;
	
	// on va rechercher dans la liste de l'objet
	BMessage		*functions = NULL;
	FunctionItem	*current = NULL;
	int32			functionIndex;
	int32			index;
	
	// la liste des fonctions
	index = 0;
	functions = object->FunctionList();	
	while(functions->FindInt32(VOBJECT_FUNCTION_INDEX,index,&functionIndex)==B_OK)
	{
		current =  (FunctionItem *)(functionList->FunctionList()->ItemAt(functionIndex));
		if(current!=NULL && !(current->IsASuperItem()))
		{
			// ajouter le type de retour, le nom et les parametres de la fonction
			state &= data->AddString(FUNCTION_PARAMETER_TYPE,current->ReturnType());
			state &= data->AddString(FUNCTION_PARAMETER_NAME,current->MethodeName());
			state &= data->AddString(FUNCTION_PARAMETER_ARGS,current->Arguments());
			state &= data->AddMessage(FUNCTION_PARAMETERS,current->Parameters());
		}

		// paser au suivant
		index++;
	}

	return state;
}

/**** verifier la derivation ****/
void PropertieManagerWin::CheckDerived(BMessage *data)
{
	BMessage	properties;
	BMessage	functionList;
	status_t	state = B_OK;
	
	state &= data->FindMessage(VOBJECT_PROPERTIES,&properties);
	state &= data->FindMessage(VOBJECT_FUNCTIONLIST,&functionList);
	if(state!=B_OK)
		return;

	// les données sur l'objet
	FunctionManagerItem	*functionItem = NULL;
	BMessage			newValue(M_PROPERTIES_CHANGE_MSG);
	BString				className;
	uint32				objectID;
	int32				level;
	bool				derived = false;
	
	state &= properties.FindString(VOBJECT_CLASS,&className);
	state &= properties.FindInt32(VOBJECT_ID,(int32 *)&objectID);	
	state &= properties.FindInt32(VOBJECT_LEVEL,&level);	
 	functionItem = FunctionItemFromClass(className.String());
 	if(functionItem==NULL)
 		return;

	// chercher si on doit deriver
	if(level==1)
		derived = true;
	else
	{
		FunctionItem	*current = NULL;
		BList			*functions = NULL;
		BString			name;
		int32			index = 0;
		int32			selected;
		bool			hasConstructor;
		
		// on va chercher dans la liste
		hasConstructor = false;
		functions = functionItem->FunctionList();
		while(functionList.FindInt32(VOBJECT_FUNCTION_INDEX,index,&selected)==B_OK)
		{
			current = (FunctionItem *)(functions->ItemAt(selected));
			if(current!=NULL && !current->IsASuperItem())
			{
				name = current->MethodeName();
				if(name.FindFirst(className,0)!=B_ERROR)
					hasConstructor = true;

				derived = hasConstructor && (index>1);
				if(derived)
					break;
			}
			
			// passer au suivant
			index++;
		}
	}

	// modifier l'objet	
	newValue.AddInt32(M_OBJECT_SELECTED,objectID);
	newValue.AddBool(VOBJECT_DERIVED_CLASS,derived);
	newValue.AddBool(M_NEED_REFRESH,true);
	newValue.AddInt32(M_PROPERTIES_INDEX,0);

	// changer la valeur
	g_ObjectManager.SendMessage(&newValue);

	// modifier en interne si c'est l'objet affiché
	if(objectID==_selectedObject)
	{
		_properties.ReplaceBool(VOBJECT_DERIVED_CLASS,derived);
		g_PropertieManager.SendMessage(M_REFRESH_FUNCTION_LIST_MSG);	
	}	
}

// ===========================
// = Gestion des Dependances =
// ===========================

/**** acceder au dependance d'un objet reperé par son nom de classe ****/
DependencieManagerItem *PropertieManagerWin::DependenciesItemFromClass(const char *name)
{
	DependencieManagerItem	*current = NULL;
	int32					nbDependencyItem;
	
	nbDependencyItem = _dependencyList.CountItems() - 1;
	for(int32 index=nbDependencyItem;index>=0;index--)
	{
		// chercher dans la liste le nom de la classe
		current = (DependencieManagerItem *)(_dependencyList.ItemAt(index));	
		if(current!=NULL && current->ClassName() == name)
			return current;
	}

	// pas trouvé
	return NULL;
}

// ===============
// = Generateurs =
// ===============

/**** selection d'un generateur ****/
void PropertieManagerWin::SelectGenerator(BMessage *message)
{
	// verifier que le menu n'est pas vide
	if(_generatorMenu->CountItems()==0)
		return;
	
	BMenuItem		*marked = NULL;
	const char		*selectedID = NULL;
	int32			index;
	
	// recuperer l'item marqué
	marked = _generatorMenu->FindMarked();
	if(marked==NULL)
		return;
		
	// index de l'item selectionné
	index = _generatorMenu->IndexOf(marked);

	// recuperer par la fenetre parent l'ID correspondant
	if(_generatorList.FindString(M_GENERATOR_ID,index,&selectedID)!=B_OK)
		return;
	
	// mettre a jour le generateur selectionné
	_generatorID.SetTo(selectedID);

	// lancer la mise a jour des 2 vues
	// si on a un objet de selectionné
	if(_selectedObject!=(int32)NULL)
	{
		_codeTextView->SelectGenerator(selectedID);
		_dependencieList->SelectGenerator(selectedID);	
	}
}

/**** liste des generateurs ****/
void PropertieManagerWin::CreateGeneratorList(BMessage *data)
{
	// on doit avoir un message
	if(data==NULL)
		return;

	// mettre a jour en interne la liste des generateurs
	// on va virer l'element de reponse "_previous_"
	_generatorList = *data;
	_generatorList.RemoveData("_previous_");
	
	// mettre a jour dans la vue d'edition
	BuildGeneratorMenu();
}

/**** creer le menu des generateur ****/
void PropertieManagerWin::BuildGeneratorMenu()
{
	const char		*generatorName = NULL;
	
	// on va recuperer les infos du message
	for(int32 i=0;_generatorList.FindString(M_GENERATOR_NAME,i,&generatorName)==B_OK;i++)
		_generatorMenu->AddItem(new BMenuItem(generatorName,new BMessage(M_SELECT_GENERATOR_MSG)));
	
	// definir le generateur du menu par defaut
	SetDefaultGeneratorMenu();

	// definir le destinataire des menus
	_generatorMenu->SetTargetForItems(this);
}

/**** changer le menu en fonction du generateur selectionné ****/
void PropertieManagerWin::SetDefaultGeneratorMenu()
{
	const char		*generatorID = NULL;

	// recuperer le generateur par defaut des preferences
	// il n'y en a peux-etre pas, dans ce cas on prend le premier de la liste
	// sinon on le prend et le valide comme le courant
	_defaultGeneratorID = g_MeTOSPrefs->GetString(_defaultGeneratorID,"default-generator-id");
	_generatorID.SetTo(_defaultGeneratorID);
	if(_defaultGeneratorID.Length()==0)
	{
		// on va verifier si on a un generateur
		// on definit toujours avec le premier par defaut
		if(_generatorList.FindString(M_GENERATOR_ID,0,&_defaultGeneratorID)==B_OK)
			_generatorID.SetTo(_defaultGeneratorID);
	}
	
	// on va recuperer les infos du message
	for(int32 i=0;_generatorList.FindString(M_GENERATOR_ID,i,&generatorID)==B_OK;i++)
		if(strcmp(generatorID,_generatorID.String())==0)
			(_generatorMenu->ItemAt(i))->SetMarked(true);
}

/**** generer la liste des dependances pour un generateur donné ****/
status_t PropertieManagerWin::PreGenerateDependencies(BMessage *data,const char *ID)
{
	// on va parcourir la liste des items de dependance
	// et recuperer ces dernieres si elle correspondent
	// au generateur passé en parametre
	DependencieManagerItem	*current = NULL;
	status_t				state = B_OK;
	int32					nbItems = 0;
	
	nbItems = _dependencyList.CountItems();
	for(int32 i=0;i<nbItems;i++)
	{
		// generer la liste pour chaque item
		current = (DependencieManagerItem *)(_dependencyList.ItemAt(i));
		if(current!=NULL)
			state &= current->PreGenerateDependencies(data,ID);
	}
	
	return state;
}

// ===========================
// = Gestion du code externe =
// ===========================

/**** ajouter un item de code ****/
void PropertieManagerWin::AddExternalCode()
{
	// on va recuperer le code dans la Text View et l'ajouter au message
	// on va verifier qu'il y a du texte a ajouter
	if(_codeTextView->TextLength()==0)
		return;

	// appel de la metode d'ajout d'item de la liste des items de code
	_codeListView->AddExternalCodeItem(_codeTextView->Text());
}

/**** selectionner un code externe ****/
void PropertieManagerWin::SelectExternalCode()
{
	int32	selected;
	
	// afficher dans la vue de texte le code
	// qui vient d'etre selectionné
	selected = _codeListView->CurrentSelection();
	if(selected<0)
	{
		// si on est la c'est que l'on a pas d'item selectionné
		// on va alors effacer notre vue de texte
		_codeTextView->Delete(0,_codeTextView->TextLength());
	}
	else
	{	
		// ok on recupere le texte et on le place dans la vue
		// de texte
		CodeManagerItem		*item = NULL;
	
		// ok copier le texte dans la vue
		item = (CodeManagerItem *)(_codeListView->ItemAt(selected));
		if(item!=NULL)
			_codeTextView->SetText(item->Code());
	}
	
	// bloquer le looper pour ca
	if(Lock())
	{
		// rafraichir l'affichage de la vue
		_codeTextView->Invalidate();
		Unlock();
	}
	else
		g_MetosApp->SendDebbug("Error : Lock() in PropertieManagerWin::SelectExternalCode()",VDEBUG_ERROR);
}

// =============================================================
// = Gestion des DependencieItem et des DependencieManagerItem =
// =============================================================

/**** ajouter un item de dependance ****/
bool PropertieManagerWin::AddDependencyItem(BMessage *data)
{
	if(data->IsEmpty())
		return false;
	
	// ajouter le nouvel item
	DependencieManagerItem		*newItem = NULL;	

	newItem = new DependencieManagerItem(data);
	return _dependencyList.AddItem(newItem);
}

// ===========================
// = Gestion des preferences =
// ===========================

/**** rafraichir les preferences ****/
void PropertieManagerWin::RefreshPreferences()
{
	BString		generatorDefault;
	VPropertie	*item = NULL;
	int32		nbItems;
	
	// rafraichir tous les items de la liste
	nbItems = _itemsCache.CountItems();
	for(int32 index=0;index<nbItems;index++)
	{
		item = (VPropertie *)(_itemsCache.ItemAt(index));
		if(item!=NULL)
			item->RefreshPreferences();
	}
	
	// recuperer les information
	generatorDefault = g_MeTOSPrefs->GetString(_defaultGeneratorID,"default-generator-id");
	if(_defaultGeneratorID!=generatorDefault)
	{
		// definir dans le menu le code par defaut
		SetDefaultGeneratorMenu();
	
		// regenerer le menu des codes externes
		if(Lock())
		{
//			_codeTextView->BuildCodesMenu();
//			_codeTextView->DisplayCode();
			Unlock();
		}
		else
			g_MetosApp->SendDebbug("Error : Lock() in PropertieManagerWin::RefreshPreferences()",VDEBUG_ERROR);
	}	
}

/**** sauver les preferences ****/
void PropertieManagerWin::SavePreferences()
{
	// on va sauver la taille des colonnes de la liste
	BRect				*bounds = NULL;

	// charger les preferences
	g_MeTOSPrefs->Load();

	// frame de la fenetre
	g_MeTOSPrefs->SetRect(Frame(),"propertiesManager-rect");

	// vue colonnes
	bounds = _columnView[0]->RectFrom(0);
	g_MeTOSPrefs->SetInt32(bounds->IntegerWidth(),"propertie-manager-column-list");
	bounds = _columnView[1]->RectFrom(0);
	g_MeTOSPrefs->SetInt32(bounds->IntegerWidth(),"external-code-column-list");
	bounds = _columnView[2]->RectFrom(0);
	g_MeTOSPrefs->SetInt32(bounds->IntegerWidth(),"dependencies-manager-column-list");
	
	// sauver
	g_MeTOSPrefs->Save();
}

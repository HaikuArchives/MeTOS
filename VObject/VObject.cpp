/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#include "VObject.h"
#include "WindowSupportVObjectView.h"
#include "MetosMessageDefs.h"
#include "MetosGlobalDefs.h"
#include "VPreferenceFile.h"
#include "VPreference.h"
#include <app/Application.h>
#include <interface/Window.h>
#include <interface/Menu.h>
#include <support/List.h>
#include <interface/TabView.h>
#include <time.h>

/**** constructeur ****/
VObject::VObject()
{
	// initialiser les données
	_object = NULL;
	_father = NULL;
	_functionList = NULL;

	// code externe
	// ce message est ajouté aux proprietes car il peux dependre
	// d'un objet bien precis !
	BMessage	externalCode(VOBJECT_EXTERNAL_CODE_MSG);
	
	// creer le message des proprietes
	_properties = new BMessage(VOBJECT_PROPERTIES_MSG);
	_settings = new BMessage(VOBJECT_SETTINGS_MSG);
	_infos = new BMessage(VOBJECT_INFOS_MSG);
	_lockedProperties = new BMessage(VOBJECT_LOCKED_PROPERTIES_MSG);
	_nameAsProperties = new BMessage(VOBJECT_NAME_AS_PROPERTIES_MSG);
	_functionList = new BMessage(VOBJECT_FUNCTION_LIST_MSG);

	// puis renseigner les champs avec les valeurs d'initiation
	// ces donnees ne seront pas sauvées dans un projet
	_settings->AddBool(VOBJECT_BE_OBJECT,false);
	_settings->AddBool(VOBJECT_ONLY_ONE,false);
	_settings->AddString(VOBJECT_ADDON_SOURCE,"Not Defined");

	// proprietes qui peuvent changer et qui sont sauvegardées
	_properties->AddInt32(VOBJECT_TYPE,UNDEFINED_TYPE);
	_properties->AddString(VOBJECT_CLASS,"No Class");
	_properties->AddString(VOBJECT_NAME,"No Name");
	_properties->AddInt32(VOBJECT_ID,0);	
	_properties->AddInt32(VOBJECT_FATHER_ID,0);
	_properties->AddInt32(VOBJECT_LEVEL,0);
	_properties->AddBool(VOBJECT_DERIVED_CLASS,false);
	_properties->AddBool(VOBJECT_IS_LOCKED,false);
	_properties->AddMessage(VOBJECT_EXTERNAL_CODE,&externalCode);

	// proprietes speciale
	// cacher certaines proprietes pour la fenetre d'edition
	_lockedProperties->AddInt32(VOBJECT_TYPE,VOBJECT_PROPERTIES_NOTCONVERTED);
	_lockedProperties->AddInt32(VOBJECT_CLASS,VOBJECT_PROPERTIES_HIDDEN);
	_lockedProperties->AddInt32(VOBJECT_ID,VOBJECT_PROPERTIES_NOTCONVERTED);
	_lockedProperties->AddInt32(VOBJECT_FATHER_ID,VOBJECT_PROPERTIES_NOTCONVERTED);
	_lockedProperties->AddInt32(VOBJECT_LEVEL,VOBJECT_PROPERTIES_NOTCONVERTED);
	_lockedProperties->AddInt32(VOBJECT_DERIVED_CLASS,VOBJECT_PROPERTIES_HIDDEN);
	_lockedProperties->AddInt32(VOBJECT_IS_LOCKED,VOBJECT_PROPERTIES_NOTCONVERTED);
	_lockedProperties->AddInt32(VOBJECT_ADDON_SOURCE,VOBJECT_PROPERTIES_NOTCONVERTED);
	_lockedProperties->AddInt32(VOBJECT_EXTERNAL_CODE,VOBJECT_PROPERTIES_NOTCONVERTED);

	// generer l'identifiant
	GenerateId();
}

/**** destructeur ****/
VObject::~VObject()
{
	// annuler le pointer
	_object = NULL;
	
	// liberer les messages et la liste
	delete _properties;
	delete _infos;
	delete _settings;
	delete _lockedProperties;
	delete _nameAsProperties;
	delete _functionList;
}	

/**** apres appel du constructeur ****/
// ne peut etre surchargé ca sert par exemple a ajouter des truc apres la creation (ex : BWindow ajout supportView)
// sert aussi a renseigner la CSLib ou pas selon le type de l'objet
status_t VObject::AfterCreateObject()
{
	BString				className = ClassName();
	VPreferenceFile		functionsFile(className.String(),ADDON_OBJECTS_FOLDER);
	VPreferenceFile		externalCodeFile(className.String(),ADDON_EXTERNAL_CODES_FOLDER);
	object_type			type = Type();
	status_t			state = B_OK;

	// utiliser le fichier de parametre de fonctions
	functionsFile.Load();
	state &= SetFunctions(&functionsFile);

	// on va essayer de trouver un fichier parametre du code externe
	if(externalCodeFile.Load()==B_OK)
		LoadDefaultExternalCode(&externalCodeFile);

	// verifier tout de meme si l'objet existe
	// et si le type est defini
	if(_object==NULL || type==UNDEFINED_TYPE)
		return B_ERROR;
	
	// si on viens de creer une fenetre on doit ajouter la vue de support
	// pour gerer nos objets		
	switch(type)
	{
		case WINDOW_TYPE:
			{
				// creer une vue support et l'ajouter la vue de support
				WindowSupportVObjectView	*supportView = NULL;
				BWindow						*window = NULL;
				BRect						windowFrame;
				
				window = dynamic_cast<BWindow *>(_object);
				if(window==NULL)
					return B_ERROR;
					
				if(window->Lock())
				{
					windowFrame = window->Bounds();
					supportView = new WindowSupportVObjectView(windowFrame);				
					window->AddChild(supportView);
					window->Unlock();
				}
				else
					printf("Erreur : Lock() dans VObject::AfterCreateObject()\n");
			}
			break;
		case OTHER_TYPE:
			// autre type
			break;
		case UNDEFINED_TYPE:
			// erreur on doit avoir un type
			break;
		default:	
			// tous les autres
			// type autre rien pour l'instant
			break;
	}
	
	// ok tout c'est bien passé
	return state;
}

// ======================
// = bloquage du Looper =
// ======================

/**** bloquer le looper ****/
bool VObject::LooperLock()
{
	// l'objet doit etre attaché
	if(!IsAttached())
		return false;

	bool	state = false;
	
	// verifier q'une vue est attaché
	switch(Type())
	{
		case UNDEFINED_TYPE:
		case PROJECT_TYPE:
		case APPLICATION_TYPE:
		case MENU_TYPE:
		case POPUPMENU_TYPE:
		case MENUITEM_TYPE:
		case LISTITEM_TYPE:
		case SHELF_TYPE:
		case BITMAP_TYPE:
			break;
		case WINDOW_TYPE:
			if(((BWindow *)_object)->IsLocked())
					printf("Erreur : Lock() dans VObject::LooperLock()\n");
			else
				state = ((BWindow *)_object)->Lock();
			break;
		default:
			if((((BView *)_object)->Looper())->IsLocked())
				printf("Erreur : LockLooper() dans VObject::LooperLock()\n");
			else
				state = ((BView *)_object)->LockLooper();
	}
	
	return state;
}

/**** le debloquer si c'est possible ****/
bool VObject::LooperUnlock()
{
	// l'objet doit etre attaché
	if(!IsAttached())
		return false;

	bool	state = true;
	
	// verifier q'une vue est attaché
	switch(Type())
	{
		case UNDEFINED_TYPE:
		case PROJECT_TYPE:
		case APPLICATION_TYPE:
		case MENU_TYPE:
		case MENUITEM_TYPE:
		case POPUPMENU_TYPE:
		case LISTITEM_TYPE:
		case SHELF_TYPE:
		case BITMAP_TYPE:
			state = false;
			break;
		case WINDOW_TYPE:
			if(((BWindow *)_object)->IsLocked())
				((BWindow *)_object)->Unlock();			
			else
				printf("Erreur : Unlock() dans VObject::Unlock()\n");
			break;
		default:
			if((((BView *)_object)->Looper())->IsLocked())
				((BView *)_object)->UnlockLooper();
			else
				printf("Erreur : UnlockLooper() dans VObject::LooperUnlock()\n");
	}
	
	return state;
}

/**** generer un Identifiant unique ****/
void VObject::GenerateId()
{
	uint32	id = 0;
	
	id = (uint32)this + time(0);
	_properties->ReplaceInt32(VOBJECT_ID,id);	
}

/**** fixer l'id du parent dans la hierarchie ****/
void VObject::SetParentId(VObject *father)
{
	// verifier tout de meme si le parent est valide
	if(father==NULL)
		return;
		
	// ok tout va bien on va renseigner la donnee
	// et garder le pointer sur le parent pour la generation de code
	int32	parentId;
	
	_father = father;
	parentId = father->Id();
	_properties->ReplaceInt32(VOBJECT_FATHER_ID,parentId);
}

// =============
// = Affichage =
// =============

/**** rafraichir graphiquement un objet ****/
void VObject::Invalidate()
{	
	// on doit avoir un pointer
	// et on ne doit pas etre en train de quitter
	if(_object==NULL || g_DoQuit)
		return;

	BWindow		*window = NULL;
	BView		*view = NULL;
	bool		isHidden = false;

	// se baser sut le type pour faire ca
	switch(Type())
	{
		case UNDEFINED_TYPE:
			// erreur le type doit etre defini
			break;
		case WINDOW_TYPE:
			{				
				window = dynamic_cast<BWindow *>(_object);
				if(window==NULL)
					return;
		
				// verifier l'etat des fenetres
				if(window->Lock())
				{
					isHidden = window->IsHidden();
					window->Unlock();
				}
				else
					printf("Erreur : Lock() dans VObject::Invalide()\n");
				
				// est-elle caché
				if(isHidden)
					window->Show();
				else
					BMessenger(window).SendMessage(M_ACTIVE_WINDOW_MSG);
				
				// invalider la vue support
				if(window->Lock())
				{
					view = window->ChildAt(0);
					view->Invalidate();
					window->Unlock();
				}
				else
					printf("Erreur : Lock() dans VObject::Invalide()\n");
			}
			break;
		case PROJECT_TYPE:
		case APPLICATION_TYPE:
		case MENUITEM_TYPE:
		case POPUPMENU_TYPE:
		case LISTITEM_TYPE:
		case BITMAP_TYPE:
		case SHELF_TYPE:
		case OTHER_TYPE:
			// pas pour ceux-la
			break;
		default:	
			{
				BRect	bounds;
				
				// cas normal (une vue)
				view = dynamic_cast<BView *>(_object);
				if(view==NULL)
					return;
				
				window = view->Window();
				// la vue doit etre attachée
				if(window==NULL)
					return;
					
				if(view->LockLooper())
				{
					bounds = view->Bounds();
					bounds.InsetBy(-15,-15);
					view->ConvertToScreen(&bounds);
					window->ChildAt(0)->ConvertFromScreen(&bounds);
					window->ChildAt(0)->Invalidate(bounds);				
					view->UnlockLooper();
				}
				else
					printf("Erreur : LockLooper() dans VObject::Invalide()\n");
			}
	}
}

// =====================
// = Actions speciales =
// =====================

/**** creation menu speciaux ****/
BMenu *VObject::BuildActionMenu()
{
	// par defaut on ajoute rien
	return NULL;
}

/**** de meme pour les actions elles sont fonctions du menu special ****/
void VObject::ExecuteAction(BString &name,int32 index)
{
	// on ne fait rien
}

// =======================
// = Recuperer des Infos =
// =======================

/**** recuperer le type des proprietes ****/
object_type VObject::Type()
{
	object_type		type;

	if(_properties->FindInt32(VOBJECT_TYPE,(int32 *)&type)!=B_OK)
		return UNDEFINED_TYPE;
		
	return type;
}

/**** recuperer l'id des proprietes ****/
uint32 VObject::Id()
{
	uint32	id;

	if(_properties->FindInt32(VOBJECT_ID,(int32 *)&id)!=B_OK)
		return (uint32)NULL;
		
	return id;
}

/**** recuperer l'id du parent ****/
uint32 VObject::ParentId()
{
	uint32	parentId;

	if(_properties->FindInt32(VOBJECT_FATHER_ID,(int32 *)&parentId)!=B_OK)
		return (uint32)NULL;
		
	return parentId;
}

/**** recuperer l'id des proprietes ****/
int32 VObject::Level()
{
	int32	level;

	if(_properties->FindInt32(VOBJECT_LEVEL,&level)!=B_OK)
		return 0;
		
	return level;
}

/**** recuperer le nom de la classe ****/
const char *VObject::ClassName() const
{
	const char	*className = NULL;
	
	_properties->FindString(VOBJECT_CLASS,&className);
	return className;
}

/**** savoir si l'objet est disponible ****/
bool VObject::IsAttached()
{
	bool	state = true;
	
	// verifier q'une vue est attaché
	switch(Type())
	{
		case UNDEFINED_TYPE:
			state = false;
			break;
		case PROJECT_TYPE:
		case APPLICATION_TYPE:
		case WINDOW_TYPE:
		case MENU_TYPE:
		case LISTITEM_TYPE:
		case MENUITEM_TYPE:
		case POPUPMENU_TYPE:
		case SHELF_TYPE:
		case BITMAP_TYPE:
			state = true;
			break;
		default:
			if(((BView *)_object)->Window()==NULL)
				state = false;
	}
	
	return state;
}

// ==========================
// = Gestion des Proprietes =
// ==========================

/**** mettre a jour les donnees en interne ****/
void VObject::UpdateProperties(BMessage *data,int32 index)
{
	// rafraichir les proprietes VObjectProperties
	char			*name;
	const void		*buffer;
	type_code		type; 
	int32			count;
	ssize_t			size;  

	// trouver la propriete a l'inde donnée et essayer de
	// modifier la propriete interne correspondante a un index donnees
	data->GetInfo(B_ANY_TYPE, 0, &name, &type, &count);
	if(data->FindData(name,type,0,&buffer,&size)==B_OK)
		_properties->ReplaceData(name,type,index,buffer,size);
}

/**** mettre a jour les donnees en interne ****/
void VObject::ReplaceProperties(BMessage *data)
{
	// rafraichir les proprietes VObjectProperties
	char			*name;
	const void		*buffer;
	type_code		type; 
	int32			count;
	ssize_t			size; 
	int32			index;
	
	
	// trouver les proprietes
	for(int32 i = 0;data->GetInfo(B_ANY_TYPE, i, &name, &type, &count) == B_OK;i++)
		for(index = 0;index<count;index++)
			if(data->FindData(name,type,index,&buffer,&size)==B_OK)
				_properties->ReplaceData(name,type,index,buffer,size);
}

// ==========================
// = Gestion des VFunctions =
// ==========================

/**** recuperer les fonctions selectionnées ****/
status_t VObject::SetFunctions(BMessage *data)
{
	// on doit avoir un pointer valide
	// et pas un message vide
	if(data==NULL && !data->IsEmpty())
		return B_ERROR;

	*_functionList = *data;		
	_functionList->what = VOBJECT_FUNCTION_LIST_MSG;

	return B_OK;
}

// ===========================
// = Gestion du code externe =
// ===========================

/**** charger le code externe si il existe (ceux par defaut) ****/
void VObject::LoadDefaultExternalCode(BMessage *data)
{
	// le fichier ne doit pas etre vide
	if(data->IsEmpty())
		return ;

	BMessage	externalCode;
	BMessage	propertiesCode;
	BString		code;
	BString		indexName;
	char		*name;
	uint32		dataType; 
	int32		count; 
	int32		defaultCode;
	
	// recuperer le message porteur du code en interne
	_properties->FindMessage(VOBJECT_EXTERNAL_CODE,&propertiesCode);

	// recuperer la chaine par defaut pour chaque generateur, si elle existe
	for(int32 i = 0;data->GetInfo(B_MESSAGE_TYPE, i, &name, &dataType, &count) == B_OK;i++)
	{
		// recuperer le message correpondant au code defini par son nom
		// il doit de toute facon y en avoir que un par nom (dont on ne gere pas count)
		// et on va recuperer la bon code selon celui definit par defaut
		if(data->FindMessage(name,&externalCode)==B_OK && externalCode.FindInt32(VOBJECT_CODE_INDEX,&defaultCode)==B_OK)
		{
			// trouver la chaine si l'index est correcte
			// si on le trouve on remplace ou ajoute a l'existant
			if(defaultCode>-1 && externalCode.FindString(VOBJECT_EXTERNAL_CODE,defaultCode,&code)==B_OK)
			{	
				// le code
				if(propertiesCode.ReplaceString(name,code)!=B_OK)
					propertiesCode.AddString(name,code);
			
				// l'index d'edition pour ce code
				indexName = name;
				indexName << VOBJECT_CODE_INDEX;
				if(propertiesCode.ReplaceInt32(indexName.String(),defaultCode)!=B_OK)
					propertiesCode.AddInt32(indexName.String(),defaultCode);				
			}
		}
		
		// reinitialiser
		defaultCode = -1;
	}

	// mettre a jour en interne
	_properties->ReplaceMessage(VOBJECT_EXTERNAL_CODE,&propertiesCode);	
}

// =======================================
// = Bloquer/debloquer l'acces a l'objet =
// =======================================

/**** bloquer l'objet ****/
void VObject::LockObject()
{
	// on remplace simplement la valeur
	_properties->ReplaceBool(VOBJECT_IS_LOCKED,true);
}

/**** debloquer l'objet ****/
void VObject::UnlockObject()
{
	// on recupere la valeur
	_properties->ReplaceBool(VOBJECT_IS_LOCKED,false);
}

/**** tester si l'objet est bloqué ****/
bool VObject::IsLock()
{
	bool	locked = false;
	
	// on recupere la valeur
	if(_properties->FindBool(VOBJECT_IS_LOCKED,&locked)!=B_OK)
		return false;

	return locked;
}

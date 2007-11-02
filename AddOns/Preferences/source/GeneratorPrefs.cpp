// ===========================
// = Gestion des generateurs =
// ===========================
#include "GeneratorPrefs.h"
#include "MetosMessageDefs.h"
#include "MetosGlobalDefs.h"
#include "VPreferenceFile.h"
#include "VLanguage.h"

#include <Box.h>
#include <Application.h>
#include <ListView.h>
#include <ScrollView.h>

/**** constructeur ****/
GeneratorPrefs::GeneratorPrefs(BRect frame)
: VPreference(frame,"Generator",METOS_GROUP)
{
	BRect				bounds = Bounds();
	BScrollView			*supportScroll = NULL;
	BBox				*box = NULL;
	BString				buffer;

	// decallage a cause du titre
	bounds.top += VPREFERENCE_TITLE_HIGH;
	bounds.InsetBy(7,7);

	// le bloc Debug
	box = new BBox(bounds,"genertrator-settings");
	box->SetLabel(_language->GetTranslationFor("GENERATOR_SETTINGS"));	

	bounds.OffsetTo(0,0);
	bounds.InsetBy(12,16);
	bounds.top += 2;
	bounds.right -= B_V_SCROLL_BAR_WIDTH;
	// liste des generateurs
	_generatorListView = new BListView(bounds,"generator-list");
	_generatorListView->SetSelectionMessage(new BMessage(SELECT_METOS_GENERATOR));
	supportScroll = new BScrollView("support-scroll",_generatorListView,B_FOLLOW_LEFT | B_FOLLOW_TOP,0,false,true);

	box->AddChild(supportScroll);
	AddChild(box);	
}

/**** destructeur ****/
GeneratorPrefs::~GeneratorPrefs()
{
	// vider la liste
	ClearList();
}

/**** attachement a la fenetre ****/
void GeneratorPrefs::AttachedToWindow()
{
	// attachement standard
	VPreference::AttachedToWindow();
	
	// definir les destinataires
	_generatorListView->SetTarget(this);
	
	// demander la liste des generateurs
	// mais seulement si on ne la pas deja
	if(_generatorListView->CountItems()==0)
		be_app_messenger.SendMessage(M_GENERATOR_NAME_LIST_MSG,this);
}

/**** gestion des messages ****/
void GeneratorPrefs::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
	// liste des generateurs
	case M_GENERATOR_NAME_LIST_MSG:
		AddGeneratorsItem(message);
		break;
	// on selectionne un generateur
	case SELECT_METOS_GENERATOR:
		SelectDefaultGenerator();
		break;
	default:
		VPreference::MessageReceived(message);
	}
}

/**** selection du generateur ****/
void GeneratorPrefs::SelectDefaultGenerator()
{
	GeneratorListItem	*currentItem = NULL;
	int32				selected = -1;
	
	// la selection doit etre valide
	selected = _generatorListView->CurrentSelection();
	if(selected<0)
		return;
		
	// parcourir la liste et mettre a jour tous les items
	for(int32 i=0;i<_generatorListView->CountItems();i++)
	{
		currentItem = (GeneratorListItem *)(_generatorListView->ItemAt(i));
		if(currentItem!=NULL)
		{
			currentItem->SetDefault(false);
			if(selected==i)
				currentItem->SetDefault(true);
		}
	}
	
	// rafraichier l'affichage
	_generatorListView->Invalidate();
}

/**** recuperer la liste des generateurs ****/
void GeneratorPrefs::AddGeneratorsItem(BMessage *message)
{
	GeneratorListItem	*newItem = NULL;
	BString				generatorId;
	BString				generatorName;
	BString				defaultGenerator;

	// charger les preferences
	defaultGenerator = g_MeTOSPrefs->GetString("-","default-generator-id");
	
	// on va recuperer les infos du message
	for(int32 i=0;message->FindString(M_GENERATOR_NAME,i,&generatorName)==B_OK;i++)
	{
		if(message->FindString(M_GENERATOR_ID,i,&generatorId)==B_OK)
		{
			// creer le nouvel item
			newItem = new GeneratorListItem(generatorId.String(),generatorName.String());
			if(generatorId == defaultGenerator)
				newItem->SetDefault(true);
			
			// ajouter a la liste
			_generatorListView->AddItem(newItem);
		}
	}
}

/**** vider la liste des items ****/
void GeneratorPrefs::ClearList()
{
	int32	nbItems;
	
	// liberer le memoire prise par chaque items
	nbItems = _generatorListView->CountItems();
	for(int32 i=0;i<nbItems;i++)
		delete (GeneratorListItem *)(_generatorListView->RemoveItem((int32)0));
	
}

/**** sauver les preferences ****/
void GeneratorPrefs::SavePrefs()
{
	GeneratorListItem	*currentItem = NULL;

	// relire avant
	g_MeTOSPrefs->Load();
	
	// parcourir la liste et mettre a jour tous les items
	for(int32 i=0;i<_generatorListView->CountItems();i++)
	{
		currentItem = (GeneratorListItem *)(_generatorListView->ItemAt(i));
		if(currentItem!=NULL && currentItem->IsDefault())
		{
			// on definit celui par defaut et on quitte la boucle
			g_MeTOSPrefs->SetString(currentItem->ID(),"default-generator-id");
			break;
		}
	}

	// sauver
	g_MeTOSPrefs->Save();
}

// ====================
// = Item de la liste =
// ====================

/**** constructeur ****/
GeneratorListItem::GeneratorListItem(const char *id,const char *name)
{
	// initialiser
	_generatorID = id;
	_generatorName = name;
	_default = false;
}

/**** dessin ****/
void GeneratorListItem::DrawItem(BView *owner,BRect bounds,bool complete)
{ 
	// selection ou pas
	if(IsSelected())
		owner->SetHighColor(200,200,255,255);
	else
	{
		// si c'est celui par defaut
		if(_default)
			owner->SetHighColor(230,230,255,255);		
		else
			owner->SetHighColor(255,255,255,255);
	}	
	owner->SetLowColor(owner->HighColor());	
	owner->FillRect(bounds);

	// couleur du label
	owner->SetHighColor(BLACK_COLOR);

	if(_default)
		owner->DrawString("*",BPoint(bounds.left+2, bounds.bottom - 2));

	// nom du generateur
	_buffer.SetTo("Name : ");
	_buffer << _generatorName;

	// si c'est celui par defaut on ajoute une etoile
	owner->DrawString(_buffer.String(),BPoint(bounds.left+14, bounds.bottom - 2));

	// identifiant
	_buffer.SetTo("ID : ");
	_buffer << _generatorID;
	owner->DrawString(_buffer.String(),BPoint(bounds.left+191, bounds.bottom - 2));
}
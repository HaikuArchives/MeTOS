/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
/*********************************/
/* MeTOS code manager item class */
/*********************************/
#include "CodeManagerItem.h"
#include "MetosGlobalDefs.h"
#include "MetosMessageDefs.h"
#include "PropertieManagerWin.h"
#include "MetosApp.h"
#include "VObject.h"
#include "CColumnView.h"
#include <app/Message.h>
#include <interface/View.h>
#include <interface/Bitmap.h>
#include <support/String.h>
#include <stdlib.h>

/**** constructeur ****/
CodeManagerItem::CodeManagerItem()
: BListItem()
{
	// initialiser les données
	InitData();

	// initialiser les chaine vide
	SetCode("\0");
	SetGenerator("\0");
	SetDefault(false);
}

/**** constructeur ****/
CodeManagerItem::CodeManagerItem(const char *code,const char *generator,bool isDefault)
: BListItem()
{
	// initialiser les données
	InitData();
	
	// initialiser les chaine vide
	SetCode(code);
	SetGenerator(generator);
	SetDefault(isDefault);
}

/**** initialiser des données obligatoire ****/
void CodeManagerItem::InitData()
{
	// initialiser
	_code = NULL;
	_generator = NULL;
	_checkIcon = NULL;
	_isDefault = false;

	// l'icon de check
	// et la vue colonne
	if(g_MetosApp->LockLooper())
	{
		_columnView = g_MetosApp->_propertieManagerWin->ColumnView(1);
		_checkIcon = g_MetosApp->GetBitmap("CheckFunction");
		g_MetosApp->UnlockLooper();
	}
	else
		g_MetosApp->SendDebbug("Error : LockLooper() in CodeManagerItem::CodeManagerItem()",VDEBUG_ERROR);

	// preferences
	RefreshPreferences();
}

/**** destructeur ****/
CodeManagerItem::~CodeManagerItem()
{
	// liberer la memoire
	free(_code);
	free(_generator);
}

/**** Dessin des Items ****/
void CodeManagerItem::DrawItem(BView *owner,BRect bounds,bool complete)
{
	BRect		*position = NULL;

	// si le code est selectionné on change la couleur de fond
	if(IsSelected())
		owner->SetHighColor(_itemColor);
	else
		owner->SetHighColor(WHITE_COLOR);
	owner->SetLowColor(owner->HighColor());
	owner->FillRect(bounds);

	// le trait gris
	owner->SetHighColor(VIEW_COLOR);
	owner->StrokeLine(BPoint(0,bounds.bottom),BPoint(bounds.right,bounds.bottom));

	// afficher l'image si c'est celui par defaut
	position = _columnView->RectFrom(0);
	if(_isDefault)
	{
		owner->SetDrawingMode(B_OP_ALPHA);
		owner->DrawBitmap(_checkIcon,BPoint(position->left+(position->Width()-12)/2,bounds.top));
		owner->SetDrawingMode(B_OP_COPY);
	}

	// la separation du par defaut
	owner->SetHighColor(VIEW_COLOR);
	owner->StrokeLine(BPoint(position->right,bounds.top),BPoint(position->right,bounds.bottom-1));

	// ecrire le debut du code
	owner->SetHighColor(BLACK_COLOR);
	position = _columnView->RectFrom(1);
	owner->DrawString(_code,BPoint(position->left+10,bounds.bottom-4));
}

/**** ajuster la taille de l'item ****/
void CodeManagerItem::Update(BView *owner, const BFont *font)
{
	// on prend de quoi afficher l'icon
	// et le texte d'info
	SetHeight(15);
}

// =====================
// = fixer les données =
// =====================

/**** definir le code ****/
void CodeManagerItem::SetCode(const char *code)
{
	// verifier le parametre
	if(code==NULL)
		return;
	
	char	*buffer = NULL;
	int32	stringSize = 0;

	// recopier le code
	stringSize = strlen(code) + 1;
	buffer = (char *)realloc(_code,stringSize);
	if(buffer!=NULL)
	{
		_code = buffer;
		memcpy(_code,code,stringSize);
	}
}

/**** definir le generateur ****/
void CodeManagerItem::SetGenerator(const char *generatorID)
{
	// verifier le parametre
	if(generatorID==NULL)
		return;

	char	*buffer = NULL;
	int32	stringSize = 0;

	// recopier le generateur
	stringSize = strlen(generatorID) + 1;
	buffer = (char *)realloc(_generator,stringSize);
	if(buffer!=NULL)
	{
		_generator = buffer;
		memcpy(_generator,generatorID,stringSize);
	}
}

/**** definir par defaut ****/
void CodeManagerItem::SetDefault(bool isDefault)
{	_isDefault = isDefault;	}

/**** rafraichir les preferences ****/
void CodeManagerItem::RefreshPreferences()
{
	rgb_color tempColor = {200,200,255,255};
	_itemColor = g_ColorsPrefs->GetRGBColor(tempColor,"list-color");
}

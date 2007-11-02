/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
/**************************/
/* MeTOS Dependencie item */
/**************************/
#include "DependencieItem.h"
#include "VPreferenceFile.h"
#include "MetosGlobalDefs.h"
#include "PropertieManagerWin.h"
#include "CColumnView.h"
#include "MetosApp.h"
#include <interface/View.h>
#include <storage/Entry.h>
#include <interface/Bitmap.h>
#include <storage/AppFileInfo.h>
#include <storage/Node.h>
#include <storage/Mime.h>
#include <stdlib.h>

/**** constructeur ****/
DependencieItem::DependencieItem(const char *filepath,const char *generator,const char *MIMEtype)
: BListItem()
{
	// initialiser
	_generatorID = NULL;
	_icon = new BBitmap(BRect(0,0,15,15 ), B_CMAP8,true);
	_columnView = g_MetosApp->_propertieManagerWin->ColumnView(1);
	
	// on va remplir l'icon de la couleur transparente
	memset(_icon->Bits(),B_TRANSPARENT_MAGIC_CMAP8,256);

	// preferences
	RefreshPreferences();

	// recuperer le reste d'info
	_state = _path.SetTo(filepath);
	if(_state!=B_OK)
		return;
	
	// verifier la validite du generateur
	if(!((generator!=NULL) && (strlen(generator)>0)))
	{
		_state = B_ERROR;
		return;
	}

	// definir le generateur
	int32			stringSize = 0;

	stringSize = strlen(generator) + 1;
	_generatorID = (char *)realloc(_generatorID,stringSize);
	memcpy(_generatorID,generator,stringSize);

	// definir les infos sur l'objet
	SetMIME(MIMEtype);
}

/**** destructeur ****/
DependencieItem::~DependencieItem()
{
	// liberer la memoire prise par les chaines de caracteres
	if(_generatorID!=NULL)
		free(_generatorID);
		
	// icon
	delete _icon;
}

/**** Dessin des Items ****/
void DependencieItem::DrawItem(BView *owner,BRect bounds,bool complete)
{
	BRect	*position;

	// dessiner le cadre
	owner->SetHighColor(VIEW_COLOR);
	owner->StrokeLine(BPoint(0,bounds.bottom),BPoint(bounds.right,bounds.bottom));

	// dessiner le fond
	bounds.bottom -= 1;
	if(IsSelected())
		owner->SetHighColor(_itemColor);
	else
		owner->SetHighColor(WHITE_COLOR);
	owner->SetLowColor(owner->HighColor());
	owner->FillRect(bounds);

	// dessin de l'icon
	owner->SetDrawingMode(B_OP_ALPHA);
	owner->DrawBitmap(_icon,BPoint(2,bounds.top+4));
	owner->SetDrawingMode(B_OP_COPY);

	// chemin et type MIME
	owner->SetHighColor(BLACK_COLOR);
	position = _columnView->RectFrom(2);
	_truncBuffer = _path.Path();
	owner->TruncateString(&_truncBuffer,B_TRUNCATE_END,(position->left-22));
	owner->DrawString(_truncBuffer.String(),BPoint(22,bounds.top+15));
	owner->DrawString(_typeMIME,BPoint(position->left+4,bounds.top+15));

	owner->SetHighColor(VIEW_COLOR);
	owner->StrokeLine(BPoint(position->left,bounds.bottom),BPoint(position->left,bounds.top));
}

/**** ajuster la taille de l'item ****/
void DependencieItem::Update(BView *owner, const BFont *font)
{
	// on prend de quoi afficher l'icon
	// et le texte d'info
	SetHeight(23);
}

/**** definir le chemin ****/
void DependencieItem::SetPath(const char *path)
{
	// recuperer le reste d'info
	_state = _path.SetTo(path);
	if(_state!=B_OK)
		return;
}

/**** definir le type MIME ****/
void DependencieItem::SetMIME(const char *typeMIME)
{
	// on va pas recopier le même
	if(typeMIME==_typeMIME)
		return;
	
	// si on a un type MIME on se base sur le chemin
	// pour le trouver
	if(typeMIME==NULL)
	{
		BNode			node(_path.Path());
		BNodeInfo		nodeInfo(&node);

		// recuperer le type MIME
		nodeInfo.GetType(_typeMIME);
	}
	else
		// si il existe on va le recopier
		strcpy(_typeMIME,typeMIME);	
		
	// si c'est un executable on va reprendre son icon
	if(strcmp(_typeMIME,"application/x-vnd.Be-elfexecutable")==0)
	{
		BAppFileInfo	fileinfo;
		BFile			file(_path.Path(), B_READ_ONLY);

		// recuperer la signature de cet executable
		// pour ca on va devoir se baser sur le chemin
		fileinfo.SetTo(&file);	
		fileinfo.GetSignature(_typeMIME);
	}

	// creer le type
	BMimeType		MIME(_typeMIME);
	
	// type non valide ?
	if(!MIME.IsValid())
		return;

	// sinon recuperation de l'icon du type MIME
	MIME.GetIcon(_icon,B_MINI_ICON);
}

/**** rafraichir les preferences ****/
void DependencieItem::RefreshPreferences()
{
	rgb_color tempColor = {200,200,255,255};
	_itemColor = g_ColorsPrefs->GetRGBColor(tempColor,"list-color");
}

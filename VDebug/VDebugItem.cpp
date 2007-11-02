/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
/*********************/
/* Pour le debbugage */
/*********************/
#include "VDebugItem.h"
#include "MetosApp.h"
#include "MetosWin.h"
#include "VPreferenceFile.h"
#include "MetosGlobalDefs.h"
#include <interface/View.h>
#include <interface/Bitmap.h>
#include <stdlib.h>

/**** constructeur ****/
VDebugItem::VDebugItem(const char *message,int32 type)
: BListItem()
{
	// initialiser
	_message = NULL;
	_type = VDEBUG_DEBUG;

	// recuperer les donnees
	_type = type;
	if(message!=NULL)
	{
		int32	messageSize = strlen(message) + 1;
		_message = (char *)malloc(messageSize);
		memcpy(_message,message,messageSize);
	}
	
	// en fonction du type on choisi l'image
	if(g_MetosApp->_metosWin->Lock())
	{
		_infoBitmap = g_MetosApp->_metosWin->InfoBitmap(_type);
		g_MetosApp->_metosWin->Unlock();
	}
	else
		printf("Erreur : Lock() dans VDebugItem::VDebugItem()\n");
	
	// les preferences
	RefreshPreferences();
}

/**** destructeur ****/
VDebugItem::~VDebugItem()
{
	// liberer la memoire
	if(_message!=NULL)
		free(_message);
}

/**** dessiner l'item ****/
void VDebugItem::DrawItem(BView *owner,BRect bounds,bool complete)
{
	if(IsSelected())
		owner->SetHighColor(_itemColor);
	else
		owner->SetHighColor(255,255,255);
	owner->SetLowColor(owner->HighColor());
	owner->FillRect(bounds);

	// l'image
	if(_infoBitmap!=NULL)
	{
		owner->SetDrawingMode(B_OP_ALPHA);
		owner->DrawBitmap(_infoBitmap,BPoint(bounds.left+1,bounds.top));
		owner->SetDrawingMode(B_OP_COPY);
	}
	
	// le text
	owner->SetHighColor(0,0,0);
	owner->DrawString(_message,BPoint(bounds.left+18,bounds.bottom-2));
}

/**** rafraichir les preferences ****/
void VDebugItem::RefreshPreferences()
{
	rgb_color tempColor = {200,200,255,255};
	_itemColor = g_ColorsPrefs->GetRGBColor(tempColor,"list-color");
}

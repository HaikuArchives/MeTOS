/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/

/*************************/
/* MeTOS info view class */
/*************************/
#include "MetosApp.h"
#include "AddOnManagerWin.h"
#include "PropertieManagerInfoView.h"
#include "PropertieManagerWin.h"
#include "FunctionManagerItem.h"
#include "MetosGlobalDefs.h"
#include "CUrlView.h"
#include "VObject.h"
#include "MetosLanguagesDefs.h"
#include "VLanguage.h"

#include <interface/TextView.h>
#include <app/Message.h>
#include <support/String.h>
#include <interface/Bitmap.h>
#include <storage/Entry.h>
#include <AppFileInfo.h>

/**** constructeur ****/
PropertieManagerInfoView::PropertieManagerInfoView(BRect frame,const char *name,PropertieManagerWin *support)
: BView(frame,name,B_FOLLOW_ALL, B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE)
{
	// initialiser
	_supportWindow = support;
	_icon = NULL;

	// vue pour le about
	_about = new BTextView(BRect(54,53,frame.right-2,103),"about-text",BRect(0,0,frame.right-70,50),B_FOLLOW_LEFT_RIGHT,B_WILL_DRAW | B_FRAME_EVENTS);
	_about->MakeSelectable(false);
	_about->SetViewColor(255,255,255);
	AddChild(_about);

	// pour le mail et le site web
	_mail = new CUrlView(BRect(54,29,frame.right-2,40),"","",MAIL_LINK);
	_mail->SetViewColor(255,255,255);
	AddChild(_mail);

	_www = new CUrlView(BRect(54,41,frame.right-2,52),"","",WWW_LINK);
	_www->SetViewColor(255,255,255);
	AddChild(_www);

	// fixer la couleur de la vue
	SetViewColor(255,255,255);

	// object courant, aucun
	SetCurrentObject((uint32)NULL);
}

/**** destructeur ****/
PropertieManagerInfoView::~PropertieManagerInfoView()
{
	// liberer la memoire prise par l'icon
	if(_icon!=NULL)
		delete _icon;
}

/**** gestion des messages ****/
void PropertieManagerInfoView::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
	default:
		BView::MessageReceived(message);	
	}
}

/**** dessin de la vue ****/
void PropertieManagerInfoView::Draw(BRect updateRect)
{
	BRect	bounds = Bounds();

	SetHighColor(DARK_1_COLOR);
	StrokeLine(BPoint(bounds.left,bounds.top),BPoint(bounds.right,bounds.top));
	StrokeLine(BPoint(bounds.left,bounds.top),BPoint(bounds.left,bounds.bottom));

	bounds.InsetBy(1,1);
	SetHighColor(DARK_2_COLOR);
	StrokeRect(bounds);
	
	// ajuster la taille du rectangle
	bounds.InsetBy(1,1);

	// ecrire les titres
	SetHighColor(0,0,0);	
	DrawString(g_Languages->GetTranslationFor(L_INFO_VERSION),BPoint(10,15));	
	DrawString(g_Languages->GetTranslationFor(L_INFO_AUTHOR),BPoint(10,27));	
	DrawString(g_Languages->GetTranslationFor(L_INFO_EMAIL),BPoint(10,39));	
	DrawString(g_Languages->GetTranslationFor(L_INFO_LINK),BPoint(10,51));	
	DrawString(g_Languages->GetTranslationFor(L_INFO_ABOUT),BPoint(10,63));	

	// les données
	DrawString(_version.String(),BPoint(55,15));	
	DrawString(_author.String(),BPoint(55,27));	

	// l'icone
	if(_icon!=NULL)
	{
		SetDrawingMode(B_OP_ALPHA);
		DrawBitmap(_icon,BPoint(10,70));
		SetDrawingMode(B_OP_COPY);
	}

	SetHighColor(215,215,215);
	StrokeLine(BPoint(bounds.left,107),BPoint(bounds.right,107));

	// les titres
	SetHighColor(0,0,0);	
	DrawString(g_Languages->GetTranslationFor(L_INFO_OBJECT_ID),BPoint(10,117));
	DrawString(g_Languages->GetTranslationFor(L_INFO_PARENT_ID),BPoint(10,127));
	DrawString(g_Languages->GetTranslationFor(L_INFO_CLASS_NAME),BPoint(10,137));
	DrawString(g_Languages->GetTranslationFor(L_INFO_BE_OBJECT),BPoint(10,147));
	DrawString(g_Languages->GetTranslationFor(L_INFO_DERIVED),BPoint(10,157));
	
	// les données
	DrawString(_id.String(),BPoint(75,117));
	DrawString(_parentId.String(),BPoint(75,127));
	DrawString(_className.String(),BPoint(75,137));
	DrawString(_isBeOject.String(),BPoint(75,147));
	DrawString(_derivedClass.String(),BPoint(75,157));
}

/**** selection d'un objet ****/
void PropertieManagerInfoView::SetCurrentObject(uint32 selection)
{
	// fixer l'objet courant
	_currentObject = selection;
	
	// rafraichir
	RefreshValues();
}

/**** rafraichir certaine valeurs ****/
void PropertieManagerInfoView::RefreshValues()
{
	// initialiser
	_version = "";
	_author = "";
	_mail->SetUrl("");	
	_mail->SetLabel("");	
	_www->SetUrl("");
	_www->SetLabel("");
	_about->SetText("");
	_id = "";
	_parentId = "";
	_className = "";
	_isBeOject = "";
	_derivedClass = "";
	_icon=NULL;

	if(_currentObject!=(uint32)NULL)
	{
		// afficher les infos
		FunctionManagerItem	*functionItem = NULL;
		BMessage			*properties = NULL;
		BMessage			*settings = NULL;
		BMessage			*infos = NULL;
		BString				buffer;
		uint32				id;
		uint32				parentId;
		bool				isBe;
		bool				derivedClass;

		// recuperer les proprietes
		properties = _supportWindow->Properties();
		settings = _supportWindow->Settings();
		infos = _supportWindow->Infos();

		// les données
		infos->FindString(VOBJECT_VERSION,&_version);
		infos->FindString(VOBJECT_AUTHOR,&_author);
		infos->FindString(VOBJECT_EMAIL,&buffer);
		_mail->SetLabel(buffer.String());	
		_mail->SetUrl(buffer.String());	
		infos->FindString(VOBJECT_WWW,&buffer);
		_www->SetLabel(buffer.String());
		_www->SetUrl(buffer.String());
		infos->FindString(VOBJECT_ABOUT,&buffer);
		_about->SetText(buffer.String());
		
		properties->FindInt32(VOBJECT_ID,(int32 *)&id);
		_id << id;
		properties->FindInt32(VOBJECT_FATHER_ID,(int32 *)&parentId);
		_parentId << parentId;
		properties->FindString(VOBJECT_CLASS,&_className);
		settings->FindBool(VOBJECT_BE_OBJECT,&isBe);
		if(isBe)
			_isBeOject = g_Languages->GetTranslationFor(L_INFO_YES);
		else
			_isBeOject = g_Languages->GetTranslationFor(L_INFO_NO);

		// on peut ne pas trouver (cas d'un projet)
		if(properties->FindBool(VOBJECT_DERIVED_CLASS,&derivedClass)!=B_OK)
			derivedClass = false;
			
		if(derivedClass)
			_derivedClass = g_Languages->GetTranslationFor(L_INFO_YES);
		else
			_derivedClass = g_Languages->GetTranslationFor(L_INFO_NO);
		
		// recuperer l'icon
		functionItem = _supportWindow->FunctionItemFromClass(_className.String());
		if(functionItem==NULL)
			_icon = NULL;
		else
		{
			BAppFileInfo	fileinfo;
			BFile			file(functionItem->AddOnRef(), B_READ_ONLY);

			// recuperer le fichier
			fileinfo.SetTo(&file);	

			// on va essayer de recuperer l'icon
			_icon = new BBitmap(BRect(0,0,31,31 ), B_CMAP8);
			if(fileinfo.GetIcon(_icon, B_LARGE_ICON) != B_OK)
				fileinfo.GetTrackerIcon(_icon, B_LARGE_ICON);		
		}
	}
	
	// on rafraichi l'affichage
	// si la vue est attachée
	if(Looper()==NULL)
		return;
		
	// ok c'est attaché
	_mail->Invalidate();
	_www->Invalidate();
	Invalidate();
}

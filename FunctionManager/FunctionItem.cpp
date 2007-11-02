/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
/**********************************/
/* Item de la liste des fonctions */
/**********************************/
#include "FunctionItem.h"
#include "MetosApp.h"
#include "VPreferenceFile.h"
#include "MetosGlobalDefs.h"
#include "CSClass.h"
#include <interface/OutlineListView.h>
#include <interface/View.h>
#include <interface/Bitmap.h>

/**** Constructeur ****/
FunctionItem::FunctionItem(const char *returnType,const char *name,int16 protection)
: BListItem()
{
	// initialiser
	InitData();
	
	// fixer les données
	_returnType = returnType;
	_name = name;
	_protection = protection;

	// virer les espaces devant la chaine
	// dans le cas du constructeur ou du destructeur
	if(_returnType.Length()<2)
		_returnType.RemoveFirst(" ");
}

/**** creer un item a partir d'un message ****/
FunctionItem::FunctionItem(BMessage *archive)
: BListItem()
{
	// initialiser
	InitData();

	// on va utiliser AddParameter pour ajouter les parametres 
	BMessage	parameters;
	const char	*parameterType;
	const char	*parameterName;
	const char	*parameterValue;
	status_t	state = B_OK;
	
	// type de retour puis nom puis parametres
	state &= archive->FindString(FUNCTION_PARAMETER_TYPE,&_returnType);
	state &= archive->FindString(FUNCTION_PARAMETER_NAME,&_name);
	state &= archive->FindMessage(FUNCTION_PARAMETERS,&parameters);
	for(int32 index=0;parameters.FindString(FUNCTION_PARAMETER_TYPE,index,&parameterType)==B_OK;index++)
	{
		// on ne doit pas prendre la valeur comme une erreur
		// car on en a pas toujours
		state &= parameters.FindString(FUNCTION_PARAMETER_NAME,index,&parameterName);
		parameters.FindString(FUNCTION_PARAMETER_VALUE,index,&parameterValue);
		if(state==B_OK)
			AddParameter(parameterType,parameterName,parameterValue,index==0);
	}	
}

/**** initialiser les données ****/
void FunctionItem::InitData()
{
	// fixer les données par defaut
	_parameters.what = FUNCTION_PARAMETERS_MSG;
	_checkFunction = NULL;
	_returnType = "";
	_name = "";
	_args = "";
	_protection = CSMT_PUBLIC;
	_isAConstructor = false;
	_isASuperItem = false;
	_isChecked = false;

	// charger la couleur de fond
	RefreshPreferences();
}

/**** Destructeur ****/
FunctionItem::~FunctionItem()
{
}

/**** ajouter les parametres ****/
void FunctionItem::AddParameter(const char *type,const char *name,const char *value,bool firstArg)
{
	// ajouter a notre liste de parametres
	_parameters.AddString(FUNCTION_PARAMETER_TYPE,type);
	_parameters.AddString(FUNCTION_PARAMETER_NAME,name);
	_parameters.AddString(FUNCTION_PARAMETER_VALUE,value);

	// ajouter a la chaine les valeurs
	if(!firstArg)
		_args << ",";
	
	// le type
	_args << type;
	if(strlen(type)>0 && type[0]!='*')	
		_args << " ";
	
	// le nom
	_args << name;
	
	// la valeur
	if(strlen(value)>0)
		_args << " = " << value;
}

/**** archiver les données dans le message ****/
status_t FunctionItem::Archive(BMessage *archive)
{
	// cette fonction est utilisé pour la sauvegarde des
	// données
	BMessage	function(FUNCTION_MSG);
	status_t	state = B_OK;

	// type de retour puis nom puis parametres
	state &= function.AddString(FUNCTION_PARAMETER_TYPE,_returnType);
	state &= function.AddString(FUNCTION_PARAMETER_NAME,_name);
	state &= function.AddMessage(FUNCTION_PARAMETERS,&_parameters);
	state &= archive->AddMessage(FUNCTION_ITEM,&function);

	// retourner le resultat
	return state;
}

/**** Dessin des Items ****/
void FunctionItem::DrawItem(BView *owner,BRect bounds,bool complete)
{
	// recadrer
	bounds.left = 0;

	// couleur de fond
	owner->SetHighColor(WHITE_COLOR);
	if(IsSelected())
		owner->SetHighColor(_itemColor);
	else if(_isASuperItem)
		owner->SetHighColor(LIGHT_2_COLOR);
	owner->SetLowColor(owner->HighColor());
	owner->FillRect(bounds);

	// dessiner le cadre
	owner->SetHighColor(DARK_1_COLOR);
	owner->StrokeLine(BPoint(0,bounds.bottom),BPoint(bounds.right,bounds.bottom));	

	// le petit carre
	if(_isASuperItem)
	{
		BRect	itemRect(bounds.left,bounds.top,bounds.left+bounds.Height(),bounds.top+bounds.Height());
		BRect	latchRect;

		latchRect = itemRect;
		latchRect.InsetBy(3,3);

		owner->SetHighColor(WHITE_COLOR);
		owner->StrokeLine(BPoint(latchRect.left,latchRect.top),BPoint(latchRect.right,latchRect.top));
		owner->StrokeLine(BPoint(latchRect.left,latchRect.top),BPoint(latchRect.left,latchRect.bottom));

		owner->SetHighColor(BLACK_COLOR);
		owner->StrokeLine(BPoint(latchRect.right,latchRect.top),BPoint(latchRect.right,latchRect.bottom));
		owner->StrokeLine(BPoint(latchRect.left,latchRect.bottom),BPoint(latchRect.right,latchRect.bottom));

		latchRect.InsetBy(1,1);
		owner->SetHighColor(VIEW_COLOR);
		owner->FillRect(latchRect);

		owner->SetHighColor(DARK_2_COLOR);
		owner->StrokeLine(BPoint(latchRect.right,latchRect.top),BPoint(latchRect.right,latchRect.bottom));
		owner->StrokeLine(BPoint(latchRect.left,latchRect.bottom),BPoint(latchRect.right,latchRect.bottom));

		latchRect.right -=1;
		latchRect.bottom -=1;
		owner->SetHighColor(BLACK_COLOR);
		owner->StrokeLine(BPoint(latchRect.left+2,latchRect.top+4),BPoint(latchRect.right-2,latchRect.top+4));
		if(IsExpanded())
			owner->StrokeLine(BPoint(latchRect.left+4,latchRect.top+2),BPoint(latchRect.left+4,latchRect.bottom-2));
	}
	
	// la selection de la metode
	bounds.left += 16 + OutlineLevel() * 10;
	if(_isChecked && _checkFunction!=NULL)
	{
		owner->SetDrawingMode(B_OP_OVER);
		owner->DrawBitmap(_checkFunction,BPoint(bounds.left,bounds.top+1));
		owner->SetDrawingMode(B_OP_COPY);
	}

	// le texte
	bounds.left += 18;
	bounds.top += 12;
	owner->SetHighColor(BLACK_COLOR);
	owner->DrawString(_returnType.String(),BPoint(bounds.left,bounds.top));
	bounds.left += owner->StringWidth(_returnType.String());
	owner->DrawString(_name.String(),BPoint(bounds.left,bounds.top));
	bounds.left += owner->StringWidth(_name.String());
	owner->DrawString("(",BPoint(bounds.left,bounds.top));	
	bounds.left += owner->StringWidth("(");
	owner->DrawString(_args.String(),BPoint(bounds.left,bounds.top));	
	bounds.left += owner->StringWidth(_args.String());
	owner->DrawString(")",BPoint(bounds.left,bounds.top));
}

/**** Changer la taille ****/
void FunctionItem::Update(BView *owner, const BFont *font)
{
	// changer la taille
	SetHeight(17);
}

/**** rafraichir les preferences ****/
void FunctionItem::RefreshPreferences()
{
	rgb_color tempColor = {200,200,255,255};
	_itemColor = g_ColorsPrefs->GetRGBColor(tempColor,"list-color");
}

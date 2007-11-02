/**********************************************/
/* Addon de la proprietes _resizeMask de BeOS */
/**********************************************/
#include "_resize_mode.h"
#include <interface/View.h>
#include <interface/Box.h>
#include <interface/RadioButton.h>

/**** constructeur ****/
_ResizeMode::_ResizeMode(entry_ref *entry)
: VPropertie("_resize_mode",entry)
{
	// noms gerés
	BMessage	*managedName = ManagedNames();

	// valeur par defaut
	_resize_mode = B_FOLLOW_NONE;
	
	// ajouter les noms gerés
	managedName->AddString(VPROPERTIE_ALTERN_NAME,"_resize_mode");

	// premier groupe
	_groupH = new BBox(BRect(0,0,145,60),"",B_FOLLOW_LEFT | B_FOLLOW_TOP,B_WILL_DRAW,B_NO_BORDER);
	_bFollowLeft = new BRadioButton(BRect(0,0,143,15),"bFollowLeft","B_FOLLOW_LEFT",NULL);
	_bFollowRight = new BRadioButton(BRect(0,15,143,30),"bFollowRight","B_FOLLOW_RIGHT",NULL);
	_bFollowLeftRight = new BRadioButton(BRect(0,30,143,45),"bFollowLeftRight","B_FOLLOW_LEFT_RIGHT",NULL);
	_bFollowHCenter = new BRadioButton(BRect(0,45,143,60),"bFollowHCenter","B_FOLLOW_H_CENTER",NULL);
	_groupH->AddChild(_bFollowLeft);
	_groupH->AddChild(_bFollowRight);
	_groupH->AddChild(_bFollowLeftRight);
	_groupH->AddChild(_bFollowHCenter);
	// second
	_groupV = new BBox(BRect(145,0,290,60),"",B_FOLLOW_LEFT | B_FOLLOW_TOP,B_WILL_DRAW,B_NO_BORDER);
	_bFollowTop = new BRadioButton(BRect(0,0,143,15),"bFollowTop","B_FOLLOW_TOP",NULL);
	_bFollowBottom = new BRadioButton(BRect(0,15,143,30),"bFollowBottom","B_FOLLOW_BOTTOM",NULL);
	_bFollowTopBottom = new BRadioButton(BRect(0,30,143,45),"bFollowTopBottom","B_FOLLOW_TOP_BOTTOM",NULL);
	_bFollowVCenter = new BRadioButton(BRect(0,45,143,60),"bFollowVCenter","B_FOLLOW_V_CENTER",NULL);
	_groupV->AddChild(_bFollowTop);
	_groupV->AddChild(_bFollowBottom);
	_groupV->AddChild(_bFollowTopBottom);
	_groupV->AddChild(_bFollowVCenter);
}

/**** destructeur ****/
_ResizeMode::~_ResizeMode()
{
	// liberer la memoire
	delete _groupH;
	delete _groupV;
}

/**** function de clonage d'une propriete ****/
VPropertie* _ResizeMode::InstantiateVPropertie(entry_ref *entry)
{
	// creer le MObject apres MeTOS va appeler la methode CreateObject() pour reelement
	// creer l'objet graphique (fenetre,vue etc...)
	return (new _ResizeMode(entry));
}

/**** gerer l'affichage ****/
void _ResizeMode::Draw(BView *owner,BRect bounds,bool complete)
{
	BString		buffer;

	// transformer en chaine
	ConvertModeToString(_resize_mode,buffer);
	
	// apres on va l'afficher
	owner->SetLowColor(owner->HighColor());
	owner->SetHighColor(0,0,0);
	owner->DrawString(buffer.String(),BPoint(bounds.left+5,bounds.top+15));	
}

/**** recuperer la valeur qui nous interesse ****/
void _ResizeMode::SetProperties(BMessage *properties)
{
	// verifier que la proprietes existe dans l'item
	if(properties!=NULL)
		properties->FindInt32(PropertiesName(),Index(),(int32 *)&_resize_mode);
}

/**** renvoyer la bonne valeur ****/
void _ResizeMode::Properties(BMessage *objectProperties)
{
	objectProperties->AddInt32(PropertiesName(),(int32)_resize_mode);
}

/**** ajouter le BBox a l'ecran ****/
void _ResizeMode::AddBox(BView *owner,BBox *box,float x,float y)
{
	BView	*children = NULL;
	
	// ajouter la chackBox et modifier la couleur de fond
	owner->AddChild(box);
	box->SetViewColor(_itemColor);
	box->SetLowColor(_itemColor);
	for(int32 index=0;index<box->CountChildren();index++)
	{
		children = box->ChildAt(index);
		if(children!=NULL)
		{
			children->SetViewColor(_itemColor);
			children->SetLowColor(_itemColor);
		}
	}
	box->MoveTo(x,y);
}

/**** selection l'item pour editer la donnée ****/
void _ResizeMode::SelectItem(BView *owner,BRect bounds)
{
	// recuperer les composante
	if((_resize_mode & B_FOLLOW_H_CENTER)==B_FOLLOW_H_CENTER)
		_bFollowHCenter->SetValue(B_CONTROL_ON);
	else if((_resize_mode & B_FOLLOW_RIGHT)==B_FOLLOW_RIGHT)
		_bFollowRight->SetValue(B_CONTROL_ON);
	else if((_resize_mode & B_FOLLOW_LEFT_RIGHT)==B_FOLLOW_LEFT_RIGHT)
		_bFollowLeftRight->SetValue(B_CONTROL_ON);
	else if((_resize_mode & B_FOLLOW_LEFT)==B_FOLLOW_LEFT)
		_bFollowLeft->SetValue(B_CONTROL_ON);
			
	if((_resize_mode & B_FOLLOW_V_CENTER)==B_FOLLOW_V_CENTER)
		_bFollowVCenter->SetValue(B_CONTROL_ON);
	else if((_resize_mode & B_FOLLOW_BOTTOM)==B_FOLLOW_BOTTOM)
		_bFollowBottom->SetValue(B_CONTROL_ON);
	else if((_resize_mode & B_FOLLOW_TOP_BOTTOM)==B_FOLLOW_TOP_BOTTOM)
		_bFollowTopBottom->SetValue(B_CONTROL_ON);
	else if((_resize_mode & B_FOLLOW_TOP)==B_FOLLOW_TOP)
		_bFollowTop->SetValue(B_CONTROL_ON);

	// ajouter les radio-boutons
	AddBox(owner,_groupH,bounds.left+3,bounds.bottom);
	AddBox(owner,_groupV,bounds.left+148,bounds.bottom);

	// ajuster la taille
	SetHeight(bounds.Height() + 65);
}

/**** deselection l'item ****/
void _ResizeMode::UnSelectItem(BView *owner,BRect bounds)
{
	uint32		newResizeMode = B_FOLLOW_NONE;
	
	// ajuster la taille
	SetHeight(bounds.Height() - 65);

	if(_bFollowHCenter->Value()==B_CONTROL_ON)
		newResizeMode |= B_FOLLOW_H_CENTER;
	else if(_bFollowRight->Value()==B_CONTROL_ON)
		newResizeMode |= B_FOLLOW_RIGHT;
	else if(_bFollowLeftRight->Value()==B_CONTROL_ON)
		newResizeMode |= B_FOLLOW_LEFT_RIGHT;
	else if(_bFollowLeft->Value()==B_CONTROL_ON)
		newResizeMode |= B_FOLLOW_LEFT;
			
	if(_bFollowVCenter->Value()==B_CONTROL_ON)
		newResizeMode |= B_FOLLOW_V_CENTER;
	else if(_bFollowBottom->Value()==B_CONTROL_ON)
		newResizeMode |= B_FOLLOW_BOTTOM;
	else if(_bFollowTopBottom->Value()==B_CONTROL_ON)
		newResizeMode |= B_FOLLOW_TOP_BOTTOM;
	else if(_bFollowTop->Value()==B_CONTROL_ON)
		newResizeMode |= B_FOLLOW_TOP;
	
	// verifier si la valeur a changée
	SetChange(newResizeMode!=_resize_mode);
	_resize_mode = newResizeMode;
	
	// les enlever
	owner->RemoveChild(_groupH);
	owner->RemoveChild(_groupV);
}

/**** recuperer la valeur sour forme de chaine ****/
void _ResizeMode::ValueAsString(BMessage *properties,BString &value,int32 index)
{
	uint32		resize_mode;
	
	if(properties->FindInt32(PropertiesName(),index,(int32 *)&resize_mode)==B_OK)
		ConvertModeToString(resize_mode,value);
}

/**** recuperer sous forme de chaine la valeur du resize_mode ****/
void _ResizeMode::ConvertModeToString(uint32 &mode,BString &string)
{
	// fixer le radio bouton selectionné
	if((mode & B_FOLLOW_H_CENTER)==B_FOLLOW_H_CENTER)
		string << " | B_FOLLOW_H_CENTER";
	else if((mode & B_FOLLOW_RIGHT)==B_FOLLOW_RIGHT)
		string << " | B_FOLLOW_RIGHT";
	else if((mode & B_FOLLOW_LEFT_RIGHT)==B_FOLLOW_LEFT_RIGHT)
		string << " | B_FOLLOW_LEFT_RIGHT";
	else if((mode & B_FOLLOW_LEFT)==B_FOLLOW_LEFT)
		string << " | B_FOLLOW_LEFT";
	
	if((mode & B_FOLLOW_V_CENTER)==B_FOLLOW_V_CENTER)
		string << " | B_FOLLOW_V_CENTER";
	else if((mode & B_FOLLOW_BOTTOM)==B_FOLLOW_BOTTOM)
		string << " | B_FOLLOW_BOTTOM";
	else if((mode & B_FOLLOW_TOP_BOTTOM)==B_FOLLOW_TOP_BOTTOM && (mode & B_FOLLOW_LEFT_RIGHT)==B_FOLLOW_LEFT_RIGHT)
		string = " | B_FOLLOW_ALL_SIDES";
	else if((mode & B_FOLLOW_TOP_BOTTOM)==B_FOLLOW_TOP_BOTTOM)
		string << " | B_FOLLOW_TOP_BOTTOM";
	else if((mode & B_FOLLOW_TOP)==B_FOLLOW_TOP)
		string << " | B_FOLLOW_TOP";

	if(mode==B_FOLLOW_NONE)
		string = " | B_FOLLOW_NONE";
	
	// enlever le " | " en debut de chaine
	if(string.Length()>0)
		string.Remove(0,3);
}

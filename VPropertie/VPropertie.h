/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _VPROPERTIE_H
#define _VPROPERTIE_H
/*********************************************/
/* Classe qui sert a creer les addon pour    */
/* gerer les proprietes des objets graphique */
/*********************************************/
#include <interface/ListItem.h>
#include <support/String.h>
#include <interface/View.h>
#include <app/Message.h>
#include <storage/Entry.h>

// proprietes speciales
#define		VPROPERTIE_ALTERN_NAME				"propertie-altern-name"

class BBitmap;

class VPropertie :	public BListItem
{
public:
	VPropertie(const char *name,entry_ref *entry);
	virtual ~VPropertie();

	virtual	VPropertie	*InstantiateVPropertie(entry_ref *entry) =0;						// recreer une proprietes
	virtual	void		Draw(BView *owner,BRect bounds,bool complete = false) =0;			// dessin utilisateur de la donnée
	virtual	void		SelectItem(BView *owner,BRect bounds);								// selectionne l'item
	virtual	void		UnSelectItem(BView *owner,BRect bounds);							// deselectionne l'item
	virtual	void		SetProperties(BMessage *properties) =0;								// acces aux proprietes
	virtual	void		Properties(BMessage *objectProperties) =0;							// modifier les proprietes
	virtual void		ValueAsString(BMessage *properties,BString &value,int32 index) =0;	// recuperer la valeur sour forme de chaine
			void		SetIndex(int32 index);												// definir l'index geré
			void		RefreshPreferences();												// rafraichir les preferences
			void		SetPropertiesName(const char *value);								// definir la propriete editée
			bool		NameIsManaged(const char *name);									// verifier si ce nom est geré (ca peux etre un type)

	inline	void		SetNeedRefresh()						{ _needRefresh = true; }			// on a besoin de rafraichir (est a false par defaut)
	inline	void		SetChange(bool value)					{ _hasChange = value; }				// fixer si la donnée a été modifié
	inline	void		SetEnable(bool value)					{ _enable = value; }				// rendre editable ou pas un propriete
	inline	void		SetSizeProperties(float value)			{ _sizePropertiesInfo = value; }	// fixer la taille de la premiere colonne
	inline	bool		IsNeedRefresh()							{ return _needRefresh; }			// a t-on demandé de rafraichir
	inline	int32		Index()									{ return _index; }					// fixe l'index de la données (comme les BMessages)
	inline	const char	*PropertiesName()						{ return _propertiesName; }			// nom de la propriete gerés						
	inline	const char	*AddonName()							{ return _addonName; }				// nom de la propriete gerés						
	inline	bool		IsChange()								{ return _hasChange; }				// sert a savoir si la donnée a été modifié
	inline	bool		IsEnable()								{ return _enable; }					// voir si on peux editer la propriete
	inline	float		SizeProperties()						{ return _sizePropertiesInfo; }		// recuperer la taille de la premiere colonne
	inline	BMessage	*ManagedNames()							{ return &_managedNames; }			// liste de nom et type gerés
	inline	BEntry		*Entry()								{ return &_entry; }					// recuperer l'entry sur le fichier

protected:
	rgb_color			_itemColor;				// couleur de selection	

private:
	BMessage			_managedNames;			// liste des nom gerés
	char				*_addonName;			// no de l'addon
	char				*_propertiesName;		// nom actuellement geré
	BBitmap				*_icon;					// l'icon
	bool				_needRefresh;			// on veux rafraichir les autres affichages (objets)
	int32				_index;					// fixe l'index de la donnée, si il y a plusieurs données de memes nom
	bool				_hasChange;				// sert a savoir si la donnée a été modifié
	bool				_enable;				// editable ?
	float				_sizePropertiesInfo;	// fixer la taille de la premiere colonne
	BString				_truncBuffer;			// buffer pour tronquer les chaines
	BEntry				_entry;					// reference sur le fichier

			void	Update(BView *owner, const BFont *font);					// modifier la taille
			void	DrawItem(BView *owner,BRect bounds,bool complete = false);	// dessin de l'addon
			void	SetIcon();													// definir l'icon
};

#endif

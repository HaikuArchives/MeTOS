/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _VTOOL_H
#define _VTOOL_H
/********************/
/* MeTOS tool class */
/********************/
#include <interface/Button.h>
#include <kernel/image.h>

class BEntry;
class MButtonTool;
class VTool;
class BBitmap;

// constantes
#define		TOOL_SISE		37

class VTool : public BButton
{
public:
	VTool(const char *name,image_id addonId,BEntry *entry);
	virtual ~VTool();

	virtual	void		DrawTool(BRect bounds);					// dessiner le bouton soit-meme
	virtual	void		DoAction(BPoint *where) = 0;			// clique gauche executer l'action
	virtual	void		SelectTools(BPoint *where);				// clique droit selection (peux servir pour autre chose)
	virtual	void		MessageReceived(BMessage *message);		// gestion des messages
			void		RefreshPreferences();					// rafraichir les preferences
			
	inline	image_id	AddonId()		{ return _addonId; }	// l'identifiant de l'image de l'addon
			const char	*Name();								// le nom de l'addon


private:
	image_id		_addonId;
	BBitmap			*_icon;
	bool			_entered;
	rgb_color		_buttonColor;
	
			void	Draw(BRect updateRect);		// dessiner le bouton
			void	MouseDown(BPoint point);	// on clique sur le bouton
			void	MouseUp(BPoint point);		// on relache le clique sur le bouton
			void	MouseMoved(BPoint point, uint32 transit,const BMessage *message);	// on passe au dessus (souris)
};

#endif

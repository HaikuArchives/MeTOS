/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _VPREFERENCECONTAINER_H
#define _VPREFERENCECONTAINER_H
/*************************/
/* MeTOS Container class */
/* pour les preferences  */
/*************************/
#include <interface/View.h>

// definition de la classe
class VPreferenceContainer : public BView
{
public:
	VPreferenceContainer(BRect frame,const char *name);
	virtual ~VPreferenceContainer();
	
protected:	
	virtual	void	Draw(BRect updateRect);		// fonction de dessin de la vue
};

#endif

/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _TOOLMANAGERVIEW_H
#define _TOOLMANAGERVIEW_H
/*************************/
/* vue custom des outils */
/*************************/
#include <interface/View.h>

// constantes
#define		FRAME_SIZE		4

class ToolManagerView : public BView
{
public:
	ToolManagerView(BRect frame,const char *name);
	~ToolManagerView();

	virtual void		Draw(BRect updateRect);		// dessiner la vue un peu autrement

protected:
};

#endif

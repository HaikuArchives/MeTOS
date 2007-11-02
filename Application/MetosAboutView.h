/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _METOABOUTVIEW_H
#define _METOABOUTVIEW_H
/***************************/
/* vue de la fenetre about */
/***************************/
#include <interface/View.h>

class BBitmap;

class MetosAboutView : public BView
{
public:
	MetosAboutView(BRect);
	virtual	~MetosAboutView();

	virtual	void	Draw(BRect updateRect);
	virtual	void	Pulse();

protected:
	BBitmap			*_background;
	BBitmap			*_smoothBitmap;
	BBitmap			*_logo[2];
	BView			*_smoothView;
	int32			_animCounter;
	int32			_indexText;
	drawing_mode	_mode;

			void	DrawAnimation();
};

#endif

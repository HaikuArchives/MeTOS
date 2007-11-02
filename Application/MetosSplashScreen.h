/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _METOSSPLASHSCREEN_H
#define _METOSSPLASHSCREEN_H
/****************************/
/* fenetre de splash-screen */
/****************************/
#include <interface/Window.h>
#include <interface/View.h>
#include <support/String.h>

class BBitmap;

// messages
#define			M_PROGRESSSTART_MSG		'Mpdm'
#define			M_PROGRESSINCREASE_MSG	'Mpim'

// données
#define			M_TOTALPROGRESS			"total-progress"
#define			M_TEXTDISPLAY			"text-display"

// vue ou on affiche la bitmap
class DisplayView : public BView
{
public:
	DisplayView(BRect frame);
	~DisplayView();	

	virtual void	Draw(BRect updateRect);

			void	SetNbFiles(float nbFiles);
			void	InvalidateText();
	inline	void	IncreaseCounter()					{ _counter += _counterStep; }
	inline	void	InvalidateStatus()					{ Invalidate(_parsingRect); }
	inline	void	SetTextDisplay(const char *value)	{ _displayText = value; }
	
protected:
	BString		_displayText;
	BBitmap		*_screenBitmap;
	BBitmap		*_logo[2];
	float		_nbFiles;
	float		_counter;
	float		_counterStep;
	BRect		_parsingRect;
};

// fenetre du splash-screen
class MetosSplashScreen : public BWindow
{
public:
	MetosSplashScreen(BRect frame);
	~MetosSplashScreen();	

	virtual void	MessageReceived(BMessage *message);

protected:
	DisplayView	*_view;

	BRect		GetFrame();
};

#endif

/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _VTIPS_H
#define _VTIPS_H
/********************/
/* MeTOS tips class */
/********************/
#include <app/Message.h>
#include <support/String.h>
#include <storage/Entry.h>

// message
#define		V_TIPS_MSG		'Mtip'
#define		V_TIPS_TEXT		"tips-text"

// class
class VTips : public BMessage
{
public:
	VTips(const char *name);
	virtual ~VTips();

	bool		Exist();						// le fichier existe ou pas ?
	status_t	AddTips(const char *newTips);	// ajouter un tips
	status_t	Save();							// sauver les tips

private:
	BString		_filePath;		// chemins du fichier
	BEntry		_tipsFile;		// fichier
};

#endif

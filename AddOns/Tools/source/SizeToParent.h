#ifndef _SIZETOPARENT_H
#define _SIZETOPARENT_H
/*********************/
/* MeTOS tools class */
/*********************/
#include "VTool.h"
#include <app/Messenger.h>

class BEntry;

class SizeToParent : public VTool
{
public:
	SizeToParent(image_id addonId,BEntry *entry);

			void	DoAction(BPoint *where);
			void	RefreshPreferences();
	
private:
	BMessenger	_application;
	int32		_stepGrid;
};

#endif

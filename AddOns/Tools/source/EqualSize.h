#ifndef _EQUALSIZE_H
#define _EQUALSIZE_H
/*********************/
/* MeTOS tools class */
/*********************/
#include "VTool.h"
#include <app/Messenger.h>

class BEntry;

class EqualSize : public VTool
{
public:
	EqualSize(image_id addonId,BEntry *entry);

			void	DoAction(BPoint *where);
			void	RefreshPreferences();
	
private:
	BMessenger	_application;
	int32		_stepGrid;
};

#endif

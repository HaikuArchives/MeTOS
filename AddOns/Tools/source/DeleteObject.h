#ifndef _DELETEOBJECT_H
#define _DELETEOBJECT_H
/*********************/
/* MeTOS tools class */
/*********************/
#include "VTool.h"
#include <app/Messenger.h>

class BEntry;

class DeleteObject : public VTool
{
public:
	DeleteObject(image_id addonId,BEntry *entry);

			void	DoAction(BPoint *where);

private:
	BMessenger	_application;
};

#endif

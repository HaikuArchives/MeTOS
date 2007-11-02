/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _CPPCODEGENERATOR_H
#define _CPPCODEGENERATOR_H
/**********************************/
/* MeTOS C++ Code Generator Addon */
/**********************************/
#include "VCodeGenerator.h"
#include <kernel/image.h>
#include <app/Messenger.h>
#include <app/Message.h>
#include <support/List.h>
#include <support/String.h>

class CPPCodeWin;

class CPPCodeGenerator : public VCodeGenerator
{
public:
	CPPCodeGenerator(image_id addonId,BEntry *entry);
	~CPPCodeGenerator();

			void		StartGenerator(BMessage *archive);			// appeler le generateur	

	inline const char	*DisplayLabel()		{ return "C++ Generator"; }	// ce qui faut afficher

protected:
	CPPCodeWin		*_CPPCodeWin;		// fenetre
};

#endif
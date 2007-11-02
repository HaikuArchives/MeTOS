/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _BEPASCALCODEGENERATOR_H
#define _BEPASCALCODEGENERATOR_H
/*************************************/
/* MeTOS PASCAL Code Generator Addon */
/*************************************/
#include "VCodeGenerator.h"
#include <image.h>

class CPPCodeWin;
class BBitmap;
class BEntry;
class BMessage;

class BePascalCodeGenerator : public VCodeGenerator
{
public:
	BePascalCodeGenerator(image_id addonId,BEntry *entry);
	~BePascalCodeGenerator();

			void		StartGenerator(BMessage *archive);					// appeler le generateur

	inline const char	*DisplayLabel()		{ return "Pascal Generator"; }	// ce qui faut afficher
};

#endif
/***************************/
/* MeTOS preferences class */
/***************************/

#include "MetosPreference.h"
#include "MetosMessageDefs.h"

#include <FindDirectory.h>
#include <Directory.h>
#include <Path.h>
#include <Entry.h>
#include <File.h>
#include <Control.h>

// debug
//#include <iostream.h>

/**** constructeur ****/
MetosPreference::MetosPreference(const char *fileName)
: MPreferenceFile(fileName)
{
}

/**** destructeur ****/
MetosPreference::~MetosPreference()
{		
}

/**** sauvegarder les prefs ****/
status_t MetosPreference::Save()
{	
	status_t	status;

	// Sauver les donnees du message	
	BMessage::MakeEmpty();
	status = AddRect("appWindow-rect",_appWindowRect);
	status &= AddRect("addOnManager-rect",_addOnManagerRect);
	status &= AddRect("objectManager-rect",_objectManagerRect);
	status &= AddRect("propertiesManager-rect",_propertiesManagerRect);
	status &= AddRect("toolsManager-rect",_toolsManagerRect);
	status &= AddRect("undoManager-rect",_undoManagerRect);
	status &= AddRect("about-rect",_aboutWindowRect);	
	status &= AddRect("preference-rect",_preferenceManagerRect);
	status &= AddInt32("selction-color",*(int32 *)&_selctionColor);
	status &= AddInt32("selction-back-color",*(int32 *)&_selctionBackColor);
	status &= AddInt32("list-color",*(int32 *)&_listColor);
	status &= AddInt32("use-grid",_useGrid);
	status &= AddInt32("step-grid",_stepGrid);
	
	// sauver les donnees
	status &= MPreferenceFile::Save();

	return status;
}

/**** charger les prefs ****/
status_t MetosPreference::Load()
{
	status_t	status;

	// charger les donnees
	status = MPreferenceFile::Load();

	// si on ne trouve pas une valeur on l'initialise avec sa valeur par defaut
	if((status &= FindRect(,&_appWindowRect))!=B_OK)
		_appWindowRect = ;
	if((status &= FindRect(,&_addOnManagerRect))!=B_OK)
		_addOnManagerRect = ;
	if((status &= FindRect(,&_objectManagerRect))!=B_OK)
		_objectManagerRect = ;
	if((status &= FindRect(,&_propertiesManagerRect))!=B_OK)
		_propertiesManagerRect = ;
	if((status &= FindRect(,&_toolsManagerRect))!=B_OK)
		_toolsManagerRect = ;
	if((status &= FindRect(,&_undoManagerRect))!=B_OK)
		_undoManagerRect = ;
	if((status &= FindRect(,&_aboutWindowRect))!=B_OK)
		_aboutWindowRect = ;
	if((status &= FindRect(,&_preferenceManagerRect))!=B_OK)
		_preferenceManagerRect = ;


	if((status &= FindInt32("selction-color",(int32 *)&_selctionColor))!=B_OK)
		_selctionColor = (rgb_color){0,255,0,100};
	if((status &= FindInt32("selction-back-color",(int32 *)&_selctionBackColor))!=B_OK)
		_selctionBackColor = (rgb_color){0,155,0,100};
	if((status &= FindInt32("list-color",(int32 *)&_listColor))!=B_OK)
		_listColor = (rgb_color){200,200,255,255};
	if((status &= FindInt32("use-grid",_useGrid))!=B_OK)
		_useGrid = B_CONTROL_OFF;
	if((status &= FindInt32("step-grid",_stepGrid))!=B_OK)
		_stepGrid = 10;
		
	return status;
}
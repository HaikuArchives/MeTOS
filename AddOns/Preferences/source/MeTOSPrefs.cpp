/*********************************************/
/* C'est l'exemple de code pour integrer des */
/* vues de preferences dans MeTOS            */
/*********************************************/
// les vues preferences
#include "ApplicationPrefs.h"
#include "BackgroundPrefs.h"
#include "DebugPrefs.h"
#include "GeneratorPrefs.h"
#include "ScreenPrefs.h"
#include "CPPGeneratorPrefs.h"
#include "UpgraderPrefs.h"
#include "LanguagesPrefs.h"

#include <List.h>

// on a besoin d'une metode externe
// qui retourne une liste le VPreference (la liste peux contenir un seul element)

extern "C" __declspec(dllexport) void instantiate_VPreferences(BList *container,BRect frame);

/**** function de creation de l'objet pour l'architecture Addon ****/
void instantiate_VPreferences(BList *container,BRect frame)
{
	// verifier que le pointer est valide
	if(container==NULL)
		return;
	
	// ajouter les objets VPreference
	container->AddItem(new ApplicationPrefs(frame));
	container->AddItem(new BackgroundPrefs(frame));
	container->AddItem(new DebugPrefs(frame));
	container->AddItem(new GeneratorPrefs(frame));
	container->AddItem(new ScreenPrefs(frame));
	container->AddItem(new UpgraderPrefs(frame));
	container->AddItem(new CPPGeneratorPrefs(frame));
	container->AddItem(new LanguagesPrefs(frame));
}


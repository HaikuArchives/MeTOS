/*********************************************/
/* C'est l'exemple de code pour integrer des */
/* vues de preferences dans MeTOS            */
/*********************************************/
// les vues preferences
#include "ColorsPrefs.h"
#include <support/List.h>

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
	container->AddItem(new ColorsPrefs(frame));
}


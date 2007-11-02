/*********************************************/
/* C'est l'exemple de code pour integrer des */
/* object proprietes dans MeTOS              */
/*********************************************/
#include <support/List.h>
#include "BOOL.h"
#include "BPNT.h"
#include "BYTE.h"
#include "CSTR.h"
#include "FLOT.h"
#include "LONG.h"
#include "MSGG.h"
#include "RECT.h"
#include "SHRT.h"

// on a besoin d'une metode externe
// qui retourne une liste de VPropertie
extern "C" __declspec(dllexport) void instantiate_VProperties(BList *container,entry_ref *entry);

/**** function de creation des objets pour l'architecture Addon ****/
void instantiate_VProperties(BList *container,entry_ref *entry)
{
	// verifier que le pointer est valide
	if(container==NULL)
		return;
	
	// ajouter les objets VPropertie
	container->AddItem(new BOOL(entry));
	container->AddItem(new BPNT(entry));
	container->AddItem(new BYTE(entry));
	container->AddItem(new CSTR(entry));
	container->AddItem(new FLOT(entry));
	container->AddItem(new LONG(entry));
	container->AddItem(new MSGG(entry));
	container->AddItem(new RECT(entry));
	container->AddItem(new SHRT(entry));
}

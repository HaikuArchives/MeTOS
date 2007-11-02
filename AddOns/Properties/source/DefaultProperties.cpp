/*********************************************/
/* C'est l'exemple de code pour integrer des */
/* object proprietes dans MeTOS              */
/*********************************************/
// les vues preferences
#include <support/List.h>
#include <storage/Entry.h>
#include "_color.h"
#include "_flags.h"
#include "_fname.h"
#include "_frame.h"
#include "_layout.h"
#include "_optimize.h"
#include "_path.h"
#include "_resize_mode.h"
#include "_resource.h"
#include "_style.h"
#include "_type_binary.h"
#include "_val.h"
#include "_wfeel.h"
#include "_wlook.h"

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
	container->AddItem(new _Color(entry));
	container->AddItem(new _Flags(entry));
	container->AddItem(new _Fname(entry));
	container->AddItem(new _Frame(entry));
	container->AddItem(new _Layout(entry));	
	container->AddItem(new _Path(entry));
	container->AddItem(new _Optimize(entry));
	container->AddItem(new _ResizeMode(entry));
	container->AddItem(new _Resource(entry));
	container->AddItem(new _Style(entry));
	container->AddItem(new _TypeBinary(entry));	
	container->AddItem(new _Val(entry));
	container->AddItem(new _Wfeel(entry));
	container->AddItem(new _Wlook(entry));
}

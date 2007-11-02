// =================================
// = Preferences de la mise a jour =
// =================================
#include "UpgraderPrefs.h"
#include "MetosMessageDefs.h"
#include "MetosGlobalDefs.h"
#include "VPreferenceFile.h"
#include "VLanguage.h"

#include <Box.h>
#include <TextControl.h>
#include <Window.h>
#include <String.h>

/**** constructeur ****/
UpgraderPrefs::UpgraderPrefs(BRect frame)
: VPreference(frame,"Upgrade",METOS_GROUP)
{
	BRect				bounds = Bounds();
	BString				buffer;
	BBox				*box = NULL;

	// decallage a cause du titre
	bounds.top += VPREFERENCE_TITLE_HIGH;
	bounds.InsetBy(7,7);

	// le bloc principale
	bounds.bottom = bounds.top + 70;
	box = new BBox(bounds,"upgrader");
	box->SetLabel(_language->GetTranslationFor("PREFERENCE_NAME"));

	// url et script de mise a jour
	_upgraderURL = new BTextControl(BRect(5,20,320,34),"upgrader-URL",_language->GetTranslationFor("UPGRADER_URL"),"",NULL);
	_upgraderURL->SetDivider(25);
	_upgraderScript = new BTextControl(BRect(5,42,320,56),"upgrader-Script",_language->GetTranslationFor("UPGRADER_SCRIPT"),"",NULL);
	_upgraderScript->SetDivider(70);

	// fixer les parametres
	_upgraderURL->SetText((g_MeTOSPrefs->GetString(METOS_UPGRADER_URL,"upgrader-url")).String());
	_upgraderScript->SetText((g_MeTOSPrefs->GetString(METOS_UPGRADER_SCRIPT,"upgrader-script")).String());

	box->AddChild(_upgraderURL);
	box->AddChild(_upgraderScript);
	AddChild(box);
}

/**** sauver les preferences ****/
void UpgraderPrefs::SavePrefs()
{
	// relire avant
	g_MeTOSPrefs->Load();
	
	// sauver les parametres
	// gui-manager
	g_MeTOSPrefs->SetString(_upgraderURL->Text(),"upgrader-url");
	g_MeTOSPrefs->SetString(_upgraderScript->Text(),"upgrader-script");

	// sauver
	g_MeTOSPrefs->Save();
}

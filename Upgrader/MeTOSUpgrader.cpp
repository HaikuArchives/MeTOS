/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/* Upgrader is based on source-code given by   */
/* Marco Nelissen (all right reserved)         */
/***********************************************/
#include <stdio.h>
#include <Application.h>
#include "Upgrader.h"

/**** fonction main ****/
int main(int argc, char *argv[])
{
	int		versionSize = 0;
	int		scriptUrlSize = 0;

	// on doit avoir la version en parametre
	if(argc<2)
		return 0;

	// on va verifier que la version fait 4 caractere
	versionSize = strlen(argv[1]);
	scriptUrlSize = strlen(argv[2]);
	if(versionSize!=4 || scriptUrlSize<=1)
		return 0;
	
	// creation d'une beapp necessaire
	BApplication app("application/x-vnd.CKJ-MeTOS-upgrader");

	// mettre la version actuelle de MeTOS (prise par l'exe en parametre)
	// puis le script d'upgrade
	DoUpgrade(argv[1],argv[2]);
}

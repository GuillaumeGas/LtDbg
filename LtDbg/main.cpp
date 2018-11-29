#include <iostream>
#include <exception>

#include "Exceptions.hpp"
#include "Dbg.hpp"

#define DEFAULT_KERNEL_IMAGE_PATH "C:\\Users\\Guillaume\\Documents\\Visual Studio 2017\\Projects\\ltkernel\\iso\\boot\\ltkernel.img"

/*
	- p : Mode pas � pas, affiche la prochaine instruction � ex�cuter
	- c : Continue l'ex�cution
	- r : Affiche le contenu des registres
	- d [taille] : Affiche le code assembleur, sur taille octets
		Dans tous les cas on indique par une fl�che la prochaine instruction qui sera ex�cut�e.
		La command 'n' permet, quand elle a �t� pr�c�d�e d'un 'df', de demander la suite du code asm
    - m addr [taille] : Par d�fault, affiche sous forme hexa 50 octets.
		Avec param, affiche les 'taille' octets.
	- df : disassemble function, d�sassemble depuis le d�but par block de 20 inst par exemple
	    La command 'n' permet, quand elle a �t� pr�c�d�e d'un 'df', de demander la suite du code asm
*/

int main(int argc, char ** argv)
{
	bool error = false;

	try
	{
		Dbg dbg;
		if (argc > 1)
			dbg.Start(argv[1]);
		else
			dbg.Start(DEFAULT_KERNEL_IMAGE_PATH);
	}
	catch (const DbgException & exc)
	{
		std::cout << exc.ToString() << std::endl;
		error = true;
	}
	catch (const std::exception & exc)
	{
		std::cout << "[!] Dbg unhandled exception : " << exc.what() << std::endl;
		error = true;
	}

	if (error)
	{
		std::cout << "Press Enter to exit..." << std::endl;
		getchar();
		return -1;
	}
	return 0;
}
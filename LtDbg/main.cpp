#include <iostream>
#include <exception>

#include "Exceptions.hpp"
#include "Dbg.hpp"
#include "Response.hpp"
#include "LtKinc/ltkinc.h"

#define DEFAULT_KERNEL_IMAGE_PATH "C:\\Users\\Guillaume\\Documents\\Visual Studio 2017\\Projects\\LtMicros\\LtMicros\\iso\\boot\\ltmicros.img"
#define PIPE_NAME                 "\\\\.\\pipe\\ltdbgpipe"

/*
	- p : Mode pas à pas, affiche la prochaine instruction à exécuter
	- c : Continue l'exécution
	- r : Affiche le contenu des registres
	- d [taille] : Affiche le code assembleur, sur taille octets
		Dans tous les cas on indique par une flèche la prochaine instruction qui sera exécutée.
		La command 'n' permet, quand elle a été précédée d'un 'df', de demander la suite du code asm
    - m addr [taille] : Par défault, affiche sous forme hexa 50 octets.
		Avec param, affiche les 'taille' octets.

	- bp addr | symbolName : place un breakpoint en fonction d'une adresse, ou d'un nom de fonction
	- bd id : désactive un breakpoint
	- be id : active un breakpoint
	- bc id : supprime un breakpoint
	- bl    : renvoie la liste des breakpoints (couple id - addr) 
	- locals : va chercher les noms et valeurs des variables locales de la frame courante

	- df : disassemble function, désassemble depuis le début par block de 20 inst par exemple
	    La command 'n' permet, quand elle a été précédée d'un 'df', de demander la suite du code asm
*/

using namespace std;

static void HandleResponse(DbgResponsePtr res)
{
	switch (res->command)
	{
	case CMD_DISASS:
		cout << res->content << endl;
		break;
	default:
		cout << res->content << endl;
	}
}

static void CmdDebuggerRoutine(Dbg & dbg, KeDebugContext * firstContext)
{
	string lastInput = "";
	KeDebugContext * context = firstContext;

	do
	{
		string input = "";
		DbgResponsePtr res;

		cout << "kd> ";
		getline(cin, input);

		if (input == "")
		{
			res = dbg.ExecuteCommand(lastInput, context);
		}
		else
		{
			res = dbg.ExecuteCommand(input);
			lastInput = input;
		}

		HandleResponse(res);
		context = res->context;
	} while (dbg.IsConnected());
}

int main(int argc, char ** argv)
{
	bool error = false;

	try
	{
		Dbg dbg;

		if (argc >= 2)
			dbg.Connect(argv[1]);
		else 
			dbg.Connect(PIPE_NAME);

		if (argc > 2)
			dbg.SetSymbolsPath(argv[2]);
		else
			dbg.SetSymbolsPath(DEFAULT_KERNEL_IMAGE_PATH);

		DbgResponsePtr res = dbg.ExecuteCommand(CMD_CONNECT);
		if (res->status != DBG_STATUS_SUCCESS)
		{
			std::cout << "Connection failed with LtKernel ! (Connect command returned " << res->status << std::endl;
			return 0;
		}

		cout << ">> LtKernel Debugger <<" << endl << endl;

		CmdDebuggerRoutine(dbg, res->context);
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
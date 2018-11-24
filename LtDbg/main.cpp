#include <iostream>
#include <exception>

#include "Exceptions.hpp"
#include "Dbg.hpp"

#define DEFAULT_KERNEL_IMAGE_PATH "C:\\Users\\Guillaume\\Documents\\Visual Studio 2017\\Projects\\ltkernel\\iso\\boot\\ltkernel.img"

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

	std::cout << "Press Enter to exit..." << std::endl;
	getchar();

	if (error)
		return -1;
	return 0;
}
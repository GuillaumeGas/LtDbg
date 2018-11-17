#include <iostream>
#include <exception>
#include "Exceptions.hpp"
#include "Dbg.hpp"

int main()
{
	bool error = false;

	try
	{
		Dbg dbg;
		dbg.Start();
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
#include <iostream>
#include "sftri_type.h"

int main()
{
	sftri_type<bool> b(false);
	std::cout << bool(b) << std::endl;
	std::cout << *b << std::endl;
	b = false;
	std::cout << bool(b) << std::endl;
	std::cout << *b << std::endl;
}

#include <iostream>
#include "sf_tri_type.h"


int main()
{
	skyfire::sf_tri_type<bool> b(false);
	std::cout << bool(b) << std::endl;
	std::cout << *b << std::endl;
	b = false;
	std::cout << bool(b) << std::endl;
	std::cout << *b << std::endl;
}

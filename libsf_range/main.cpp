#include "sf_range.h"
#include <iostream>

using namespace skyfire;
using namespace std;

int main()
{
	for (auto p : range(5))
	{
		cout << p << endl;
	}
	cout << "----------" << endl;
	for(auto p:range(9,0,-2))
	{
		cout << p << endl;
	}
	cout << "----------" << endl;
	for(auto p:range(2,6))
	{
		cout << p << endl;
	}
}
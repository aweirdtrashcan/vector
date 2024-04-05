#include "vector.h"
#include <iostream>

int main(void) {
	vector<std::string> v;

	v.push_back("Hello");
	v.push_back("Dear");
	v.push_back("C++");
	v.push_back("World");

	v[2] = "C+++";

	for (const std::string& a : v) {
		std::cout << a << "\n";
	}
}
#include "vector.h"
#include <iostream>

void printlist(list<const char*> list) {
	for (const auto& s : list) {
		std::cout << s << "\n";
	}
}

int main(void) {
	list<const char*> v(150);

	v.push_back("Hello World");
	v.push_back("Hello World");
	v.push_back("Hello World");
	v.push_back("Hello World");
	v.push_back("Hello World");
	v.push_back("Hello World");
	v.push_back("Hello World");
	v.push_back("Hello World");
	v.push_back("Hello World");
	v.push_back("Hello World");
	v.push_back("Hello World");
	v.push_back("Hello World");
	v.push_back("Hello World");
	v.push_back("Hello World");
	v.push_back("Hello World");
	v.push_back("Hello World");
	v.push_back("Hello World");

	printlist(v);
}
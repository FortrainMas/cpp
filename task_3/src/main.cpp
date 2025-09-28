#include <cstdlib>
#include <iostream>

#include "regex/regex.hpp"

int main(int arc, char** argv){
	std::string reg_string;
	std::cin >> reg_string;
	Regex regex(reg_string);

	std::string str;
	while(true){
		std::cin >> str;

		if (regex.match(str)) {
			std::cout << "true" << std::endl;
		} else {
			std::cout << "false" << std::endl;
		}
	}
}

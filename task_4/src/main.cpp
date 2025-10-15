#include <iostream>
#include <sstream>
#include <string>

#include "converter/Converter.hpp"
#include "converter/Convertible.hpp"

int main(int arc, char** argv){
    std::string left = argv[1];
    std::string right = argv[2];

    std::ostringstream buffer;
    buffer << std::cin.rdbuf();
    std::string content = buffer.str(); 

    std::cout << "XML2TOML" << std::endl;
    std::cout << Converter<XMLConverter, JSONConverter>::convert(content) << std::endl;
    return 0;
}

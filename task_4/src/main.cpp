#include <iostream>
#include <sstream>
#include <string>
#include <functional>

#include "converter/Converter.hpp"
#include "converter/Convertible.hpp"

int main(int argc, char** argv){
    std::string left = argv[1];
    std::string right = argv[2];

    std::ostringstream buffer;
    buffer << std::cin.rdbuf();
    std::string content = buffer.str(); 

    using ConvFunc = std::function<std::string(const std::string&)>;
    static const std::map<std::pair<std::string, std::string>, ConvFunc> converters = {
        {{"xml", "json"}, [](const std::string& s){ return Converter<XMLConverter, JSONConverter>::convert(s); }},
        {{"xml", "toml"}, [](const std::string& s){ return Converter<XMLConverter, TOMLConverter>::convert(s); }},
        {{"json", "toml"}, [](const std::string& s){ return Converter<JSONConverter, TOMLConverter>::convert(s); }},
        {{"toml", "json"}, [](const std::string& s){ return Converter<TOMLConverter, JSONConverter>::convert(s); }},
        {{"json", "xml"},  [](const std::string& s){ return Converter<JSONConverter, XMLConverter>::convert(s); }},
    };

    auto it = converters.find({left, right});
    if (it == converters.end()) {
        std::cerr << "Unsupported conversion: " << left << " -> " << right << std::endl;
        return 1;
    }

    std::cout << it->second(content) << std::endl;

    return 0;
}

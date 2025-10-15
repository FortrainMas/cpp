#pragma once
#include "Convertible.hpp"

template<Convertible From, Convertible To>
struct Converter {
    static std::string convert(const std::string& text){
        return To::from_ast(From::to_ast(text));
    }
};

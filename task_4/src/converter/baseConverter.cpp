#include <string>
#include <memory>
#include <variant>
#include <vector>
#include <map>
#include <cctype>
#include <cstring>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <iostream>

#include "converter/AST.hpp"
#include "converter/Convertible.hpp"

void BaseConverter::skip_ws(const char*& p) {
    while (*p && std::isspace(static_cast<unsigned char>(*p))) ++p;
}

std::string BaseConverter::parse_string(const char*& p) {
    if (*p != '"') throw std::runtime_error("Expected string");
    ++p;
    std::string s;
    while (*p && *p != '"') s.push_back(*p++);
    if (*p != '"') throw std::runtime_error("Unterminated string");
    ++p;
    return s;
}

static int hex_digit_value(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return 10 + (c - 'a');
    if (c >= 'A' && c <= 'F') return 10 + (c - 'A');
    return -1;
}


double BaseConverter::parse_number(const char*& p) {
    const char* start = p;
    if (*p == '-') ++p;
    if (!std::isdigit(static_cast<unsigned char>(*p))) throw std::runtime_error("Invalid number");
    while (std::isdigit(static_cast<unsigned char>(*p))) ++p;
    if (*p == '.') {
        ++p;
        if (!std::isdigit(static_cast<unsigned char>(*p))) throw std::runtime_error("Invalid number (frac)");
        while (std::isdigit(static_cast<unsigned char>(*p))) ++p;
    }
    if (*p == 'e' || *p == 'E') {
        ++p;
        if (*p == '+' || *p == '-') ++p;
        if (!std::isdigit(static_cast<unsigned char>(*p))) throw std::runtime_error("Invalid number (exp)");
        while (std::isdigit(static_cast<unsigned char>(*p))) ++p;
    }
    std::string numstr(start, p - start);
    try {
        return std::stod(numstr);
    } catch (...) {
        throw std::runtime_error("Failed to convert number");
    }
}

std::string BaseConverter::escape_string(const std::string& s) {
    std::ostringstream o;
    for (unsigned char c : s) {
        switch (c) {
            case '"': o << "\\\""; break;
            case '\\': o << "\\\\"; break;
            case '\b': o << "\\b"; break;
            case '\f': o << "\\f"; break;
            case '\n': o << "\\n"; break;
            case '\r': o << "\\r"; break;
            case '\t': o << "\\t"; break;
            default:
                if (c < 0x20) {
                    o << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)c << std::dec;
                } else {
                    o << c;
                }
        }
    }
    return o.str();
}


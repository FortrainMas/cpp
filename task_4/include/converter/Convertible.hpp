#pragma once
#include <concepts>
#include <memory>
#include <string>

#include <cctype>
#include <cstring>
#include <string>
#include <memory>
#include <variant>
#include <vector>
#include <map>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <functional>

#include "converter/AST.hpp"

template <typename T>
concept Convertible = requires(T a, const std::string& text, const AST& ast) 
{ 
    { a.to_ast(text) } -> std::same_as<AST>;
    { a.from_ast(ast) } -> std::same_as<std::string>;
};

class BaseConverter {
protected:
    static void skip_ws(const char*& p);
    static std::string parse_string(const char *& p);
    static double parse_number(const char *& p);
    static std::string escape_string(const std::string& s);
};

class JSONConverter : BaseConverter {
public:
    static AST to_ast(const std::string& text);
    static std::string from_ast(const AST& ast);

private:
    static AST parse_value(const char*& p);
    static AST parse_object(const char*& p);
    static AST parse_array(const char*& p);
    static AST parse_literal(const char*& p, const char* lit, ASTNode::Value val);

    static void serialize_value(const AST& node, std::ostringstream& out);
};

class XMLConverter {
public:
    static AST to_ast(const std::string& text);
    static std::string from_ast(const AST& ast);
private:
    static void skip_ws(const char*& p);
    static std::string parse_tag_name(const char*& p);
    static std::string parse_text(const char*& p);
};

class TOMLConverter {
public:
    static AST to_ast(const std::string& text);
    static std::string from_ast(const AST& ast);

private:
    static void skip_ws(const char*& p) {
        while (*p && std::isspace(static_cast<unsigned char>(*p))) ++p;
    }

    static std::string parse_key(const char*& p) {
        skip_ws(p);
        const char* start = p;
        while (*p && (std::isalnum(static_cast<unsigned char>(*p)) || *p == '_' || *p == '-')) ++p;
        if (p == start) throw std::runtime_error("Expected key");
        return std::string(start, p);
    }

    static AST parse_value(const char*& p) {
        skip_ws(p);
        if (!*p) throw std::runtime_error("Unexpected end of input");

        if (*p == '"') return std::make_shared<ASTNode>(parse_string(p));
        if (*p == '[') return parse_array(p);
        if (std::strncmp(p, "true", 4) == 0) { p += 4; return std::make_shared<ASTNode>(true); }
        if (std::strncmp(p, "false", 5) == 0) { p += 5; return std::make_shared<ASTNode>(false); }
        if (*p == '-' || std::isdigit(static_cast<unsigned char>(*p))) return std::make_shared<ASTNode>(parse_number(p));

        throw std::runtime_error("Unknown TOML value");
    }

    static std::string parse_string(const char*& p) {
        if (*p != '"') throw std::runtime_error("Expected '\"'");
        ++p;
        std::string result;
        while (*p && *p != '"') {
            if (*p == '\\') {
                ++p;
                if (*p == 'n') result.push_back('\n');
                else if (*p == 't') result.push_back('\t');
                else result.push_back(*p);
            } else {
                result.push_back(*p);
            }
            ++p;
        }
        if (*p != '"') throw std::runtime_error("Unterminated string");
        ++p;
        return result;
    }

    static double parse_number(const char*& p) {
        char* end = nullptr;
        double val = std::strtod(p, &end);
        if (p == end) throw std::runtime_error("Invalid number");
        p = end;
        return val;
    }

    static AST parse_array(const char*& p) {
        ++p; // skip '['
        ASTNode::Array arr;
        skip_ws(p);
        if (*p == ']') { ++p; return std::make_shared<ASTNode>(arr); }

        while (true) {
            AST val = parse_value(p);
            arr.push_back(val);
            skip_ws(p);
            if (*p == ']') { ++p; break; }
            if (*p != ',') throw std::runtime_error("Expected ',' in array");
            ++p;
        }

        return std::make_shared<ASTNode>(std::move(arr));
    }
};

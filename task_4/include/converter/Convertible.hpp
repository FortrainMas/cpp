#pragma once
#include <concepts>
#include <memory>
#include <string>

#include "converter/AST.hpp"

template <typename T>
concept Convertible = requires(T a, const std::string& text, const AST& ast) 
{ 
    { a.to_ast(text) } -> std::same_as<AST>;
    { a.from_ast(ast) } -> std::same_as<std::string>;
};

class BaseConverter {
protected:
    static double parse_number(const char *& p);
    static std::string parse_string(const char *& p);
    static std::string escape_string(const std::string& s);
    static void skip_ws(const char*& p);
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

class XMLConverter : BaseConverter {
public:
    static AST to_ast(const std::string& text);
    static std::string from_ast(const AST& ast);
private:
    static std::string parse_tag_name(const char*& p);
    static std::string parse_text(const char*& p);
};

class TOMLConverter {
public:
    static AST to_ast(const std::string& text);
    static std::string from_ast(const AST& ast);

private:
    static bool parse_bool(const char*& p);
    static double parse_number(const char *& p);
    static std::string parse_string(const char *& p);
    static std::string parse_key(const char*& p);
    static void skip_ws(const char*& p);
    static void write_obj(std::ostringstream& out, const AST& obj, const std::string& prefix);
    static AST parse_inline_object(const char*& p);
    static AST parse_value(const char*& p);
    static AST parse_array(const char*& p);
};

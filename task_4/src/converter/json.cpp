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


AST JSONConverter::parse_value(const char*& p) {
    skip_ws(p);
    if (*p == '{') return parse_object(p);
    if (*p == '[') return parse_array(p);
    if (*p == '"') {
        std::string s = parse_string(p);
        return std::make_shared<ASTNode>(std::move(s));
    }
    if (*p == '-' || std::isdigit(static_cast<unsigned char>(*p))) {
        double d = parse_number(p);
        return std::make_shared<ASTNode>(d);
    }
    if (std::strncmp(p, "true", 4) == 0) return parse_literal(p, "true", true);
    if (std::strncmp(p, "false", 5) == 0) return parse_literal(p, "false", false);
    if (std::strncmp(p, "null", 4) == 0) return parse_literal(p, "null", nullptr);
    throw std::runtime_error(std::string("Unexpected character in JSON: '") + (*p ? *p : '?') + "'");
}

AST JSONConverter::to_ast(const std::string& text) {
    const char* p = text.c_str();
    skip_ws(p);
    AST root = parse_value(p);
    skip_ws(p);
    if (*p != '\0') throw std::runtime_error("Trailing characters after JSON value");
    return root;
}



AST JSONConverter::parse_object(const char*& p) {
    if (*p != '{') throw std::runtime_error("Expected '{' at object start");
    ++p; 
    skip_ws(p);
    ASTNode::Object obj;

    if (*p == '}') { ++p; return std::make_shared<ASTNode>(obj); }

    while (true) {
        skip_ws(p);
        if (*p != '"') throw std::runtime_error("Expected string key for object");
        std::string key = parse_string(p);
        skip_ws(p);
        if (*p != ':') throw std::runtime_error("Expected ':' after key in object");
        ++p;
        skip_ws(p);
        AST value = parse_value(p);
        obj.emplace(std::move(key), value);
        skip_ws(p);
        if (*p == '}') { ++p; break; }
        if (*p != ',') throw std::runtime_error("Expected ',' between object members");
        ++p;
    }

    return std::make_shared<ASTNode>(std::move(obj));
}

AST JSONConverter::parse_array(const char*& p) {
    if (*p != '[') throw std::runtime_error("Expected '[' at array start");
    ++p;
    skip_ws(p);
    ASTNode::Array arr;

    if (*p == ']') { ++p; return std::make_shared<ASTNode>(arr); }

    while (true) {
        skip_ws(p);
        AST elem = parse_value(p);
        arr.push_back(elem);
        skip_ws(p);
        if (*p == ']') { ++p; break; }
        if (*p != ',') throw std::runtime_error("Expected ',' between array elements");
        ++p;
    }

    return std::make_shared<ASTNode>(std::move(arr));
}



AST JSONConverter::parse_literal(const char*& p, const char* lit, ASTNode::Value val) {
    size_t len = std::strlen(lit);
    if (std::strncmp(p, lit, len) != 0) throw std::runtime_error(std::string("Expected literal '") + lit + "'");
    p += len;
    return std::make_shared<ASTNode>(std::move(val));
}


void JSONConverter::serialize_value(const AST& node, std::ostringstream& out) {
    if (!node) { out << "null"; return; }
    std::visit([&](auto&& v) {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, std::nullptr_t>) {
            out << "null";
        } else if constexpr (std::is_same_v<T, bool>) {
            out << (v ? "true" : "false");
        } else if constexpr (std::is_same_v<T, double>) {
            out << v;
        } else if constexpr (std::is_same_v<T, std::string>) {
            out << '"' << escape_string(v) << '"';
        } else if constexpr (std::is_same_v<T, ASTNode::Array>) {
            out << '[';
            bool first = true;
            for (const auto& el : v) {
                if (!first) out << ',';
                first = false;
                serialize_value(el, out);
            }
            out << ']';
        } else if constexpr (std::is_same_v<T, ASTNode::Object>) {
            out << '{';
            bool first = true;
            for (const auto& kv : v) {
                if (!first) out << ',';
                first = false;
                out << '"' << escape_string(kv.first) << "\":";
                serialize_value(kv.second, out);
            }
            out << '}';
        } else {
            throw std::runtime_error("Non exhaustive visitor");
        }
    }, node->value);
}

std::string JSONConverter::from_ast(const AST& ast) {
    std::ostringstream out;
    serialize_value(ast, out);
    return out.str();
}

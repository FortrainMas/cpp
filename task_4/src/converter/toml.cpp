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
#include "converter/Convertible.hpp"


std::string TOMLConverter::parse_key(const char*& p) {
    skip_ws(p);
    const char* start = p;
    while (*p && (std::isalnum(static_cast<unsigned char>(*p)) || *p == '_' || *p == '-')) ++p;
    if (p == start) throw std::runtime_error("Expected key");
    return std::string(start, p);
}

std::string TOMLConverter::parse_string(const char*& p) {
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

double TOMLConverter::parse_number(const char*& p) {
    char* end = nullptr;
    double val = std::strtod(p, &end);
    if (p == end) throw std::runtime_error("Invalid number");
    p = end;
    return val;
}

void TOMLConverter::skip_ws(const char*& p) {
    while (*p && std::isspace(static_cast<unsigned char>(*p))) ++p;
}

bool TOMLConverter::parse_bool(const char*& p) {
    if (std::strncmp(p, "true", 4) == 0) { p += 4; return true; }
    if (std::strncmp(p, "false", 5) == 0) { p += 5; return false; }
    throw std::runtime_error("Invalid boolean");
}

AST TOMLConverter::parse_value(const char*& p) {
    skip_ws(p);
    if (!*p) throw std::runtime_error("Unexpected end of input");

    if (*p == '"') return std::make_shared<ASTNode>(parse_string(p));
    if (*p == '[') return parse_array(p);
    if (*p == '{') return parse_inline_object(p);
    if (std::strncmp(p, "true", 4) == 0 || std::strncmp(p, "false", 5) == 0)
        return std::make_shared<ASTNode>(parse_bool(p));
    if (*p == '-' || std::isdigit(static_cast<unsigned char>(*p)))
        return std::make_shared<ASTNode>(parse_number(p));

    throw std::runtime_error("Unknown TOML value");
}

AST TOMLConverter::parse_inline_object(const char*& p) {
    ++p;
    ASTNode::Object obj;
    skip_ws(p);

    if (*p == '}') { ++p; return std::make_shared<ASTNode>(obj); }

    while (true) {
        std::string key = parse_key(p);
        skip_ws(p);
        if (*p != '=') throw std::runtime_error("Expected '=' in inline object");
        ++p;
        AST val = parse_value(p);
        obj[key] = val;

        skip_ws(p);
        if (*p == '}') { ++p; break; }
        if (*p != ',') throw std::runtime_error("Expected ',' in inline object");
        ++p;
    }

    return std::make_shared<ASTNode>(std::move(obj));
}


AST TOMLConverter::parse_array(const char*& p) {
    ++p;
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


AST TOMLConverter::to_ast(const std::string& text) {
    const char* p = text.c_str();
    ASTNode::Object root;
    std::string current_section;

    auto get_section = [&](ASTNode::Object& obj, const std::string& dotted) -> ASTNode::Object& {
        const char* s = dotted.c_str();
        const char* start = s;
        ASTNode::Object* current = &obj;

        while (*s) {
            if (*s == '.') {
                std::string part(start, s);
                if (!current->count(part)) {
                    (*current)[part] = std::make_shared<ASTNode>(ASTNode::Object{});
                }
                current = &std::get<ASTNode::Object>((*current)[part]->value);
                ++s;
                start = s;
            } else {
                ++s;
            }
        }

        std::string last(start, s);
        if (!current->count(last)) {
            (*current)[last] = std::make_shared<ASTNode>(ASTNode::Object{});
        }

        return std::get<ASTNode::Object>((*current)[last]->value);
    };

    while (*p) {
        skip_ws(p);
        if (!*p) break;
        if (*p == '#') {
            while (*p && *p != '\n') ++p;
            continue;
        }
        if (*p == '[') {
            ++p;
            const char* start = p;
            while (*p && *p != ']') ++p;
            if (*p != ']') throw std::runtime_error("Unterminated section header");
            current_section = std::string(start, p++);
            continue;
        }

        std::string key = parse_key(p);
        skip_ws(p);
        if (*p != '=') throw std::runtime_error("Expected '='");
        ++p;
        AST value = parse_value(p);

        ASTNode::Object* target = &root;
        if (!current_section.empty()) {
            target = &get_section(root, current_section);
        }

        const char* s = key.c_str();
        const char* start = s;
        ASTNode::Object* current = target;

        while (*s) {
            if (*s == '.') {
                std::string part(start, s);
                if (!current->count(part)) {
                    (*current)[part] = std::make_shared<ASTNode>(ASTNode::Object{});
                }
                current = &std::get<ASTNode::Object>((*current)[part]->value);
                ++s;
                start = s;
            } else {
                ++s;
            }
        }

        std::string last(start, s);
        (*current)[last] = value;

        while (*p && *p != '\n') ++p;
        if (*p == '\n') ++p;
    }

    return std::make_shared<ASTNode>(std::move(root));
}


void TOMLConverter::write_obj(std::ostringstream& out, const AST& node, const std::string& prefix) {
    std::visit([&](auto&& v) {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, ASTNode::Object>) {
            for (const auto& [k, val] : v) {
                if (!val) throw std::runtime_error("Null value in object");
                if (!std::holds_alternative<ASTNode::Object>(val->value))
                    out << k << " = " << from_ast(val) << "\n";
            }
            for (const auto& [k, val] : v) {
                if (!val) throw std::runtime_error("Null value in object");
                if (std::holds_alternative<ASTNode::Object>(val->value)) {
                    const auto& obj = std::get<ASTNode::Object>(val->value);
                    bool has_keys = false;
                    for (auto& [_, inner] : obj) if (inner) { has_keys = true; break; }
                    if (!has_keys) continue;
                    std::string new_prefix = prefix.empty() ? k : prefix + "." + k;
                    out << "\n[" << new_prefix << "]\n";
                    write_obj(out, val, new_prefix);
                }
            }
        } else if constexpr (std::is_same_v<T, ASTNode::Array>) {
            out << "[";
            bool first = true;
            for (const auto& el : v) {
                if (!el) throw std::runtime_error("Null value in array");
                if (!first) out << ", ";
                first = false;

                std::visit([&](auto&& val){
                    using U = std::decay_t<decltype(val)>;
                    if constexpr (std::is_same_v<U, ASTNode::Object>) {
                        out << "{";
                        bool inner_first = true;
                        for (auto& [k,vv] : val) {
                            if (!vv) throw std::runtime_error("Null in object inside array");
                            if (!inner_first) out << ", ";
                            inner_first = false;
                            out << k << " = " << from_ast(vv);
                        }
                        out << "}";
                    } else {
                        out << from_ast(el);
                    }
                }, el->value);
            }
            out << "]";
        } else if constexpr (std::is_same_v<T, std::string>) {
            out << '"' << v << '"';
        } else if constexpr (std::is_same_v<T, double>) {
            out << v;
        } else if constexpr (std::is_same_v<T, bool>) {
            out << (v ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::nullptr_t>) {
            throw std::runtime_error("Cannot serialize null to TOML");
        } else {
            throw std::runtime_error("Unknown type in ASTNode::Value");
        }
    }, node->value);
}


std::string TOMLConverter::from_ast(const AST& ast) {
    std::ostringstream out;
    TOMLConverter::write_obj(out, ast, "");
    return out.str();
}

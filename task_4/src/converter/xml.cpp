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
#include <iostream>
#include <functional>
#include <algorithm>

#include "converter/AST.hpp"
#include "converter/Converter.hpp"

void XMLConverter::skip_ws(const char*& p) {
    while (*p && std::isspace(static_cast<unsigned char>(*p))) ++p;
}

std::string XMLConverter::parse_tag_name(const char*& p) {
    const char* start = p;
    while (*p && (std::isalnum(static_cast<unsigned char>(*p)) || *p == '_' || *p == '-' || *p == '.'))
        ++p;
    if (p == start) throw std::runtime_error("Expected tag name");
    return std::string(start, p);
}

std::string XMLConverter::parse_text(const char*& p) {
    std::ostringstream out;
    while (*p && *p != '<') out.put(*p++);
    return out.str();
}

AST XMLConverter::to_ast(const std::string& text) {
    const char* p = text.c_str();

    std::function<std::pair<std::string, AST>(const char*&)> parse_node;

    parse_node = [&](const char*& p) -> std::pair<std::string, AST> {
        skip_ws(p);
        if (*p != '<') throw std::runtime_error("Expected '<'");

        ++p;
        if (*p == '/') throw std::runtime_error("Unexpected closing tag");

        std::string name = parse_tag_name(p);

        skip_ws(p);
        bool self_closing = false;
        if (*p == '/') { self_closing = true; ++p; skip_ws(p); }
        if (*p != '>') throw std::runtime_error("Expected '>'");
        ++p;

        if (self_closing)
            return {name, std::make_shared<ASTNode>(nullptr)};

        ASTNode::Object obj;
        std::string accumulated_text;

        while (true) {
            skip_ws(p);

            if (*p == '<' && *(p + 1) == '/') {
                p += 2;
                std::string close_name = parse_tag_name(p);
                if (close_name != name)
                    throw std::runtime_error("Mismatched closing tag: " + close_name + " != " + name);
                skip_ws(p);
                if (*p != '>') throw std::runtime_error("Expected '>' at closing tag");
                ++p;
                break;
            }

            if (*p == '<') {
                if (!accumulated_text.empty()) {
                    obj["#text"] = std::make_shared<ASTNode>(accumulated_text);
                    accumulated_text.clear();
                }

                auto [child_name, child_node] = parse_node(p);
                if (obj.count(child_name)) {
                    auto arr = std::get_if<ASTNode::Array>(&obj[child_name]->value);
                    if (arr) arr->push_back(child_node);
                    else obj[child_name] = std::make_shared<ASTNode>(
                        ASTNode::Array{obj[child_name], child_node});
                } else {
                    obj[child_name] = child_node;
                }
            } else {
                accumulated_text += parse_text(p);
            }
        }

        if (!obj.empty()) {
            return {name, std::make_shared<ASTNode>(obj)};
        } else {
            std::string trimmed = accumulated_text;
            trimmed.erase(trimmed.begin(), std::find_if(trimmed.begin(), trimmed.end(), [](unsigned char ch){ return !std::isspace(ch); }));
            trimmed.erase(std::find_if(trimmed.rbegin(), trimmed.rend(), [](unsigned char ch){ return !std::isspace(ch); }).base(), trimmed.end());
            return {name, std::make_shared<ASTNode>(trimmed)};
        }
    };

    auto [root_name, root_node] = parse_node(p);
    ASTNode::Object root_obj;
    root_obj[root_name] = root_node;
    return std::make_shared<ASTNode>(root_obj);
}

std::string XMLConverter::from_ast(const AST& ast) {
    std::ostringstream out;

    auto escape = [](const std::string& s) -> std::string {
        std::ostringstream oss;
        for (char c : s) {
            switch (c) {
                case '&': oss << "&amp;"; break;
                case '<': oss << "&lt;"; break;
                case '>': oss << "&gt;"; break;
                case '"': oss << "&quot;"; break;
                case '\'': oss << "&apos;"; break;
                default: oss << c;
            }
        }
        return oss.str();
    };

    std::function<void(const AST&, const std::string&)> write_xml;

    write_xml = [&](const AST& node, const std::string& name_) {
        std::string name = name_;
        std::replace(name.begin(), name.end(), ' ', '-');

        if (!node) return;

        std::visit([&](auto&& v) {
            using T = std::decay_t<decltype(v)>;

            if constexpr (std::is_same_v<T, ASTNode::Object>) {
                out << "<" << name << ">";
                for (const auto& [k, val] : v)
                    write_xml(val, k);
                out << "</" << name << ">";
            } else if constexpr (std::is_same_v<T, ASTNode::Array>) {
                for (const auto& el : v)
                    write_xml(el, name);
            } else if constexpr (std::is_same_v<T, std::string>) {
                out << "<" << name << ">" << escape(v) << "</" << name << ">";
            } else if constexpr (std::is_same_v<T, double>) {
                out << "<" << name << ">" << v << "</" << name << ">";
            } else if constexpr (std::is_same_v<T, bool>) {
                out << "<" << name << ">" << (v ? "true" : "false") << "</" << name << ">";
            } else if constexpr (std::is_same_v<T, std::nullptr_t>) {
                out << "<" << name << "/>";
            }
        }, node->value);
    };

    if (auto root = std::get_if<ASTNode::Object>(&ast->value)) {
        for (const auto& [k, v] : *root)
            write_xml(v, k);
    }

    return out.str();
}

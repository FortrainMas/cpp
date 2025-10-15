#pragma once
#include <string>
#include <vector>
#include <map>
#include <variant>

struct ASTNode;
using AST = std::shared_ptr<ASTNode>;

struct ASTNode {
    using Object = std::map<std::string, AST>;
    using Array = std::vector<AST>;
    using Value = std::variant<bool, double, std::string, std::nullptr_t, Object, Array>;

    Value value;

    ASTNode() = default;
    explicit ASTNode(Value value) : value(value) {}
};

#pragma once
#include <string>
#include <vector>
#include <map>
#include <variant>
#include <memory>

struct ASTNode;
using AST = std::shared_ptr<ASTNode>;

struct ASTNode {
    using Object = std::map<std::string, AST>;
    using Array = std::vector<AST>;
    using Value = std::variant<bool, double, std::string, std::nullptr_t, Object, Array>;

    Value value;

    ASTNode() = default;
    explicit ASTNode(Value value) : value(std::move(value)) {}

    bool operator==(const ASTNode& other) const {

        if (value.index() != other.value.index()) return false;

        return std::visit([&](auto&& av) -> bool {
            using T = std::decay_t<decltype(av)>;

            if constexpr (std::is_same_v<T, Object>) {
                const auto& bv = std::get<Object>(other.value);
                if (av.size() != bv.size()) return false;
                for (const auto& [k, v] : av) {
                    auto it = bv.find(k);
                    if (it == bv.end()) return false;
                    if (!v || !it->second) return false; // nullptr check
                    if (!(*v == *it->second)) return false;
                }
                return true;
            } else if constexpr (std::is_same_v<T, Array>) {
                const auto& bv = std::get<Array>(other.value);
                if (av.size() != bv.size()) return false;
                for (size_t i = 0; i < av.size(); ++i) {
                    if (!av[i] || !bv[i]) return false;       // проверка на nullptr
                    if (!(*av[i] == *bv[i])) return false;    // разыменовываем только av[i] и bv[i] один раз
                }
                return true;
            } else {
                const auto& bv = std::get<T>(other.value);
                return av == bv;
            }
        }, value);
    }
};

inline bool operator==(const AST& a, const AST& b) {
    if (!a && !b) return true;   // оба nullptr
    if (!a || !b) return false;  // один nullptr
    return *a == *b;             // безопасно разыменовываем
}

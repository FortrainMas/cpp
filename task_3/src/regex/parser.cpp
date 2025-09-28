#include <iostream>
#include <stdexcept>

#include "regex/parser.hpp"
#include "regex/nfa.hpp"


std::vector<std::string> ParserPartion::get_values() {
    if (this->string == ".") {
        std::vector<std::string> s;
        for (char c = 'a'; c <= 'z'; ++c) s.push_back(std::string(1, c));
        for (char c = 'A'; c <= 'Z'; ++c) s.push_back(std::string(1, c));
        for (char c = '0'; c <= '9'; ++c) s.push_back(std::string(1, c));
        s.push_back(std::string(1, ' '));
        return s;
    }

    if (this->string.front() == '[' && this->string.back() == ']') {
        std::vector<std::string> values;
        for (auto it = string.begin()+1; it != string.end()-1; ++it) {
            if (it != string.begin() && std::next(it) != string.end() && *it == '-') {
                char start = *(it - 1);
                char end   = *(it + 1);

                if (start < end) {
                    for (char c = start + 1; c < end; ++c) {
                        values.push_back(std::string(1, c));
                    }
                }
            } else {
                values.push_back(std::string(1, *it));
            }
        }
        return values;
    }

    std::vector<std::string> values;

    for(char c : string) {
        values.push_back(std::string(1, c));
    }

    return values;
}


Parser::Parser(const std::string& string) : string_(std::move(string)) {
    // for (char c : string_) {
    //     if (this->specials_.count(c) + this->letters_numbers_.count(c) == 0) {
    //         throw std::invalid_argument("Invalid regex");
    //     }
    // }
}

std::shared_ptr<NFA> Parser::parse() {
    std::vector<ParserPartion> parts = this->get_parts();
    
    std::shared_ptr<NFA> nfa = std::make_shared<NFA>();
    for (ParserPartion part : parts) {
        nfa->add_node(
            std::make_shared<NFANode>(
                part.get_values(), 
                part.modifier == ParserModifier::Star, 
                part.modifier == ParserModifier::Plus, 
                part.modifier == ParserModifier::Question)
        );
    }

    return nfa;
}

std::vector<ParserPartion> Parser::get_parts() {
    std::vector<ParserPartion> parts;

    auto l = string_.begin();
    auto r = l;

    bool inBrackets = *l == '[';
    ParserModifier modifier = ParserModifier::None;

    while (r != string_.end()) {
        r = std::next(r);

        switch (*r) {
            case '*': modifier = ParserModifier::Star; continue;
            case '?': modifier = ParserModifier::Question; continue;
            case '+': modifier = ParserModifier::Plus; continue;
            default: break;
        }

        if (!inBrackets) {
            auto local_r = r;
            if (modifier != ParserModifier::None) {
                local_r -= 1;
            }
            std::string prev_string(l, local_r);
            ParserPartion part(prev_string, modifier);
            parts.emplace_back(part);

            l = r;
            modifier = ParserModifier::None;
        }

        switch(*r) {
            case '[': inBrackets = true; continue;
            case ']': inBrackets = false; continue;
            default: break;
        }
    }

    if (l != string_.end()) {
        std::string prev_string(l, r);
        ParserPartion part(prev_string, modifier);
        parts.emplace_back(part);
    }

    return parts;
}

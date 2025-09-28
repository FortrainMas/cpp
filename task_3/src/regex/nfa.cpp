#include <iostream>
#include "regex/nfa.hpp"

NFA::NFA() {
    std::vector<std::string> empty;
    starting_node_ = std::make_shared<NFANode>(empty);
    finishing_node_ = starting_node_;
}

void NFA::add_node(std::shared_ptr<NFANode> node) {
    for (auto c : node -> values_) {
        
        auto it = finishing_node_->transitions_.find(c);
        if (it != finishing_node_->transitions_.end() &&
            it->second.final_) {
            continue;
        }

        NFATransition transition = NFATransition(node, false);
        finishing_node_->transitions_.insert_or_assign(c, transition);
    }

    if (node->star_modifier_) {
        finishing_node_->transitions_.insert_or_assign("eps", NFATransition(node, false));
        
        for (auto c : node->values_) {
            node->transitions_.insert_or_assign(c, NFATransition(node, true));
        }
    }

    if (node->plus_modifier_) {
        for (auto c : node->values_) {
            node->transitions_.insert_or_assign(c, NFATransition(node, true));
        }
    }

    if (node->question_modifier_) {
        finishing_node_->transitions_.insert_or_assign("eps", NFATransition(node, false));
    }

    finishing_node_ = node;
}

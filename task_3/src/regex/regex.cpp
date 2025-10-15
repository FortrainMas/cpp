#include <iostream>

#include "regex/nfa.hpp"
#include "regex/regex.hpp"
#include "regex/parser.hpp"

Regex::Regex(const std::string& str) {
	nfa_ = Parser(str).parse();
}

bool Regex::match(const std::string &string) {
	std::shared_ptr<NFANode> starting_node_ = this->nfa_->starting_node_;	

	for (char c : string) {
		bool failed = false;
		do {
			auto it = starting_node_->transitions_.find(std::string(1, c));
			if (it != starting_node_->transitions_.end()) {
				starting_node_ = it->second.node_;
				break;
			} 

			it = starting_node_->transitions_.find(std::string("eps"));
			if (it != starting_node_->transitions_.end()) {
				starting_node_ = it->second.node_;
			} else {
				return failed;
			}

		} while (true);
	}

	auto it = starting_node_->transitions_.find("eps");
	while (it != starting_node_->transitions_.end()) {
	    starting_node_ = it->second.node_;
	    it = starting_node_->transitions_.find("eps");
	}

	return starting_node_ == this->nfa_->finishing_node_;
}

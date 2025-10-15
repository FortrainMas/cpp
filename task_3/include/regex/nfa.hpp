#include <unordered_map>
#include <string>
#include <vector>
#include <memory>

class NFANode;

class NFATransition
{
public:
    NFATransition(std::shared_ptr<NFANode> node, bool final) 
        : node_(node), final_(final) {};
    std::shared_ptr<NFANode> node_;
    bool final_;
};

class NFANode
{
public:
    NFANode(std::vector<std::string> values,
            bool star_modifier = false,
            bool plus_modifier = false,
            bool question_modifier = false)
        : values_(std::move(values)),
          star_modifier_(star_modifier),
          plus_modifier_(plus_modifier),
          question_modifier_(question_modifier),
          transitions_{} {};

    std::vector<std::string> values_;
    std::unordered_map<std::string, NFATransition> transitions_;

    bool star_modifier_;
    bool plus_modifier_;
    bool question_modifier_;
};

// namespace detail {
// class NfaNodeImpl;
// }
// using NfaNode = const detail::NfaNodeImpl;

class NFA {
public:
    NFA();
    std::shared_ptr<NFANode> starting_node_;
    std::shared_ptr<NFANode> finishing_node_;
    void add_node(std::shared_ptr<NFANode> node);
};

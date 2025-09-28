#include <ostream>
#include <string>
#include <vector>
#include <memory>
#include <set>

class NFA;

enum class ParserModifier 
{
    Star,
    Question,
    Plus,
    None
};

class ParserPartion
{
public:
    ParserPartion(const std::string& string, ParserModifier modifier) : string(string), modifier(modifier) {};
    std::string string;
    ParserModifier modifier;

    std::vector<std::string> get_values();

    friend std::ostream& operator<<(std::ostream& os, const ParserPartion& p) {
        static const char* names[] = { "Star", "Question", "Plus", "None" };
        return os << "Value: " << p.string << ", Modifier: " << names[static_cast<int>(p.modifier)];
    }
};

class Parser
{
public:
    Parser(const std::string& string);
    std::shared_ptr<NFA> parse();
    const std::string& string_;
    std::vector<ParserPartion> get_parts();
private:
    static inline const std::set<char> specials_ {'*', '?', '+', '.', '[', ']'};

    static inline const std::set<char> letters_numbers_ = [](){
        std::set<char> s;
        s.insert(' ');
        for (char c = 'a'; c <= 'z'; ++c) s.insert(c);
        for (char c = 'A'; c <= 'Z'; ++c) s.insert(c);
        for (char c = '0'; c <= '9'; ++c) s.insert(c);
        return s;
    }();

    static inline const std::set<char> split_up_chars_ = [](){
        std::set<char> s;
        s.insert('.');
        s.insert(' ');
        s.insert('[');
        for (char c = 'a'; c <= 'z'; ++c) s.insert(c);
        for (char c = 'A'; c <= 'Z'; ++c) s.insert(c);
        for (char c = '0'; c <= '9'; ++c) s.insert(c);
        return s;
    }();
};

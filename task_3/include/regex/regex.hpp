#include <string>
#include <memory>

class NFA;

class Regex 
{
public:
	explicit Regex(const std::string& string);
	bool match(const std::string& string);
private:
	std::shared_ptr<NFA> nfa_;
};

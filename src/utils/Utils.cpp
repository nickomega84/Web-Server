#include "../../include/utils/Utils.hpp"
#include <sstream>
#include <map>

std::string Utils::intToString(int value) {
	std::ostringstream oss;
	oss << value;
	return oss.str();
}


#include "../../include/utils/Utils.hpp"
#include <sstream>

std::string Utils::intToString(int value) {
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

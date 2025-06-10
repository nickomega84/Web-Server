#include "../../include/utils/Utils.hpp"
#include <sstream>
#include <map>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>


std::string Utils::intToString(int value) {
	std::ostringstream oss;
	oss << value;
	return oss.str();
}


std::string Utils::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

bool strToBool(const std::string& str) {
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    return (lower == "true" || lower == "on" || lower == "yes" || lower == "1");
}
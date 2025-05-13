#include "../../include/utils/ErrorPages.hpp"

const char* getErrorPagePath(int code) {
	for (int i = 0; errorPages[i].code != -1; ++i) {
		if (errorPages[i].code == code)
			return errorPages[i].file;
	}
	return NULL;
}

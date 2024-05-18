#include "utils.hpp"

bool utils::isDirectory(const std::string &path) {
	struct stat buf;
	if (stat(path.c_str(), &buf) == -1)
		return false;
	return S_ISDIR(buf.st_mode);
}

bool utils::isFile(const std::string &s) {
	struct stat buf;
	if (stat(s.c_str(), &buf) == -1)
		return false;
	return S_ISREG(buf.st_mode);
}

bool utils::pathExists(std::string path) {
	struct stat buf;
	return stat(path.c_str(), &buf) == 0;
}

std::string utils::getFile(std::string path) {
	std::ifstream ifs;
	std::stringstream buf;

	ifs.open(path.c_str());
	buf << ifs.rdbuf();
	ifs.close();
	return buf.str();
}

std::string utils::formatSize(size_t size) {
	std::stringstream ss;

	const char *units[] = {"B", "KB", "MB", "GB"};
	int unit = 0;
	while (size > 1024 && unit < 3) {
		size /= 1024;
		++unit;
	}
	ss << size << units[unit];
	return ss.str();
}

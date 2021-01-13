#pragma once
#include <string>
#include <ctime>
class File
{

private:
	std::string _type;
	std::string _path;
	std::time_t _lastModified;
	int _size;
public:
	File(const std::string& t, const std::string& p, const std::time_t& l, const int s) {
		_type = t;
		_path = p;
		_lastModified = l;
		_size = s;
	}

	const std::string& type() const { return _type; }
	const std::string& path() const { return _path; }
	const std::time_t& lastModified() const { return _lastModified; }
	const int size() const { return _size; }
};


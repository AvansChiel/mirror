#pragma once
#include <string>
#include <ctime>
class File
{
public:
	std::string type;
	std::string path;
	std::time_t lastModified;
	int size;
};


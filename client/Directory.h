#pragma once
#include <vector>
#include "File.h"
#include <string>
class Directory
{
public:
	std::vector<Directory> folders;
	std::vector<File> files;
	std::string name;
	int size;
	int lastmodified;
};


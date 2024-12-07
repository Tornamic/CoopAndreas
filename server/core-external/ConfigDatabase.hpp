#pragma once

#include <iostream>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cstring>
#include <cstdlib>

//#include "../thirdparty-libraries/iniparser.hpp"
#include "../thirdparty-libraries/inicpp/inicpp.h"

namespace ConfigDatabase
{
	void Init(unsigned int &var, const std::string &filename);
	bool Create(const std::string &filename);
	bool Load(unsigned int &var, const std::string &filename);
}


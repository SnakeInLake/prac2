#pragma once
#include <string>
#include "json.hpp"

using json = nlohmann::json;

bool createSchema(const std::string& schema_filename, const std::string& schema_name);

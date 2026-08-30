#pragma once
#include <fstream>
#include <sstream>
#include <map>
#include <string>
#include <vector>
#include "Node.h"
#include "table.h"

void Generate(Node* n, std::stringstream& mainStream, std::stringstream& funcStream, std::vector<std::map<std::string, SymbolInfo>>& tableStack);
#pragma once
#include <vector>
#include <string>
#include "token.h"

std::vector<Token> lexer(std::string File2_Text, int& line);
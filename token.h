#pragma once
#include <string>

enum TokenType {

	Text,
	Num,
	Simbol,
	DoubleSimbol,
	Quotes,
	Keyword,
	Waiting

};

struct Token {	

	TokenType type;

	std::string value;
}; 

std::string TypeName(TokenType type);

bool IsKeyWord(const std::string& value);
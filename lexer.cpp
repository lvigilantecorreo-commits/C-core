#include "token.h"
#include "lexer.h"
#include <iostream>
#include <string>
#include <cctype>

static const std::string Keywords[] = {
	"if", "else", "while", "for", "return",
	"int", "float", "string", "bool", "void", "struct"
};

bool IsKeyWord(const std::string& value) {

	for (const std::string& k : Keywords) {

		if (value == k) {
			return true;
		}
	}

	return false;
}

void TokenGen(std::vector <Token>& Tokens, std::string& TokenBuffer) {

	if (!TokenBuffer.empty()) {

		if (isdigit(TokenBuffer[0])) {

			Token TokenNumber;
			TokenNumber.type = Num;
			TokenNumber.value = TokenBuffer;
			Tokens.push_back(TokenNumber);

		}
		else if (isalpha(TokenBuffer[0])) {

			Token TokenText;
			TokenText.type = IsKeyWord(TokenBuffer) ? Keyword : Text;
			TokenText.value = TokenBuffer;
			Tokens.push_back(TokenText);
		}

		TokenBuffer.clear();

	}
}

std::vector <Token> lexer(std::string File2_Text, int& line) {

	std::vector <Token> Tokens;
	std::string TokenBuffer;
	char miniquotes = '"';
	char slash = '/';

	for (int i = 0; i < File2_Text.length(); i++) {

		if (File2_Text[i] == '\n') {
			line++;
		}

		bool isDecimalPoint = (File2_Text[i] == '.' && !TokenBuffer.empty() && isdigit(TokenBuffer[0]));

		if (isalpha(File2_Text[i]) || isdigit(File2_Text[i]) || isDecimalPoint) {

			if (!TokenBuffer.empty() && isdigit(TokenBuffer[0]) && isalpha(File2_Text[i])) {

				std::cout << "[-] Error: identifier cannot start with a digit (line " << line << ")" << std::endl;

				TokenBuffer.clear();

				while (i < File2_Text.length() && (isalpha(File2_Text[i]) || isdigit(File2_Text[i]))) {
					i++;
				}
				i--;

				continue;
			}

			TokenBuffer += File2_Text[i];

		}
		else {

			TokenGen(Tokens, TokenBuffer);

			if (!isspace(File2_Text[i])) {

				Token TokenSimbol;
				TokenSimbol.type = Waiting;
				TokenSimbol.value = Waiting;

				if (i + 1 < File2_Text.length() && File2_Text[i] == slash && File2_Text[i + 1] == slash) {

					i++;
					while (i < File2_Text.length() && File2_Text[i] != '\n') {
						i++;
					}
					continue;
				}

				if (File2_Text[i] == miniquotes) {

					i++;
					while (i < File2_Text.length() && File2_Text[i] != miniquotes) {
						TokenBuffer += File2_Text[i];
						i++;
					}

					if (i >= File2_Text.length()) {

						std::cout << "[-] Error: unterminated string (line " << line << ")" << std::endl;
						TokenBuffer.clear();
						continue;

					}
					else {

						Token TokenQuotes;
						TokenQuotes.type = Quotes;
						TokenQuotes.value = TokenBuffer;
						Tokens.push_back(TokenQuotes);

						TokenBuffer.clear();
						continue;

					}
				}

				if (i + 1 < File2_Text.length()) {

					std::string possiblePair = "";
					possiblePair += File2_Text[i];
					possiblePair += File2_Text[i + 1];

					if (possiblePair == "==" || possiblePair == "!=" || possiblePair == "<=" ||
						possiblePair == ">=" || possiblePair == "&&" || possiblePair == "||") {

						TokenBuffer += File2_Text[i];
						TokenBuffer += File2_Text[i + 1];

						Token TokenDoubleSimbol;
						TokenDoubleSimbol.type = DoubleSimbol;
						TokenDoubleSimbol.value = TokenBuffer;

						Tokens.push_back(TokenDoubleSimbol);

						i = i + 1;

					}
					else {

						TokenSimbol.type = Simbol;
						TokenSimbol.value = File2_Text[i];
						Tokens.push_back(TokenSimbol);

					}

				}
				else if (isprint((unsigned char)File2_Text[i])) {

					TokenSimbol.type = Simbol;
					TokenSimbol.value = File2_Text[i];
					Tokens.push_back(TokenSimbol);
				}
				else {

					std::cout << "[-] Error: unrecognized character (line " << line << ")" << std::endl;
				}

				TokenBuffer.clear();
			}

		}

	}

	TokenGen(Tokens, TokenBuffer);

	return Tokens;

}
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "lexer.h"
#include "parser.h"

std::string TypeName(TokenType type) {

	switch (type) {

	case Text:
		return "Text::";

	case Num:
		return "Numbers::";

	case Simbol:
		return "Simbols::";

	case DoubleSimbol:
		return "2-Simbols::";

	case Quotes:
		return "TextInComs::";

	case Keyword:
		return "Keyword::";

	default:
		return "Unknown";
	}
}

void TokenOutput(std::vector <Token>& FinalToken) {

	for (int i = 0; i < FinalToken.size(); i++) {

		std::cout << TypeName(FinalToken[i].type) << FinalToken[i].value << std::endl;
	}

}

int main(int argc, char* argv[]) {

	int pos = 0;

	std::vector <Token> FinalToken;

	std::vector <Token> Token_Container;

	std::string File2_Text;

	int line = 1;

	if (argc < 2) {

		std::cout << "[!] Error : could not open file" << std::endl;
		return 1;
	}
	else {

		std::ifstream File2(argv[1]);

		if (File2) {

			std::cout << "File +" << std::endl;

			while (std::getline(File2, File2_Text)) {

				std::cout << "[+]-Success : ";
				std::cout << File2_Text << std::endl;
				FinalToken = lexer(File2_Text, line);

				TokenOutput(FinalToken);

				Token_Container.insert(Token_Container.end(), FinalToken.begin(), FinalToken.end());
				std::cout << std::endl << std::endl;

				line++;
			}

			std::stringstream mainStream;
			std::stringstream funcStream;

			parser(Token_Container, mainStream, funcStream);

			std::ofstream ASMOutput("output.asm");

			ASMOutput << "global main" << std::endl;
			ASMOutput << "extern ExitProcess" << std::endl;
			ASMOutput << "section .text" << std::endl;
			ASMOutput << "main:" << std::endl;
			ASMOutput << "    push rbp" << std::endl;
			ASMOutput << "    mov rbp, rsp" << std::endl;
			ASMOutput << "    and rsp, -16" << std::endl;
			ASMOutput << "    sub rsp, 48" << std::endl;

			ASMOutput << mainStream.str();

			ASMOutput << "    mov ecx, eax" << std::endl;
			ASMOutput << "    call ExitProcess" << std::endl << std::endl << std::endl;

			ASMOutput << funcStream.str();

			std::cout << std::endl << std::endl;
			std::cout << "ALL TOKENS : " << Token_Container.size() << std::endl;
		}

	}


	return 0;
}
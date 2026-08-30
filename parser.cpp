#include <iostream>
#include <fstream>
#include <string>
#include "lexer.h"
#include "Node.h"
#include <map>
#include <vector>
#include "table.h"
#include "parser.h"
#include <sstream>
#include "codegen.h"

// int x = 5; 
// 
// int i ( int a , int b ) { return b }

// parsear funcion

static int OffsetCounter = 8;

Node* ParseIf(std::vector <Token>& FinalToken, int& pos, std::vector<std::map<std::string, SymbolInfo>>& tableStack);
Node* ParseWhile(std::vector <Token>& FinalToken, int& pos, std::vector<std::map<std::string, SymbolInfo>>& tableStack);
Node* ParseFor(std::vector <Token>& FinalToken, int& pos, std::vector<std::map<std::string, SymbolInfo>>& tableStack);
Node* ParseKeayword(std::vector <Token>& FinalToken, int& pos, std::vector<std::map<std::string, SymbolInfo>>& tableStack);

Node* ParseBodyStatement(std::vector <Token>& FinalToken, int& pos, std::vector<std::map<std::string, SymbolInfo>>& tableStack) {

	Node* sentence = nullptr;

	if (FinalToken[pos].type == Keyword && FinalToken[pos].value == "if") {

		sentence = ParseIf(FinalToken, pos, tableStack);
	}
	else if (FinalToken[pos].type == Text && pos + 1 < FinalToken.size() && FinalToken[pos + 1].value == "=") {

		Node* asignNode = new Node;
		(*asignNode).name = FinalToken[pos].value;
		(*asignNode).op = "=";
		pos++;
		pos++;
		Node* valueNode = ParseSum(FinalToken, pos, tableStack);
		(*asignNode).left = valueNode;

		sentence = asignNode;
	}
	else if (FinalToken[pos].type == Keyword && FinalToken[pos].value == "while") {

		sentence = ParseWhile(FinalToken, pos, tableStack);
	}
	else if (FinalToken[pos].type == Keyword && FinalToken[pos].value == "for") {

		sentence = ParseFor(FinalToken, pos, tableStack);
	}
	else if (FinalToken[pos].type == Keyword) {

		sentence = ParseKeayword(FinalToken, pos, tableStack);
	}
	else {

		sentence = ParseSum(FinalToken, pos, tableStack);
	}

	return sentence;
}

Node* ParseFunction(std::vector<Token>& FinalToken, int& pos, std::vector<std::map<std::string, SymbolInfo>>& tableStack) {

	Node* node = new Node;
	(*node).DataType = FinalToken[pos].value;

	pos++;

	(*node).name = FinalToken[pos].value;

	SymbolInfo FuncInfo;
	FuncInfo.type = (*node).DataType;
	tableStack.back()[(*node).name] = FuncInfo;

	pos++;

	(*node).isFunction = true;

	if (pos < FinalToken.size() && FinalToken[pos].value == "(") {

		pos++;

		tableStack.push_back({});

		int savedOffset = OffsetCounter;
		OffsetCounter = 8;

		while (pos < FinalToken.size() && FinalToken[pos].value != ")") {

			if (FinalToken[pos].value != ",") {                // int i ( int a , int b ) { return b }

				Node* node2 = new Node;
				(*node2).DataType = FinalToken[pos].value;

				SymbolInfo FuncType;
				FuncType.type = FinalToken[pos].value;
				FuncType.offset = OffsetCounter;
				(*node2).offset = OffsetCounter;
				OffsetCounter += 8;

				if (pos + 1 < FinalToken.size()) {
					tableStack.back()[FinalToken[pos + 1].value] = FuncType;
				}

				pos++;

				(*node2).name = FinalToken[pos].value;
				(*node).params.push_back(node2);

				pos++;
			}

			else {

				pos++;
			}
		}

		pos++;

		if (pos < FinalToken.size() && FinalToken[pos].value == "{") {

			pos++;

			while (pos < FinalToken.size() && FinalToken[pos].value != "}") {

				if (FinalToken[pos].value != ";") {

					if (FinalToken[pos].value == "return") {

						pos++;

						Node* nodeReturn = new Node;
						(*nodeReturn).name = "return";

						Node* value = ParseComparison(FinalToken, pos, tableStack);
						(*nodeReturn).left = value;

						(*node).body.push_back(nodeReturn);

					}
					else {

						Node* node3 = new Node;
						(*node3).DataType = FinalToken[pos].value;

						pos++;

						if (pos < FinalToken.size()) {
							(*node3).name = FinalToken[pos].value;
						}
						(*node).body.push_back(node3);

						SymbolInfo FuncType;
						if (pos - 1 >= 0) {
							FuncType.type = FinalToken[pos - 1].value;
						}
						FuncType.offset = OffsetCounter;
						OffsetCounter += 8;

						if (pos < FinalToken.size()) {
							tableStack.back()[FinalToken[pos].value] = FuncType;
						}

						pos++;

						if (pos < FinalToken.size() && FinalToken[pos].value == "=") {

							pos++;

							Node* valueNode = ParseSum(FinalToken, pos, tableStack);

							(*node3).left = valueNode;
						}
					}
				}
				else {

					pos++;
				}

			}

			pos++;

		}

		(*node).value = OffsetCounter - 8;
		OffsetCounter = savedOffset;

		tableStack.pop_back();

		return node;

	}
	else {

		return nullptr;

	}

}

// recorre las sentencias del programa
void ParseStatement(std::vector <Token>& FinalToken, int& pos, std::vector<std::map<std::string, SymbolInfo>>& tableStack, std::stringstream& mainStream, std::stringstream& funcStream) {

	while (pos < FinalToken.size()) {

		bool wasBlock = false;



		if (FinalToken[pos].type == Keyword && FinalToken[pos].value == "if") {

			Node* caseIf = ParseIf(FinalToken, pos, tableStack);

			PrintTree(caseIf);
			wasBlock = true;
			Generate(caseIf, mainStream, funcStream, tableStack);
		}
		else if (FinalToken[pos].type == Text && pos + 1 < FinalToken.size() && FinalToken[pos + 1].value == "=") {

			Node* node = new Node;
			(*node).name = FinalToken[pos].value;
			(*node).op = "=";
			pos++;
			pos++;
			Node* valueNode = ParseSum(FinalToken, pos, tableStack);
			(*node).left = valueNode;

			PrintTree(node);
			wasBlock = false;
			Generate(node, mainStream, funcStream, tableStack);

		}
		else if (FinalToken[pos].type == Keyword && FinalToken[pos].value == "while") {

			Node* caseWhile = ParseWhile(FinalToken, pos, tableStack);

			PrintTree(caseWhile);
			wasBlock = true;
			Generate(caseWhile, mainStream, funcStream, tableStack);
		}
		else if (FinalToken[pos].type == Keyword && FinalToken[pos].value == "for") {

			Node* caseFor = ParseFor(FinalToken, pos, tableStack);

			PrintTree(caseFor);
			wasBlock = true;
		}
		else if (FinalToken[pos].type == Keyword) {

			Node* case1 = ParseKeayword(FinalToken, pos, tableStack);
			PrintTree(case1);
			Generate(case1, mainStream, funcStream, tableStack);

			if (case1 != nullptr && case1->isFunction) {
				wasBlock = true;
			}
		}
		else {
			Node* case2 = ParseComparison(FinalToken, pos, tableStack);
			PrintTree(case2);
			Generate(case2, mainStream, funcStream, tableStack);
		}

		if (!wasBlock) {

			if (pos < FinalToken.size() && FinalToken[pos].value != ";") {
				std::cout << " [-] Error: expected ';'" << std::endl;
			}
			else if (pos < FinalToken.size()) {
				pos++;
			}
		}
	}
}

//stirng x = "hola";
//int testfunc(a,b);

Node* ParseKeayword(std::vector <Token>& FinalToken, int& pos, std::vector<std::map<std::string, SymbolInfo>>& tableStack) {

	if (FinalToken[pos].type == Keyword && pos + 2 < FinalToken.size() && FinalToken[pos + 2].value != "(") {

		Node* node = new Node;
		(*node).DataType = FinalToken[pos].value;

		pos++;

		//error
		if (pos >= FinalToken.size() || FinalToken[pos].type != Text) {

			return nullptr;

		}
		//-

		if (pos + 2 < FinalToken.size() && FinalToken[pos + 1].value == "=" && FinalToken[pos + 2].type == Quotes) {

			(*node).name = FinalToken[pos].value;

			SymbolInfo TypeInfo;
			TypeInfo.type = (*node).DataType;
			TypeInfo.offset = OffsetCounter;
			OffsetCounter += 8;
			tableStack.back()[(*node).name] = TypeInfo;

			pos++;
			pos++;
			if (pos < FinalToken.size()) {
				(*node).Text = FinalToken[pos].value;
			}
			pos++;

			return node;
		}

		(*node).name = FinalToken[pos].value;

		SymbolInfo TypeInfo;
		TypeInfo.type = (*node).DataType;
		TypeInfo.offset = OffsetCounter;
		OffsetCounter += 8;
		tableStack.back()[(*node).name] = TypeInfo;

		pos++;

		//error
		if (pos >= FinalToken.size() || FinalToken[pos].type != Simbol) {


			while (pos < FinalToken.size() && FinalToken[pos].value != ";") {

				pos++;
			}

			return nullptr;
		}
		//- 

		else {

			pos++;
		}

		Node* valueNode = ParseSum(FinalToken, pos, tableStack);

		(*node).left = valueNode;

		return node;

	}
	else {

		if (pos + 2 < FinalToken.size() && FinalToken[pos + 2].value == "(") {

			Node* funcNode = ParseFunction(FinalToken, pos, tableStack);
			return funcNode;
		}
		else {

			while (pos < FinalToken.size() && FinalToken[pos].value != ";") {

				pos++;
			}

			return nullptr;
		}
	}
}

// parsear if
Node* ParseIf(std::vector <Token>& FinalToken, int& pos, std::vector<std::map<std::string, SymbolInfo>>& tableStack) {

	if (FinalToken[pos].value != "if") {

		return nullptr;
	}

	pos++;

	if (pos >= FinalToken.size() || FinalToken[pos].value != "(") {

		return nullptr;
	}

	pos++;

	Node* node = new Node;
	(*node).name = "if";

	Node* condition = ParseComparison(FinalToken, pos, tableStack);

	(*node).left = condition;

	if (pos >= FinalToken.size() || FinalToken[pos].value != ")") {

		while (pos < FinalToken.size() && FinalToken[pos].value != "}") {

			pos++;
		}

		return nullptr;
	}

	pos++;

	if (pos >= FinalToken.size() || FinalToken[pos].value != "{") {

		while (pos < FinalToken.size() && FinalToken[pos].value != "}") {

			pos++;
		}

		return nullptr;
	}

	pos++;

	// body del if
	while (pos < FinalToken.size() && FinalToken[pos].value != "}") {

		Node* sentence = ParseBodyStatement(FinalToken, pos, tableStack);

		(*node).body.push_back(sentence);

		if (pos < FinalToken.size() && FinalToken[pos].value == ";") {

			pos++;
		}
	}

	if (pos < FinalToken.size() && FinalToken[pos].value == "}") {

		pos++;
	}

	if (pos < FinalToken.size() && FinalToken[pos].value == "else") {

		pos++;

		if (pos >= FinalToken.size() || FinalToken[pos].value != "{") {

			while (pos < FinalToken.size() && FinalToken[pos].value != "}") {

				pos++;
			}

			return node;
		}

		pos++;

		// body del else
		while (pos < FinalToken.size() && FinalToken[pos].value != "}") {

			Node* sentence = ParseBodyStatement(FinalToken, pos, tableStack);

			(*node).elseBody.push_back(sentence);

			if (pos < FinalToken.size() && FinalToken[pos].value == ";") {

				pos++;
			}
		}

		if (pos < FinalToken.size() && FinalToken[pos].value == "}") {

			pos++;
		}
	}

	return node;
}

// parsear while
Node* ParseWhile(std::vector <Token>& FinalToken, int& pos, std::vector<std::map<std::string, SymbolInfo>>& tableStack) {

	if (FinalToken[pos].value != "while") {

		return nullptr;
	}

	pos++;

	if (pos >= FinalToken.size() || FinalToken[pos].value != "(") {

		return nullptr;
	}

	pos++;

	Node* node = new Node;
	(*node).name = "while";

	Node* condition = ParseComparison(FinalToken, pos, tableStack);

	(*node).left = condition;

	if (pos >= FinalToken.size() || FinalToken[pos].value != ")") {

		while (pos < FinalToken.size() && FinalToken[pos].value != "}") {

			pos++;
		}

		return nullptr;
	}

	pos++;

	if (pos >= FinalToken.size() || FinalToken[pos].value != "{") {

		while (pos < FinalToken.size() && FinalToken[pos].value != "}") {

			pos++;
		}

		return nullptr;
	}

	pos++;

	// body del while
	while (pos < FinalToken.size() && FinalToken[pos].value != "}") {

		Node* sentence = ParseBodyStatement(FinalToken, pos, tableStack);

		(*node).body.push_back(sentence);

		if (pos < FinalToken.size() && FinalToken[pos].value == ";") {

			pos++;
		}
	}

	if (pos < FinalToken.size() && FinalToken[pos].value == "}") {

		pos++;
	}

	return node;
}

// parsear for
Node* ParseFor(std::vector <Token>& FinalToken, int& pos, std::vector<std::map<std::string, SymbolInfo>>& tableStack) {

	if (FinalToken[pos].value != "for") {

		return nullptr;
	}

	pos++;

	if (pos >= FinalToken.size() || FinalToken[pos].value != "(") {

		return nullptr;
	}

	pos++;

	Node* node = new Node;
	(*node).name = "for";

	Node* init = ParseKeayword(FinalToken, pos, tableStack);
	(*node).forParts.push_back(init);

	if (pos < FinalToken.size() && FinalToken[pos].value == ";") {

		pos++;
	}

	Node* cond = ParseComparison(FinalToken, pos, tableStack);
	(*node).forParts.push_back(cond);

	if (pos < FinalToken.size() && FinalToken[pos].value == ";") {

		pos++;
	}

	Node* step = ParseSum(FinalToken, pos, tableStack);
	(*node).forParts.push_back(step);

	if (pos >= FinalToken.size() || FinalToken[pos].value != ")") {

		while (pos < FinalToken.size() && FinalToken[pos].value != "}") {

			pos++;
		}

		return nullptr;
	}

	pos++;

	if (pos >= FinalToken.size() || FinalToken[pos].value != "{") {

		while (pos < FinalToken.size() && FinalToken[pos].value != "}") {

			pos++;
		}

		return nullptr;
	}

	pos++;

	// body del for
	while (pos < FinalToken.size() && FinalToken[pos].value != "}") {

		Node* sentence = ParseBodyStatement(FinalToken, pos, tableStack);

		(*node).body.push_back(sentence);

		if (pos < FinalToken.size() && FinalToken[pos].value == ";") {

			pos++;
		}
	}

	if (pos < FinalToken.size() && FinalToken[pos].value == "}") {

		pos++;
	}

	return node;
}

// parsear numero, variable, parentesis o llamada a funcion
Node* ParseNum(std::vector <Token>& FinalToken, int& pos, std::vector<std::map<std::string, SymbolInfo>>& tableStack) {


	if (pos >= FinalToken.size()) {

		return nullptr;
	}

	if (FinalToken[pos].value == "(") {

		pos++;

		Node* result = ParseSum(FinalToken, pos, tableStack);

		if (pos >= FinalToken.size() || FinalToken[pos].value != ")") {

			while (pos < FinalToken.size() && FinalToken[pos].value != ";") {
				pos++;
			}

			return nullptr;

		}

		pos++;

		return result;
	}
	else if (FinalToken[pos].type == Text) {

		if (pos + 1 < FinalToken.size() && FinalToken[pos + 1].value == "(") {

			Node* callNode = new Node;
			(*callNode).name = FinalToken[pos].value;
			(*callNode).DataType = "call";

			pos++;

			pos++;

			while (pos < FinalToken.size() && FinalToken[pos].value != ")") {

				Node* arg = ParseComparison(FinalToken, pos, tableStack);
				(*callNode).params.push_back(arg);

				if (pos < FinalToken.size() && FinalToken[pos].value == ",") {

					pos++;
				}
			}

			pos++;

			return callNode;
		}

		bool found = false;

		for (int i = tableStack.size() - 1; i >= 0; i--) {

			if (tableStack[i].find(FinalToken[pos].value) != tableStack[i].end()) {

				found = true;
				break;
			}
		}

		if (!found) {


			std::cout << "[!] Error, invalid vairiable name or non exsitient";


			while (pos < FinalToken.size() && FinalToken[pos].value != ";") {

				pos++;
			}

			return nullptr;
		}

		Node* node = new Node;

		(*node).name = FinalToken[pos].value;
		(*node).DataType = "";

		for (int i = tableStack.size() - 1; i >= 0; i--) {
			if (tableStack[i].find(FinalToken[pos].value) != tableStack[i].end()) {
				(*node).offset = tableStack[i][FinalToken[pos].value].offset;
				break;
			}
		}

		pos++;

		return node;

	}
	else if (FinalToken[pos].type == Num) {

		Node* node = new Node;
		(*node).value = std::stoi(FinalToken[pos].value);
		(*node).name = "";
		(*node).DataType = "";

		pos++;

		return node;

	}
	else {

		std::cout << "[-] Error: expected number or '('" << std::endl;

		while (pos < FinalToken.size() && FinalToken[pos].value != ";") {

			pos++;
		}

		return nullptr;
	}
}

// parsear multiplicacion y division
Node* ParseMultiplication(std::vector <Token>& FinalToken, int& pos, std::vector<std::map<std::string, SymbolInfo>>& tableStack) {

	Node* Nleft = ParseNum(FinalToken, pos, tableStack);

	while (pos < FinalToken.size() && (FinalToken[pos].value == "*" || FinalToken[pos].value == "/")) {

		Node* node2 = new Node;
		(*node2).op = FinalToken[pos].value;
		(*node2).left = Nleft;
		(*node2).name = "";
		(*node2).DataType = "";

		pos++;

		Node* Nright = ParseNum(FinalToken, pos, tableStack);

		(*node2).right = Nright;

		Nleft = node2;
	}

	return Nleft;
}

// parsear suma y resta
Node* ParseSum(std::vector <Token>& FinalToken, int& pos, std::vector<std::map<std::string, SymbolInfo>>& tableStack) {

	Node* Nleft = ParseMultiplication(FinalToken, pos, tableStack);

	while (pos < FinalToken.size() && (FinalToken[pos].value == "+" || FinalToken[pos].value == "-")) {

		Node* node2 = new Node;
		(*node2).op = FinalToken[pos].value;
		(*node2).left = Nleft;
		(*node2).name = "";
		(*node2).DataType = "";

		pos++;

		Node* Nright = ParseMultiplication(FinalToken, pos, tableStack);

		(*node2).right = Nright;

		Nleft = node2;
	}

	return Nleft;
}

// parsear comparaciones
Node* ParseComparison(std::vector <Token>& FinalToken, int& pos, std::vector<std::map<std::string, SymbolInfo>>& tableStack) {

	Node* Nleft = ParseSum(FinalToken, pos, tableStack);

	while (pos < FinalToken.size() && (FinalToken[pos].value == ">" || FinalToken[pos].value == "<" ||
		FinalToken[pos].value == "==" || FinalToken[pos].value == "!=" ||
		FinalToken[pos].value == ">=" || FinalToken[pos].value == "<=")) {

		Node* node2 = new Node;
		(*node2).op = FinalToken[pos].value;
		(*node2).left = Nleft;
		(*node2).name = "";
		(*node2).DataType = "";

		pos++;

		Node* Nright = ParseSum(FinalToken, pos, tableStack);

		(*node2).right = Nright;

		Nleft = node2;
	}

	return Nleft;
}

// imprime el arbol para depurar
void PrintTree(Node* node) {

	Node* value1 = node;

	if (node == nullptr) {
		std::cout << "[ERROR]";
		return;
	}

	if ((*value1).isFunction == true) {

		std::cout << "[func " << (*value1).DataType << " " << (*value1).name << "(";

		for (int i = 0; i < (*value1).params.size(); i++) {

			std::cout << (*value1).params[i]->DataType << " " << (*value1).params[i]->name;

			if (i < (*value1).params.size() - 1) {
				std::cout << ", ";
			}
		}

		std::cout << ") { ";

		for (int i = 0; i < (*value1).body.size(); i++) {

			if ((*value1).body[i]->name == "return") {
				std::cout << "return ";
				PrintTree((*value1).body[i]->left);
			}
			else {
				PrintTree((*value1).body[i]);
			}
			std::cout << "; ";
		}

		std::cout << "}]";
		return;
	}

	if ((*value1).DataType == "call") {

		std::cout << "[call " << (*value1).name << "(";

		for (int i = 0; i < (*value1).params.size(); i++) {

			PrintTree((*value1).params[i]);

			if (i < (*value1).params.size() - 1) {
				std::cout << ", ";
			}
		}

		std::cout << ")]";
		return;
	}

	if ((*value1).name == "if") {

		std::cout << "[if (";
		PrintTree((*value1).left);
		std::cout << ") { ";

		for (int i = 0; i < (*value1).body.size(); i++) {

			PrintTree((*value1).body[i]);
			std::cout << "; ";
		}

		std::cout << "}";

		if ((*value1).elseBody.size() > 0) {

			std::cout << " else { ";

			for (int i = 0; i < (*value1).elseBody.size(); i++) {

				PrintTree((*value1).elseBody[i]);
				std::cout << "; ";
			}

			std::cout << "}";
		}

		std::cout << "]";
		return;
	}

	if ((*value1).op == "=") {

		std::cout << "[" << (*value1).name << " = ";
		PrintTree((*value1).left);
		std::cout << "]";
		return;
	}

	if ((*value1).name == "while") {

		std::cout << "[while (";
		PrintTree((*value1).left);
		std::cout << ") { ";

		for (int i = 0; i < (*value1).body.size(); i++) {

			PrintTree((*value1).body[i]);
			std::cout << "; ";
		}

		std::cout << "}]";
		return;
	}

	if ((*value1).name == "for") {

		std::cout << "[for (";

		for (int i = 0; i < (*value1).forParts.size(); i++) {

			PrintTree((*value1).forParts[i]);

			if (i < (*value1).forParts.size() - 1) {
				std::cout << "; ";
			}
		}

		std::cout << ") { ";

		for (int i = 0; i < (*value1).body.size(); i++) {

			PrintTree((*value1).body[i]);
			std::cout << "; ";
		}

		std::cout << "}]";
		return;
	}

	if ((*value1).DataType == "string") {

		std::cout << "[";
		std::cout << (*value1).DataType << " " << (*value1).name << " = " << '"' << (*value1).Text << '"';
		std::cout << "]";

	}
	else if ((*value1).DataType != "") {

		std::cout << "[";
		std::cout << (*value1).DataType << " " << (*value1).name << " = ";
		PrintTree((*value1).left);
		std::cout << "]";
	}
	else if ((*value1).name != "") {

		std::cout << (*value1).name;
	}
	else if ((*value1).op == "") {

		std::cout << (*value1).value;
	}
	else {

		std::cout << "[";
		PrintTree((*value1).left);
		std::cout << (*value1).op;
		PrintTree((*value1).right);
		std::cout << "]";
	}
}

// punto de entrada del parser
void parser(std::vector <Token>& FinalToken, std::stringstream& mainStream, std::stringstream& funcStream) {

	int pos = 0;
	std::vector<std::map<std::string, SymbolInfo>> tableStack;
	tableStack.push_back({});

	ParseStatement(FinalToken, pos, tableStack, mainStream, funcStream);

}
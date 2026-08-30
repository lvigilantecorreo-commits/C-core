#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "lexer.h"
#include "Node.h"
#include <map>
#include <vector>
#include "table.h"
#include "parser.h"

//(5 + 5) + 5

void Generate(Node* n, std::stringstream& mainStream, std::stringstream& funcStream, std::vector<std::map<std::string, SymbolInfo>>& tableStack) {

	static int LabelCount = 0;

	// funcion - va entera a funcStream

	if ((*n).isFunction == true) {

		funcStream << (*n).name << ":" << std::endl;
		funcStream << "push rbp" << std::endl;
		funcStream << "mov rbp, rsp" << std::endl;
		funcStream << "sub rsp, " << (*n).value << std::endl;

		if ((*n).params.size() >= 1) {
			funcStream << "mov [rbp-" << (*n).params[0]->offset << "], rcx" << std::endl;
		}
		if ((*n).params.size() >= 2) {
			funcStream << "mov [rbp-" << (*n).params[1]->offset << "], rdx" << std::endl;
		}

		for (int i = 0; i < (*n).body.size(); i++) {

			Generate((*n).body[i], funcStream, funcStream, tableStack);
		}

		funcStream << "mov rsp, rbp" << std::endl;
		funcStream << "pop rbp" << std::endl;
		funcStream << "ret" << std::endl;

		return;
	}

	//Nodo num (¿) - [ 1 ]

	if ((*n).left == nullptr && (*n).right == nullptr && (*n).name == "") {

		mainStream << "mov rax, " << (*n).value << std::endl;

	}
	else if ((*n).left == nullptr && (*n).right == nullptr && (*n).name != "" && (*n).DataType != "call" && !(*n).isFunction) {

		mainStream << "mov rax, [rbp-" << (*n).offset << "]" << " ; " << (*n).name << std::endl;
	}



	//suma (+) - [ 1 + 1 ]

	if ((*n).op == "+") {

		Generate((*n).left, mainStream, funcStream, tableStack);

		mainStream << "push rax" << std::endl;

		Generate((*n).right, mainStream, funcStream, tableStack);

		mainStream << "pop rbx" << std::endl;
		mainStream << "add rax, rbx" << std::endl;

	}

	//resta (-) - [ 1 - 1 ]

	if ((*n).op == "-") {

		Generate((*n).right, mainStream, funcStream, tableStack);

		mainStream << "push rax" << std::endl;

		Generate((*n).left, mainStream, funcStream, tableStack);

		mainStream << "pop rbx" << std::endl;
		mainStream << "sub rax, rbx" << std::endl;

	}

	//multi (*) - [ 1 * 1 ]

	if ((*n).op == "*") {

		Generate((*n).left, mainStream, funcStream, tableStack);

		mainStream << "push rax" << std::endl;

		Generate((*n).right, mainStream, funcStream, tableStack);

		mainStream << "pop rbx" << std::endl;
		mainStream << "imul rax, rbx" << std::endl;

	}

	//div (/) - [ 1 / 1 ]

	if ((*n).op == "/") {

		Generate((*n).right, mainStream, funcStream, tableStack);

		mainStream << "push rax" << std::endl;

		Generate((*n).left, mainStream, funcStream, tableStack);

		mainStream << "xor rdx, rdx" << std::endl;

		mainStream << "pop rbx" << std::endl;
		mainStream << "idiv rbx" << std::endl;

	}

	//menor que (<) - [ 1 < 1 ]

	if ((*n).op == "<") {

		Generate((*n).right, mainStream, funcStream, tableStack);

		mainStream << "push rax" << std::endl;

		Generate((*n).left, mainStream, funcStream, tableStack);

		mainStream << "xor rdx, rdx" << std::endl;

		mainStream << "pop rbx" << std::endl;
		mainStream << "cmp rax, rbx" << std::endl;
		mainStream << "setl al" << std::endl;
		mainStream << "movzx rax, al" << std::endl;

	}

	//mayor que (>) - [ 1 > 1 ]

	if ((*n).op == ">") {

		Generate((*n).right, mainStream, funcStream, tableStack);

		mainStream << "push rax" << std::endl;

		Generate((*n).left, mainStream, funcStream, tableStack);

		mainStream << "xor rdx, rdx" << std::endl;

		mainStream << "pop rbx" << std::endl;
		mainStream << "cmp rax, rbx" << std::endl;
		mainStream << "setg al" << std::endl;
		mainStream << "movzx rax, al" << std::endl;

	}

	//es? (==) - [ 1 + 1 ]

	if ((*n).op == "==") {

		Generate((*n).right, mainStream, funcStream, tableStack);

		mainStream << "push rax" << std::endl;

		Generate((*n).left, mainStream, funcStream, tableStack);

		mainStream << "xor rdx, rdx" << std::endl;

		mainStream << "pop rbx" << std::endl;
		mainStream << "cmp rax, rbx" << std::endl;
		mainStream << "sete al" << std::endl;
		mainStream << "movzx rax, al" << std::endl;

	}

	//no es/no igual (!=) - [ 1 != 2 ]

	if ((*n).op == "!=") {

		Generate((*n).right, mainStream, funcStream, tableStack);

		mainStream << "push rax" << std::endl;

		Generate((*n).left, mainStream, funcStream, tableStack);

		mainStream << "xor rdx, rdx" << std::endl;

		mainStream << "pop rbx" << std::endl;
		mainStream << "cmp rax, rbx" << std::endl;
		mainStream << "setne al" << std::endl;
		mainStream << "movzx rax, al" << std::endl;

	}

	//menor o igual que (<=) - [ 1 <= 1 ]

	if ((*n).op == "<=") {

		Generate((*n).right, mainStream, funcStream, tableStack);

		mainStream << "push rax" << std::endl;

		Generate((*n).left, mainStream, funcStream, tableStack);

		mainStream << "xor rdx, rdx" << std::endl;

		mainStream << "pop rbx" << std::endl;
		mainStream << "cmp rax, rbx" << std::endl;
		mainStream << "setle al" << std::endl;
		mainStream << "movzx rax, al" << std::endl;

	}

	//mayor o igual que (>=) - [ 1 >= 1 ]

	if ((*n).op == ">=") {

		Generate((*n).right, mainStream, funcStream, tableStack);

		mainStream << "push rax" << std::endl;

		Generate((*n).left, mainStream, funcStream, tableStack);

		mainStream << "xor rdx, rdx" << std::endl;

		mainStream << "pop rbx" << std::endl;
		mainStream << "cmp rax, rbx" << std::endl;
		mainStream << "setge al" << std::endl;
		mainStream << "movzx rax, al" << std::endl;

	}
	//0 < 1// = 1
	// if / keyword

	if ((*n).name == "if") {

		int MyLabel = LabelCount;
		LabelCount++;

		Generate((*n).left, mainStream, funcStream, tableStack);
		mainStream << "cmp rax, 0" << std::endl; //bandera is zero = false o true, depnde si es 1 o 0
		mainStream << "je Lelse_" << MyLabel << std::endl; // lee la bandera, si es 0 no salta si es 1 salta

		for (int i = 0; i < (*n).body.size(); i++) {

			Generate((*n).body[i], mainStream, funcStream, tableStack);
		}

		mainStream << "jmp Lend_" << MyLabel << std::endl;

		mainStream << "Lelse_" << MyLabel << ":" << std::endl;

		for (int i = 0; i < (*n).elseBody.size(); i++) {

			Generate((*n).elseBody[i], mainStream, funcStream, tableStack);

		}

		mainStream << "Lend_" << MyLabel << ":" << std::endl;


	}

	if ((*n).name == "while") {

		int MyLabel = LabelCount;
		LabelCount++;

		mainStream << "Lstart_" << MyLabel << ":" << std::endl;
		Generate((*n).left, mainStream, funcStream, tableStack);
		mainStream << "cmp rax, 0" << std::endl;
		mainStream << "je Lend_" << MyLabel << std::endl;

		for (int i = 0; i < (*n).body.size(); i++) {

			Generate((*n).body[i], mainStream, funcStream, tableStack);
		}

		mainStream << "jmp Lstart_" << MyLabel << std::endl;

		mainStream << "Lend_" << MyLabel << ":" << std::endl;
	}

	if ((*n).DataType != "" && (*n).name != "" && (*n).DataType != "call" && !(*n).isFunction) {

		Generate((*n).left, mainStream, funcStream, tableStack);

		int offset = -1;
		for (int i = tableStack.size() - 1; i >= 0; i--) {
			if (tableStack[i].find((*n).name) != tableStack[i].end()) {
				offset = tableStack[i][(*n).name].offset;
				break;
			}
		}
		mainStream << "mov [rbp-" << offset << "], rax" << " ; " << (*n).name << std::endl;

	}

	if ((*n).op == "=") {

		int offset = -1;
		for (int i = tableStack.size() - 1; i >= 0; i--) {
			if (tableStack[i].find((*n).name) != tableStack[i].end()) {
				offset = tableStack[i][(*n).name].offset;
				break;
			}
		}
		Generate((*n).left, mainStream, funcStream, tableStack);

		mainStream << "mov [rbp-" << offset << "], rax" << " ; " << (*n).name << std::endl;
	}

	if ((*n).name == "return") {

		Generate((*n).left, mainStream, funcStream, tableStack);
	}

	if ((*n).DataType == "call") {

		if ((*n).params.size() >= 1) {
			Generate((*n).params[0], mainStream, funcStream, tableStack);
			mainStream << "mov rcx, rax" << std::endl;
		}
		if ((*n).params.size() >= 2) {
			Generate((*n).params[1], mainStream, funcStream, tableStack);
			mainStream << "mov rdx, rax" << std::endl;
		}

		mainStream << "call " << (*n).name << std::endl;
	}
}
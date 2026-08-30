#pragma once
#include <string>

struct Node {

	int value = 0;

	bool isFunction = false;

	int offset = 0;

	std::string op = "";

	Node* left = nullptr;

	Node* right = nullptr;

    std::string name = "";

	std::string DataType = "";

	std::string Text = "";

	std::string Error = "";

	std::vector<Node*> body;

	std::vector<Node*> params;
	
	std::vector<Node*> elseBody;

	std::vector<Node*> forParts;
};

#pragma once
#include <vector>
#include <string>
#include <map>
#include "token.h"
#include "Node.h"
#include "table.h"

Node* ParseKeayword(std::vector<Token>& FinalToken, int& pos, std::vector<std::map<std::string, SymbolInfo>>& tableStack);
Node* ParseIf(std::vector<Token>& FinalToken, int& pos, std::vector<std::map<std::string, SymbolInfo>>& tableStack);
Node* ParseWhile(std::vector<Token>& FinalToken, int& pos, std::vector<std::map<std::string, SymbolInfo>>& tableStack);
Node* ParseFor(std::vector<Token>& FinalToken, int& pos, std::vector<std::map<std::string, SymbolInfo>>& tableStack);
Node* ParseSum(std::vector<Token>& FinalToken, int& pos, std::vector<std::map<std::string, SymbolInfo>>& tableStack);
Node* ParseMultiplication(std::vector<Token>& FinalToken, int& pos, std::vector<std::map<std::string, SymbolInfo>>& tableStack);
Node* ParseComparison(std::vector<Token>& FinalToken, int& pos, std::vector<std::map<std::string, SymbolInfo>>& tableStack);
Node* ParseNum(std::vector<Token>& FinalToken, int& pos, std::vector<std::map<std::string, SymbolInfo>>& tableStack);
void PrintTree(Node* node);
Node* ParseFunction(std::vector<Token>& FinalToken, int& pos, std::vector<std::map<std::string, SymbolInfo>>& tableStack);
void parser(std::vector<Token>& FinalToken, std::stringstream& mainStream, std::stringstream& funcStream);
void ParseStatement(std::vector<Token>& FinalToken, int& pos, std::vector<std::map<std::string, SymbolInfo>>& tableStack, std::ofstream& ASMOutput);
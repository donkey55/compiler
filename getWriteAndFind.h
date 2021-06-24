#pragma once
#include "symbolTable.h"

void getSym(Token& token, unsigned int length);
void parsingProgram(Token& token);
void writeFile(Token& token);
void writeFile(string message);
void writeFile(int line, string message);
bool isRightCharCon(Token& token);
void goBack(Token& token, int index, int line);
void saveNow(Token& token, int& index, int& line);

void saveTypeAndSym(SymbolTableRow& temp, string type, string symbol);
bool findIsDef(SymbolTableRow row);
string findAllIsDef(string name);
vector<string> getFunParLen(string name);
SymbolTableRow findAll(string name);
bool strIsEqual(string name1, string name2);
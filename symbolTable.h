#pragma once
#include <string>
#include <vector>
using namespace std;
typedef struct arraydata
{
	string arrSymbol;
	int dim; // 1 || 2 ����ά��
	int length1;
	int length2;
} ArrayData;


class SymbolTableRow
{
public:
	string name;
	string type; //fun or var or const
	string symbol;// symbol : void int char array
	string value; // const or varInit ???
	ArrayData array;
	bool isKonwn = false;
	vector<string> funParameters; //���������Ĳ����б�
	SymbolTableRow();
	SymbolTableRow(string name);
	SymbolTableRow(string name, string type, string symbol);
	SymbolTableRow(string name, string type, string symbol, string value, bool isKonwn);

	void clear();
};


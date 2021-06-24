#include "symbolTable.h"

SymbolTableRow::SymbolTableRow()
{
}

SymbolTableRow::SymbolTableRow(string nameIn):
	name(nameIn)
{
}

SymbolTableRow::SymbolTableRow(string nameIn, string typeIn, string symbolIn):
	name(nameIn), type(typeIn), symbol(symbolIn)
{
}

SymbolTableRow::SymbolTableRow(string nameIn, string typeIn, string symbolIn, string valueIn, bool isKonwnIn) :
	name(nameIn), type(typeIn), symbol(symbolIn), value(valueIn), isKonwn(isKonwnIn)
{
}


void SymbolTableRow::clear()
{
	name.clear();
	symbol.clear();
	type.clear();
	value.clear();
	funParameters.clear();
	array.arrSymbol.clear();
	array.dim = 0;
	array.length1 = 0;
	array.length2 = 0;
	isKonwn = false;
}


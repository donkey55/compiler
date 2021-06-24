#pragma once
#include <string>
#include "symbolTable.h"
using namespace std;

enum class QuadOperator {
	constDef, varDefNoInit, varDefInit, scanf, printf,
	equal, add, sub, mul, div,
	//所谓的factor 操作应该是最底层的取值与赋值操作。
	//变量与常量取值，数组取值，字符与整数赋值。
	exprBegin, exprEnd,
	factorArrOne, factorArrTwo, factorIden, factorChar, factorInt, factorFunCall
	, fun,
	funParameterDef,
	funValueTableRow,
	funEnd,
	callBegin,
	callEnd,
	Return,
	ifBegin,
	ifEnd,
	elseBegin,
	elseEnd,
	conditionBeq, // ==
	conditionBge, // >=
	conditionBgt, // > 
	conditionBle, // <=
	conditionBlt, // <
	conditionBne, // !=
	whileBegin,
	whileEnd,
	forBegin,
	forEnd,
	forStride,
	switchBegin,
	switchEnd,
	caseBegin,
	caseEnd,
	defaultEnd,
	null,
	allEnd
};

class Quadruaple
{
public:
	QuadOperator op;
	SymbolTableRow num1;
	SymbolTableRow num2;
	SymbolTableRow result;
	Quadruaple();
	Quadruaple(QuadOperator op, SymbolTableRow num1, SymbolTableRow num2, SymbolTableRow res);
	Quadruaple(QuadOperator op, SymbolTableRow num1, SymbolTableRow num2);
	Quadruaple(QuadOperator op, SymbolTableRow num1);
	Quadruaple(QuadOperator op);
	string toString();
	void clear();
};

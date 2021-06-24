#include "quadruaple.h"
#include "symbolTable.h"

Quadruaple::Quadruaple()
{
}
Quadruaple::Quadruaple(QuadOperator opIn, SymbolTableRow num1In, SymbolTableRow num2In, SymbolTableRow resIn) :
	op(opIn), num1(num1In), num2(num2In), result(resIn)
{
}
Quadruaple::Quadruaple(QuadOperator opIn, SymbolTableRow num1In, SymbolTableRow num2In) :
	op(opIn), num1(num1In), num2(num2In)
{
}

Quadruaple::Quadruaple(QuadOperator opIn, SymbolTableRow num1In) :
	op(opIn), num1(num1In)
{
}

Quadruaple::Quadruaple(QuadOperator opIn) :
	op(opIn)
{
}

string Quadruaple::toString()
{
	string tempRes = "";
	switch (op)
	{
	case QuadOperator::constDef:
		tempRes = "constDef";
		break;
	case QuadOperator::varDefNoInit:
		tempRes = "constDef";
		break;
	case QuadOperator::varDefInit:
		tempRes = "constDef";
		break;
	case QuadOperator::scanf:
		tempRes = "constDef";
		break;
	case QuadOperator::printf:
		tempRes = "constDef";
		break;
	case QuadOperator::equal:
		tempRes = "constDef";
		break;
	case QuadOperator::add:
		tempRes = "constDef";
		break;
	case QuadOperator::sub:
		tempRes = "constDef";
		break;
	case QuadOperator::mul:
		tempRes = "constDef";
		break;
	case QuadOperator::div:
		tempRes = "constDef";
		break;
	case QuadOperator::exprBegin:
		tempRes = "constDef";
		break;
	case QuadOperator::exprEnd:
		break;
	case QuadOperator::factorArrOne:
		break;
	case QuadOperator::factorArrTwo:
		break;
	case QuadOperator::factorIden:
		break;
	case QuadOperator::factorChar:
		break;
	case QuadOperator::factorInt:
		break;
	case QuadOperator::factorFunCall:
		break;
	case QuadOperator::fun:
		break;
	case QuadOperator::funParameterDef:
		break;
	case QuadOperator::funValueTableRow:
		break;
	case QuadOperator::funEnd:
		break;
	case QuadOperator::callBegin:
		break;
	case QuadOperator::callEnd:
		break;
	case QuadOperator::Return:
		break;
	case QuadOperator::ifBegin:
		break;
	case QuadOperator::ifEnd:
		break;
	case QuadOperator::elseBegin:
		break;
	case QuadOperator::elseEnd:
		break;
	case QuadOperator::conditionBeq:
		break;
	case QuadOperator::conditionBge:
		break;
	case QuadOperator::conditionBgt:
		break;
	case QuadOperator::conditionBle:
		break;
	case QuadOperator::conditionBlt:
		break;
	case QuadOperator::conditionBne:
		break;
	case QuadOperator::whileBegin:
		break;
	case QuadOperator::whileEnd:
		break;
	case QuadOperator::forBegin:
		break;
	case QuadOperator::forEnd:
		break;
	case QuadOperator::forStride:
		break;
	case QuadOperator::switchBegin:
		break;
	case QuadOperator::switchEnd:
		break;
	case QuadOperator::caseBegin:
		break;
	case QuadOperator::caseEnd:
		break;
	case QuadOperator::defaultEnd:
		break;
	case QuadOperator::null:
		break;
	case QuadOperator::allEnd:
		break;
	default:
		break;
	}
	return tempRes;
}

void Quadruaple::clear()
{
	op = QuadOperator::null;
	num1.clear();
	num2.clear();
	result.clear();
}






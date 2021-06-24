#pragma once
#include "quadruaple.h"
#include "Token.h"
using namespace std;
extern vector<SymbolTableRow>* curTables;
extern vector<SymbolTableRow> globalTables;
extern vector<SymbolTableRow> funTables;

void error(int line, string message);
void getAndWrite(Token& token);
void getNext(Token& token);
void parsingProgram(Token& token);
//

void parConstantDes(Token& token);
void parConstantDef(Token& token);
void parConstantDesLoop(Token& token);
void parIdenEqlInt(Token& token);
void parIdenEqlChar(Token& token);
void parUnsignInteger(Token& token);
void parSignInteger(Token& token);
void parVariableDes(Token& token);
void parVariableDef(Token& token);
void parVariableDesLoop(Token& token);
void parVariableDefNoInit(Token& token);
void parVariableDefInit(Token& token);
//以上是常量说明
bool isVarDes(Token& token);
void parVarDefNoInitLoop(Token& token);
void parConstant(Token& token); //常量分析
void parConstantLoop(Token& token);
void parConstantTwoLoop(Token& token);
bool isInit(Token& token);

//以上是变量说明

void parFunction(Token& token);
void parVoidFunction(Token& token);
void parReturnFunction(Token& token);
void parCompoundStatement(Token& token);
void parDeclarationHeader(Token& token);
void parParametersTable(Token& token);
void parParametersTableLoop(Token& token);
void parStatementList(Token& token);
//
void parStatement(Token& token);
void parLoopStatement(Token& token);
void parStride(Token& token);
void parCondStatement(Token& token);
QuadOperator parRelationalOp(Token &token, Quadruaple quad);
QuadOperator conditionQuadruaple(const string symbol);
void parCondition(Token& token);
void parCallFunctionStatement(Token& token);
void parCallVoidStatement(Token& token);
void parCallReturnStatement(Token& token);
void parValueTable(Token& token, vector<string> paraList, string funName);
void parAssignStatement(Token& token);
void parScanfStatement(Token& token);
void parPrintfStatement(Token& token);
void parString(Token& token);
void parSwitchStatement(Token& token);
void parCaseStatament(Token& token, SymbolTableRow back1, int tempSwitchCount);
void parSituationTable(Token& token, SymbolTableRow back1, int tempSwitchCount);
void parDefault(Token& token, int tempSwitchCount);
void parReturnStatement(Token& token);
//

void parExpr(Token& token);
void parItem(Token& token);
void parFactor(Token& token);
void parMainFunction(Token& token);


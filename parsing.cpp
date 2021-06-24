#include <algorithm>
#include <iostream>
#include "Token.h"
#include "getWriteAndFind.h"
#include "parsing.h"
#include "symbolTable.h"
#include "quadruaple.h"

#define next(b)             \
	if (token.symbol == b)  \
	{                       \
		getAndWrite(token); \
	}
#define writeError(a) error(token.lineNum, a)

int funCount = 0;  // fun递归计数
int funCount2 = 0; // fun递归计数
int tempCount = 0;
int ifCount = 0;
int whileCount = 0;
int forCount = 0;
int switchCount = 0;
int caseCount = 0;
string exprType;					 // 每一个表达式的类型
bool isHasReturn = false;			 // 标记是否出现return语句
vector<SymbolTableRow> globalTables; // 全局表
vector<SymbolTableRow> funTables;	 // 函数表
SymbolTableRow temp;				 // 全局表与函数表中间变量
SymbolTableRow funTemp;				 // 函数声明中间变量
vector<SymbolTableRow>* curTables;	 // 指向当前所在表
vector<Quadruaple> quadruaples;

void error(int line, string message)
{
	writeFile(line, message);
}

void getAndWrite(Token& token)
{
	writeFile(token);
	getNext(token);
}

void getNext(Token& token)
{
	if (token.index < token.length)
	{
		getSym(token, token.length);
	}
	else
	{
		token.symbol = "";
	}
}
//分析入口
void parsingProgram(Token& token)
{
	int index, line;
	curTables = &globalTables;
	parConstantDes(token);
	saveNow(token, index, line);
	if (isVarDes(token))
	{
		goBack(token, index, line);
		parVariableDes(token);
		parFunction(token);
	}
	else
	{
		goBack(token, index, line);
		parFunction(token);
	}
	parMainFunction(token);
	writeFile("<程序>");
}
// 常量声明
void parConstantDes(Token& token)
{
	int index, line;
	saveNow(token, index, line);
	getNext(token);
	if (token.symbol != "CONSTTK")
	{
		goBack(token, index, line);
		return;
	}
	goBack(token, index, line);
	parConstantDesLoop(token);
	writeFile("<常量说明>");
}

void parConstantDesLoop(Token& token)
{
	int index, line;
	saveNow(token, index, line);
	getNext(token);
	if (token.symbol != "CONSTTK")
	{
		goBack(token, index, line);
		return;
	}
	writeFile(token);
	temp.type = "const";
	parConstantDef(token);
	saveNow(token, index, line);
	getNext(token);
	if (token.symbol == "SEMICN")
	{
		writeFile(token);
		parConstantDesLoop(token);
	}
	else
	{
		goBack(token, index, line);
		writeError("k");
		parConstantDesLoop(token);
	}
}

void parConstantDef(Token& token)
{
	getNext(token);
	if (token.symbol == "INTTK")
	{
		writeFile(token);
		temp.symbol = token.symbol;
		parIdenEqlInt(token);
	}
	else if (token.symbol == "CHARTK")
	{
		writeFile(token);
		temp.symbol = token.symbol;
		parIdenEqlChar(token);
	}
	writeFile("<常量定义>");
}

//const int＜标识符＞＝＜整数＞{ ,＜标识符＞＝＜整数＞ }

void parIdenEqlInt(Token& token)
{
	int index, line;
	getNext(token);
	string op;
	if (token.symbol == "IDENFR")
	{
		temp.name = token.word;
		getAndWrite(token);
	}
	if (token.symbol == "ASSIGN")
	{
		getAndWrite(token);
	}
	if (token.symbol == "PLUS" || token.symbol == "MINU" || token.symbol == "INTCON")
	{
		if (token.symbol == "PLUS")
		{
			op = "+";
		}
		else if (token.symbol == "MINU")
		{
			op = "-";
		}
		else
		{
			op = "";
		}
	}
	parSignInteger(token);
	temp.value = op + token.word; // add value
	temp.isKonwn = true;
	if (!findIsDef(temp))
	{
		// 没有定义 加进符号表
		curTables->push_back(temp);
	}
	else
	{
		writeError("b"); //重定义常量
	}
	// gen si yuan
	quadruaples.push_back(Quadruaple(QuadOperator::constDef, temp));
	temp.clear();
	saveNow(token, index, line);
	getNext(token);
	if (token.symbol == "COMMA")
	{
		writeFile(token);
		saveTypeAndSym(temp, "const", "INTTK");
		parIdenEqlInt(token);
	}
	else
	{
		goBack(token, index, line);
	}
}

//const char＜标识符＞＝＜字符＞{ ,＜标识符＞＝＜字符＞ }

void parIdenEqlChar(Token& token)
{
	int index, line;
	getNext(token);
	if (token.symbol == "IDENFR")
	{
		temp.name = token.word;
		getAndWrite(token);
	}
	if (token.symbol == "ASSIGN")
	{
		getAndWrite(token);
	}

	if (token.symbol == "CHARCON")
	{
		if (!isRightCharCon(token))
		{
			writeError("a");
		}
		temp.value = "\'" + token.word + "\'"; // add value
		temp.isKonwn = true;
		if (!findIsDef(temp))
		{
			curTables->push_back(temp);
		}
		else
		{
			writeError("b");
		}
		// gen si yuan
		quadruaples.push_back(Quadruaple(QuadOperator::constDef, temp));
		temp.clear();
		saveNow(token, index, line);
		getAndWrite(token);
	}
	if (token.symbol == "COMMA")
	{
		writeFile(token);
		saveTypeAndSym(temp, "const", "CHARTK");
		parIdenEqlChar(token);
	}
	else
	{
		goBack(token, index, line);
	}
}

//整数

void parSignInteger(Token& token)
{
	if (token.symbol == "PLUS" || token.symbol == "MINU")
	{
		getAndWrite(token);
		parUnsignInteger(token);
		writeFile("<整数>");
	}
	else
	{
		parUnsignInteger(token);
		writeFile("<整数>");
	}
}

//无符号整数

void parUnsignInteger(Token& token)
{
	if (token.symbol == "INTCON")
	{
		writeFile(token);
		writeFile("<无符号整数>");
	}
	else
	{
		writeError("i");
	}
}

//变量声明

void parVariableDes(Token& token)
{
	parVariableDesLoop(token);
	writeFile("<变量说明>");
}

//变量定义

void parVariableDesLoop(Token& token)
{
	int index, line;
	parVariableDef(token);
	saveNow(token, index, line);
	getNext(token);
	if (token.symbol == "SEMICN")
	{
		writeFile(token);
		saveNow(token, index, line);
		if (isVarDes(token))
		{
			goBack(token, index, line);
			parVariableDesLoop(token);
		}
		else
		{
			goBack(token, index, line);
		}
	}
	else
	{
		goBack(token, index, line);
		writeError("k");
		saveNow(token, index, line);
		if (isVarDes(token))
		{
			goBack(token, index, line);
			parVariableDesLoop(token);
		}
		else
		{
			goBack(token, index, line);
		}
	}
}

void parVariableDef(Token& token)
{
	int index, line;
	saveNow(token, index, line);
	getNext(token);
	if (token.symbol == "CHARTK" || token.symbol == "INTTK")
	{
		goBack(token, index, line);
		temp.type = "var";
		if (isInit(token))
		{
			parVariableDefInit(token);
		}
		else
		{
			parVariableDefNoInit(token);
		}
		writeFile("<变量定义>");
	}
	else
	{
		goBack(token, index, line);
	}
}

//变量定义无初始化

void parVariableDefNoInit(Token& token)
{
	getNext(token);
	if (token.symbol == "CHARTK" || token.symbol == "INTTK")
	{
		writeFile(token);
		temp.symbol = token.symbol;
		parVarDefNoInitLoop(token);
	}
	writeFile("<变量定义无初始化>");
}

void parVarDefNoInitLoop(Token& token)
{
	int index, line;
	string symbolTemp = temp.symbol;
	getNext(token);
	if (token.symbol == "IDENFR")
	{
		temp.name = token.word;
		saveNow(token, index, line);
		getAndWrite(token);
	}
	if (token.symbol == "LBRACK")
	{
		temp.array.arrSymbol = symbolTemp; //先保存array的类型 从上方继承下来的
		temp.symbol = "ARRTK";			   // 覆盖之前的symbol 表示其为数组类型
		temp.array.dim = 1;
		getAndWrite(token);
		parUnsignInteger(token);
		temp.array.length1 = stoi(token.word);
		saveNow(token, index, line);
		getNext(token);
		if (token.symbol == "RBRACK")
		{
			writeFile(token);
			saveNow(token, index, line);
			getNext(token);
		}
		else
		{
			writeError("m");
			goBack(token, index, line);
			getNext(token);
		}
	}
	if (token.symbol == "LBRACK")
	{
		//进行到这里的时候 已经在上一步保存了 其为array类型在symbol里面
		//覆盖其维度与length2即可
		temp.array.dim = 2;
		getAndWrite(token);
		parUnsignInteger(token);
		temp.array.length2 = stoi(token.word);
		saveNow(token, index, line);
		getNext(token);
		if (token.symbol == "RBRACK")
		{
			writeFile(token);
			saveNow(token, index, line);
			getNext(token);
		}
		else
		{
			goBack(token, index, line);
			writeError("m");
			getNext(token);
		}
	}
	if (!findIsDef(temp))
	{
		curTables->push_back(temp);
	}
	else
	{
		writeError("b");
	}
	// gen si yuan
	quadruaples.push_back(Quadruaple(QuadOperator::varDefNoInit, temp));
	temp.clear();
	if (token.symbol == "COMMA")
	{
		writeFile(token);
		saveTypeAndSym(temp, "var", symbolTemp);
		parVarDefNoInitLoop(token);
	}
	else
	{
		goBack(token, index, line);
	}
}

//变量定义及初始化

void parVariableDefInit(Token& token)
{
	getNext(token);
	int index, line;
	if (token.symbol == "CHARTK" || token.symbol == "INTTK")
	{
		temp.symbol = token.symbol;
		getAndWrite(token);
	}
	if (token.symbol == "IDENFR")
	{
		temp.name = token.word;
		getAndWrite(token);
	}
	if (token.symbol == "ASSIGN")
	{
		getAndWrite(token);
		parConstant(token);
		if (token.symbol.substr(0, 3) != temp.symbol.substr(0, 3))
		{
			writeError("o");
		}
		// save value
		saveNow(token, index, line);
		getNext(token);
	}
	if (token.symbol == "LBRACK")
	{
		temp.array.arrSymbol = temp.symbol;
		temp.symbol = "ARRTK";
		temp.array.dim = 1;
		getAndWrite(token);
		parUnsignInteger(token);
		temp.array.length1 = stoi(token.word);
		saveNow(token, index, line);
		getNext(token);
		if (token.symbol == "RBRACK")
		{
			getAndWrite(token);
		}
		else
		{
			goBack(token, index, line);
			writeError("m");
			getNext(token);
		}
		if (token.symbol == "ASSIGN")
		{
			getAndWrite(token);
			if (token.symbol == "LBRACE")
			{
				getAndWrite(token);
			}
			if (token.symbol == "RBRACE") {
				saveNow(token, index, line);
				writeError("n");
				getAndWrite(token);
			}
			else {
				funCount = 0;
				parConstantLoop(token);
				if (funCount != temp.array.length1)
				{
					writeError("n");
				}
				if (token.symbol == "RBRACE")
				{
					saveNow(token, index, line);
					writeFile(token);
					getNext(token);
				}
			}
			
		}
	}
	if (token.symbol == "LBRACK")
	{
		temp.array.dim = 2;
		getAndWrite(token);
		parUnsignInteger(token);
		temp.array.length2 = stoi(token.word);
		saveNow(token, index, line);
		getNext(token);
		if (token.symbol == "RBRACK")
		{
			getAndWrite(token);
		}
		else
		{
			goBack(token, index, line);
			writeError("m");
			getNext(token);
		}
		if (token.symbol == "ASSIGN")
		{
			getAndWrite(token);
			if (token.symbol == "LBRACE")
			{
				getAndWrite(token);
			}
			if (token.symbol == "RBRACE") {
				saveNow(token, index, line);
				writeError("n");
				getAndWrite(token);
			}
			else {
				funCount2 = 0;
				parConstantTwoLoop(token);
				if (funCount2 != temp.array.length1)
				{
					writeError("n");
				}
				if (token.symbol == "RBRACE")
				{
					saveNow(token, index, line);
					writeFile(token);
					getNext(token);
				}
			}
			
		}
	}
	if (!findIsDef(temp))
	{
		curTables->push_back(temp);
	}
	else
	{
		writeError("b");
	}
	// saved except Array????
	// gen si yuan
	quadruaples.push_back(Quadruaple(QuadOperator::varDefInit, temp));
	temp.clear();
	if (token.symbol == "SEMICN")
	{
		goBack(token, index, line);
		writeFile("<变量定义及初始化>");
	}
}

//常量 用在 变量定义及初始化上

void parConstant(Token& token)
{
	string op = "";
	if (token.symbol == "CHARCON")
	{
		if (!isRightCharCon(token))
		{
			writeError("a");
		}
		else
		{
			writeFile(token);
			writeFile("<常量>");
		}
		temp.value += "\'" + token.word + "\'";
	}
	else
	{
		if (token.symbol == "PLUS" || token.symbol == "MINU" || token.symbol == "INTCON")
		{
			if (token.symbol == "PLUS")
			{
				op = "+";
			}
			else if (token.symbol == "MINU")
			{
				op = "-";
			}
			else
			{
				op = "";
			}
			temp.value += op;
		}
		parSignInteger(token);
		temp.value += token.word;
		writeFile("<常量>");
	}
}

//一维数组初始化 常量循环

void parConstantLoop(Token& token)
{
	parConstant(token);
	temp.value += " ";
	if (temp.symbol == "ARRTK")
	{
		if (token.symbol.substr(0, 3) != temp.array.arrSymbol.substr(0, 3))
		{
			writeError("o");
		}
	}
	else
	{
		if (token.symbol.substr(0, 3) != temp.array.arrSymbol.substr(0, 3))
		{
			writeError("o");
		}
	}
	funCount++;
	getNext(token);
	if (token.symbol == "COMMA")
	{
		
		getAndWrite(token);
		parConstantLoop(token);
	}
}

//二维数组初始化 常量循环

void parConstantTwoLoop(Token& token)
{
	if (token.symbol == "LBRACE")
	{
		getAndWrite(token);
	}
	if (token.symbol == "RBRACE")
	{
		writeError("n");
	}
	else {
		funCount = 0;
		parConstantLoop(token);
		if (funCount != temp.array.length2)
		{
			writeError("n");
		}
		if (token.symbol == "RBRACE")
		{
			getAndWrite(token);
		}
	}
	funCount2++;
	if (token.symbol == "COMMA")
	{
		getAndWrite(token);
		parConstantTwoLoop(token);
	}
}

//是否是变量初始化定义

bool isInit(Token& token)
{
	bool flag = false;
	int i = token.index;
	while (1)
	{
		if (i >= token.file.length() || token.file.at(i) == ';')
		{
			break;
		}
		if (token.file.at(i) == '=')
		{
			flag = true;
			break;
		}
		i++;
	}
	return flag;
}

//是否为变量声明

bool isVarDes(Token& token)
{
	getNext(token);
	if (token.symbol == "CHARTK" || token.symbol == "INTTK")
	{
		getSym(token, token.length);
	}
	else
	{
		return false;
	}
	if (token.symbol == "IDENFR")
	{
		getSym(token, token.length);
	}
	if (token.symbol == "LPARENT")
	{
		return false;
	}
	else
	{
		return true;
	}
}

// 函数
// temp在分析函数的时候 存储最外层的符号表，同时根据参数列表的设置 进行寻找添加函数的参数类型与参数数量
//在函数内层的符号表建立的过程中 使用 funTemp这个全局变量 来保存与添加内层函数符号表建立过程。

void parFunction(Token& token)
{
	int index, line;
	funTables.clear();
	saveNow(token, index, line);
	getNext(token);
	temp.type = "fun";
	if (token.symbol == "VOIDTK")
	{
		getNext(token);
		if (token.symbol == "MAINTK")
		{
			goBack(token, index, line);
			return;
		}
		goBack(token, index, line);
		parVoidFunction(token);
		quadruaples.push_back(Quadruaple( QuadOperator::funEnd));
		parFunction(token);
	}
	else
	{
		goBack(token, index, line);
		isHasReturn = false;
		parReturnFunction(token);
		if (!isHasReturn)
		{
			writeError("h");
		}
		quadruaples.push_back(Quadruaple(QuadOperator::funEnd));
		parFunction(token);
	}
}

void parVoidFunction(Token& token)
{
	string funName;
	int index, line;
	getNext(token);
	if (token.symbol == "VOIDTK")
	{
		temp.symbol = token.symbol;
		getAndWrite(token);
	}
	if (token.symbol == "IDENFR")
	{
		funName = token.word;
		token.voidFun.push_back(funName);
		temp.name = token.word;
		getAndWrite(token);
	}
	quadruaples.push_back(Quadruaple(QuadOperator::fun, temp)); // 函数定义四元式
	if (token.symbol == "LPARENT")
	{
		writeFile(token);
		curTables = &funTables;
		parParametersTable(token);
		curTables = &globalTables;
		if (!findIsDef(temp))
		{
			curTables->push_back(temp);
		}
		else
		{
			writeError("b");
		}
		temp.clear();
		saveNow(token, index, line);
		getNext(token);
	}
	if (token.symbol == "RPARENT")
	{
		getAndWrite(token);
	}
	else
	{
		goBack(token, index, line);
		writeError("l");
		getNext(token);
	}
	if (token.symbol == "LBRACE")
	{
		writeFile(token);
		curTables = &funTables; // 更改当前的tables
		parCompoundStatement(token);
		curTables->clear();
		curTables = &globalTables;
		getNext(token);
	}
	if (token.symbol == "RBRACE")
	{
		
		writeFile(token);
		writeFile("<无返回值函数定义>");
	}
}

void parReturnFunction(Token& token)
{
	int index, line;
	parDeclarationHeader(token);
	quadruaples.push_back(Quadruaple(QuadOperator::fun, temp));
	string funName = token.word;
	token.returnFun.push_back(funName);
	getNext(token);
	if (token.symbol == "LPARENT")
	{
		writeFile(token);
		curTables = &funTables;
		parParametersTable(token);
		curTables = &globalTables;
		if (!findIsDef(temp))
		{
			curTables->push_back(temp);
		}
		else
		{
			writeError("b");
		}
		temp.clear();
		saveNow(token, index, line);
		getNext(token);
	}
	if (token.symbol == "RPARENT")
	{
		getAndWrite(token);
	}
	else
	{
		goBack(token, index, line);
		writeError("l");
		getNext(token);
	}
	if (token.symbol == "LBRACE")
	{
		writeFile(token);
		curTables = &funTables; // 更改当前的tables
		parCompoundStatement(token);
		curTables->clear();
		curTables = &globalTables;
		getNext(token);
	}
	if (token.symbol == "RBRACE")
	{
		
		writeFile(token);
		writeFile("<有返回值函数定义>");
	}
}

void parDeclarationHeader(Token& token)
{
	getNext(token);
	if (token.symbol == "INTTK" || token.symbol == "CHARTK")
	{
		temp.symbol = token.symbol;
		getAndWrite(token);
	}
	if (token.symbol == "IDENFR")
	{
		temp.name = token.word;
		writeFile(token);
		writeFile("<声明头部>");
	}
}

void parParametersTable(Token& token)
{
	int index, line;
	saveNow(token, index, line);
	getNext(token);
	if (token.symbol == "RPARENT")
	{
		goBack(token, index, line);
		writeFile("<参数表>"); //  空参数表
	}
	else
	{
		goBack(token, index, line);
		parParametersTableLoop(token);
		writeFile("<参数表>");
	}
}

void parParametersTableLoop(Token& token)
{
	int index = token.index, line = token.lineNum;
	getNext(token);
	if (token.symbol == "INTTK" || token.symbol == "CHARTK")
	{
		temp.funParameters.push_back(token.symbol); // 将参数加到外层函数参数表里面
		funTemp.symbol = token.symbol;
		getAndWrite(token);
	}
	if (token.symbol == "IDENFR")
	{
		funTemp.name = token.word;
		saveNow(token, index, line);
		getAndWrite(token);
	}
	quadruaples.push_back(Quadruaple(QuadOperator::funParameterDef, funTemp));
	if (!findIsDef(funTemp))
	{
		curTables->push_back(funTemp); // 加到内层
	}
	else
	{
		writeError("b");
	}
	funTemp.clear();
	if (token.symbol == "COMMA")
	{
		writeFile(token);
		parParametersTableLoop(token);
	}
	else
	{
		goBack(token, index, line);
	}
}

//复合语句

void parCompoundStatement(Token& token)
{
	int index, line;
	parConstantDes(token);
	saveNow(token, index, line);
	if (isVarDes(token))
	{
		goBack(token, index, line);
		parVariableDes(token);
		parStatementList(token);
		writeFile("<复合语句>");
	}
	else
	{
		goBack(token, index, line);
		parStatementList(token);
		writeFile("<复合语句>");
	}
}

void parStatementList(Token& token)
{
	int index, line;
	saveNow(token, index, line);
	getNext(token);
	if (token.symbol == "RBRACE")
	{
		goBack(token, index, line);
		writeFile("<语句列>");
	}
	else
	{
		goBack(token, index, line);
		parStatement(token);
		parStatementList(token);
	}
}

void endOfStatement(Token& token)
{
	int index, line;
	saveNow(token, index, line);
	getNext(token);
	if (token.symbol == "SEMICN")
	{
		writeFile(token);
		writeFile("<语句>");
	}
	else
	{
		goBack(token, index, line);
		writeError("k");
	}
}

//语句

void parStatement(Token& token)
{
	int index, line;
	saveNow(token, index, line);
	getNext(token);
	if (token.symbol == "WHILETK" || token.symbol == "FORTK")
	{
		goBack(token, index, line);
		parLoopStatement(token);
		writeFile("<语句>");
	}
	else if (token.symbol == "IFTK")
	{
		goBack(token, index, line);
		parCondStatement(token);
		writeFile("<语句>");
	}
	else if (token.symbol == "IDENFR")
	{
		getNext(token);
		if (token.symbol == "ASSIGN")
		{
			goBack(token, index, line);
			parAssignStatement(token);
			endOfStatement(token);
			return;
		}
		else if (token.symbol == "LBRACK")
		{
			goBack(token, index, line);
			parAssignStatement(token);
			endOfStatement(token);
			return;
		}
		else if (token.symbol == "LPARENT")
		{
			goBack(token, index, line);
			parCallFunctionStatement(token);
			endOfStatement(token);
		}
	}
	else if (token.symbol == "SCANFTK")
	{
		goBack(token, index, line);
		parScanfStatement(token);
		endOfStatement(token);
	}
	else if (token.symbol == "PRINTFTK")
	{
		goBack(token, index, line);
		parPrintfStatement(token);
		endOfStatement(token);
	}
	else if (token.symbol == "SWITCHTK")
	{
		goBack(token, index, line);
		parSwitchStatement(token);
		writeFile("<语句>");
	}
	else if (token.symbol == "SEMICN")
	{
		writeFile(token);
		writeFile("<语句>");
	}
	else if (token.symbol == "RETURNTK")
	{
		goBack(token, index, line);
		isHasReturn = true;
		parReturnStatement(token);
		endOfStatement(token);
	}
	else if (token.symbol == "LBRACE")
	{
		writeFile(token);
		parStatementList(token);
		saveNow(token, index, line);
		getNext(token);
		if (token.symbol == "RBRACE")
		{
			writeFile(token);
			writeFile("<语句>");
		}
	}
	else
	{
		goBack(token, index, line);
		writeError("k");
	}
}

//循环语句
// while '('＜条件＞')'＜语句＞
// for'('＜标识符＞＝＜表达式＞;＜条件＞;＜标识符＞＝＜标识符＞(+|-)＜步长＞')'＜语句＞

void parLoopStatement(Token& token)
{
	int index, line;
	int tempWhileCount;
	getNext(token);
	if (token.symbol == "WHILETK")
	{
		whileCount++;
		tempWhileCount= whileCount;
		quadruaples.push_back(Quadruaple(QuadOperator::whileBegin, SymbolTableRow(to_string(tempWhileCount))));
		getAndWrite(token);
		if (token.symbol == "LPARENT")
		{
			writeFile(token);
			parCondition(token);
			saveNow(token, index, line);
			getNext(token);
		}
		if (token.symbol == "RPARENT")
		{
			writeFile(token);
			parStatement(token);
		}
		else
		{
			goBack(token, index, line);
			writeError("l");
			parStatement(token);
		}
		quadruaples.push_back(Quadruaple(QuadOperator::whileEnd, SymbolTableRow(to_string(tempWhileCount))));
	}
	else if (token.symbol == "FORTK")
	{	
		forCount++;
		int tempForCount = forCount;
		Quadruaple quad;
		writeFile(token);
		getNext(token);
		next("LPARENT");
		if (token.symbol == "IDENFR")
		{
			quad.op = QuadOperator::equal;
			quad.num1 = findAll(token.word);
			if (findAllIsDef(token.word) == "")
			{
				writeError("c");
			}
			getAndWrite(token);
		}
		if (token.symbol == "ASSIGN")
		{
			writeFile(token);
			parExpr(token);
			quad.num2 = quadruaples.back().result;
			quadruaples.push_back(quad);
			saveNow(token, index, line);
			getNext(token);
		}
		quadruaples.push_back(Quadruaple(QuadOperator::forBegin, SymbolTableRow(to_string(tempForCount))));
		if (token.symbol == "SEMICN")
		{
			writeFile(token);
			parCondition(token);
			saveNow(token, index, line);
			getNext(token);
		}
		
		else
		{
			token.index = index;
			writeError("k");
			parCondition(token);
			saveNow(token, index, line);
			getNext(token);
		}
		if (token.symbol == "SEMICN")
		{
			getAndWrite(token);
		}
		else
		{
			writeError("k");
		}
		quad.clear();
		if (token.symbol == "IDENFR")
		{
			SymbolTableRow res = findAll(token.word);
			quad.result = res; //被赋值的
			if (res.name == "")
			{
				writeError("c");
			}
			else if (res.type == "const")
			{
				writeError("j");
			}
			getAndWrite(token);
		}
		next("ASSIGN");
		if (token.symbol == "IDENFR")
		{
			SymbolTableRow res = findAll(token.word);
			quad.num1 = res;
			if (res.name == "")
			{
				writeError("c");
			}
			getAndWrite(token);
		}
		if (token.symbol == "PLUS" || token.symbol == "MINU")
		{
			string value = token.word;
			writeFile(token);
			parStride(token);
			value = value + token.word;
			quad.num2 = SymbolTableRow(value);
			quad.op = QuadOperator::forStride;
			
			saveNow(token, index, line);
			getNext(token);
		}
		if (token.symbol == "RPARENT")
		{
			writeFile(token);
			parStatement(token);
			quadruaples.push_back(quad);
		}
		else
		{
			goBack(token, index, line);
			writeError("l");
			parStatement(token);
		}
		quadruaples.push_back(Quadruaple(QuadOperator::forEnd, SymbolTableRow(to_string(tempForCount))));
	}
	writeFile("<循环语句>");
}

void parStride(Token& token)
{
	getNext(token);
	parUnsignInteger(token);
	writeFile("<步长>");
}

//???先读入之后 直接输出 似乎仅仅适合这次无错误的作业 如果读入之后 出现错误，但是错误的已经输出了，就需要
//进行回溯。也就是前面全部都错了。。。

/*10天后：
回上一条 错误处理不需要输出词法语法分析的结果 输出不需要担心
但是我还是更改了全部逻辑：先判断symbol是否合法 再考虑是否进行输出的问题 毕竟错误处理是需要靠输出来debug的（捂脸）
*/

//＜条件语句＞  ::= if '('＜条件＞')'＜语句＞［else＜语句＞］

void parCondStatement(Token& token)
{
	ifCount++;
	int index, line;
	int tempIfCount = ifCount;
	getNext(token);
	next("IFTK");
	if (token.symbol == "LPARENT")
	{
		quadruaples.push_back(Quadruaple(QuadOperator::ifBegin,
			SymbolTableRow(to_string(tempIfCount))));
		writeFile(token);
		parCondition(token);
		saveNow(token, index, line);
		getNext(token);
	}
	if (token.symbol == "RPARENT")
	{
		writeFile(token);
		parStatement(token);
	}
	else
	{
		goBack(token, index, line);
		writeError("l");
		parStatement(token);
	}

	saveNow(token, index, line);
	getNext(token);
	if (token.symbol == "ELSETK")
	{
		quadruaples.push_back(Quadruaple(QuadOperator::ifEnd,
			SymbolTableRow(to_string(tempIfCount), "", "", "1", false)));
		writeFile(token);
		parStatement(token);
		quadruaples.push_back(Quadruaple(QuadOperator::elseEnd,
			SymbolTableRow(to_string(tempIfCount))));
	}
	else
	{
		quadruaples.push_back(Quadruaple(QuadOperator::ifEnd,
			SymbolTableRow(to_string(tempIfCount), "", "", "0", false)));
		goBack(token, index, line);
	}
	writeFile("<条件语句>");
}

//条件
//＜表达式＞＜关系运算符＞＜表达式＞

void parCondition(Token& token)
{
	Quadruaple back1 = quadruaples.back();
	Quadruaple quad;
	parExpr(token);
	quad.num1 = quadruaples.back().result;
	if (exprType == "CHARTK")
	{
		writeError("f");
	}
	quad.op = parRelationalOp(token, quad);
	parExpr(token);
	quad.num2 = quadruaples.back().result;
	if (exprType == "CHARTK")
	{
		writeError("f");
	}
	if (back1.op == QuadOperator::ifBegin)
	{
		quad.result = SymbolTableRow("ifEndLabel" + to_string(ifCount));
	}
	else if (back1.op == QuadOperator::whileBegin)
	{
		quad.result = SymbolTableRow("whileEndLabel" + to_string(whileCount));
	}
	else
	{
		quad.result = SymbolTableRow("forEndLabel" + to_string(forCount));
	}
	quadruaples.push_back(quad);
	writeFile("<条件>");
}

QuadOperator parRelationalOp(Token& token, Quadruaple quad)
{
	getNext(token);
	if (token.symbol == "LSS" || token.symbol == "LEQ" || token.symbol == "GRE" || token.symbol == "GEQ" || token.symbol == "NEQ" || token.symbol == "EQL")
	{
		writeFile(token);
		return conditionQuadruaple(token.symbol);
	}
}

QuadOperator conditionQuadruaple(const string symbol)
{
	if (symbol == "LSS") // <
	{
		return QuadOperator::conditionBge;
	}
	else if (symbol == "LEQ") // <=
	{
		return QuadOperator::conditionBgt;
	}
	else if (symbol == "GRE") // >
	{
		return QuadOperator::conditionBle;
	}
	else if (symbol == "GEQ") // >=
	{
		return QuadOperator::conditionBlt;
	}
	else if (symbol == "NEQ") // !=
	{
		return QuadOperator::conditionBeq;
	}
	else if (symbol == "EQL") // ==
	{
		return QuadOperator::conditionBne;
	}
}

void parCallFunctionStatement(Token& token)
{
	getNext(token);
	if (find(token.returnFun.begin(), token.returnFun.end(), token.word) != token.returnFun.end())
	{
		writeFile(token);
		parCallReturnStatement(token);
	}
	else if (find(token.voidFun.begin(), token.voidFun.end(), token.word) != token.voidFun.end())
	{
		writeFile(token);
		parCallVoidStatement(token);
	}
	else
	{
		writeError("c");
	}
}

void parCallVoidStatement(Token& token)
{

	int index, line;
	string funName = token.word;
	vector<string> paraList = getFunParLen(token.word);
	getNext(token);
	if (token.symbol == "LPARENT")
	{
		writeFile(token);
		funCount = 0;
		quadruaples.push_back(Quadruaple(QuadOperator::callBegin, SymbolTableRow(funName)));
		parValueTable(token, paraList, funName);
		if (funCount != paraList.size())
		{
			writeError("d");
		}
		quadruaples.push_back(Quadruaple(QuadOperator::callEnd, SymbolTableRow(funName)));
		saveNow(token, index, line);
		getNext(token);
	}
	if (token.symbol == "RPARENT")
	{
		writeFile(token);
		writeFile("<无返回值函数调用语句>");
	}
	else
	{
		goBack(token, index, line);
		writeError("l");
		writeFile("<无返回值函数调用语句>");
	}
}

void parCallReturnStatement(Token& token)
{

	int index, line;
	string funName = token.word;
	saveNow(token, index, line);
	vector<string> paraList = getFunParLen(token.word);
	getNext(token);
	if (token.symbol == "LPARENT")
	{
		writeFile(token);
		funCount = 0;
		quadruaples.push_back(Quadruaple(QuadOperator::callBegin, SymbolTableRow(funName)));
		parValueTable(token, paraList, funName);
		if (funCount != paraList.size())
		{
			writeError("d");
		}
		quadruaples.push_back(Quadruaple(QuadOperator::callEnd, SymbolTableRow(funName))); //调用函数
		saveNow(token, index, line);
		getNext(token);
	}
	if (token.symbol == "RPARENT")
	{
		writeFile(token);
		writeFile("<有返回值函数调用语句>");
	}
	else
	{
		goBack(token, index, line);
		writeError("l");
		writeFile("<有返回值函数调用语句>");
	}
}

//＜值参数表＞::= ＜表达式＞{,＜表达式＞}｜＜空＞

void parValueTable(Token& token, vector<string> paraList, string funName)
{
	int index, line;
	saveNow(token, index, line);
	getNext(token);
	if (token.symbol == "RPARENT")
	{
		goBack(token, index, line);
		writeFile("<值参数表>");
	}
	else if (token.symbol == "SEMICN")
	{
		goBack(token, index, line);
	}
	else
	{
		goBack(token, index, line);
		int funCountTemp = funCount;
		parExpr(token);
		quadruaples.push_back(Quadruaple(QuadOperator::funValueTableRow, quadruaples.back().result, SymbolTableRow(funName)));
		funCount = funCountTemp;
		if (paraList.size() != 0)
		{
			if (funCount < paraList.size() && paraList[funCount] != exprType)
			{
				writeError("e");
			} //定义三个 调用时使用四个 会出现越界的情况。
			else if (funCount > paraList.size())
			{
				writeError("e");
			}
		}
		funCount++;
		saveNow(token, index, line);
		getNext(token);
		if (token.symbol == "COMMA")
		{
			writeFile(token);
			parValueTable(token, paraList, funName);
		}
		else
		{
			goBack(token, index, line);
			parValueTable(token, paraList, funName);
		}
	}
}

//＜赋值语句＞::=＜标识符＞＝＜表达式＞|
//＜标识符＞'['＜表达式＞']'=＜表达式＞|＜标识符＞'['＜表达式＞']''['＜表达式＞']' =＜表达式＞

void parAssignStatement(Token& token)
{
	getNext(token);
	SymbolTableRow res;
	SymbolTableRow back1;
	SymbolTableRow back2;
	if (token.symbol == "IDENFR")
	{
		res = findAll(token.word);
		if (res.name == "")
		{
			writeError("c");
		}
		else
		{
			if (res.type == "const")
			{
				writeError("j");
			}
		}
		getAndWrite(token);
	}
	if (token.symbol == "ASSIGN")
	{
		writeFile(token);
		parExpr(token);
		quadruaples.push_back(Quadruaple(QuadOperator::equal, res, quadruaples.back().result));
		writeFile("<赋值语句>");
		return;
	}
	if (token.symbol == "LBRACK")
	{
		writeFile(token);
		parExpr(token);
		back1 = quadruaples.back().result;
		if (exprType != "INTTK")
		{
			writeError("i");
		}
		getNext(token);
	}
	if (token.symbol == "RBRACK")
	{
		getAndWrite(token);
	}
	else
	{
		writeError("m");
	}
	if (token.symbol == "ASSIGN")
	{
		writeFile(token);
		parExpr(token);
		quadruaples.push_back(Quadruaple(QuadOperator::equal, res, back1, quadruaples.back().result));
		writeFile("<赋值语句>");
		return;
	}
	if (token.symbol == "LBRACK")
	{
		writeFile(token);
		parExpr(token);
		back2 = quadruaples.back().result;
		if (exprType != "INTTK")
		{
			writeError("i");
		}
		back1.value = res.name + " " + to_string(res.array.length2);
		back1.symbol = "ARRTK"; 
		back1.array.dim = 2;
		//疯狂藏数据
		
		getNext(token);
	}
	if (token.symbol == "RBRACK")
	{
		getAndWrite(token);
	}
	else
	{
		writeError("m");
	}
	if (token.symbol == "ASSIGN")
	{
		writeFile(token);
		parExpr(token);
		quadruaples.push_back(Quadruaple(QuadOperator::equal, back1, back2, quadruaples.back().result));
		writeFile("<赋值语句>");
		return;
	}
}

void parScanfStatement(Token& token)
{
	int index, line;
	getNext(token);
	next("SCANFTK");
	next("LPARENT");
	if (token.symbol == "IDENFR")
	{
		SymbolTableRow res = findAll(token.word);
		if (res.name == "")
		{
			writeError("c");
		}
		else if (res.type == "const")
		{
			writeError("j");
		}
		quadruaples.push_back(Quadruaple(QuadOperator::scanf, res));
		saveNow(token, index, line);
		getAndWrite(token);
	}
	if (token.symbol == "RPARENT")
	{
		writeFile(token);
		writeFile("<读语句>");
	}
	else
	{
		goBack(token, index, line);
		writeError("l");
		writeFile("<读语句>");
	}
}

//＜写语句＞::= printf '(' ＜字符串＞,＜表达式＞ ')'| printf '('＜字符串＞ ')'| printf '('＜表达式＞')'

void parPrintfStatement(Token& token)
{
	int index, line;
	getNext(token);
	next("PRINTFTK");
	Quadruaple printTemp = Quadruaple(QuadOperator::printf);
	if (token.symbol == "LPARENT")
	{
		writeFile(token);
		saveNow(token, index, line);
		getNext(token);
		if (token.symbol == "STRCON")
		{
			goBack(token, index, line);
			parString(token);
			printTemp.num1.value = token.word;
			saveNow(token, index, line);
			getNext(token);
		}
		else
		{
			goBack(token, index, line);
			parExpr(token);
			/*printTemp.num1.value = "";*/
			printTemp.num2 = quadruaples.back().result;
			saveNow(token, index, line);
			getNext(token);
		}
	}
	if (token.symbol == "COMMA")
	{
		writeFile(token);
		parExpr(token);
		//表达式生成的中间代码的最后一项的结果。
		printTemp.num2 = quadruaples.back().result;
		saveNow(token, index, line);
		getNext(token);
	}
	if (token.symbol == "RPARENT")
	{
		quadruaples.push_back(printTemp);
		writeFile(token);
		writeFile("<写语句>");
	}
	else
	{
		goBack(token, index, line);
		writeError("l");
		writeFile("<写语句>");
	}
}

void parString(Token& token)
{
	int index, line;
	getNext(token);
	if (token.symbol == "STRCON")
	{
		if (token.word.size() == 0)
		{
			writeError("a");
		}
		else
		{
			for (int i = 0; i < token.word.size(); ++i)
			{
				if (token.word[i] < 32 || token.word[i] > 126 || token.word[i] == 34)
				{
					writeError("a");
				}
			}
		}
		writeFile(token);
		writeFile("<字符串>");
	}
}

void parSwitchStatement(Token& token)
{
	int index, line;
	int tempSwitchCount = switchCount;
	SymbolTableRow back1;
	getNext(token);
	next("SWITCHTK");
	quadruaples.push_back(Quadruaple(QuadOperator::switchBegin, SymbolTableRow(to_string(tempSwitchCount))));
	if (token.symbol == "LPARENT")
	{
		writeFile(token);
		parExpr(token);
		back1 = quadruaples.back().result;
		temp.symbol = exprType;
		saveNow(token, index, line);
		getNext(token);
	}
	if (token.symbol == "RPARENT")
	{
		getAndWrite(token);
	}
	else
	{
		goBack(token, index, line);
		writeError("l");
		getNext(token);
	}
	quadruaples.push_back(Quadruaple(QuadOperator::switchBegin, SymbolTableRow(to_string(tempSwitchCount))));
	quadruaples.push_back(Quadruaple(QuadOperator::equal, SymbolTableRow("$switch" + to_string(tempSwitchCount)), back1));
	back1 = quadruaples.back().num1;
	
	if (token.symbol == "LBRACE")
	{
		writeFile(token);
		parSituationTable(token, back1, tempSwitchCount);
		saveNow(token, index, line);
		getNext(token);
		if (token.symbol == "DEFAULTTK")
		{
			goBack(token, index, line);
			getNext(token);
			parDefault(token, tempSwitchCount);
			getNext(token);
		}
		else
		{
			writeError("p");
		}
	}
	if (token.symbol == "RBRACE")
	{
		writeFile(token);
		writeFile("<情况语句>");
	}
	quadruaples.push_back(Quadruaple(QuadOperator::switchEnd, SymbolTableRow(to_string(tempSwitchCount))));
	switchCount++;
}

void parSituationTable(Token& token, SymbolTableRow back1, int tempSwitchCount)
{
	int index, line;
	parCaseStatament(token, back1, tempSwitchCount);
	saveNow(token, index, line);
	getNext(token);
	if (token.symbol == "CASETK")
	{
		goBack(token, index, line);
		parSituationTable(token, back1, tempSwitchCount);
	}
	else if (token.symbol == "DEFAULTTK")
	{
		goBack(token, index, line);
		writeFile("<情况表>");
	}
	else
	{
		goBack(token, index, line);
		writeFile("<情况表>");
	}
}

void parCaseStatament(Token& token, SymbolTableRow back1, int tempSwitchCount)
{
	int tempCaseCount = caseCount;
	quadruaples.push_back(Quadruaple(QuadOperator::caseBegin, SymbolTableRow(to_string(tempCaseCount))));
	getNext(token);
	if (token.symbol == "CASETK")
	{
		string value = "";
		getAndWrite(token);
		if (token.symbol == "CHARCON")
		{
			if (!isRightCharCon(token))
			{
				writeError("a");
			}
			else
			{
				writeFile(token);
				writeFile("<常量>");
			}
			value = "\'" + token.word + "\'";
		}
		else
		{
			if (token.symbol == "PLUS" || token.symbol == "MINU")
			{
				if (token.symbol == "PLUS") {
					value += "+";
				}
				else {
					value += "-";
				}
				getAndWrite(token);
				parUnsignInteger(token);
				writeFile("<整数>");
			}
			else
			{
				parUnsignInteger(token);
				writeFile("<整数>");
			}
			value += token.word;
			writeFile("<常量>");
		}
		quadruaples.push_back(Quadruaple(QuadOperator::conditionBne, back1,
			SymbolTableRow(value, "const", "", value, true), SymbolTableRow("caseEndLabel" + to_string(tempCaseCount))));
		if (token.symbol.substr(0, 4) != temp.symbol.substr(0, 4) &&
			token.symbol.substr(0, 3) != temp.symbol.substr(0, 3))
		{
			//token.sym = charcon  or  intcon
			// temp.symbol == intTK or charTk???
			writeError("o");
		}
		getNext(token);
	}
	if (token.symbol == "COLON")
	{
		writeFile(token);
		parStatement(token);
		writeFile("<情况子语句>");
	}
	quadruaples.push_back(Quadruaple(QuadOperator::caseEnd,
		SymbolTableRow(to_string(tempCaseCount)), SymbolTableRow(to_string(tempSwitchCount))));
	caseCount++;
}

void parDefault(Token& token, int tempSwitchCount)
{
	getNext(token);
	next("DEFAULTTK");
	if (token.symbol == "COLON")
	{
		writeFile(token);
		parStatement(token);
		writeFile("<缺省>");
	}
	quadruaples.push_back(Quadruaple(QuadOperator::defaultEnd, SymbolTableRow(to_string(tempSwitchCount))));
}

//＜返回语句＞   ::=  return['('＜表达式＞')']

void parReturnStatement(Token& token)
{
	int index, line;
	getNext(token);
	if (token.symbol == "RETURNTK")
	{
		writeFile(token);
		saveNow(token, index, line);
		getNext(token);
	}
	if (token.symbol == "SEMICN")
	{
		goBack(token, index, line);
		if (globalTables.back().symbol != "VOIDTK")
		{
			writeError("h");
		}
		quadruaples.push_back(Quadruaple(QuadOperator::Return, quadruaples.back().result));
		writeFile("<返回语句>");
		return;
	}
	if (token.symbol == "LPARENT")
	{
		writeFile(token);
		parExpr(token);
		quadruaples.push_back(Quadruaple(QuadOperator::Return, quadruaples.back().result));
		if (exprType != globalTables.back().symbol)
		{
			if (globalTables.back().symbol == "VOIDTK")
			{
				writeError("g");
			}
			else
			{
				writeError("h");
			}
		}
		saveNow(token, index, line);
		getNext(token);
	}
	if (token.symbol == "RPARENT")
	{
		writeFile(token);
		writeFile("<返回语句>");
	}
	else
	{
		goBack(token, index, line);
		writeError("l");
		writeFile("<返回语句>");
	}
}

//＜表达式＞    ::= ［＋｜－］＜项＞{＜加法运算符＞＜项＞}

void parExpr(Token& token)
{
	int index, line;
	SymbolTableRow num1;
	QuadOperator op;
	saveNow(token, index, line);
	getNext(token);
	/*quadruaples.push_back(Quadruaple(QuadOperator::exprBegin));*/
	//得到顶层res 如果有后续 则 作为num1，否则 直接返回上一级
	if (token.symbol == "PLUS" || token.symbol == "MINU")
	{
		if (token.symbol == "PLUS")
		{
			op = QuadOperator::add;
		}
		else
		{
			op = QuadOperator::sub;
		}
		writeFile(token);
		parItem(token);
		num1 = quadruaples.back().result;
		quadruaples.push_back(Quadruaple(op));
		quadruaples.back().num1.name = "";
		quadruaples.back().num2 = num1;
		quadruaples.back().result = SymbolTableRow("@t" + to_string(tempCount++), "", "INTTK", "", false);
		exprType = "INTTK";
	}
	else
	{
		goBack(token, index, line);
		parItem(token);
	}
	num1 = quadruaples.back().result;
	saveNow(token, index, line);
	getNext(token);
	while (token.symbol == "PLUS" || token.symbol == "MINU")
	{
		if (token.symbol == "PLUS")
		{
			op = QuadOperator::add;
		}
		else
		{
			op = QuadOperator::sub;
		}
		writeFile(token);
		parItem(token);
		exprType = "INTTK";
		quadruaples.push_back(Quadruaple(
			op, num1, quadruaples.back().result,
			SymbolTableRow("@t" + to_string(tempCount++), "", exprType, "",false)));
		num1 = quadruaples.back().result;
		saveNow(token, index, line);
		getNext(token);
	}
	/*SymbolTableRow resultTemp = quadruaples.back().result;
	quadruaples.push_back(Quadruaple(QuadOperator::exprEnd));
	quadruaples.back().result = resultTemp;*/
	goBack(token, index, line);
	writeFile("<表达式>");
}

//＜项＞::= ＜因子＞{＜乘法运算符＞＜因子＞}

void parItem(Token& token)
{
	int index, line;
	SymbolTableRow num1;
	parFactor(token);
	num1 = quadruaples.back().result;
	//得到顶层的res 如果有后续使用num1，没有后续直接返回上级
	saveNow(token, index, line);
	getNext(token);
	while (token.symbol == "MULT" || token.symbol == "DIV")
	{
		QuadOperator op;
		if (token.symbol == "MULT")
		{
			op = QuadOperator::mul;
		}
		else
		{
			op = QuadOperator::div;
		}
		writeFile(token);
		exprType = "INTTK";
		parFactor(token);
		//拿到顶层的res 作为num2，并push进去 得到下一层的num1，不如说是更新num1
		quadruaples.push_back(Quadruaple(
			op, num1, quadruaples.back().result,
			SymbolTableRow("@t" + to_string(tempCount++), "", exprType, "", false)));
		num1 = quadruaples.back().result;
		saveNow(token, index, line);
		getNext(token);
	}
	goBack(token, index, line);
	writeFile("<项>");
}

//＜因子＞:: = ＜标识符＞｜＜标识符＞'['＜表达式＞']' | ＜标识符＞'['＜表达式＞']''['＜表达式＞']' |
//'('＜表达式＞')'｜＜整数＞ | ＜字符＞｜＜有返回值函数调用语句＞

void parFactor(Token& token)
{
	int index, line, line1, index1;
	string word;
	Quadruaple quad;
	SymbolTableRow back1;
	SymbolTableRow back2;
	saveNow(token, index, line);
	getNext(token);
	if (token.symbol == "IDENFR")
	{
		word = token.word;
		string res = findAllIsDef(word);
		SymbolTableRow foo = findAll(word);
		if (res == "")
		{
			writeError("c");
		}
		else
		{
			exprType = res;
		}
		saveNow(token, index1, line1);
		writeFile(token);
		getNext(token);
		if (token.symbol == "LPARENT")
		{
			goBack(token, index, line);
			getNext(token);
			parCallReturnStatement(token);
			quad.op = QuadOperator::factorFunCall;
			quad.result = SymbolTableRow("@t" + to_string(tempCount++), "fun", res, "", false);
			quadruaples.push_back(quad);
			exprType = res;
			writeFile("<因子>");
			return;
		}
		else if (token.symbol == "LBRACK")
		{
			writeFile(token);
			parExpr(token); //得到的最后的四元式的结果表项。
			back1 = quadruaples.back().result;
			if (exprType != "INTTK")
			{
				writeError("i");
			}
			exprType = res;
			saveNow(token, index, line);
			getNext(token);
			if (token.symbol == "RBRACK")
			{
				writeFile(token);
				saveNow(token, index, line);
				getNext(token);
			}
			else
			{
				writeError("m");
			}
			if (token.symbol == "LBRACK")
			{
				writeFile(token);
				parExpr(token);
				back2 = quadruaples.back().result;
				if (exprType != "INTTK")
				{
					writeError("i");
				}
				exprType = res;
				saveNow(token, index, line);
				getNext(token);
				if (token.symbol == "RBRACK")
				{
					//二维数组
					quad.op = QuadOperator::factorArrTwo;
					quad.num1 = back1;
					quad.num2 = back2;
					quad.result = SymbolTableRow("@t" + to_string(tempCount++), "var", res, word + " " + to_string(foo.array.length2), false);
					quadruaples.push_back(quad);
					writeFile(token);
					writeFile("<因子>");
				}
				else
				{
					goBack(token, index, line);
					writeError("m");
					writeFile("<因子>");
				}
			}
			else
			{
				//一维数组
				//备选：当数组被用到时进行取值，而不是在第一次访问到数组的时候进行取值，
				//此时 将取数组的信息保存在value中，在上层进行访问时，将数组value的信息展开
				//并进行递归访问取值。考虑到后来的事情，感觉会很难办，
				//所以采用下面的方案：当遇到数组的时候直接进行取值，并保存到中间变量t中，
				//然后存入栈中，在需要访问时直接取出此值即可
				// string value = "";
				// SymbolTableRow backTemp = quadruaples.back().result;
				// if (backTemp.symbol != "ARRTK"){
				// 	value = word + "[" + backTemp.name + "]";
				// }
				// else {
				// 	value += word + "[" + backTemp.value + "]";
				// }
				// quad.op = QuadOperator::factorArrOne;
				// quad.result = SymbolTableRow(word, "var", "ARRTK", value, false);

				quad.op = QuadOperator::factorArrOne;
				quad.num1 = SymbolTableRow(word, "var", "ARRTK");
				quad.num2 = back1;
				quad.result = SymbolTableRow("@t" + to_string(tempCount++), "var", res, "", false);
				quadruaples.push_back(quad);
				goBack(token, index, line);
				writeFile("<因子>");
			}
		}
		else
		{
			//只有 标识符
			//op: factorIden 在mips 里面相当于 得到Iden的值 给 res。
			quad.clear();
			quad.op = QuadOperator::factorIden;
			quad.num1 = SymbolTableRow(word, "var", res);                                        
			if (foo.isKonwn)
			{
				quad.result = SymbolTableRow(foo.value, "const", res, foo.value, true);
			}
			else
			{
				quad.result = SymbolTableRow(word, "var", res, "", false);
			}
			quadruaples.push_back(quad);
			quad.clear();
			goBack(token, index1, line1);
			writeFile("<因子>");
		}
	}
	else if (token.symbol == "LPARENT")
	{
		writeFile(token);
		//在这里没有必要重新创建新的Quadruaple，
		//parExpr会直接产生一个存在quadruaples顶端的元素，在这里返回后直接从最上层取出即可。
		parExpr(token);
		quadruaples.back().result.symbol = "INTTK";
		saveNow(token, index, line);
		getNext(token);
		if (token.symbol == "RPARENT")
		{
			writeFile(token);
			writeFile("<因子>");
		}
		else
		{
			goBack(token, index, line);
			writeError("l");
			writeFile("<因子>");
		}
	}
	else if (token.symbol == "CHARCON")
	{
		if (!isRightCharCon(token))
		{
			writeError("a");
		}
		exprType = "CHARTK";
		quad.op = QuadOperator::factorChar;
		quad.num1 = SymbolTableRow("", "const", exprType, "\'" + token.word + "\'", true);
		quad.result = SymbolTableRow(quad.num1.value, "const", exprType, quad.num1.value, true);
		quadruaples.push_back(quad);
		quad.clear();
		writeFile(token);
		writeFile("<因子>");
	}
	else if (token.symbol == "PLUS" || token.symbol == "MINU" || token.symbol == "INTCON")
	{
		quad.op = QuadOperator::factorInt;
		if (token.symbol == "PLUS")
		{
			quad.num1.value = "+";
		}
		else if (token.symbol == "MINU")
		{
			quad.num1.value = "-";
		}
		else
		{
			quad.num1.value = "";
		}
		parSignInteger(token);
		exprType = "INTTK";
		quad.num1 = SymbolTableRow("", "const", exprType, quad.num1.value + token.word, true);
		quad.result = SymbolTableRow(quad.num1.value, "const", exprType, quad.num1.value, true);
		quadruaples.push_back(quad);
		quad.clear();
		writeFile("<因子>");
	}
	else
	{
		exprType = "";
	}
}

//Main函数

void parMainFunction(Token& token)
{
	int index = 0, line = 0;
	getNext(token);
	next("VOIDTK");
	next("MAINTK");
	globalTables.push_back(SymbolTableRow("main", "fun", "VOIDTK"));
	quadruaples.push_back(Quadruaple(QuadOperator::fun, globalTables.back()));
	if (token.symbol == "LPARENT")
	{
		saveNow(token, index, line);
		getAndWrite(token);
	}
	if (token.symbol == "RPARENT")
	{
		getAndWrite(token);
	}
	else
	{
		goBack(token, index, line);
		writeError("k");
		getNext(token);
	}
	if (token.symbol == "LBRACE")
	{
		writeFile(token);
		curTables = &funTables; // 更改当前的tables
		parCompoundStatement(token);
		curTables->clear();
		curTables = &globalTables;
		getNext(token);
	}
	if (token.symbol == "RBRACE")
	{
		writeFile(token);
		writeFile("<主函数>");
	}
	quadruaples.push_back(Quadruaple(QuadOperator::allEnd));

}

bool isRightCharCon(Token& token)
{
	if (token.word != "+" && token.word != "*" && token.word != "-" && token.word != "/" && token.word != "_" &&
		!isalpha(token.word[0]) && !isdigit(token.word[0]))
	{
		return false;
	}
	return true;
}

void goBack(Token& token, int index, int line)
{
	token.index = index;
	token.lineNum = line;
}

void saveNow(Token& token, int& index, int& line)
{
	index = token.index;
	line = token.lineNum;
}

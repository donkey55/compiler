#pragma once
#include "quadruaple.h"
#include <map>
#include <stack>
#include <queue>
extern vector<Quadruaple> quadruaples;
extern int tempCount;

class GenMips
{

public:


private:
	string nowFun = "global";
	int globalAddr = 0; 
	int localAddr = 0; 
	int stringCount = 0; 

	vector<string> dataInstr;
	vector<string> textInstr;
	vector<int> tCountStack;
	vector<SymbolTableRow> funValueTable;
	
	
	map<string, map<string, string>> funConstMap;
	stack<Quadruaple> funValueTableStack;
	
	map<string, string> globalVarMap; 
	map<string, string> varAddrMap; 
	map<string, SymbolTableRow> funDefMap;
	map<string, string> regToVar;//key : 寄存器名称，value:存储对应的中间的变量
	map<string, string> varToReg;
	vector<string> freeRegQueue;

public:
	GenMips();
	void genBegin();
private:
	bool isGlobal();
	void genGlobalVarInit(Quadruaple globalVar);
	void genGlobalVarNoInit(Quadruaple globalVar);
	void genLocalVarInit(Quadruaple localVar);
	void genLocalVarNoInit(Quadruaple localVar);
	void genFunPara(Quadruaple funPara);
	void genFunCall(Quadruaple);
	void genArrayOne(Quadruaple arrayOneVar);
	void genArrayTwo(Quadruaple arrayTwoVar);
	void genEqual(Quadruaple equalVar);
	void genPrintf(Quadruaple printVar);
	void genScanf(Quadruaple scanfVar);
	void genAddAndSub(Quadruaple calVar, string instr);
	void genDivAndMul(Quadruaple calVar, string instr);
	void insertGlobal(Quadruaple globalVar);
	string getAddr(SymbolTableRow num);
	void getValue(SymbolTableRow num, string reg);
	string allocReg(SymbolTableRow num);
	void freeReg(SymbolTableRow num, string numReg);
	string getReg(SymbolTableRow num);
	vector<string> saveEnv();
	void recoveryEnv(vector<string> saveOrder);
	void initRegs();
};



#include "quadruaple.h"
#include "genMips.h"
#include <iostream>
#include <map>
#include <fstream>
#include <sstream>
#include <stack>

std::ofstream mips("mips.txt");

using namespace std;
void GenMips::genBegin()
{
	string num1Reg;
	string num2Reg;
	string resultReg;
	for (auto temp : quadruaples)
	{
		if (temp.op == QuadOperator::constDef)
		{
			funConstMap[nowFun][temp.num1.name] = temp.num1.value;
		}
		else if (temp.op == QuadOperator::varDefInit)
		{
			if (isGlobal())
			{
				genGlobalVarInit(temp);
			}
			else
			{
				genLocalVarInit(temp);
			}
		}
		else if (temp.op == QuadOperator::varDefNoInit)
		{
			if (isGlobal())
			{
				genGlobalVarNoInit(temp);
			}
			else
			{
				genLocalVarNoInit(temp);
			}
		}
		else if (temp.op == QuadOperator::printf)
		{
			genPrintf(temp);
		}
		else if (temp.op == QuadOperator::scanf)
		{
			genScanf(temp);
		}
		else if (temp.op == QuadOperator::factorArrOne)
		{
			genArrayOne(temp);
		}
		else if (temp.op == QuadOperator::factorArrTwo)
		{
			genArrayTwo(temp);
		}
		else if (temp.op == QuadOperator::factorFunCall)
		{
			//代表有返回值函数的返回，在此之前已经调用了函数，返回值已经被存储到了v0寄存器中。 
			resultReg = allocReg(temp.result);
			textInstr.push_back("move " + resultReg + " $v0");
		}
		else if (temp.op == QuadOperator::add)
		{
			genAddAndSub(temp, "addu");
		}
		else if (temp.op == QuadOperator::sub)
		{
			genAddAndSub(temp, "subu");
		}
		else if (temp.op == QuadOperator::div)
		{
			genDivAndMul(temp, "div");
		}
		else if (temp.op == QuadOperator::mul)
		{
			genDivAndMul(temp, "mul");
		}
		else if (temp.op == QuadOperator::equal)
		{
			genEqual(temp);
		}
		else if (temp.op == QuadOperator::fun)
		{
			nowFun = temp.num1.name;
			funDefMap[nowFun] = temp.num1; //记录当前函数的名字 返回值
			varAddrMap.clear();
			freeRegQueue.clear();
			initRegs();
			localAddr = 0;
			textInstr.push_back("$$" + nowFun + ":");
		}
		else if (temp.op == QuadOperator::funEnd) {
			textInstr.push_back("jr $ra");
		}
		else if (temp.op == QuadOperator::funParameterDef)
		{
			genFunPara(temp);
		}
		else if (temp.op == QuadOperator::callBegin)
		{
			funValueTableStack.push(temp);
		}
		else if (temp.op == QuadOperator::callEnd)
		{
			genFunCall(temp);
		}
		else if (temp.op == QuadOperator::funValueTableRow)
		{
			funValueTableStack.push(temp);
		}
		else if (temp.op == QuadOperator::Return)
		{
			if (funDefMap[nowFun].symbol != "VOIDTK")
			{
				if (temp.num1.name[0] == '@') {
					textInstr.push_back("move $v0 " + varToReg[temp.num1.name]);
					freeReg(temp.num1, varToReg[temp.num1.name]);
					//中间变量用完需要被释放掉。
				}
				else {
					//非中间变量，直接从内存中取出。
					getValue(temp.num1, "$v0");
				}

			}
			if (nowFun == "main") {
				textInstr.push_back("j $$allEnd");
			}
			textInstr.push_back("jr $ra");
		}
		else if (temp.op == QuadOperator::ifBegin)
		{
			//nothing
		}
		//六种条件语句，操作数均为表达式
		else if (temp.op == QuadOperator::conditionBeq)
		{
			num1Reg = getReg(temp.num1);
			num2Reg = getReg(temp.num2);
			textInstr.push_back("beq " + num1Reg + " " + num2Reg + " $$" + temp.result.name);
			freeReg(temp.num1, num1Reg);
			freeReg(temp.num2, num2Reg);
		}
		else if (temp.op == QuadOperator::conditionBge)
		{
			num1Reg = getReg(temp.num1);
			num2Reg = getReg(temp.num2);
			textInstr.push_back("bge " + num1Reg + " " + num2Reg + " $$" + temp.result.name);
			freeReg(temp.num1, num1Reg);
			freeReg(temp.num2, num2Reg);
		}
		else if (temp.op == QuadOperator::conditionBgt)
		{
			num1Reg = getReg(temp.num1);
			num2Reg = getReg(temp.num2);
			textInstr.push_back("bgt " + num1Reg + " " + num2Reg + " $$" + temp.result.name);
			freeReg(temp.num1, num1Reg);
			freeReg(temp.num2, num2Reg);
		}
		else if (temp.op == QuadOperator::conditionBle)
		{
			num1Reg = getReg(temp.num1);
			num2Reg = getReg(temp.num2);
			textInstr.push_back("ble " + num1Reg + " " + num2Reg + " $$" + temp.result.name);
			freeReg(temp.num1, num1Reg);
			freeReg(temp.num2, num2Reg);
		}
		else if (temp.op == QuadOperator::conditionBlt)
		{
			num1Reg = getReg(temp.num1);
			num2Reg = getReg(temp.num2);
			textInstr.push_back("blt " + num1Reg + " " + num2Reg + " $$" + temp.result.name);
			freeReg(temp.num1, num1Reg);
			freeReg(temp.num2, num2Reg);
		}
		else if (temp.op == QuadOperator::conditionBne)
		{
			num1Reg = getReg(temp.num1);
			num2Reg = getReg(temp.num2);
			textInstr.push_back("bne " + num1Reg + " " + num2Reg + " $$" + temp.result.name);
			freeReg(temp.num1, num1Reg);
			freeReg(temp.num2, num2Reg);
		}
		else if (temp.op == QuadOperator::ifEnd)
		{
			if (temp.num1.value == "1")
			{
				textInstr.push_back("j $$elseEndLabel" + temp.num1.name);
			}
			textInstr.push_back("$$ifEndLabel" + temp.num1.name + ":");
		}
		else if (temp.op == QuadOperator::elseEnd)
		{
			textInstr.push_back("$$elseEndLabel" + temp.num1.name + ":");
		}
		else if (temp.op == QuadOperator::whileBegin)
		{
			textInstr.push_back("$$whileBeginLabel" + temp.num1.name + ":");
		}
		else if (temp.op == QuadOperator::whileEnd)
		{
			textInstr.push_back("j $$whileBeginLabel" + temp.num1.name);
			textInstr.push_back("$$whileEndLabel" + temp.num1.name + ":");
		}
		else if (temp.op == QuadOperator::forBegin)
		{
			textInstr.push_back("$$forBeginLabel" + temp.num1.name + ":");
		}
		else if (temp.op == QuadOperator::forEnd)
		{
			textInstr.push_back("j $$forBeginLabel" + temp.num1.name);
			textInstr.push_back("$$forEndLabel" + temp.num1.name + ":");
		}
		else if (temp.op == QuadOperator::forStride)
		{
			num1Reg = getReg(temp.num1);
			textInstr.push_back("addiu " + num1Reg + " " + num1Reg + " " + temp.num2.name);
			textInstr.push_back("sw " + num1Reg + " " + getAddr(temp.result));
			freeReg(temp.num1, num1Reg);
		}
		else if (temp.op == QuadOperator::switchBegin) {
			//nothing
		}
		else if (temp.op == QuadOperator::switchEnd) {
			textInstr.push_back("$$switchEndLabel" + temp.num1.name + ":");
		}
		else if (temp.op == QuadOperator::caseBegin) {
			//nothing
		}
		else if (temp.op == QuadOperator::caseEnd) {
			textInstr.push_back("j $$switchEndLabel" + temp.num2.name);
			textInstr.push_back("$$caseEndLabel" + temp.num1.name + ":");
		}
		else if (temp.op == QuadOperator::defaultEnd) {
			textInstr.push_back("j $$switchEndLabel" + temp.num1.name);
		}
		else if (temp.op == QuadOperator::allEnd) {
			textInstr.push_back("$$allEnd:");
		}
	}

	mips << ".data" << endl;
	for (auto instr : dataInstr)
	{
		mips << instr << endl;
	}
	mips << ".text" << endl;
	mips << "j $$main" << endl;
	for (auto instr : textInstr)
	{
		mips << instr << endl;
	}
}

bool GenMips::isGlobal()
{
	return nowFun == "global";
}

void GenMips::genGlobalVarInit(Quadruaple globalVar)
{
	//生成 .space
	dataInstr.push_back(globalVar.num1.name + ":");
	if (globalVar.num1.symbol == "CHARTK") {
		dataInstr.push_back(".word " + globalVar.num1.value);
	}
	else {
		dataInstr.push_back(".word " + globalVar.num1.value);
	}
	insertGlobal(globalVar);
}

void GenMips::genGlobalVarNoInit(Quadruaple globalVar)
{
	dataInstr.push_back(globalVar.num1.name + ":");
	if (globalVar.num1.symbol != "ARRTK") {
		dataInstr.push_back(".space 4 ");
	}
	else {
		if (globalVar.num1.array.dim == 1) {
			dataInstr.push_back(".space " + to_string(globalVar.num1.array.length1 * 4));
		}
		else if (globalVar.num1.array.dim == 2) {
			dataInstr.push_back(".space " + to_string(globalVar.num1.array.length1 * 4 * globalVar.num1.array.length2));
		}

	}
	insertGlobal(globalVar);
}

void GenMips::genLocalVarInit(Quadruaple localVar)
{
	string name = localVar.num1.name;
	varAddrMap[name] = to_string(localAddr);
	if (localVar.num1.symbol != "ARRTK")
	{
		textInstr.push_back("li $t0 " + localVar.num1.value);
		textInstr.push_back("sw $t0 " + to_string(0 - localAddr) + "($sp)");
		localAddr += 4;
	}
	else
	{
		vector<string> values;
		string temp;
		istringstream ss(localVar.num1.value);
		while (ss >> temp)
		{
			values.push_back(temp);
		}
		int index = localVar.num1.array.dim == 1 ? localVar.num1.array.length1 : localVar.num1.array.length1 * localVar.num1.array.length2;
		for (int i = 0; i < index; i++)
		{
			textInstr.push_back("li $t0 " + values[i]);
			textInstr.push_back("sw $t0 " + to_string(0 - localAddr) + "($sp)");
			localAddr += 4;
		}
	}
}

void GenMips::genLocalVarNoInit(Quadruaple localVar)
{
	SymbolTableRow num1Temp = localVar.num1;
	varAddrMap[num1Temp.name] = to_string(localAddr);
	if (localVar.num1.symbol != "ARRTK")
	{
		localAddr += 4;
	}
	else
	{
		localAddr += (num1Temp.array.dim == 1 ? num1Temp.array.length1 : num1Temp.array.length1 * num1Temp.array.length2) * 4;
	}
}

void GenMips::genFunPara(Quadruaple funPara)
{
	//将参数表中的变量定义添加到局部变量表中，并为之分配内存，并从栈中取出值？
	//不需要从栈中取值，直接将参数表中的变量与栈中对应的地址相对应即可，顺便可对
	//localAddr进行递增。
	varAddrMap[funPara.num1.name] = to_string(localAddr);
	localAddr += 4;
}

void GenMips::genFunCall(Quadruaple funCall)
{
	vector<string> saveOrder;
	saveOrder = saveEnv();
	int localAddrTemp = localAddr;
	localAddr = 0;
	vector<Quadruaple> rightTables;
	while (funValueTableStack.top().op != QuadOperator::callBegin)
	{
		rightTables.insert(rightTables.begin(), funValueTableStack.top());
		funValueTableStack.pop();
	}
	funValueTableStack.pop();
	for (auto rightRow : rightTables) {
		string numReg = getReg(rightRow.num1); // 从原sp的地址偏移下的值
		//放进新的sp地址偏移下
		textInstr.push_back("sw " + numReg + " -" + to_string(localAddr + localAddrTemp) + "($sp)");
		localAddr += 4;
		freeReg(rightRow.num1, numReg);
	}

	textInstr.push_back("subu $sp $sp " + to_string(localAddrTemp));
	textInstr.push_back("jal $$" + funCall.num1.name);
	textInstr.push_back("addu $sp $sp " + to_string(localAddrTemp));
	localAddr = localAddrTemp; //恢复到保存之后的最下面。但是是不是可以在恢复之后恢复到保存当前环境之前的状态？
	//确实可以，因为这些值都被恢复之后不需要再进行使用，可以直接从寄存器中取得。
	recoveryEnv(saveOrder);
	
}

void GenMips::genArrayOne(Quadruaple arrayOneVar)
{
	//这实现真sb
	string num1Reg;
	string num2Reg;
	string resultReg;
	SymbolTableRow num1 = arrayOneVar.num1;
	num1Reg = allocReg(num1);
	textInstr.push_back("la " + num1Reg + " " + getAddr(num1)); // 取得数组的基地址
	num2Reg = getReg(arrayOneVar.num2);
	textInstr.push_back("sll $t9 " + num2Reg + " 2");
	if (varAddrMap[num1.name] == "")
	{
		textInstr.push_back("addu " + num1Reg + " " + num1Reg + " $t9"); // 全局数组 ，地址增加
	}
	else
	{
		textInstr.push_back("subu " + num1Reg + " " + num1Reg + " $t9");  //局部数组，地址减少
	}
	// 得到真正的地址
	resultReg = allocReg(arrayOneVar.result);
	textInstr.push_back("lw " + resultReg + " (" + num1Reg + ")"); // 取出数组元素
	freeReg(num1, num1Reg);
	freeReg(arrayOneVar.num2, num2Reg);
}

void GenMips::genArrayTwo(Quadruaple arrayTwoVar)
{
	string value = arrayTwoVar.result.value;
	string arrayName;
	string arrayLength1;
	string num1Reg;
	string num2Reg;
	string num2Reg2;
	string resultReg;
	for (int i = 0; i < value.length(); i++)
	{
		if (value[i] == ' ')
		{
			arrayName = value.substr(0, i);
			arrayLength1 = value.substr(i + 1, value.length());
		}
	}
	SymbolTableRow num1 = arrayTwoVar.num1;
	SymbolTableRow num2 = arrayTwoVar.num2;
	num1Reg = allocReg(SymbolTableRow(arrayName));
	textInstr.push_back("la " + num1Reg + " " + getAddr(SymbolTableRow(arrayName)));
	if (num1.isKonwn) {
		if (num2.isKonwn) {
			if (num1.symbol == "INTTK") {

			}
		}
	}
	num2Reg = getReg(num1);	// 一维下标	
	num2Reg2 = getReg(num2);	// 二维下标
	textInstr.push_back("mul $t9 " + num2Reg + " " + arrayLength1); // 数组长度
	textInstr.push_back("addu $t9 $t9 " + num2Reg2);			//
	textInstr.push_back("sll $t9 $t9 2");				// 相对于数组初始的相对地址
	// 数组 分两种 ，全局数组的地址是增张的，局部数组的地址是反向增长的。
	if (varAddrMap[arrayName] == "")
	{
		textInstr.push_back("addu " + num1Reg + " " + num1Reg + " $t9");
	}
	else
	{
		textInstr.push_back("subu " + num1Reg + " " + num1Reg + " $t9");
	}
	resultReg = allocReg(arrayTwoVar.result);
	textInstr.push_back("lw " + resultReg + " (" + num1Reg + ")");
	freeReg(num1, num2Reg);
	freeReg(num2, num2Reg2);
	freeReg(SymbolTableRow(arrayName), num1Reg); // 如果此被添加映射 会发生什么？
}

void GenMips::genEqual(Quadruaple equal)
{
	string num1Reg;
	string num2Reg;
	string num2Reg2;
	string resultReg;
	if (equal.num1.symbol != "ARRTK")
	{
		if (equal.num1.name[0] == '$') {
			varAddrMap[equal.num1.name] = to_string(localAddr);
			localAddr += 4;
		}
		num2Reg = getReg(equal.num2);
		textInstr.push_back("sw " + num2Reg + " " + getAddr(equal.num1));
		freeReg(equal.num2, num2Reg);
	}
	else if (equal.num1.array.dim == 1)
	{
		num1Reg = allocReg(equal.num1);
		textInstr.push_back("la " + num1Reg + " " + getAddr(equal.num1)); // 取得数组的基地址
		num2Reg = getReg(equal.num2);
		textInstr.push_back("sll $t9 " + num2Reg + " 2");
		if (varAddrMap[equal.num1.name] == "")
		{
			textInstr.push_back("addu " + num1Reg + " " + num1Reg + " $t9");
		}
		else
		{
			textInstr.push_back("subu " + num1Reg + " " + num1Reg + " $t9");
		}
		resultReg = getReg(equal.result);
		textInstr.push_back("sw " + resultReg + " (" + num1Reg + ")");
		freeReg(equal.num1, num1Reg);
		freeReg(equal.num2, num2Reg);
		freeReg(equal.result, resultReg);

	}
	else
	{
		string value = equal.num1.value;
		string arrayName;
		string arrayLength1;
		for (int i = 0; i < value.length(); i++)
		{
			if (value[i] == ' ')
			{
				arrayName = value.substr(0, i);
				arrayLength1 = value.substr(i + 1, value.length());
			}
		}
		num1Reg = allocReg(SymbolTableRow(arrayName));
		textInstr.push_back("la " + num1Reg + " " + getAddr(SymbolTableRow(arrayName)));
		num2Reg = getReg(equal.num1);	// 一维下标	
		num2Reg2 = getReg(equal.num2);	// 二维下标
		//使用t9的原因是 数组的下标是一个表达式可能为中间变量，如果在表达式上进行复用寄存器时，。。好像确实没有问题，
		//但如果是一个变量，后面需要使用时，会用到此寄存器挺难受的。
		//如果在这里进行特判分析呢？但是 如果不对变量进行记录，仅仅表示此寄存器当前繁忙，无法寻找到此寄存器当前对应的变量
		//那么下次需要用到此变量时会直接从内存load，并重新分配寄存器，不需要考虑。
		//如果为其分配寄存器并添加映射，那么会出现一些情况：
		//所有表达式结果拿到的对应的寄存器都不能在计算中进行复用。
		//同时在跳转函数时，需要保存的环境会增多？似乎是只需要进行保存当前正在使用的寄存器即可,
		//但是如此以来t9需要被保存，t9所存储的是二维数组的地址偏移。
		//。主要在我需不需要进行映射处理。暂时先不加，可以添加试一下
		//所以不如不存？不添加映射即可。也就不会用到t9。
		textInstr.push_back("mul $t9 " + num2Reg + " " + arrayLength1); // 数组长度
		textInstr.push_back("addu $t9 $t9 " + num2Reg2);			//
		textInstr.push_back("sll $t9 $t9 2");				// 相对于数组初始的相对地址
		// 数组 分两种 ，全局数组的地址是增张的，局部数组的地址是反向增长的。
		if (varAddrMap[arrayName] == "")
		{
			textInstr.push_back("addu " + num1Reg + " " + num1Reg + " $t9");
		}
		else
		{
			textInstr.push_back("subu " + num1Reg + " " + num1Reg + " $t9");
		}
		resultReg = getReg(equal.result);
		textInstr.push_back("sw " + resultReg + " (" + num1Reg + ")");
		freeReg(equal.num1, num2Reg);
		freeReg(equal.num2, num2Reg2);
		freeReg(SymbolTableRow(arrayName), num1Reg);
		freeReg(equal.result, resultReg);
	}
}

void GenMips::genPrintf(Quadruaple printVar)
{
	if (printVar.num1.value != "")
	{
		dataInstr.push_back("$$string" + to_string(stringCount) + ": ");
		dataInstr.push_back(".asciiz \"" + printVar.num1.value + "\"");
		textInstr.push_back("li $v0 4");
		textInstr.push_back("la $a0 $$string" + to_string(stringCount));
		textInstr.push_back("syscall");
		++stringCount;
	}

	if (printVar.num2.name != "") {
		if (printVar.num2.symbol == "INTTK")
		{
			textInstr.push_back("li $v0 1");
		}
		else if (printVar.num2.symbol == "CHARTK")
		{
			textInstr.push_back("li $v0 11");
		}
		if (printVar.num2.isKonwn)
		{
			textInstr.push_back("li $a0 " + printVar.num2.name);
		}
		else
		{
			// lw $a0 getAddr(printVar.num2);
			// 此结果可能为一个中间变量，那么寄存器里面应该有保存指，可能是一个局部变量或者是一个全局变量，这样
			// 需要到内存取出。
			if (varToReg[printVar.num2.name] != "" ) { // 有寄存器 意味着他是中间变量
				textInstr.push_back("move $a0 " + varToReg[printVar.num2.name]); //
				freeReg(printVar.num2, varToReg[printVar.num2.name]);
			}
			else {
				getValue(printVar.num2, "$a0");//不是中间变量，则按照以前的方法进行取值。
			}

		}
		textInstr.push_back("syscall");
	}

	textInstr.push_back("li $v0 11");
	textInstr.push_back("li $a0 \'\\n\'");
	textInstr.push_back("syscall");
}

void GenMips::genScanf(Quadruaple scanfVar)
{

	if (scanfVar.num1.symbol == "INTTK")
	{
		textInstr.push_back("li $v0 5");
		textInstr.push_back("syscall");

	}
	if (scanfVar.num1.symbol == "CHARTK")
	{
		textInstr.push_back("li $v0 12");
		textInstr.push_back("syscall");
	}
	textInstr.push_back("sw $v0 " + getAddr(scanfVar.num1.name));
}

void GenMips::genAddAndSub(Quadruaple calVar, string instr)
{
	SymbolTableRow num1 = calVar.num1;
	SymbolTableRow num2 = calVar.num2;
	if (calVar.num1.name != "")
	{
		string instruction = instr;
		string num1Reg = getReg(num1);
		string num2Reg = getReg(num2);
		// 考虑一个问题：
		// 加减乘除运算的运算数的类型：
		// 全局或局部变量
		// 中间变量 需要被保存到寄存器中，需要值保存在寄存器中，需要被记录所存储寄存器的位置。
		// 常量 可以直接li 到 临时寄存器中 同样如此。
		// 全局 局部 变量 也 需要被lw到寄存器中,但是这种是使用即释放，不会被存储与记录到寄存器使用状态中
		// 但是在参与运算中的每一个寄存器在使用之后都需要被赋予为空闲状态
		// 运算结果一定是一个中间变量，需要被记录在寄存器，这个逻辑可以加在寄存器分配中，对中间变量进行特判即可。
		textInstr.push_back(instr + " " + allocReg(calVar.result) + " " + num1Reg + " " + num2Reg);
		freeReg(num1, num1Reg);
		freeReg(num2, num2Reg);
		//textInstr.push_back("sw $t2 " + getAddr(calVar.result));
	}
	// 没有num1时
	else
	{
		string num2Reg = getReg(num2);
		textInstr.push_back(instr + " " + allocReg(calVar.result) + " $0" + " " + num2Reg);
		freeReg(num2, num2Reg);
		//textInstr.push_back("sw $t2 " + getAddr(calVar.result));
	}
}

void GenMips::genDivAndMul(Quadruaple calVar, string instr)
{

	string num1Reg = getReg(calVar.num1);
	string num2Reg = getReg(calVar.num2);
	textInstr.push_back(instr + " " + allocReg(calVar.result) + " " + num1Reg + " " + num2Reg);
	freeReg(calVar.num1, num1Reg);
	freeReg(calVar.num2, num2Reg);
	//textInstr.push_back("sw $t2 " + getAddr(calVar.result));
}

GenMips::GenMips()
{
}

void GenMips::insertGlobal(Quadruaple globalVar)
{
	SymbolTableRow num1Temp = globalVar.num1;
	globalVarMap[globalVar.num1.name] = globalAddr;
	if (num1Temp.symbol == "ARRTK")
	{
		if (num1Temp.array.dim == 1)
		{
			globalAddr += 4 * num1Temp.array.length1;
		}
		else
		{
			globalAddr += 4 * num1Temp.array.length1 * num1Temp.array.length2;
		}
	}
	else
	{
		globalAddr += 4;
	}
}

string GenMips::getAddr(SymbolTableRow num)
{
	return varAddrMap[num.name] != "" ? "-" + varAddrMap[num.name] + "($sp)" : num.name;
}

void GenMips::getValue(SymbolTableRow num, string reg)
{
	if (num.isKonwn)
	{
		textInstr.push_back("li " + reg + " " + num.name);
	}
	else
	{
		textInstr.push_back("lw " + reg + " " + getAddr(num));
	}
}


string GenMips::allocReg(SymbolTableRow num) {
	string result = freeRegQueue.front();
	regToVar[result] = num.name;
	freeRegQueue.erase(freeRegQueue.begin());
	if (!num.isKonwn) {
		//不给常量分配映射，但很多常量是可以再继续被优化，直接参与运算，不再进行分配寄存器
		varToReg[num.name] = result;
	}
	return result;
}

void GenMips::freeReg(SymbolTableRow num, string numReg) {
	regToVar[numReg] = "";
	if (!num.isKonwn) {
		varToReg.erase(num.name);
	}
	freeRegQueue.push_back(numReg);
	
	
	
	//释放寄存器
}


string GenMips::getReg(SymbolTableRow num) {
	string numReg;
	if (num.name[0] == '@') {
		numReg = varToReg[num.name];
	}
	else {
		// 为局部或全局变量分配寄存器，并在后面被释放
		numReg = allocReg(num);
		getValue(num, numReg); // 方便进行加简进行运算，在此添加load值到所分配的寄存器中
	}
	return numReg;
}

vector<string> GenMips::saveEnv() {
	vector<string> result;
	textInstr.push_back("sw $ra -" + to_string(localAddr) + "($sp)"); //save ra
	localAddr += 4;

	for (map<string, string>::iterator iter = regToVar.begin(); iter != regToVar.end(); iter++) {
		if (iter->second != "") {
			result.push_back(iter->first);
			textInstr.push_back("sw " + iter->first + " -" + to_string(localAddr) + "($sp)");
			localAddr += 4;
		}
	}
	return result;
	
}
void GenMips::recoveryEnv(vector<string> saveOrder) {
	for (int i = saveOrder.size() - 1; i >= 0; i--) {
		localAddr -= 4;
		textInstr.push_back("lw " + saveOrder[i] + " -" + to_string(localAddr) + "($sp)");
	}
	localAddr -= 4;
	textInstr.push_back("lw $ra -" + to_string(localAddr) + "($sp)");
	
}

void GenMips::initRegs() {
	varToReg.clear();
	for (int i = 0; i < 9; i++) {
		regToVar["$t" + to_string(i)] = "";
		freeRegQueue.push_back("$t" + to_string(i));
	}
}
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cctype>
#include "Token.h"
#include "symbolTable.h"
#include "quadruaple.h"
#include "genMips.h"
#include <map>

extern ofstream os;
using namespace std;
extern vector<Quadruaple> quadruaples;
void parsingProgram(Token& token);
void kkk();
int main() {
    std::ifstream in("testfile.txt");
    std::stringstream buffer;
    kkk();
    buffer << in.rdbuf();
    Token token(buffer.str());
    unsigned int length = token.file.length();
    cout << token.file << endl;
    parsingProgram(token);
    GenMips gen = GenMips();
    gen.genBegin();
    os.close();
    return 0;
}


void kkk() {
    
}




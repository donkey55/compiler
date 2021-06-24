#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include "Token.h"
#include "getWriteAndFind.h"
using namespace std;
extern vector<SymbolTableRow>* curTables;
extern vector<SymbolTableRow> globalTables;
extern vector<SymbolTableRow> funTables;
std::ofstream os("error.txt");
std::ofstream os2("output.txt");

bool isKeyWord(string word) {
    transform(word.begin(), word.end(), word.begin(), ::tolower);
    vector<string> keyWords = { "int", "char", "void", "main", "if", "else", "switch", "case", "default",
                               "while", "for", "scanf", "printf", "return", "const" };
    for (const string& string1 : keyWords) {
        if (word == string1) {
            return true;
        }
    }
    return false;
}

void getSym(Token& token, unsigned int length) {
    token.clearToken();
    token.symbol.clear();
    char ch = token.file.at(token.index);
    while (isspace(ch) && token.index < length) {
        if (ch == '\n') {
            token.lineNum++;
        }
        ++token.index;
        if (token.index < length) {
            ch = token.file.at(token.index);
        }
    }
    if (isalpha(ch) || ch == '_') {
        while ((isalpha(ch) || isdigit(ch) || ch == '_') && token.index < length) {
            token.word += ch;
            ++token.index;
            if (token.index < length) {
                ch = token.file.at(token.index);
            }
        }
        if (isKeyWord(token.word)) {
            string str = token.word;
            transform(str.begin(), str.end(), str.begin(), ::toupper);
            /*transform(token.word.begin(), token.word.end(), token.word.begin(), ::tolower);*/
            token.symbol = str + "TK";
        }
        else {
            transform(token.word.begin(), token.word.end(), token.word.begin(), ::tolower);
            token.symbol = "IDENFR";
        }
    }
    else if (isdigit(ch)) {
        while (token.index < length && isdigit(ch)) {
            token.word += ch;
            ++token.index;
            if (token.index < length) {
                ch = token.file.at(token.index);
            }
        }
        token.symbol = "INTCON";
    }
    else if (ch == '\"') {
        ++token.index;
        if (token.index < length) {
            ch = token.file.at(token.index);
            while (ch != '\"' && token.index < length) {
                if (ch == '\\') {
                
                    token.word += ch;
                }
                token.word += ch;
                ++token.index;
                if (token.index < length) {
                    ch = token.file.at(token.index);
                }
            }
            token.symbol = "STRCON";
            ++token.index;
        }
    }
    else if (ch == '\'') {
        if (++token.index < length) {
            token.word += token.file.at(token.index++); //??????????
            token.symbol = "CHARCON"; //bug 字符只有 加法 乘法 字母 数字
        }
        ++token.index;
    }
    else if (ch == '+') {
        token.word += ch;
        token.index++;
        token.symbol = "PLUS";
    }
    else if (ch == '-') {
        token.word += ch;
        token.index++;
        token.symbol = "MINU";
    }
    else if (ch == '*') {
        token.word += ch;
        token.index++;
        token.symbol = "MULT";
    }
    else if (ch == '/') {
        token.word += ch;
        token.index++;
        token.symbol = "DIV";
    }
    else if (ch == '<') {
        token.word += ch;
        ++token.index;
        if (token.index < length) {
            ch = token.file.at(token.index);
            if (ch == '=') {
                token.word += ch;
                ++token.index;
                token.symbol = "LEQ";
            }
            else {
                token.symbol = "LSS";
            }
        }
    }
    else if (ch == '>') {
        token.word += ch;
        ++token.index;
        if (token.index < length) {
            ch = token.file.at(token.index);
            if (ch == '=') {
                token.word += ch;
                ++token.index;
                token.symbol = "GEQ";
            }
            else {
                token.symbol = "GRE";
            }
        }
    }
    else if (ch == ':') {
        token.word += ch;
        token.index++;
        token.symbol = "COLON";
    }
    else if (ch == '=') {
        token.word += ch;
        ++token.index;
        if (token.index < length) {
            ch = token.file.at(token.index);
            if (ch == '=') {
                token.word += ch;
                ++token.index;
                token.symbol = "EQL";
            }
            else {
                token.symbol = "ASSIGN";
            }
        }
    }
    else if (ch == ';') {
        token.word += ch;
        token.index++;
        token.symbol = "SEMICN";
    }
    else if (ch == ',') {
        token.word += ch;
        token.index++;
        token.symbol = "COMMA";
    }
    else if (ch == '(') {
        token.word += ch;
        token.index++;
        token.symbol = "LPARENT";
    }
    else if (ch == ')') {
        token.word += ch;
        token.index++;
        token.symbol = "RPARENT";
    }
    else if (ch == '[') {
        token.word += ch;
        token.index++;
        token.symbol = "LBRACK";
    }
    else if (ch == ']') {
        token.word += ch;
        token.index++;
        token.symbol = "RBRACK";
    }
    else if (ch == '{') {
        token.word += ch;
        token.index++;
        token.symbol = "LBRACE";
    }
    else if (ch == '}') {
        token.word += ch;
        token.index++;

        token.symbol = "RBRACE";
    }
    else if (ch == '!') {
        token.word += ch;
        ++token.index;
        if (token.index < length) {
            ch = token.file.at(token.index);
            if (ch == '=') {
                token.word += ch;
                ++token.index;
                token.symbol = "NEQ";
            }
        }
    }
}

void writeFile(Token& token) {
    if (!token.symbol.empty()) {
        os2 << token.symbol << " " << token.word << endl;
    }
}


void writeFile(string message) {
    os2 << message << endl;
}

void writeFile(int line, string message)
{
    os << line << " " << message << endl;
}

//错误处理使用的判断函数
void saveTypeAndSym(SymbolTableRow& temp, string type, string symbol) {
    temp.type = type;
    temp.symbol = symbol;
}

bool findIsDef(SymbolTableRow row)
{
    for (auto temp : *curTables) {
        if (strIsEqual(temp.name, row.name)) {
            return true;
        }
    }
    return false;
}

string findAllIsDef(string name)
{
    for (auto temp : *curTables) {
        if (strIsEqual(temp.name, name)) {
            if (temp.symbol == "ARRTK") {
                return temp.array.arrSymbol;
            }
            else {
                return temp.symbol;
            }
        }
    }
    for (auto temp : globalTables) {
        if (strIsEqual(temp.name, name)) {
            if (temp.symbol == "ARRTK") {
                return temp.array.arrSymbol;
            }
            else {
                return temp.symbol;
            }
        }
    }
    return "";
}

vector<string> getFunParLen(string name) {
    for (auto temp : globalTables) {
        if (strIsEqual(temp.name, name)) {
            return temp.funParameters;
        }
    }
    return {};
}

SymbolTableRow findAll(string name) {
    for (auto temp : *curTables) {
        if (strIsEqual(temp.name, name)) {
            return temp;
        }
    }
    for (auto temp : globalTables) {
        if (strIsEqual(temp.name, name)) {
            return temp;
        }
    }
    return SymbolTableRow("", "", "");
}

bool strIsEqual(string name1, string name2) {
    transform(name1.begin(), name1.end(), name1.begin(), ::toupper);
    transform(name2.begin(), name2.end(), name2.begin(), ::toupper);
    return name1 == name2;
}

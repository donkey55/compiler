#pragma once
#include <string>
#include <vector>
using namespace std;

class Token {
public:
    string file;
    int length;
    int lineNum = 1;
    string word;
    string symbol;
    vector<string> voidFun;
    vector<string> returnFun;
    int index = 0;
    Token(string file) : file(file){
        length = file.length();
    }
    void clearToken() {
        word.clear();
    }

};

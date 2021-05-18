#include <iostream>

#include "exprparser.hpp"

using namespace std;

ExpressionParser::ExpressionParser() {}

ExpressionParser::ExpressionParser(const char* expression) {
    this->expression = expression;
}

void ExpressionParser::dump() {
    cout << expression << "\n";
}

int main(int argc, const char** argv) {

    ExpressionParser parser("1234 567");
    parser.dump();

    for(int i=0; i<argc; i++) {
        cout << argv[i] << "\n";
    }    
}
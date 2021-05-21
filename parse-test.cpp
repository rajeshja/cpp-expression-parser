#include <iostream>

#include "exprparser.hpp"

using namespace std;

void parse_and_print(const char* expression) {
    ExpressionParser parser(expression);
    parser.parse();
    cout << "------------------\n";
    cout << expression << " -> ";
    parser.dump_queue(false);
    cout << "\n\n";

}

int main(int argc, const char** argv) {

    parse_and_print("12+x-(3*y^5)");
    parse_and_print("12+(x)-(3*y^5)");
    parse_and_print("12+sin(x)  - (3*y^5)");
    parse_and_print("4+18/(9-3)");
    parse_and_print("A * B + C");
    parse_and_print("A + B * C");
    parse_and_print("A * (B + C)");
    parse_and_print("A - B + C");
    parse_and_print("A * B ^ C + D");
    parse_and_print("A * (B + C * D) + E");

    // for(int i=0; i<argc; i++) {
    //     cout << argv[i] << "\n";
    // }    
}
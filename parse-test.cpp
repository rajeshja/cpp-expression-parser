#include <iostream>
#include <map>

#include "exprparser.hpp"

using namespace std;

void parse_and_print(const char* expression, bool do_evaluate) {
    ExpressionParser parser(expression);
    try {
        parser.parse();
        cout << "------------------\n";
        cout << expression << " -> ";
        parser.dump_queue(false);
        if (!do_evaluate) {
            parser.evaluate({
                {"A", 4}, 
                {"B", 5},
                {"C", 6},
                {"D", 8},
                {"E", 9},
                {"x", 7},
                {"y", 2}
            });
        }
        cout << "\n\n";
    } catch (const invalid_argument& e) {
        cout << "\nError parsing expression: " << e.what() << "\n\n";
    }
}

int main(int argc, const char** argv) {

    parse_and_print("12+x-(03*y^5)", false);
    parse_and_print("12+x-(3k*y^5)", false);
    parse_and_print("12+(x)-(3*y^5)", false);
    parse_and_print("12+sin(x)  - (3*y^5)", false);
    parse_and_print("4+18/(9-3)", false);
    parse_and_print("A * B + C", true);
    parse_and_print("+A * B + C", false);
    parse_and_print("A + B * C", false);
    parse_and_print("A * (B + C)", false);
    parse_and_print("A - B + C", false);
    parse_and_print("A * B ^ C + D", false);
    parse_and_print("A * (B + C * D) + E", false);
    parse_and_print("A * (B + C * D) + +E", false);
    parse_and_print("12+sin(x) - (3*-y^5)", false);

    // for(int i=0; i<argc; i++) {
    //     cout << argv[i] << "\n";
    // }    
}
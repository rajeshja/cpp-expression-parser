#include <iostream>
#include <iomanip>
#include <map>
#include <windows.h> // WinApi header

#include "exprparser.hpp"

using namespace std;

void parse_test_print(const char* expression, float expected_result) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    ExpressionParser parser(expression);
    try {
        parser.parse();
        cout << "------------------\n";
        cout << expression << " -> " << setprecision(8);
        parser.dump_queue(false);
        float result = parser.evaluate({
            {"A", 4}, 
            {"B", 5},
            {"C", 6},
            {"D", 8},
            {"E", 9},
            {"x", 7},
            {"y", 2}
        });
        const float TOLERANCE = 0.000001;
        if ((result-expected_result<TOLERANCE)&&(result-expected_result>-TOLERANCE)) {
            SetConsoleTextAttribute(hConsole, 2*16+0);
            cout << result << " : PASS";
            SetConsoleTextAttribute(hConsole, 0*16+7);
            cout <<"\n";
        } else {
            SetConsoleTextAttribute(hConsole, 12*16+0);
            cout << result << " expected=" << expected_result << " : FAIL";
            SetConsoleTextAttribute(hConsole, 0*16+7);
            cout <<"\n";
        }

        // cout << "\nAnswer is: " << result << "\n";
        // cout << "\n\n";
    } catch (const invalid_argument& e) {
        cout << "Error parsing expression: " << e.what() << "\n";
    }
}

int main(int argc, const char** argv) {

    parse_test_print("12+x-(03*y^5)", -77);
    parse_test_print("12+x-(3k*y^5)", 0);
    parse_test_print("12+(x)-(3*y^5)", -77);
    parse_test_print("12+sin(x)  - (3*y^5)", -83.343018);
    parse_test_print("4+18/(9-3)", 7);
    parse_test_print("A * B + C", 26);
    parse_test_print("+A * B + C", 26);
    parse_test_print("A + B * C", 34);
    parse_test_print("A * (B + C)", 44);
    parse_test_print("A - B + C", 5);
    parse_test_print("A * B ^ C + D", 62508);
    parse_test_print("A * (B + C * D) + E", 221);
    parse_test_print("A * (B + C * D) + +E", 221);
    parse_test_print("1.2e-1+sin(x) - (3*-y^5)", 96.776985);
    parse_test_print("1.2e+1+sin(x) - (3*-y^5)", 108.65698);
    parse_test_print("1.2e1+sin(x) - (3*-y^5)", 108.65698);
    parse_test_print("1.002+sin(x) - (3*-y^5.0)", 97.658989);

    // for(int i=0; i<argc; i++) {
    //     cout << argv[i] << "\n";
    // }    
}
#include <iostream>
#include <iomanip>
#include <map>
#include <chrono>
#include <time.h>
#include <ctime>
#include <windows.h> // WinApi header

#include "exprparser.hpp"

using namespace std;

map<string, float> test_cases = {
    {"A * (B + C)", 44},
    {"A - B + C", 5},
    {"A * B ^ C + D", 62508},
    {"A * (B + C * D) + E", 221},
    {"12+x-(03*y^5)", -77},
    {"12+x-(30*y^5)", -941},
    {"12+(x)-(3*y^5)", -77},
    {"12+sin(x)  - (3*y^5)", -83.343018},
    {"4+18/(9-3)", 7},
    {"A * B + C", 26},
    {"+A * B + C", 26},
    {"A + B * C", 34},
    {"A * (B + C * D) + +E", 221},
    {"1.2E-1+sin(x) - (3*-y^5)", 96.776985},
    {"(1.2E-1+sin(x)-2) - (3*-y^5)", 94.776985},
    {"(1.2E-1+sin(x)/2) - (3*-y^5)", 96.448494},
    {"(1.2E-1+sin(x)*2) - (3*-y^5)", 97.433975},
    {"(1.2E-1+sin(x)^2) - (3*-y^5)", 96.551628},
    {"(1.2E-1+sin(x)+2) - (3*-y^5)", 98.776985},
    {"1.2E+1+sin(x) - (3*-y^5)", 108.65698},
    {"1.2E1+sin(x) - (3*-y^5)", 108.65698},
    {"1.002+sin(x) - (3*-y^5.0)", 97.658989},
    {"(1+sin(pi))*2", 2},
    {"(1+sin(pi))*2-cos(pi/2)", 2},
    {"(1+sin(pi/2))*2-cos(pi/2)", 4},
    {"(x*sin(x)-x)*y", -4.8021879},
    {"(x*sin(x))*y", 9.1978121},
    {"log(10,e)", 2.3025854},
    {"log(10,e+1-1)", 2.3025854},
    {"log(100,11-1)", 2},
    {"log(pi^2,pi)", 2},
    {"4+log(100,10)/2", 5},
};

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

    for (auto entry: test_cases) {
        parse_test_print(entry.first.c_str(), entry.second);
    }
    // parse_test_print("A * (B + C)", 44);
    // parse_test_print("A - B + C", 5);
    // parse_test_print("A * B ^ C + D", 62508);
    // parse_test_print("A * (B + C * D) + E", 221);
    // parse_test_print("12+x-(03*y^5)", -77);
    // parse_test_print("12+x-(3k*y^5)", 0);
    // parse_test_print("12+(x)-(3*y^5)", -77);
    // parse_test_print("12+sin(x)  - (3*y^5)", -83.343018);
    // parse_test_print("4+18/(9-3)", 7);
    // parse_test_print("A * B + C", 26);
    // parse_test_print("+A * B + C", 26);
    // parse_test_print("A + B * C", 34);
    // parse_test_print("A * (B + C * D) + +E", 221);
    // parse_test_print("1.2e-1+sin(x) - (3*-y^5)", 96.776985);
    // parse_test_print("1.2e+1+sin(x) - (3*-y^5)", 108.65698);
    // parse_test_print("1.2e1+sin(x) - (3*-y^5)", 108.65698);
    // parse_test_print("1.002+sin(x) - (3*-y^5.0)", 97.658989);
    // parse_test_print("(1+sin(pi))*2", 2);

    if (false) {
        using namespace chrono;

        /* Performance test */
        ExpressionParser common_parser;
        long start_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
        for (short i=0; i<10000; i++) { //20,000 iterations
            for (auto entry: test_cases) {
                common_parser.set_expression(entry.first.c_str());
                common_parser.parse();
            }
        }
        long end_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

        cout << "Total time taken with single parser instance was: " << end_time-start_time << "\n";

        start_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
        for (short i=0; i<10000; i++) { //20,000 iterations
            for (auto entry: test_cases) {
                ExpressionParser parser_ind(entry.first.c_str());
                parser_ind.parse();
            }
        }
        end_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

        cout << "Total time taken was: " << end_time-start_time << "\n";

        start_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
        for (int i=0; i<10000; i++) { //20,000 iterations
            for (auto entry: test_cases) {
                ExpressionParser parser_ind(entry.first.c_str());
                parser_ind.parse();
                float result = parser_ind.evaluate({
                    {"A", 4}, 
                    {"B", 5},
                    {"C", 6},
                    {"D", 8},
                    {"E", 9},
                    {"x", 7},
                    {"y", 2}
                });
            }
        }
        end_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

        cout << "Total time taken with evaluation was: " << end_time-start_time << "\n";

        start_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
        for (int i=0; i<10000; i++) { //20,000 iterations
            for (auto entry: test_cases) {
                common_parser.set_expression(entry.first.c_str());
                common_parser.parse();
                float result = common_parser.evaluate({
                    {"A", 4}, 
                    {"B", 5},
                    {"C", 6},
                    {"D", 8},
                    {"E", 9},
                    {"x", 7},
                    {"y", 2}
                });
            }
        }
        end_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

        cout << "Total time taken with evaluation with single parser instance was: " << end_time-start_time << "\n";
    }
}
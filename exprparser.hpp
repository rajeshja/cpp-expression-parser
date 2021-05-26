#include <vector>
#include <stack>
#include <queue>
#include <map>
#include <string>
#include <optional>

#include "math_functions.hh"

using namespace std;

enum TokenType {
    number,
    //op,
    unary_operator,
    binary_operator,
    variable,
    function_1param,
    function_2param,
    function_3param
};

enum Associativity {
    left_associative,
    non_associative,
    right_associative
};

class OperatorDetails {
    public:
    OperatorDetails();
    OperatorDetails(char op, int precedence, Associativity associativity);
    char op;
    int precedence;
    Associativity associativity;
};

class OperationDetails {
    public:
    OperationDetails();
    OperationDetails(string function, NodeMathOperation operation, TokenType type);
    string function;
    NodeMathOperation operation;
    TokenType type;
    private:
};

class ExpressionToken {
    public:
    ExpressionToken();
    ExpressionToken(string token, TokenType type);
    ExpressionToken(string token, TokenType type, double value);
    ExpressionToken(string token, TokenType type, NodeMathOperation operation);
    string token;
    double token_value;
    NodeMathOperation operation;
    TokenType type;
};

class ExpressionParser {
    public:
    ExpressionParser();
    ExpressionParser(const char* expression);
    void parse();
    void dump_tokens();
    void dump_queue(bool with_headers);
    void dump_stack(bool with_headers);
    OperatorDetails get_operator_details(char op);
    optional<OperationDetails> get_function_details(string op);
    double evaluate(map<string, double> variables);
    private:
    bool add_token(int token_start, int token_end);
    bool has_precedence(ExpressionToken curr, ExpressionToken prev);
    double execute_math_operation(ExpressionToken operation, double a);
    double execute_math_operation(ExpressionToken operation, double a, double b);
    bool is_operator(char character);
    bool is_digit(char character);
    bool is_letter(char character);
    bool is_whitespace(char character);
    bool is_exponent(char character);
    bool is_function(string text);
    bool is_number(string text);
    bool is_variable(string text);
    double get_number(string text);
    void pop_opstack_to_outqueue();
    char get_last_printable_char_before(int index);
    const char* expression;
    vector<ExpressionToken> tokens;
    stack<ExpressionToken> operator_stack;
    queue<ExpressionToken> output_queue;
    vector<OperatorDetails> OPERATORS_DETAILS {
        OperatorDetails('(', 1, left_associative),
        OperatorDetails(')', 1, left_associative),
        OperatorDetails('^', 2, right_associative),
        OperatorDetails('%', 3, left_associative),
        OperatorDetails('/', 3, left_associative),
        OperatorDetails('*', 3, left_associative),
        OperatorDetails('-', 4, left_associative),
        OperatorDetails('+', 4, left_associative),
    };
    vector<char> WHITESPACE {
        ' ',
        '\t',
        '\0'
    };
    vector<char> EXPONENTS {
        'e',
        'E'
    };
    vector<char> DIGITS {
        '0',
        '1',
        '2',
        '3',
        '4',
        '5',
        '6',
        '7',
        '8',
        '9',
    };
    vector<char> LETTERS {
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
        'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
        'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
        'y', 'z', 
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
        'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
        'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
        'Y', 'Z'
    };
    vector<string> FUNCTIONS {
        "sin",
        "cos",
        "tan",
        "sec",
        "cosec",
        "log10",
        "loge",
        "abs",
        "sqrt"
    };
    map<string, OperationDetails> FUNCTIONS_MAPPING {
        {"abs", OperationDetails("abs", NODE_MATH_ABSOLUTE, function_1param)},
        {"exp", OperationDetails("exp", NODE_MATH_EXPONENT, function_1param)},
        {"sign", OperationDetails("sign", NODE_MATH_SIGN, function_1param)},
        {"round", OperationDetails("round", NODE_MATH_ROUND, function_1param)},
        {"floor", OperationDetails("floor", NODE_MATH_FLOOR, function_1param)},
        {"ceil", OperationDetails("ceil", NODE_MATH_CEIL, function_1param)},
        {"fraction", OperationDetails("fraction", NODE_MATH_FRACTION, function_1param)},
        {"trunc", OperationDetails("trunc", NODE_MATH_TRUNC, function_1param)},
        {"sqrt", OperationDetails("sqrt", NODE_MATH_SQRT, function_1param)},
        {"isqrt", OperationDetails("isqrt", NODE_MATH_INV_SQRT, function_1param)},
        {"deg2rad", OperationDetails("deg2rad", NODE_MATH_RADIANS, function_1param)},
        {"rad2deg", OperationDetails("rad2deg", NODE_MATH_DEGREES, function_1param)},
        {"sin", OperationDetails("sin", NODE_MATH_SINE, function_1param)},
        {"cos", OperationDetails("cos", NODE_MATH_COSINE, function_1param)},
        {"tan", OperationDetails("tan", NODE_MATH_TANGENT, function_1param)},
        {"sinh", OperationDetails("sinh", NODE_MATH_SINH, function_1param)},
        {"cosh", OperationDetails("cosh", NODE_MATH_COSH, function_1param)},
        {"tanh", OperationDetails("tanh", NODE_MATH_TANH, function_1param)}
    };
};

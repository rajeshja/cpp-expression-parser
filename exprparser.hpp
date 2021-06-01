#include <vector>
#include <stack>
#include <queue>
#include <map>
#include <string>
#include <optional>

#include "math_functions.hh"

using namespace std;

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
    OperationDetails(string function, NodeMathOperation operation, short no_of_params);
    string function;
    NodeMathOperation operation;
    short no_of_params;
    private:
};

class Expression_Token {
    public:
    virtual ~Expression_Token() = default;
    string text;
};

class ValueToken : public Expression_Token {
    public:
    float value;
};

class NumberToken : public ValueToken {
    public:
    NumberToken();
    NumberToken(string text);
    NumberToken(string text, float value);
    void set_value(float value);
};

class VariableToken : public ValueToken {
    public:
    VariableToken();
    VariableToken(string value);
};

class OperationToken : public Expression_Token {
    public:
    OperationToken();
    OperationToken(string text, bool is_prefix, bool is_function, short no_of_params, NodeMathOperation operation);
    bool is_prefix;
    bool is_function;
    short no_of_params;
    NodeMathOperation operation;
};

class ExpressionParser {
    public:
    ExpressionParser();
    ExpressionParser(const char* expression);
    void set_expression(const char* expression);
    void parse();
    void dump_tokens();
    void dump_queue(bool with_headers);
    void dump_stack(bool with_headers);
    OperatorDetails get_operator_details(char op);
    optional<OperationDetails> get_function_details(string op);
    float evaluate(map<string, float> variables);
    private:
    bool add_token(int token_start, int token_end);
    bool has_precedence(OperationToken* prev, OperationToken* curr);
    bool is_operator(char character);
    bool is_digit(char character);
    bool is_letter(char character);
    bool is_whitespace(char character);
    bool is_exponent(char character);
    bool is_function(string text);
    bool is_number(string text);
    bool is_constant(string text);
    bool is_variable(string text);
    float get_number(string text);
    float get_constant_value(string text);
    void pop_operationstack_to_outqueue();
    char get_last_printable_char_before(int index);
    const char* expression;
    stack<OperationToken*> operation_stack;
    queue<Expression_Token*> output_queue_new;
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
    map<string, float> CONSTANTS {
        {"pi", 3.1415927}, // Replace with M_PI 
        {"e", 2.7182818}   // and M_E 
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
    map<string, OperationDetails> FUNCTIONS_MAPPING {
        {"abs", OperationDetails("abs", NODE_MATH_ABSOLUTE, 1)},
        {"exp", OperationDetails("exp", NODE_MATH_EXPONENT, 1)},
        {"sign", OperationDetails("sign", NODE_MATH_SIGN, 1)},
        {"round", OperationDetails("round", NODE_MATH_ROUND, 1)},
        {"floor", OperationDetails("floor", NODE_MATH_FLOOR, 1)},
        {"ceil", OperationDetails("ceil", NODE_MATH_CEIL, 1)},
        {"fraction", OperationDetails("fraction", NODE_MATH_FRACTION, 1)},
        {"trunc", OperationDetails("trunc", NODE_MATH_TRUNC, 1)},
        {"sqrt", OperationDetails("sqrt", NODE_MATH_SQRT, 1)},
        {"isqrt", OperationDetails("isqrt", NODE_MATH_INV_SQRT, 1)},
        {"deg2rad", OperationDetails("deg2rad", NODE_MATH_RADIANS, 1)},
        {"rad2deg", OperationDetails("rad2deg", NODE_MATH_DEGREES, 1)},
        {"sin", OperationDetails("sin", NODE_MATH_SINE, 1)},
        {"cos", OperationDetails("cos", NODE_MATH_COSINE, 1)},
        {"tan", OperationDetails("tan", NODE_MATH_TANGENT, 1)},
        {"sinh", OperationDetails("sinh", NODE_MATH_SINH, 1)},
        {"cosh", OperationDetails("cosh", NODE_MATH_COSH, 1)},
        {"tanh", OperationDetails("tanh", NODE_MATH_TANH, 1)},
        {"+", OperationDetails("+", NODE_MATH_ADD, 2)},
        {"-", OperationDetails("-", NODE_MATH_SUBTRACT, 2)},
        {"*", OperationDetails("*", NODE_MATH_MULTIPLY, 2)},
        {"/", OperationDetails("/", NODE_MATH_DIVIDE, 2)},
        {"^", OperationDetails("^", NODE_MATH_POWER, 2)},
        {"log", OperationDetails("log", NODE_MATH_LOGARITHM, 2)},
        {"min", OperationDetails("min", NODE_MATH_MINIMUM, 2)},
        {"max", OperationDetails("max", NODE_MATH_MAXIMUM, 2)},
        {"<", OperationDetails("<", NODE_MATH_LESS_THAN, 2)},
        {">", OperationDetails(">", NODE_MATH_GREATER_THAN, 2)},
        {"mod", OperationDetails("mod", NODE_MATH_MODULO, 2)},
        {"snap", OperationDetails("snap", NODE_MATH_SNAP, 2)},
        {"arctan", OperationDetails("arctan", NODE_MATH_ARCTAN2, 2)},
        {"pingpong", OperationDetails("pingpong", NODE_MATH_PINGPONG, 2)},
        {"compare", OperationDetails("compare", NODE_MATH_COMPARE, 3)},
        {"smoothmin", OperationDetails("smoothmin", NODE_MATH_SMOOTH_MIN, 3)},
        {"smoothmax", OperationDetails("smoothmax", NODE_MATH_SMOOTH_MAX, 3)},
        {"wrap", OperationDetails("wrap", NODE_MATH_WRAP, 3)},
        {"neg", OperationDetails("wrap", NODE_MATH_NEG, 1)}
    };
};

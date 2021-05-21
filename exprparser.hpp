#include <vector>
#include <stack>
#include <queue>
#include <map>
#include <string>

using namespace std;

enum TokenType {
    number,
    //op,
    unary_operator,
    binary_operator,
    variable,
    function
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

class ExpressionToken {
    public:
    ExpressionToken();
    ExpressionToken(string token, TokenType type);
    string token;
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
    double evaluate(map<string, double> variables);
    private:
    void add_token(int token_start, int token_end);
    bool has_precedence(ExpressionToken curr, ExpressionToken prev);
    bool is_operator(char character);
    bool is_digit(char character);
    bool is_letter(char character);
    bool is_whitespace(char character);
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
    vector<char> OPERATORS {
        ')',
        '-',
        '+',
        '*',
        '%',
        '/',
        '^',
        '('
    };
    vector<OperatorDetails> OPERATORS_DETAILS {
        OperatorDetails('|', 0, right_associative),
        OperatorDetails('_', 0, right_associative),
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
};

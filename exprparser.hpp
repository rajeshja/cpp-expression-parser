#include <vector>
#include <stack>
#include <queue>
#include <map>
#include <string>

using namespace std;

enum TokenType {
    number,
    op,
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
    private:
    void add_token(int token_start, int token_end);
    bool has_precedence(ExpressionToken curr, ExpressionToken prev);
    bool is_operator(char character);
    bool is_digit(char character);
    bool is_whitespace(char character);
    bool is_function(string text);
    void pop_opstack_to_outqueue();
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

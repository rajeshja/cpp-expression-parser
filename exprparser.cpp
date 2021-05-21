#include <iostream>

#include "exprparser.hpp"

template <class T>
bool vector_contains(vector<T> vec, T item) {
    for (T elem: vec) {
        if (elem == item) {
            return true;
        }
    }
    return false;
}

template <class T>
int vector_find(vector<T> vec, T item) {
    for (int i=0; i<vec.size(); i++) {
        if (vec[i] == item) {
            return i;
        }
    }
    return -1;
}

ostream &operator << (ostream& stream, TokenType type) {
    const string tokenTypeNames[] = {"number", "operator", "variable", "function"};
    return stream << tokenTypeNames[type];
}

OperatorDetails::OperatorDetails() {}

OperatorDetails::OperatorDetails(char op, int precedence, Associativity associativity) {
    this->op = op;
    this->precedence = precedence;
    this->associativity = associativity;
}

OperatorDetails ExpressionParser::get_operator_details(char op) {
    //cout << "Searching for " << op << "\n";
    for (OperatorDetails details: OPERATORS_DETAILS) {
        //cout << "Checking against " << details.op << "\n";
        if (details.op==op) {
            //cout << "Found " << details.op << ", " << details.precedence << ", " << details.associativity << "\n";
            return details;
        }
    }
    return OPERATORS_DETAILS[0];
}

ExpressionToken::ExpressionToken() {}

ExpressionToken::ExpressionToken(string token, TokenType type) {
    this->token = token;
    this->type = type;
}

ExpressionParser::ExpressionParser() {}

ExpressionParser::ExpressionParser(const char* expression) {
    this->expression = expression;
}

bool ExpressionParser::is_digit(char character) {
    return vector_contains(DIGITS, character);
}

bool ExpressionParser::is_whitespace(char character) {
    return vector_contains(WHITESPACE, character);
}

bool ExpressionParser::is_operator(char character) {
    for (OperatorDetails op_details: OPERATORS_DETAILS) {
        if (op_details.op==character) {
            return true;
        }
    }
    return false;
    //return vector_contains(DIGITS, character);
}

bool ExpressionParser::is_function(string text) {
    return vector_contains(FUNCTIONS, text);
}

/* Checks if current operator token has precedence over previous operator token */
bool ExpressionParser::has_precedence(ExpressionToken prev, ExpressionToken curr) {
    OperatorDetails prev_details = get_operator_details(prev.token[0]);
    OperatorDetails curr_details = get_operator_details(curr.token[0]);

    // cout << "prev: " << prev.token[0] << " | curr: " << curr.token[0] << " | " 
    //     << "prevf: " << prev_details.op << " | currf: " << curr_details.op << " | " 
    //     << "prev prec: " << prev_details.precedence << " | curr prec: " << curr_details.precedence
    //     << " ; ";

    if (prev_details.precedence == curr_details.precedence) {
        //cout << (curr_details.associativity == right_associative ? "Curr has precedence with associativity. " : "Prev has precedence with associativity. ");
        return curr_details.associativity == right_associative;
    } else {
        //cout << (curr_details.precedence < prev_details.precedence ? "Curr has precedence. " : "Prev has precedence. ");
        return curr_details.precedence < prev_details.precedence;
    }

    // int prev_precedence = vector_find(OPERATORS, prev.token[0]);
    // int curr_precedence = vector_find(OPERATORS, curr.token[0]);
    // //cout << "prev: " << prev.token << ", curr: " << curr.token << ". prev prec: " << prev_precedence << ". curr prec: " << curr_precedence;
    // if (prev_precedence == curr_precedence) {

    // }
    // return prev_precedence > curr_precedence;
}

void ExpressionParser::pop_opstack_to_outqueue() {
    output_queue.push(operator_stack.top());
    // cout << "Moving from stack to queue: " << operator_stack.top().token << "\n";
    operator_stack.pop();
}

void ExpressionParser::add_token(int token_start, int token_end) {
    if (token_start<token_end) {
        string token_name(expression+token_start, expression+token_end);
        TokenType type;
        if (is_operator(expression[token_start])) {
            type = TokenType(op);
        } else if (is_digit(expression[token_start])) {
            type = TokenType(number);
        } else if (is_function(token_name)) {
            type = TokenType(function);
        } else {
            type = TokenType(variable);
        }
        ExpressionToken token(token_name, type);
        // if (tokens.size()>0) {
        //     cout << "Previous: " << tokens[tokens.size()-1].token << "; Current: " << token.token << "\n";
        // }
        tokens.insert(tokens.end(), token);

        if (token.type==number || token.type==variable) {
            // cout << "Found a number" << "\n";
            output_queue.push(token);
        } else if (token.type==op || token.type==function) {
            // cout << "Found an op" << "\n";
            if (token.token=="(" 
                || operator_stack.size()==0 
                || operator_stack.top().token=="("
                || token.type==function) {
                // cout << "\tFirst element to stack" << "\n";
                operator_stack.push(token);
            } else if (token.token[0]==')') {
                // cout << "\top is a right-bracket" << "\n";
                while (!operator_stack.empty() && operator_stack.top().token[0]!='(') {
                    pop_opstack_to_outqueue();
                }
                // cout << "Hopefully popping left bracket off stack: " << operator_stack.top().token << "\n";
                operator_stack.pop();
            } else if (!has_precedence(operator_stack.top(), token)) {
                // cout << "\top doesn't have precedence:: " << token.token << "\n";
                do {
                    pop_opstack_to_outqueue();
                } while (!operator_stack.empty() && !has_precedence(operator_stack.top(), token));
                operator_stack.push(token);
            } else {
                // cout << "\top has precedence:: " << token.token << "\n";
                // cout << "\tPushing op to stack: " << token.token << "\n";
                operator_stack.push(token);
            }
        }
        cout << token.token << " | ";
        dump_stack(false);
        cout << " | ";
        dump_queue(false);
        cout << "\n";
    }
}

void ExpressionParser::parse() {
    int i=0;
    int token_start=0;
    char current_char = expression[i];
    while (current_char!='\0') {
        if((is_operator(current_char)) 
            || (is_whitespace(current_char))
            || (is_operator(expression[i-1]))
            ) {
            add_token(token_start, i);
            token_start=i;
            if (is_whitespace(current_char)) {
                token_start++;
            }
        }
        i++;
        current_char = expression[i];
    }
    add_token(token_start, i);
    while (!operator_stack.empty()) {
        output_queue.push(operator_stack.top());
        operator_stack.pop();
    }
}

void ExpressionParser::dump_tokens() {
    cout << "Tokens" << "\n";
    cout << "============" << "\n";
    for (ExpressionToken token: tokens) {
        std::cout << token.token << ": " << token.type << "\n";
    }
    cout << "------------" << "\n";
}

void ExpressionParser::dump_queue(bool with_headers) {
    if (with_headers) {
        cout << "Output Queue" << "\n";
        cout << "============" << "\n";
    }
    for (int i=0; i<output_queue.size(); i++) {
        ExpressionToken token = output_queue.front();
        output_queue.push(token);
        output_queue.pop();
        cout << token.token << " ";
    }
    if (with_headers) {
        cout << "\n" << "------------" << "\n";
    }
}

void ExpressionParser::dump_stack(bool with_headers) {
    if (with_headers) {
        cout << "Stack Contents" << "\n";
        cout << "============" << "\n";
    }
    stack<ExpressionToken> temp;
    while (!operator_stack.empty()) {
        temp.push(operator_stack.top());
        operator_stack.pop();
    }
    while(!temp.empty()) {
        operator_stack.push(temp.top());
        cout << temp.top().token << " ";
        temp.pop();
    }
    if (with_headers) {
        cout << "\n" << "------------" << "\n";
    }
}
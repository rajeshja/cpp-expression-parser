#include <iostream>
#include <sstream>

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

bool ExpressionParser::is_letter(char character) {
    return vector_contains(LETTERS, character);
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

bool ExpressionParser::is_number(string text) {
    double parsed_num;
    istringstream num_stream(text);

    if (text[0]=='0' && text[1]=='x') {
        num_stream >> hex;
    } else if (text[0]=='0') {
        num_stream >> oct;
    }

    num_stream >> parsed_num;
    num_stream >> ws;

    return (!num_stream.fail() & num_stream.eof());
}

bool ExpressionParser::is_variable(string text) {
    if (!is_letter(text[0]) && text[0]!='_') {
        return false;
    } else {
        for (int i=1; i<text.size()-1; i++) {
            if (!is_letter(text[i]) && !is_digit(text[i]) && text[i]!='_') {
                return false;
            }
        }
    }
    return true;
}

double ExpressionParser::get_number(string text) {
        double parsed_num;
        istringstream num_stream(text);

        if (text[0]=='0' && text[1]=='x') {
            num_stream >> hex;
        } else if (text[0]=='0') {
            num_stream >> oct;
        }

        num_stream >> parsed_num;
        num_stream >> ws;

        if (!num_stream.fail() & num_stream.eof()) {
            return parsed_num;
        } else {
            return 0.0;
        }
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

char ExpressionParser::get_last_printable_char_before(int index) {
    if (index>0) {
        int pos=index-1;
        while (pos>=0) {
            if (!is_whitespace(expression[pos])) {
                return expression[pos];
            }
            pos--;
        }
    }
    return '\0';
}

void ExpressionParser::add_token(int token_start, int token_end) {
    if (token_start<token_end) {
        string token_name(expression+token_start, expression+token_end);
        TokenType type;
        if (is_operator(expression[token_start])) {
            //TODO: ("Differentiate between unary and binary operators")
            // If first token, or if previous token is ( or another operator but not )
            // then it is unary, else binary
            char prev_token_char = get_last_printable_char_before(token_start);
            if ((token_start==0)
                || (is_operator(prev_token_char) 
                    && prev_token_char!=')')) {
                if (token_name=="-") {
                    token_name="_";
                } else if (token_name=="+") {
                    token_name="|";
                }
                type = TokenType(unary_operator);
            } else {
                type = TokenType(binary_operator);
            }
        } else if (is_number(token_name)) {
            type = TokenType(number);
        } else if (is_function(token_name)) {
            type = TokenType(function);
        } else if (is_variable(token_name)) {
            type = TokenType(variable);
        } else {
            cerr << "Invalid token " << token_name;
            return;
        }
        ExpressionToken token(token_name, type);
        // if (tokens.size()>0) {
        //     cout << "Previous: " << tokens[tokens.size()-1].token << "; Current: " << token.token << "\n";
        // }

        if (token_name=="|") {
            return;
        }

        tokens.insert(tokens.end(), token);

        if (token.type==number || token.type==variable) {
            // cout << "Found a number" << "\n";
            output_queue.push(token);
        } else if (token.type==unary_operator || token.type==binary_operator || token.type==function) {
            // cout << "Found an op" << "\n";
            if (token.token=="("
                || operator_stack.size()==0
                || (operator_stack.top().token=="(" && token.token[0]!=')')
                || token.type==function
                || token.type==unary_operator) {
                // cout << "\tFirst element to stack" << "\n";
                operator_stack.push(token);
            } else if (token.token[0]==')') {
                // cout << "\top is a right-bracket" << "\n";
                while (!operator_stack.empty() && operator_stack.top().token[0]!='(') {
                    // cout << "Moving " << operator_stack.top().token << " to queue\n";
                    pop_opstack_to_outqueue();
                }
                // cout << "Hopefully popping left bracket off stack: " << operator_stack.top().token << "\n";
                if (!operator_stack.empty()) {
                    operator_stack.pop();
                }
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

double ExpressionParser::evaluate(map<string, double> variables) {
    stack<double> evaluation_stack;

    cout << "\n";

    while (!output_queue.empty()) {
        ExpressionToken curr_token = output_queue.front();
        if (curr_token.type==variable) {
            evaluation_stack.push(variables.at(curr_token.token));
        } else if (curr_token.type==number) {
            evaluation_stack.push(get_number(curr_token.token));
        } else if (curr_token.type==unary_operator || curr_token.type==function) {
            double x = evaluation_stack.top();
            evaluation_stack.pop();
            // Dummy implementation
            cout << "Evaluating " << curr_token.token << "(" << x << ")\n";
            //Push 0 as dummy value
            evaluation_stack.push(1);
        } else if (curr_token.type==binary_operator) {
            double x = evaluation_stack.top();
            evaluation_stack.pop();
            double y = evaluation_stack.top();
            evaluation_stack.pop();
            // Dummy implementation
            cout << "Evaluating " << x << curr_token.token << y << "\n";
            evaluation_stack.push(2);
        }

        stack<double> temp;
        while (!evaluation_stack.empty()) {
            temp.push(evaluation_stack.top());
            evaluation_stack.pop();
        }
        while(!temp.empty()) {
            evaluation_stack.push(temp.top());
            cout << temp.top() << " ";
            temp.pop();
        }
        output_queue.pop();
    }
    if (evaluation_stack.size()==1) {
        return evaluation_stack.top();
    } else {
        cerr << "Stack not compeletely evaluated: " << evaluation_stack.top();
        return evaluation_stack.top();
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
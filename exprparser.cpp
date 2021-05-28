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

OperationDetails::OperationDetails() {}

OperationDetails::OperationDetails(string function, NodeMathOperation operation, short no_of_params) {
    this->function = function;
    this->operation = operation;
    this->no_of_params = no_of_params;
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

optional<OperationDetails> ExpressionParser::get_function_details(string op) {
    //cout << "Searching for " << op << "\n";
    auto search = FUNCTIONS_MAPPING.find(op);
    if (search==FUNCTIONS_MAPPING.end()) {
        return {};
    } else {
        return search->second;
    }
}

ExpressionToken::ExpressionToken() {}

ExpressionToken::ExpressionToken(string token, TokenType type) {
    this->token = token;
    this->type = type;
    this->token_value = NAN;
    this->operation = NODE_MATH_ADD;
}

ExpressionToken::ExpressionToken(string token, TokenType type, float value) {
    this->token = token;
    this->type = type;
    this->token_value = value;
    this->operation = NODE_MATH_ADD;
}

ExpressionToken::ExpressionToken(string token, TokenType type, NodeMathOperation operation) {
    this->token = token;
    this->type = type;
    this->token_value = NAN;
    this->operation = operation;
}

NumberToken::NumberToken() {}

NumberToken::NumberToken(string text) {
    this->text=text;
    this->value=NAN;
}

NumberToken::NumberToken(string text, float value) {
    this->text=text;
    this->value=value;
}

void NumberToken::set_value(float value) {
    this->value=value;
}

OperationToken::OperationToken() {}

OperationToken::OperationToken(string text, bool is_prefix, bool is_function, short no_of_params) {
    this->text=text;
    this->is_prefix=is_prefix;
    this->is_function=is_function;
    this->no_of_params=no_of_params;
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

bool ExpressionParser::is_exponent(char character) {
    return vector_contains(EXPONENTS, character);
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

// TODO - Does this need to be part of the class?
//        Looks like a utility method that has no 
//        dependencies other than std ones.  
bool ExpressionParser::is_number(string text) {
    char* pEnd;
    const char* text_c_str = text.c_str(); 

    float parsed_to_f = strtof(text.c_str(), &pEnd);
    size_t num_length = (size_t)(pEnd-text_c_str);
    bool valid_parse = text.size() == num_length;
    // cout << "strtof() got this: " << parsed_to_d << "; Length is " << num_length << ", Valid Parse=" << valid_parse << "\n";

    return valid_parse;
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

float ExpressionParser::get_number(string text) {
    char* pEnd;
    const char* text_c_str = text.c_str(); 
    float parsed_to_f = strtof(text.c_str(), &pEnd);
    size_t num_length = (size_t)(pEnd-text_c_str);
    bool valid_parse = text.size() == num_length;

    if (valid_parse) {
        return parsed_to_f;
    } else {
        return 0.0;
    }
}

bool ExpressionParser::is_function(string text) {
    auto search = FUNCTIONS_MAPPING.find(text);
    return (search!=FUNCTIONS_MAPPING.end());
}

/* Checks if current operator token has precedence over previous operator token */
bool ExpressionParser::has_precedence(ExpressionToken prev, ExpressionToken curr) {
    OperatorDetails prev_details = get_operator_details(prev.token[0]);
    OperatorDetails curr_details = get_operator_details(curr.token[0]);

    if (prev_details.precedence == curr_details.precedence) {
        return curr_details.associativity == right_associative;
    } else {
        return curr_details.precedence < prev_details.precedence;
    }
}

/* Checks if current operator token has precedence over previous operator token (using new operation token) */
bool ExpressionParser::has_precedence(OperationToken prev, OperationToken curr) {
    OperatorDetails prev_details = get_operator_details(prev.text[0]);
    OperatorDetails curr_details = get_operator_details(curr.text[0]);

    if (prev_details.precedence == curr_details.precedence) {
        return curr_details.associativity == right_associative;
    } else {
        return curr_details.precedence < prev_details.precedence;
    }
    return true;
}

void ExpressionParser::pop_opstack_to_outqueue() {
    output_queue.push(operator_stack.top());
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

bool ExpressionParser::add_token(int token_start, int token_end) {
    if (token_start<token_end) {
        string token_name(expression+token_start, expression+token_end);
        TokenType type;
        float token_value = NAN;
        NodeMathOperation operation;
        if (is_operator(expression[token_start])) {
            char prev_token_char = get_last_printable_char_before(token_start);
            if ((token_start==0)
                || (is_operator(prev_token_char) 
                    && prev_token_char!=')')) {
                if (token_name=="-") {
                    token_name="neg";
                    type = function_1param;
                } else if (token_name=="+") {
                    token_name="|";
                } else {
                    type = unary_operator;
                }
            } else {
                type = binary_operator;
            }
            optional<OperationDetails> op_map = get_function_details(token_name);
            if (op_map.has_value()) {
                operation = op_map.value().operation;
            }
        } else if (is_number(token_name)) {
            type = number;
            token_value = get_number(token_name);
        } else if (is_function(token_name)) {
            optional<OperationDetails> op_map = get_function_details(token_name);
            if (op_map.has_value()) {
                if (op_map.value().no_of_params==1) {
                    type = function_1param;
                } else if (op_map.value().no_of_params==2) {
                    type = function_2param;
                } else if (op_map.value().no_of_params==3) {
                    type = function_3param;
                }
                operation = op_map.value().operation;
            }
        } else if (is_variable(token_name)) {
            type = variable;
        } else {
            cerr << "Invalid token " << token_name << "\n";
            return false;
        }

        if (token_name=="|") {
            return true;
        }

        ExpressionToken token;
        if (type==number) {
            token = ExpressionToken(token_name, type, token_value);    
        } else if (type==function_1param || type==function_2param || type==function_3param) {
            token = ExpressionToken(token_name, type, operation);
        } else if (type==unary_operator || type==binary_operator) {
            token = ExpressionToken(token_name, type, operation);
        } else {
            token = ExpressionToken(token_name, type);
        }

        if (token.type==number || token.type==variable) {
            // cout << "Found a number" << "\n";
            output_queue.push(token);
        } else if (token.type==unary_operator
                    || token.type==binary_operator
                    || token.type==function_1param
                    || token.type==function_2param
                    || token.type==function_3param) {
            // cout << "Found an op" << "\n";
            if (token.token=="("
                || operator_stack.size()==0
                || (operator_stack.top().token=="(" && token.token[0]!=')')
                || token.type==function_1param
                || token.type==function_2param
                || token.type==function_3param
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
        // cout << token.token << " | ";
        // dump_stack(false);
        // cout << " | ";
        // dump_queue(false);
        // cout << "\n";
    }
    return true;
}

void ExpressionParser::parse() {
    int i=0;
    int token_start=0;
    char current_char = expression[i];
    while (current_char!='\0') {
        if(((is_operator(current_char) && (i==0 || !is_exponent(expression[i-1])))) 
            || (is_whitespace(current_char))
            || (is_operator(expression[i-1]) && (i<2 || !is_exponent(expression[i-2])))
            ) {
            bool token_added = add_token(token_start, i);
            if (!token_added) {
                cerr << "Token start: " << token_start << ", token end: " << i << "\n";
                throw invalid_argument("Parsing error, Invalid token found.");
            }
            token_start=i;
            if (is_whitespace(current_char)) {
                token_start++;
            }
        }
        i++;
        current_char = expression[i];
    }
    bool token_added = add_token(token_start, i);
    if (!token_added) {
        throw invalid_argument("Parsing error, Invalid token found. (adding operator)");
    }
    while (!operator_stack.empty()) {
        output_queue.push(operator_stack.top());
        operator_stack.pop();
    }
}

float ExpressionParser::evaluate(map<string, float> variables) {
    stack<float> evaluation_stack;

    cout << "\n";

    while (!output_queue.empty()) {
        ExpressionToken curr_token = output_queue.front();
        if (curr_token.type==variable) {
            // cout << "Pushing " << curr_token.token << "=" << variables.at(curr_token.token) << ", ";
            evaluation_stack.push(variables.at(curr_token.token));
        } else if (curr_token.type==number) {
            // cout << "Pushing " << curr_token.token_value << ", ";
            evaluation_stack.push(curr_token.token_value);
        } else if (curr_token.type==unary_operator || curr_token.type==function_1param) {
            float x = evaluation_stack.top();
            evaluation_stack.pop();
            // cout << "Evaluating " << curr_token.token << "[" << curr_token.operation<< "]" << "(" << x << ")\n";
            float result;
            bool success = try_dispatch_float_math_fl_to_fl(
                curr_token.operation, [&](auto math_function) {
                    result = math_function(x);
                });
            evaluation_stack.push(result);
        } else if (curr_token.type==binary_operator || curr_token.type==function_2param) {
            float y = evaluation_stack.top();
            evaluation_stack.pop();
            float x = evaluation_stack.top();
            evaluation_stack.pop();
            // cout << "Evaluating " << curr_token.token << "[" << curr_token.operation<< "]" << "(" << x << "," << y << ")";
            float result;
            bool success = try_dispatch_float_math_fl_fl_to_fl(
                curr_token.operation, [&](auto math_function) {
                    result = math_function(x, y);
                });
            cout << " = " << result << "\n";
            evaluation_stack.push(result);
        } else if (curr_token.type==function_3param) {
            float z = evaluation_stack.top();
            evaluation_stack.pop();
            float y = evaluation_stack.top();
            evaluation_stack.pop();
            float x = evaluation_stack.top();
            evaluation_stack.pop();
            cout << "Evaluating " << curr_token.token << "[" << curr_token.operation<< "]" << "(" << x << "," << y << "," << z << ")\n";
            float result;
            bool success = try_dispatch_float_math_fl_fl_fl_to_fl(
                curr_token.operation, [&](auto math_function) {
                    result = math_function(x, y, z);
                });
            evaluation_stack.push(result);
        } else {
            cout << "Did nothing!\n";
        }

        stack<float> temp;
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

float ExpressionParser::execute_math_operation(ExpressionToken operation, float a) {
    return 0;
}

float ExpressionParser::execute_math_operation(ExpressionToken operation, float a, float b) {
    return 0;
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
        if (token.type==number) {
            cout << token.token_value << " ";
        } else {
            cout << token.token << " ";
        }
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
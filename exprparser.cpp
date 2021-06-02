#include <iostream>
#include <sstream>

#include "exprparser.hpp"

using namespace std;

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

VariableToken::VariableToken() {}

VariableToken::VariableToken(string text) {
    this->text=text;
}

OperationToken::OperationToken() {}

OperationToken::OperationToken(string text, bool is_prefix, bool is_function, short no_of_params, NodeMathOperation operation) {
    this->text=text;
    this->is_prefix=is_prefix;
    this->is_function=is_function;
    this->no_of_params=no_of_params;
    this->operation=operation;
}

ExpressionParser::ExpressionParser() {}

ExpressionParser::ExpressionParser(const char* expression) {
    this->expression = expression;
}

void ExpressionParser::set_expression(const char *expression)
{
  this->expression = expression;
}

bool ExpressionParser::is_digit(char character)
{
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
}

// TODO - Does this need to be part of the class?
//        Looks like a utility method that has no 
//        dependencies other than std ones.  
bool ExpressionParser::is_number(string text) {
    char* pEnd;
    const char* text_c_str = text.c_str(); 

    /*float parsed_to_f = */strtof(text.c_str(), &pEnd);
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

bool ExpressionParser::is_constant(string text) {
    auto search = CONSTANTS.find(text);
    return (search!=CONSTANTS.end());
}

float ExpressionParser::get_constant(string text) {
    auto search = CONSTANTS.find(text);
    if (search!=CONSTANTS.end()) {
        return search->second;
    } else {
        return 0;
    }
}

bool ExpressionParser::is_function(string text) {
    auto search = FUNCTIONS_MAPPING.find(text);
    return (search!=FUNCTIONS_MAPPING.end());
}

/* Checks if current operator token has precedence over previous operator token (using new operation token) */
bool ExpressionParser::has_precedence(OperationToken* prev, OperationToken* curr) {
    OperatorDetails prev_details = get_operator_details(prev->text[0]);
    OperatorDetails curr_details = get_operator_details(curr->text[0]);

    if (prev_details.precedence == curr_details.precedence) {
        return curr_details.associativity == right_associative;
    } else {
        return curr_details.precedence < prev_details.precedence;
    }
    return true;
}

void ExpressionParser::pop_operationstack_to_outqueue() {
    output_queue_new.push(operation_stack.top());
    operation_stack.pop();
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
        NodeMathOperation operation;
        Expression_Token* token_new = nullptr;
        if (is_operator(expression[token_start])) {
            char prev_token_char = get_last_printable_char_before(token_start);
            bool is_prefix;
            bool is_function;
            if ((token_start==0)
                || (is_operator(prev_token_char) 
                    && prev_token_char!=')')) {
                if (token_name=="-") {

                    token_name="neg";
                    is_prefix = true;
                    is_function = true;
                } else if (token_name=="+") {

                    token_name="|";
                    is_prefix = true;
                    is_function = true;
                } else {

                    is_prefix = true;
                    is_function = false;
                }
            } else {

                is_prefix = false;
                is_function = false;
            }
            optional<OperationDetails> op_map = get_function_details(token_name);
            if (op_map.has_value()) {
                operation = op_map.value().operation;
                token_new = new OperationToken(token_name, is_prefix, is_function,
                                            op_map.value().no_of_params,
                                            op_map.value().operation);

            } else {
                token_new = new OperationToken(token_name, false, true,
                                                0, NODE_MATH_ABSOLUTE);
            }
        } else if (is_number(token_name)) {
            token_new = new NumberToken(token_name, get_number(token_name));
        } else if (is_function(token_name)) {
            optional<OperationDetails> op_map = get_function_details(token_name);
            if (op_map.has_value()) {
                operation = op_map.value().operation;
                token_new = new OperationToken(token_name, true, true,
                                            op_map.value().no_of_params,
                                            op_map.value().operation);
            } else {
                cerr << "Should never reach here. If token is a function, then op_map MUST get created.\n";
            }
        } else if (is_constant(token_name)) {
            token_new = new NumberToken(token_name, get_constant(token_name));
        } else if (is_variable(token_name)) {
            token_new = new VariableToken(token_name);
        } else {
            cerr << "Invalid token " << token_name << "\n";
            return false;
        }

        if (token_name=="|") {
            return true;
        }

        if (dynamic_cast<NumberToken*>(token_new) || dynamic_cast<VariableToken*>(token_new)) {
            // cout << "Found a number" << "\n";
            output_queue_new.push(token_new);
        } else if (dynamic_cast<OperationToken*>(token_new)) {
            OperationToken* opToken = dynamic_cast<OperationToken*>(token_new);
            // cout << "Found an op: " << opToken->text << "\n";
            if (opToken->text=="("
                || operation_stack.size()==0
                || (operation_stack.top()->text=="(" && opToken->text[0]!=')' && opToken->text[0]!=',')
                || opToken->is_prefix==true) {
                // cout << "\tFirst element to stack, or handling brackets, or handling prefix operation: " << opToken->text << "\n";
                operation_stack.push(opToken);
            } else if (opToken->text==")" || opToken->text==",") {
                // cout << "\top is " << opToken->text << "\n";
                while (!operation_stack.empty() && operation_stack.top()->text[0]!='(') {
                    // cout << "Moving " << operator_stack.top().token << " to queue\n";
                    pop_operationstack_to_outqueue();
                }
                // cout << "Hopefully popping left bracket off stack: " << operator_stack.top().token << "\n";
                if (opToken->text!=",") {
                    if (!operation_stack.empty()) {
                        operation_stack.pop();
                    }
                }
            } else if (!has_precedence(operation_stack.top(), opToken)) {
                // cout << "\top doesn't have precedence:: " << opToken->text << "\n";
                do {
                    pop_operationstack_to_outqueue();
                } while (!operation_stack.empty() && !has_precedence(operation_stack.top(), opToken) && operation_stack.top()->text[0]!='(');
                operation_stack.push(opToken);
            } else {
                // cout << "\top has precedence:: " << opToken->text << "\n";
                // cout << "\tPushing op to stack: " << opToken->text << "\n";
                operation_stack.push(opToken);
            }
        }
        // cout << token_new->text << " | ";
        // dump_stack(false);
        // cout << " | ";
        // dump_queue(false);
        // cout << "\n";
    }
    return true;
}

void ExpressionParser::parse() {
    operation_stack = stack<OperationToken *>();
    output_queue_new = queue<Expression_Token *>();
    int i = 0;
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
        this->valid_queue = false;
        throw invalid_argument("Parsing error, Invalid token found. (adding operator)");
    }
    while (!operation_stack.empty()) {
        output_queue_new.push(operation_stack.top());
        operation_stack.pop();
    }
    this->valid_queue = true;
}

bool ExpressionParser::can_evaluate() {
    return valid_queue;
}

float ExpressionParser::evaluate(map<string, float> variables)
{
  stack<float> evaluation_stack;

  // cout << "\n";

  while (!output_queue_new.empty()) {
    Expression_Token *curr_token = output_queue_new.front();
    if (dynamic_cast<VariableToken *>(curr_token)) {
      // cout << "Pushing " << curr_token.token << "=" << variables.at(curr_token.token) << ", ";
        float value; 
        auto search = variables.find(curr_token->text);
        if (search==variables.end()) {
            throw invalid_argument("Missing value for variable: " + curr_token->text);
        } else {
            value = search->second;
        }

        evaluation_stack.push(value);
    }
    else if (dynamic_cast<NumberToken *>(curr_token)) {
      NumberToken *num_token = dynamic_cast<NumberToken *>(curr_token);
      // cout << "Pushing " << num_token->value << ", ";
      evaluation_stack.push(num_token->value);
    }
    else if (dynamic_cast<OperationToken *>(curr_token)) {
      OperationToken *opToken = dynamic_cast<OperationToken *>(curr_token);
      if (opToken->no_of_params == 1) {
        float x = evaluation_stack.top();
        evaluation_stack.pop();
        // cout << "Evaluating " << opToken->text << "[" << curr_token.operation<< "]" << "(" << x
        // << ")\n";
        float result;
        bool success = blender::nodes::try_dispatch_float_math_fl_to_fl(
            opToken->operation, [&](auto math_function) {
              result = math_function(x);
            });
        evaluation_stack.push(result);
      }
      else if (opToken->no_of_params == 2) {
        float y = evaluation_stack.top();
        evaluation_stack.pop();
        float x = evaluation_stack.top();
        evaluation_stack.pop();
        // cout << "Evaluating " << curr_token.token << "[" << curr_token.operation<< "]" << "(" <<
        // x << "," << y << ")";
        float result;
        bool success = blender::nodes::try_dispatch_float_math_fl_fl_to_fl(
            opToken->operation, [&](auto math_function) {
              result = math_function(x, y);
            });
        // cout << " = " << result << "\n";
        evaluation_stack.push(result);
      }
      else if (opToken->no_of_params == 3) {
        float z = evaluation_stack.top();
        evaluation_stack.pop();
        float y = evaluation_stack.top();
        evaluation_stack.pop();
        float x = evaluation_stack.top();
        evaluation_stack.pop();
        // cout << "Evaluating " << curr_token.token << "[" << curr_token.operation<< "]" << "(" <<
        // x << "," << y << "," << z << ")\n";
        float result;
        bool success = blender::nodes::try_dispatch_float_math_fl_fl_fl_to_fl(
            opToken->operation,
            [&](auto math_function) {
              result = math_function(x, y, z);
            });
        evaluation_stack.push(result);
      }
      else {
        cerr << "Only 1, 2 or 3 parameters are supported Found " << opToken->text
          << " with " << opToken->no_of_params << ".\n";
        throw invalid_argument("Only 1-3 parameters supported. Found " + opToken->no_of_params);
      }
    }
    else {
      cerr << "Unrecognized token type while evaluating!\n";
      throw invalid_argument("Only 1-3 parameters supported. Found " + curr_token->text);
    }

    // stack<float> temp;
    // while (!evaluation_stack.empty()) {
    //     temp.push(evaluation_stack.top());
    //     evaluation_stack.pop();
    // }
    // while(!temp.empty()) {
    //     evaluation_stack.push(temp.top());
    //     cout << temp.top() << " ";
    //     temp.pop();
    // }
    output_queue_new.pop();
  }
  if (evaluation_stack.size() == 1) {
    return evaluation_stack.top();
  }
  else if (evaluation_stack.size() == 0) {
    cerr << "Nothing to return!\n";
    return 0;
  }
  else {
    cerr << "Stack not compeletely evaluated: " << expression << " Stack size='"
         << evaluation_stack.size() << "'\n";
    return evaluation_stack.top();
  }
}

void ExpressionParser::dump_queue(bool with_headers) {
    if (with_headers) {
        cout << "Output Queue" << "\n";
        cout << "============" << "\n";
    }
    for (int i=0; i<output_queue_new.size(); i++) {
        Expression_Token* token = output_queue_new.front();
        output_queue_new.push(token);
        output_queue_new.pop();

        NumberToken* num_token = dynamic_cast<NumberToken*>(token);
        if (num_token) {
            cout << num_token->value << " ";
        } else {
            cout << token->text << " ";
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
    stack<OperationToken*> temp;
    while (!operation_stack.empty()) {
        temp.push(operation_stack.top());
        operation_stack.pop();
    }
    while(!temp.empty()) {
        operation_stack.push(temp.top());
        cout << temp.top()->text << " ";
        temp.pop();
    }
    if (with_headers) {
        cout << "\n" << "------------" << "\n";
    }
}
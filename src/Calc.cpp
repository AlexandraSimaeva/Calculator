#include "Calc.h"
#include <stack>
#include <vector>
#include <sstream>
#include <cctype>
#include <cmath>

bool Calculator::isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '(' || c == ')';
}

bool Calculator::isDigit(char c) {
    return std::isdigit(c) || c == '.';
}

int Calculator::getPriority(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    return 0;
}

std::string Calculator::handleUnaryMinus(const std::string& expr) {
    std::string result;
    for (size_t i = 0; i < expr.size(); i++) {
        if (expr[i] == '-' && (i == 0 || expr[i-1] == '(' || isOperator(expr[i-1]))) {
            result += "(0-";
            i++;
            while (i < expr.size() && isDigit(expr[i])) {
                result += expr[i];
                i++;
            }
            result += ')';
            i--;
        } else {
            result += expr[i];
        }
    }
    return result;
}

bool Calculator::validateExpression(const std::string& expr) {
    int bracketCount = 0;
    bool lastWasOperator = true;
    
    for (size_t i = 0; i < expr.size(); i++) {
        char c = expr[i];
        if (c == ' ') continue;
        
        if (c == '(') {
            bracketCount++;
            lastWasOperator = true;
        } 
        else if (c == ')') {
            bracketCount--;
            if (bracketCount < 0) return false;
            lastWasOperator = false;
        }
        else if (isOperator(c) && c != '(' && c != ')') {
            if (lastWasOperator) return false;
            lastWasOperator = true;
        }
        else if (isdigit(c)) {
            lastWasOperator = false;
        }
        else if (c == '.') {
            // Проверка корректности десятичной точки
            if (i == 0 || !isdigit(expr[i-1]) || 
                i == expr.size()-1 || !isdigit(expr[i+1])) {
                return false;
            }
        }
        else {
            return false;
        }
    }
    
    return bracketCount == 0 && !lastWasOperator;
}

double Calculator::evaluate(const std::string& expression) {
    // Валидация
    if (!validateExpression(expression)) {
        throw std::runtime_error("Invalid expression");
    }
    
    std::string expr = handleUnaryMinus(expression);
    std::vector<std::string> rpn;
    std::stack<char> operators;
    std::string number;
    
    // Shunting Yard алгоритм
    for (size_t i = 0; i < expr.size(); i++) {
        char c = expr[i];
        if (c == ' ') continue;
        
        if (isDigit(c)) {
            number += c;
            if (i + 1 >= expr.size() || !isDigit(expr[i + 1])) {
                rpn.push_back(number);
                number.clear();
            }
        }
        else if (c == '(') {
            operators.push(c);
        }
        else if (c == ')') {
            while (!operators.empty() && operators.top() != '(') {
                rpn.push_back(std::string(1, operators.top()));
                operators.pop();
            }
            if (!operators.empty()) operators.pop();
        }
        else if (isOperator(c)) {
            while (!operators.empty() && getPriority(operators.top()) >= getPriority(c)) {
                rpn.push_back(std::string(1, operators.top()));
                operators.pop();
            }
            operators.push(c);
        }
    }
    
    while (!operators.empty()) {
        rpn.push_back(std::string(1, operators.top()));
        operators.pop();
    }
    
    // Вычисление RPN
    std::stack<double> values;
    for (const auto& token : rpn) {
        if (token.size() == 1 && isOperator(token[0])) {
            if (values.size() < 2) throw std::runtime_error("Invalid expression");
            
            double b = values.top(); values.pop();
            double a = values.top(); values.pop();
            
            switch (token[0]) {
                case '+': values.push(a + b); break;
                case '-': values.push(a - b); break;
                case '*': values.push(a * b); break;
                case '/': 
                    if (b == 0) throw std::runtime_error("Division by zero");
                    values.push(a / b); 
                    break;
                default: throw std::runtime_error("Unknown operator");
            }
        } else {
            values.push(std::stod(token));
        }
    }
    
    if (values.size() != 1) throw std::runtime_error("Invalid expression");
    return values.top();
}
#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <string>
#include <stdexcept>

class Calculator {
 
    
private: // внутрянка
    bool isOperator(char c);
    bool isDigit(char c);
    int getPriority(char op);
    std::string handleUnaryMinus(const std::string& expr);
    bool validateExpression(const std::string& expr);
public://основной метод
    double evaluate(const std::string& expression);
};

#endif
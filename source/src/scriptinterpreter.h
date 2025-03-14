#pragma once
#include <string>
#include <map>
#include <vector>
#include <thread>
#include <stdexcept>
#include <cctype>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <regex>
#include <mutex>
#include <windows.h>

class QiVar {
public:
    const char* exception_type_invalid = "QiVar: type is invalid";
    enum Type { t_num, t_str };
    int type;
    double num;
    std::string str;

    QiVar() : type(t_num) {}
    QiVar(double num) : type(t_num), num(num) {}
    QiVar(const char* str) : type(t_str), str(str) {}
    QiVar(const std::string& str) : type(t_str), str(str) {}

    static std::string removeCommas(const std::string& str) {
        std::string result = str;
        result.erase(std::remove(result.begin(), result.end(), ','), result.end());
        return result;
    }

    static bool isNumber(const std::string& str) {
        try {
            std::string cleanedStr = removeCommas(str);
            std::size_t pos;
            std::stod(cleanedStr, &pos);
            return pos == cleanedStr.size();
        }
        catch (...) { return false; }
    }

    static bool isInteger(const std::string& str) {
        try {
            std::string cleanedStr = removeCommas(str);
            std::size_t pos;
            std::stoi(cleanedStr, &pos);
            return pos == cleanedStr.size();
        }
        catch (...) { return false; }
    }

    static double toNumber(const std::string& str) {
        try {
            std::string cleanedStr = removeCommas(str);
            size_t pos;
            double result = std::stod(cleanedStr, &pos);
            return pos == cleanedStr.length() ? result : 0.0;
        }
        catch (...) { return 0.0; }
    }

    static std::string toString(double num) {
        std::string str = std::to_string(num);
        if (std::fmod(num, 1.0) == 0.0) return std::to_string((long long)(num));
        str.erase(str.find_last_not_of('0') + 1, std::string::npos);
        if (!str.empty() && str.back() == '.') str.pop_back();
        return str;
    }

    bool isNumber() const { return type == t_num; }
    bool isString() const { return type == t_str; }

    double toNumber() const {
        return type == t_num ? num : toNumber(str);
    }

    std::string toString() const {
        return type == t_num ? toString(num) : str;
    }

    QiVar operator+(const QiVar& other) {
        double a = this->toNumber();
        double b = other.toNumber();
        return QiVar(a + b);
    }

    QiVar operator-(const QiVar& other) {
        double a = this->toNumber();
        double b = other.toNumber();
        return QiVar(a - b);
    }

    QiVar operator*(const QiVar& other) {
        double a = this->toNumber();
        double b = other.toNumber();
        return QiVar(a * b);
    }

    QiVar operator/(const QiVar& other) {
        double a = this->toNumber();
        double b = other.toNumber();
        return b != 0 ? QiVar(a / b) : QiVar(0.0);
    }

    bool operator>(const QiVar& other) {
        return this->toNumber() > other.toNumber();
    }

    bool operator<(const QiVar& other) {
        return this->toNumber() < other.toNumber();
    }

    bool operator==(const QiVar& other) const {
        if (this->isNumber() && other.isNumber()) {
            return this->toNumber() == other.toNumber();
        }
        return this->toString() == other.toString();
    }

    QiVar merge(const QiVar& other) {
        return QiVar(this->toString() + other.toString());
    }
};

using QiVarMap = std::map<std::string, QiVar>;

class QiScriptInterpreter
{
    enum TokenType { VARIABLE, NUMBER, STRING, OPERATOR, PAREN };
    struct Token {
        TokenType type;
        std::string value;
        int precedence;

        Token(TokenType t, const std::string& v, int p = -1)
            : type(t), value(v), precedence(p) {
        }
    };
    QiVarMap globalVariables;
    std::mutex globalMutex;
public:
    static inline const std::string error_invalid_character = "error_invalid_character";
    static inline const std::string error_not_enough_operands = "error_not_enough_operands";
    static inline const std::string error_invalid_expression = "error_invalid_expression";
    std::string trim(const std::string& s) {
        auto start = s.find_first_not_of(" \t");
        auto end = s.find_last_not_of(" \t");
        return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
    }
    std::vector<Token> tokenize(const std::string& expr) {
        std::vector<Token> tokens;
        size_t pos = 0;
        while (pos < expr.length()) {
            char c = expr[pos];
            if (isspace(c)) { pos++; continue; }

            // Variables
            if (c == '$' || isalpha(c)) {
                size_t start = pos++;
                while (pos < expr.length() && (isalnum(expr[pos]) || expr[pos] == '_')) pos++;
                tokens.emplace_back(VARIABLE, expr.substr(start, pos - start));

                // Numbers
            }
            else if (isdigit(c) || c == '.') {
                size_t start = pos++;
                bool hasDot = (c == '.');
                while (pos < expr.length() && (isdigit(expr[pos]) || (expr[pos] == '.' && !hasDot))) {
                    if (expr[pos] == '.') hasDot = true;
                    pos++;
                }
                tokens.emplace_back(NUMBER, expr.substr(start, pos - start));

                // Strings
            }
            else if (c == '\'') {
                size_t start = ++pos;
                while (pos < expr.length() && expr[pos] != '\'') pos++;
                tokens.emplace_back(STRING, expr.substr(start, pos - start));
                pos++;

                // Operators
            }
            else if (c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || c == '>' || c == '<' || c == '=') {
                if (c == '=' && pos + 1 < expr.length() && expr[pos + 1] == '=') {
                    tokens.emplace_back(OPERATOR, "==", 1);
                    pos += 2;
                }
                else {
                    std::string op(1, c);
                    int prec = 0;
                    if (c == '*' || c == '/') prec = 3;
                    else if (c == '+' || c == '-') prec = 2;
                    else if (c == '>' || c == '<' || c == '=') prec = 1;
                    else if (c == '^') prec = 0;
                    tokens.emplace_back(OPERATOR, op, prec);
                    pos++;
                }

                // Parentheses
            }
            else if (c == '(' || c == ')') {
                tokens.emplace_back(PAREN, std::string(1, c));
                pos++;

            }
            else {
                throw std::runtime_error(error_invalid_character + std::string(1, c));
            }
        }
        return tokens;
    }
    std::vector<Token> infixToPostfix(const std::vector<Token>& tokens) {
        std::vector<Token> output;
        std::vector<Token> opStack;

        for (const auto& token : tokens) {
            if (token.type == VARIABLE || token.type == NUMBER || token.type == STRING) {
                output.push_back(token);
            }
            else if (token.type == PAREN) {
                if (token.value == "(") {
                    opStack.push_back(token);
                }
                else {
                    while (!opStack.empty() && opStack.back().value != "(") {
                        output.push_back(opStack.back());
                        opStack.pop_back();
                    }
                    if (!opStack.empty()) opStack.pop_back();
                }
            }
            else if (token.type == OPERATOR) {
                while (!opStack.empty() && opStack.back().type == OPERATOR &&
                    opStack.back().precedence >= token.precedence) {
                    output.push_back(opStack.back());
                    opStack.pop_back();
                }
                opStack.push_back(token);
            }
        }

        while (!opStack.empty()) {
            output.push_back(opStack.back());
            opStack.pop_back();
        }

        return output;
    }
    std::string processInterpolation(const std::string& str, const QiVarMap& localVariables) {
        std::string result;
        size_t pos = 0;
        while (pos < str.length()) {
            size_t start = str.find('{', pos);
            if (start == std::string::npos) {
                result += str.substr(pos);
                break;
            }
            result += str.substr(pos, start - pos);
            size_t end = str.find('}', start);
            if (end == std::string::npos) {
                result += str.substr(start);
                break;
            }

            std::string varName = str.substr(start + 1, end - start - 1);
            bool isGlobal = (!varName.empty() && varName[0] == '$');
            QiVar value;

            if (isGlobal) {
                auto it = globalVariables.find(varName.substr(1));
                value = (it != globalVariables.end()) ? it->second : QiVar(0.0);
            }
            else {
                auto it = localVariables.find(varName);
                value = (it != localVariables.end()) ? it->second : QiVar(0.0);
            }

            result += value.toString();
            pos = end + 1;
        }
        return result;
    }
    QiVar evaluatePostfix(const std::vector<Token>& postfix, QiVarMap& localVariables) {
        std::vector<QiVar> stack;

        for (const auto& token : postfix) {
            if (token.type == VARIABLE) {
                bool isGlobal = (token.value[0] == '$');
                std::string varName = isGlobal ? token.value.substr(1) : token.value;
                QiVar value;

                if (isGlobal) {
                    auto it = globalVariables.find(varName);
                    value = (it != globalVariables.end()) ? it->second : QiVar(0.0);
                }
                else {
                    auto it = localVariables.find(varName);
                    value = (it != localVariables.end()) ? it->second : QiVar(0.0);
                }
                stack.push_back(value);

            }
            else if (token.type == NUMBER) {
                stack.push_back(QiVar(QiVar::toNumber(token.value)));

            }
            else if (token.type == STRING) {
                stack.push_back(QiVar(processInterpolation(token.value, localVariables)));

            }
            else if (token.type == OPERATOR) {
                if (stack.size() < 2) throw std::runtime_error(error_not_enough_operands);

                QiVar right = stack.back(); stack.pop_back();
                QiVar left = stack.back(); stack.pop_back();
                QiVar result;

                if (token.value == "+") result = left + right;
                else if (token.value == "-") result = left - right;
                else if (token.value == "*") result = left * right;
                else if (token.value == "/") result = left / right;
                else if (token.value == ">") result = QiVar(left > right ? 1.0 : 0.0);
                else if (token.value == "<") result = QiVar(left < right ? 1.0 : 0.0);
                else if (token.value == "^") result = left.merge(right);
                else if (token.value == "==") result = QiVar(left == right ? 1.0 : 0.0);

                stack.push_back(result);
            }
        }

        if (stack.size() != 1) throw std::runtime_error(error_invalid_expression);
        return stack.back();
    }
    std::vector<std::string> splitBySemicolonAndNewline(const std::string& code) {
        std::vector<std::string> result;
        size_t start = 0;
        size_t end = code.find_first_of(";\n");

        while (end != std::string::npos) {
            std::string statement = trim(code.substr(start, end - start));
            if (!statement.empty()) {
                result.push_back(statement);
            }
            start = end + 1;
            end = code.find_first_of(";\n", start);
        }
        std::string lastStatement = trim(code.substr(start));
        if (!lastStatement.empty()) {
            result.push_back(lastStatement);
        }

        return result;
    }
    QiVar execute(const std::string& code, QiVarMap& localVariables) {
        std::unique_lock<std::mutex>(globalMutex);
        auto tokens = tokenize(code);
        auto postfix = infixToPostfix(tokens);
        return evaluatePostfix(postfix, localVariables);
    }
    void interpret(const std::string& code, QiVarMap& localVariables) {
        std::unique_lock<std::mutex>(globalMutex);
        size_t eqPos = code.find('=');
        if (eqPos == std::string::npos) {
            execute(code, localVariables);
            return;
        }

        std::string left = trim(code.substr(0, eqPos));
        std::string expr = trim(code.substr(eqPos + 1));

        auto tokens = tokenize(expr);
        auto postfix = infixToPostfix(tokens);
        QiVar value = evaluatePostfix(postfix, localVariables);

        bool isGlobal = (!left.empty() && left[0] == '$');
        std::string varName = isGlobal ? left.substr(1) : left;

        if (isGlobal) {
            globalVariables[varName] = value;
        }
        else {
            localVariables[varName] = value;
        }
    }
    void interpretAll(const std::string& code, QiVarMap& localVariables) {
        std::unique_lock<std::mutex>(globalMutex);
        auto operations = splitBySemicolonAndNewline(code);
        for (const auto& op : operations) {
            size_t commentPos = op.find('#');
            std::string trimmedOp = (commentPos != std::string::npos) ? trim(op.substr(0, commentPos)) : trim(op);
            if (!trimmedOp.empty()) {
                interpret(trimmedOp, localVariables);
            }
        }
    }
    static std::string makeString(const std::string str)
    {
        std::string result = str;
        result.erase(std::remove(result.begin(), result.end(), '\''), result.end());
        result = "'" + result + "'";
        return result;
    }
    std::string removeWrap(const std::string str)
    {
        std::string result = str;
        result.erase(std::remove(result.begin(), result.end(), '\r'), result.end());
        result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());
        return result;
    }
    bool makeValue(const std::string& var, QiVar value, QiVarMap& localVariables)
    {
        if (isValidVariableName(var))
        {
            if (value.isNumber())
            {
                interpret(var + "=" + value.toString(), localVariables);
                return true;
            }
            else if (value.isString())
            {
                interpret(var + "=" + makeString(value.str), localVariables);
                return true;
            }
        }
        return false;
    }
    static bool isValidVariableName(const std::string& name)
    {
        if (name.empty()) return false;
        char firstChar = name[0];
        if (!std::isalpha(firstChar) && firstChar != '_' && firstChar != '$') return false;

        for (size_t i = 1; i < name.size(); ++i)
        {
            char ch = name[i];
            if (!std::isalnum(ch) && ch != '_') return false;
        }
        return true;
    }
    static bool parseError(std::string msg) {
        if (msg.find(QiScriptInterpreter::error_invalid_character) != std::string::npos) {
            MessageBoxW(nullptr, L"变量操作语句出现无效的字符", String::toWString(msg).c_str(), MB_ICONERROR);
        }
        else if (msg.find(QiScriptInterpreter::error_invalid_expression) != std::string::npos) {
            MessageBoxW(nullptr, L"变量操作语句无效", String::toWString(msg).c_str(), MB_ICONERROR);
        }
        else if (msg.find(QiScriptInterpreter::error_not_enough_operands) != std::string::npos) {
            MessageBoxW(nullptr, L"缺少变量操作符参数", String::toWString(msg).c_str(), MB_ICONERROR);
        }
        else if (msg.find("==") != std::string::npos) {
            MessageBoxW(nullptr, L"== 操作符使用错误", String::toWString(msg).c_str(), MB_ICONERROR);
        }
        return false;
    }
};
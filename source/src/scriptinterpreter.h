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

    static int random(int max, int min = 0)
    {
        srand(clock());
        if (min > max) std::swap(min, max);
        return min + (rand() % (max - min + 1));
    }

    static std::string removeCommas(const std::string& str) {
        std::string result = str;
        result.erase(std::remove(result.begin(), result.end(), ','), result.end());
        return result;
    }

    static std::string removePoints(const std::string& str) {
        std::string result = str;
        result.erase(std::remove(result.begin(), result.end(), '.'), result.end());
        return result;
    }

    static std::string removeChars(const std::string& str) {
        std::string result;
        for (char ch : str) {
            if (ch >= '0' && ch <= '9') result += ch;
        }
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

    static int toInteget(const std::string& str) {
        return toNumber(str);
    }

    static std::string toString(double num) {
        std::string str = std::to_string(num);
        if (std::fmod(num, 1.0) == 0.0) return std::to_string((long long)(num));
        str.erase(str.find_last_not_of('0') + 1, std::string::npos);
        if (!str.empty() && str.back() == '.') str.pop_back();
        return str;
    }

    static std::string toString(int num) {
        std::string str = std::to_string(num);
        return str;
    }

    bool isNumber() const { return type == t_num; }
    bool isString() const { return type == t_str; }

    double toNumber() const {
        return type == t_num ? num : toNumber(str);
    }

    double toInteger() const {
        return type == t_num ? (int)num : toInteget(str);
    }

    std::string toString() const {
        return type == t_num ? toString(num) : str;
    }

    std::string sub(size_t where, size_t length) const {
        std::string str = toString();
        if (str.empty()) return std::string();
        if (where > str.size()) where = str.size() - 1;
        return str.substr(where, length);
    }

    std::string subx(size_t where, size_t length) const {
        std::string str = toString();
        if (str.empty() || (!where && !length)) return std::string();
        size_t end = str.size() - where - 1;
        if (end >= str.size()) return std::string();
        size_t begin = end - length + 1;
        if (begin >= str.size()) begin = 0;
        return str.substr(begin, end - begin + 1);
    }

    double len() const {
        return type == t_num ? toString(num).size() : str.size();
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

    QiVar operator%(const QiVar& other) {
        int a = this->toInteger();
        int b = other.toInteger();
        return QiVar(a % b);
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
    enum TokenType { VARIABLE, NUMBER, STRING, OPERATOR, PAREN, FUNCTION, ARG_SEPARATOR };
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
    static inline const std::string error_unknown_functions = "error_unknown_functions";
    QiVarMap* varMap()
    {
        return &globalVariables;
    }
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

            // Functions and Variables
            if (c == '$' || isalpha(c)) {
                size_t start = pos++;
                while (pos < expr.length() && (isalnum(expr[pos]) || expr[pos] == '_')) {
                    pos++;
                }
                std::string identifier = expr.substr(start, pos - start);
                // Functions
                if (pos < expr.length() && expr[pos] == '(') {
                    tokens.emplace_back(FUNCTION, identifier, 4);
                    tokens.emplace_back(PAREN, "(");
                    pos++;
                }
                // Variables
                else {
                    tokens.emplace_back(VARIABLE, identifier);
                }
                continue;
            }
            else if (isdigit(c) || c == '.') {
                size_t start = pos++;
                bool hasDot = (c == '.');
                while (pos < expr.length() && (isdigit(expr[pos]) || (expr[pos] == '.' && !hasDot))) {
                    if (expr[pos] == '.') hasDot = true;
                    pos++;
                }
                tokens.emplace_back(NUMBER, expr.substr(start, pos - start));
            }
            // Strings
            else if (c == '\'') {
                size_t start = ++pos;
                while (pos < expr.length() && expr[pos] != '\'') pos++;
                tokens.emplace_back(STRING, expr.substr(start, pos - start));
                pos++;
            }
            // Operators
            else if (c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || c == '>' || c == '<' || c == '=' || c == '%') {
                if (c == '=' && pos + 1 < expr.length() && expr[pos + 1] == '=') {
                    tokens.emplace_back(OPERATOR, "==", 1);
                    pos += 2;
                }
                else {
                    std::string op(1, c);
                    int prec = 0;
                    if (c == '*' || c == '/' || c == '%') prec = 3;
                    else if (c == '+' || c == '-') prec = 2;
                    else if (c == '>' || c == '<' || c == '=') prec = 1;
                    else if (c == '^') prec = 0;
                    tokens.emplace_back(OPERATOR, op, prec);
                    pos++;
                }
            }
            // Parentheses
            else if (c == '(' || c == ')') {
                tokens.emplace_back(PAREN, std::string(1, c));
                pos++;
            }
            else if (c == ',') {
                tokens.emplace_back(ARG_SEPARATOR, ",");
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
                    if (!opStack.empty()) {
                        opStack.pop_back();
                        if (!opStack.empty() && opStack.back().type == FUNCTION) {
                            output.push_back(opStack.back());
                            opStack.pop_back();
                        }
                    }
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
            else if (token.type == FUNCTION) {
                opStack.push_back(token);
            }
            else if (token.type == ARG_SEPARATOR) {
                while (!opStack.empty() && opStack.back().value != "(") {
                    output.push_back(opStack.back());
                    opStack.pop_back();
                }
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
                else if (token.value == "%") result = left % right;
                else if (token.value == "==") result = QiVar(left == right ? 1.0 : 0.0);

                stack.push_back(result);
            }
            else if (token.type == FUNCTION) {
                std::string funcName = token.value;
                int argCount = 0;
                if (funcName == "sub" || funcName == "subx")
                {
                    argCount = 3;
                }
                else if (funcName == "rand") {
                    argCount = 2;
                }
                else if (funcName == "str" || funcName == "num" || funcName == "int" || funcName == "rmc" || funcName == "len") {
                    argCount = 1;
                }
                else if (funcName == "date" || funcName == "time" ||
                    funcName == "time_y" || funcName == "time_yd" || funcName == "time_ys" ||
                    funcName == "time_m" || funcName == "time_w" || funcName == "time_d" ||
                    funcName == "time_dh" || funcName == "time_dm" || funcName == "time_ds") {
                    argCount = 0;
                }
                else {
                    throw std::runtime_error(error_unknown_functions + std::string(": ") + funcName);
                }

                if (stack.size() < argCount) {
                    throw std::runtime_error(error_not_enough_operands);
                }

                std::vector<QiVar> args;
                for (int i = 0; i < argCount; ++i) {
                    args.insert(args.begin(), stack.back());
                    stack.pop_back();
                }

                QiVar result;
                if (funcName == "str") {
                    result = QiVar(args[0].toString());
                }
                else if (funcName == "num") {
                    result = QiVar(args[0].toNumber());
                }
                else if (funcName == "int")
                {
                    result = QiVar(args[0].toInteger());
                }
                else if (funcName == "rmc") {
                    std::string cleaned = QiVar::removeChars(args[0].toString());
                    result = QiVar(cleaned);
                }
                else if (funcName == "rand") {
                    result = QiVar((double)QiVar::random((int)(args[0].toNumber()), (int)(args[1].toNumber())));
                }
                else if (funcName == "sub") {
                    result = QiVar(args[0].sub(args[1].toInteger(), args[2].toInteger()));
                }
                else if (funcName == "subx") {
                    result = QiVar(args[0].subx(args[1].toInteger(), args[2].toInteger()));
                }
                else if (funcName == "len") {
                    result = QiVar(args[0].len());
                }
                else if (funcName == "date") {
                    time_t s = time(nullptr);
                    tm m; localtime_s(&m, &s);
                    result = QiVar(QiVar::toString(m.tm_year + 1900) + std::string("-") + QiVar::toString(m.tm_mon + 1) + std::string("-") + QiVar::toString(m.tm_mday));
                }
                else if (funcName == "time") {
                    time_t s = time(nullptr);
                    tm m; localtime_s(&m, &s);
                    result = QiVar(QiVar::toString(m.tm_hour) + std::string(":") + QiVar::toString(m.tm_min) + std::string(":") + QiVar::toString(m.tm_sec));
                }
                else if (funcName == "time_y") {
                    time_t s = time(nullptr);
                    tm m; localtime_s(&m, &s);
                    result = QiVar(QiVar::toString(m.tm_year + 1900));
                }
                else if (funcName == "time_yd") {
                    time_t s = time(nullptr);
                    tm m; localtime_s(&m, &s);
                    result = QiVar(QiVar::toString(m.tm_yday));
                }
                else if (funcName == "time_ys") {
                    result = QiVar((double)time(nullptr));
                }
                else if (funcName == "time_m") {
                    time_t s = time(nullptr);
                    tm m; localtime_s(&m, &s);
                    result = QiVar(QiVar::toString(m.tm_mon + 1));
                }
                else if (funcName == "time_w") {
                    time_t s = time(nullptr);
                    tm m; localtime_s(&m, &s);
                    result = QiVar(QiVar::toString(m.tm_wday));
                }
                else if (funcName == "time_d") {
                    time_t s = time(nullptr);
                    tm m; localtime_s(&m, &s);
                    result = QiVar(QiVar::toString(m.tm_mday));
                }
                else if (funcName == "time_dh") {
                    time_t s = time(nullptr);
                    tm m; localtime_s(&m, &s);
                    result = QiVar(QiVar::toString(m.tm_hour));
                }
                else if (funcName == "time_dm") {
                    time_t s = time(nullptr);
                    tm m; localtime_s(&m, &s);
                    result = QiVar(QiVar::toString(m.tm_min));
                }
                else if (funcName == "time_ds") {
                    time_t s = time(nullptr);
                    tm m; localtime_s(&m, &s);
                    result = QiVar(QiVar::toString(m.tm_sec));
                }
                else {
                    throw std::runtime_error(error_unknown_functions + std::string(": ") + funcName);
                }
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
        else if (msg.find(QiScriptInterpreter::error_unknown_functions) != std::string::npos) {
            MessageBoxW(nullptr, L"未知函数", String::toWString(msg).c_str(), MB_ICONERROR);
        }
        return false;
    }
};
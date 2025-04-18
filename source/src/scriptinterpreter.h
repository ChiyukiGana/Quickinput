﻿#pragma once
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
#include <functional>
#include <windows.h>
#include <src/inc_project.h>

namespace Qi
{
    inline std::function<void(std::string, int)> interpreter_pop;
}

class QiVar
{
public:
    const char* exception_type_invalid = "QiVar: type is invalid";
    enum Type
    {
        t_num,
        t_str
    };
    int type;
    double num;
    std::string str;

    QiVar() : type(t_num) {}
    QiVar(int num) : type(t_num), num((double)num) {}
    QiVar(double num) : type(t_num), num(num) {}
    QiVar(const char* str) : type(t_str), str(str) {}
    QiVar(const std::string& str) : type(t_str), str(str) {}

    static int random(int max, int min = 0)
    {
        srand(clock());
        if (min > max)
            std::swap(min, max);
        return min + (rand() % (max - min + 1));
    }

    static std::string removeStrs(const std::string& str, const std::vector<std::string>& strs_rm)
    {
        std::string result = str;
        for (const std::string& i : strs_rm)
        {
            size_t pos = 0;
            while ((pos = result.find(i, pos)) != std::string::npos) result.erase(pos, i.length());
        }
        return result;
    }
    static std::string removeStr(const std::string& str, const std::string& str_rm)
    {
        std::string result = str;
        size_t pos = 0;
        while ((pos = result.find(str_rm, pos)) != std::string::npos) result.erase(pos, str_rm.length());
        return result;
    }
    static std::string removeChars(const std::string& str, const std::string& chs_rm)
    {
        std::string result = str;
        result.erase(std::remove_if(result.begin(), result.end(), [&chs_rm](char c) { return chs_rm.find(c) != std::string::npos; }), result.end());
        return result;
    }
    static std::string removeChars(const std::string& str)
    {
        std::string result;
        for (char ch : str)
        {
            if (ch >= '0' && ch <= '9') result += ch;
        }
        return result;
    }
    static std::string removeChar(const std::string& str, char ch_rm)
    {
        std::string result = str;
        result.erase(std::remove(result.begin(), result.end(), ch_rm), result.end());
        return result;
    }
    static std::string removeCommas(const std::string& str)
    {
        return removeChar(str, ',');
    }
    static std::string removePoints(const std::string& str)
    {
        return removeChar(str, '.');
    }

    static bool isNumber(const std::string& str)
    {
        try
        {
            std::string cleanedStr = removeCommas(str);
            std::size_t pos;
            std::stod(cleanedStr, &pos);
            return pos == cleanedStr.size();
        }
        catch (...)
        {
            return false;
        }
    }
    static bool isInteger(const std::string& str)
    {
        try
        {
            std::string cleanedStr = removeCommas(str);
            std::size_t pos;
            std::stoi(cleanedStr, &pos);
            return pos == cleanedStr.size();
        }
        catch (...)
        {
            return false;
        }
    }
    static bool isInteger(double num)
    {
        return std::fmod(num, 1.0) == 0.0;
    }

    static double toNumber(const std::string& str)
    {
        try
        {
            std::string cleanedStr = removeCommas(str);
            size_t pos;
            double result = std::stod(cleanedStr, &pos);
            return pos == cleanedStr.length() ? result : 0.0;
        }
        catch (...)
        {
            return 0.0;
        }
    }
    static int toInteger(const std::string& str)
    {
        return toNumber(str);
    }
    static bool toBool(const std::string& str)
    {
        return str != "0" && stricmp(str.c_str(), "false") != 0;
    }
    static std::string toString(double num)
    {
        std::string str = std::to_string(num);
        if (std::fmod(num, 1.0) == 0.0)
            return std::to_string((long long)(num));
        str.erase(str.find_last_not_of('0') + 1, std::string::npos);
        if (!str.empty() && str.back() == '.')
            str.pop_back();
        return str;
    }
    static std::string toString(int num)
    {
        return std::to_string(num);
    }

    bool isNumber() const { return type == t_num; }
    bool isInteger() const { return (type == t_num && std::fmod(num, 1.0) == 0.0); }
    bool isString() const { return type == t_str; }

    double toNumber() const
    {
        return type == t_num ? num : toNumber(str);
    }
    int toInteger() const
    {
        return type == t_num ? (int)num : toInteger(str);
    }
    bool toBool() const
    {
        return type == t_num ? (bool)num : toBool(str);
    }
    std::string toString() const
    {
        return type == t_num ? toString(num) : str;
    }

    std::string sub(size_t where, size_t length = ~size_t(0)) const
    {
        std::string str = toString();
        if (str.empty())
            return std::string();
        if (where > str.size())
            where = str.size() - 1;
        return str.substr(where, length);
    }
    std::string subx(size_t where, size_t length = ~size_t(0)) const
    {
        std::string str = toString();
        if (str.empty() || (!where && !length))
            return std::string();
        size_t end = str.size() - where - 1;
        if (end >= str.size())
            return std::string();
        size_t begin = end - length + 1;
        if (begin >= str.size())
            begin = 0;
        return str.substr(begin, end - begin + 1);
    }

    double len() const
    {
        return type == t_num ? toString(num).size() : str.size();
    }
    QiVar merge(const QiVar& other)
    {
        return QiVar(this->toString() + other.toString());
    }

    QiVar operator+(const QiVar& other)
    {
        double a = this->toNumber();
        double b = other.toNumber();
        return QiVar(a + b);
    }
    QiVar operator-(const QiVar& other)
    {
        double a = this->toNumber();
        double b = other.toNumber();
        return QiVar(a - b);
    }
    QiVar operator*(const QiVar& other)
    {
        double a = this->toNumber();
        double b = other.toNumber();
        return QiVar(a * b);
    }
    QiVar operator/(const QiVar& other)
    {
        double a = this->toNumber();
        double b = other.toNumber();
        return b != 0 ? QiVar(a / b) : QiVar(0.0);
    }
    QiVar operator%(const QiVar& other)
    {
        int a = this->toInteger();
        int b = other.toInteger();
        return QiVar(a % b);
    }
    bool operator>(const QiVar& other)
    {
        return this->toNumber() > other.toNumber();
    }
    bool operator<(const QiVar& other)
    {
        return this->toNumber() < other.toNumber();
    }
    bool operator==(const QiVar& other) const
    {
        if (this->isNumber() && other.isNumber())
        {
            return this->toNumber() == other.toNumber();
        }
        return this->toString() == other.toString();
    }
};
using QiVarMap = std::map<std::string, QiVar>;

class QiFunc
{
    const size_t m_args;
    const size_t m_args_max;
public:
    QiFunc() = delete;
    QiFunc(size_t args, size_t args_max = 0) : m_args(args), m_args_max(args_max ? args_max : args) {}
    bool valid(size_t args) const
    {
        return args >= m_args && args <= m_args_max;
    }
    virtual QiVar exec(const std::vector<QiVar>& args) const { return QiVar(); }
};
class QiFuncMap : public std::map<std::string, std::unique_ptr<QiFunc>>
{
public:
    class QiFunc_date : public QiFunc
    {
    public:
        QiFunc_date() : QiFunc(0) {}
        QiVar exec(const std::vector<QiVar>& args) const
        {
            time_t s = time(nullptr);
            tm m;
            localtime_s(&m, &s);
            return QiVar::toString(m.tm_year + 1900) + std::string("-") + QiVar::toString(m.tm_mon + 1) + std::string("-") + QiVar::toString(m.tm_mday);
        }
    };
    class QiFunc_time : public QiFunc
    {
    public:
        QiFunc_time() : QiFunc(0) {}
        QiVar exec(const std::vector<QiVar>& args) const
        {
            time_t s = time(nullptr);
            tm m;
            localtime_s(&m, &s);
            return QiVar::toString(m.tm_hour) + std::string(":") + QiVar::toString(m.tm_min) + std::string(":") + QiVar::toString(m.tm_sec);
        }
    };
    class QiFunc_time_y : public QiFunc
    {
    public:
        QiFunc_time_y() : QiFunc(0) {}
        QiVar exec(const std::vector<QiVar>& args) const
        {
            time_t s = time(nullptr);
            tm m;
            localtime_s(&m, &s);
            return int(m.tm_year + 1900);
        }
    };
    class QiFunc_time_yd : public QiFunc
    {
    public:
        QiFunc_time_yd() : QiFunc(0) {}
        QiVar exec(const std::vector<QiVar>& args) const
        {
            time_t s = time(nullptr);
            tm m;
            localtime_s(&m, &s);
            return int(m.tm_yday);
        }
    };
    class QiFunc_time_ys : public QiFunc
    {
    public:
        QiFunc_time_ys() : QiFunc(0) {}
        QiVar exec(const std::vector<QiVar>& args) const
        {
            return int(time(nullptr));
        }
    };
    class QiFunc_time_m : public QiFunc
    {
    public:
        QiFunc_time_m() : QiFunc(0) {}
        QiVar exec(const std::vector<QiVar>& args) const
        {
            time_t s = time(nullptr);
            tm m;
            localtime_s(&m, &s);
            return int(m.tm_mon + 1);
        }
    };
    class QiFunc_time_w : public QiFunc
    {
    public:
        QiFunc_time_w() : QiFunc(0) {}
        QiVar exec(const std::vector<QiVar>& args) const
        {
            time_t s = time(nullptr);
            tm m;
            localtime_s(&m, &s);
            if (m.tm_wday == 0) m.tm_wday = 7;
            return int(m.tm_wday);
        }
    };
    class QiFunc_time_d : public QiFunc
    {
    public:
        QiFunc_time_d() : QiFunc(0) {}
        QiVar exec(const std::vector<QiVar>& args) const
        {
            time_t s = time(nullptr);
            tm m;
            localtime_s(&m, &s);
            return int(m.tm_mday);
        }
    };
    class QiFunc_time_dh : public QiFunc
    {
    public:
        QiFunc_time_dh() : QiFunc(0) {}
        QiVar exec(const std::vector<QiVar>& args) const
        {
            time_t s = time(nullptr);
            tm m;
            localtime_s(&m, &s);
            return int(m.tm_hour);
        }
    };
    class QiFunc_time_dm : public QiFunc
    {
    public:
        QiFunc_time_dm() : QiFunc(0) {}
        QiVar exec(const std::vector<QiVar>& args) const
        {
            time_t s = time(nullptr);
            tm m;
            localtime_s(&m, &s);
            return int(m.tm_min);
        }
    };
    class QiFunc_time_ds : public QiFunc
    {
    public:
        QiFunc_time_ds() : QiFunc(0) {}
        QiVar exec(const std::vector<QiVar>& args) const
        {
            time_t s = time(nullptr);
            tm m;
            localtime_s(&m, &s);
            return int(m.tm_sec);
        }
    };
    class QiFunc_time_ms : public QiFunc
    {
    public:
        QiFunc_time_ms() : QiFunc(0) {}
        QiVar exec(const std::vector<QiVar>& args) const
        {
            return int(clock());
        }
    };
    class QiFunc_str : public QiFunc
    {
    public:
        QiFunc_str() : QiFunc(1) {}
        QiVar exec(const std::vector<QiVar>& args) const
        {
            return args[0].toString();
        }
    };
    class QiFunc_num : public QiFunc
    {
    public:
        QiFunc_num() : QiFunc(1) {}
        QiVar exec(const std::vector<QiVar>& args) const
        {
            return args[0].toNumber();
        }
    };
    class QiFunc_int : public QiFunc
    {
    public:
        QiFunc_int() : QiFunc(1) {}
        QiVar exec(const std::vector<QiVar>& args) const
        {
            return args[0].toInteger();
        }
    };
    class QiFunc_rmc : public QiFunc
    {
    public:
        QiFunc_rmc() : QiFunc(1, 2) {}
        QiVar exec(const std::vector<QiVar>& args) const
        {
            if (args.size() == 2) return QiVar::removeChars(args[0].toString(), args[1].toString());
            return QiVar::removeChars(args[0].toString());
        }
    };
    class QiFunc_rms : public QiFunc
    {
    public:
        QiFunc_rms() : QiFunc(1, ~size_t(0)) {}
        QiVar exec(const std::vector<QiVar>& args) const
        {
            if (args.size() == 1) return QiVar::removeChars(args[0].toString());
            std::string result = args[0].toString();
            for (size_t i = 1; i < args.size(); i++) result = QiVar::removeStr(result, args[i].toString());
            return result;
        }
    };
    class QiFunc_len : public QiFunc
    {
    public:
        QiFunc_len() : QiFunc(1) {}
        QiVar exec(const std::vector<QiVar>& args) const
        {
            return args[0].len();
        }
    };
    class QiFunc_sleep : public QiFunc
    {
    public:
        QiFunc_sleep() : QiFunc(1) {}
        QiVar exec(const std::vector<QiVar>& args) const
        {
            Sleep(args[0].toInteger());
            return true;
        }
    };
    class QiFunc_pop : public QiFunc
    {
    public:
        QiFunc_pop() : QiFunc(1, 2) {}
        QiVar exec(const std::vector<QiVar>& args) const
        {
            Qi::interpreter_pop(args[0].toString(), args.size() > 1 ? args[1].toInteger() : 1000);
            return args[0];
        }
    };
    class QiFunc_rand : public QiFunc
    {
    public:
        QiFunc_rand() : QiFunc(1, 2) {}
        QiVar exec(const std::vector<QiVar>& args) const
        {
            return int(QiVar::random((int)(args[0].toInteger()), args.size() > 1 ? (int)(args[1].toInteger()) : 0));
        }
    };
    class QiFunc_cur_to : public QiFunc
    {
    public:
        QiFunc_cur_to() : QiFunc(2) {}
        QiVar exec(const std::vector<QiVar>& args) const
        {
            SIZE screen = System::screenSize();
            double x = args[0].toNumber();
            double y = args[1].toNumber();
            int ix;
            int iy;
            if (QiVar::isInteger(x)) ix = (float)x / (float)screen.cx * 65535.0f;
            else ix = x * 65535;
            if (QiVar::isInteger(y)) iy = (float)y / (float)screen.cx * 65535.0f;
            else iy = y * 65535;
            Input::MoveToA(ix, iy, Qi::key_info);
            return true;
        }
    };
    class QiFunc_cur_move : public QiFunc
    {
    public:
        QiFunc_cur_move() : QiFunc(2) {}
        QiVar exec(const std::vector<QiVar>& args) const
        {
            SIZE screen = System::screenSize();
            double x = args[0].toNumber();
            double y = args[1].toNumber();
            int ix;
            int iy;
            if (QiVar::isInteger(x)) ix = x;
            else ix = x * (double)screen.cx;
            if (QiVar::isInteger(y)) iy = y;
            else iy = y * (double)screen.cx;
            Input::Move(ix, iy, Qi::key_info);
            return true;
        }
    };
    class QiFunc_sub : public QiFunc
    {
    public:
        QiFunc_sub() : QiFunc(2, 3) {}
        QiVar exec(const std::vector<QiVar>& args) const
        {
            return args[0].sub(args[1].toInteger(),args.size() > 2 ? args[2].toInteger() : ~size_t(0));
        }
    };
    class QiFunc_subx : public QiFunc
    {
    public:
        QiFunc_subx() : QiFunc(2, 3) {}
        QiVar exec(const std::vector<QiVar>& args) const
        {
            return args[0].subx(args[1].toInteger(), args.size() > 2 ? args[2].toInteger() : ~size_t(0));
        }
    };
    class QiFunc_text_box : public QiFunc
    {
        enum
        {
            normal,
            warning,
            error
        };
    public:
        QiFunc_text_box() : QiFunc(1, 3) {}
        QiVar exec(const std::vector<QiVar>& args) const
        {
            std::string text(args[0].toString());
            std::string title("MessageBox"); if (args.size() > 1) title = args[1].toString();
            DWORD style = MB_YESNO | MB_TOPMOST;
            if (args.size() > 2)
            {
                if (args[2].toInteger() == warning) style |= MB_ICONWARNING;
                else if (args[2].toInteger() == error) style |= MB_ICONERROR;
            }
            return MessageBoxW(nullptr, String::toWString(text).c_str(), String::toWString(title).c_str(), style) == IDYES;
        }
    };
    class QiFunc_edit_box : public QiFunc
    {
    public:
        QiFunc_edit_box() : QiFunc(0, 3) {}
        QiVar exec(const std::vector<QiVar>& args) const
        {
            std::string title("TextEditBox"); if (args.size() > 0) title = args[0].toString();
            std::string text; if (args.size() > 1) text = args[1].toString();
            return String::toString(TextEditBox(nullptr, String::toWString(title).c_str(), String::toWString(text).c_str(), args.size() > 2 ? args[2].toBool() : false, WS_EX_TOPMOST, L"ICOAPP"));
        }
    };
    class QiFunc_volume : public QiFunc
    {
    public:
        QiFunc_volume() : QiFunc(0, 2) {}
        QiVar exec(const std::vector<QiVar>& args) const
        {
            return (double)Sound::SpeakerVolume(args.size() > 0 ? args[0].toInteger() : 10, args.size() > 1 ? args[1].toBool() : false);
        }
    };
    class QiFunc_file_read : public QiFunc
    {
    public:
        QiFunc_file_read() : QiFunc(1) {}
        QiVar exec(const std::vector<QiVar>& args) const
        {
            QByteArray data;
            if (File::LoadText(args[0].toString().c_str(), data)) return QiVar(QString::fromUtf8(data).toStdString());
            return QiVar();
        }
    };
    class QiFunc_file_write : public QiFunc
    {
    public:
        QiFunc_file_write() : QiFunc(2) {}
        QiVar exec(const std::vector<QiVar>& args) const
        {
            return File::SaveText(args[0].toString().c_str(), args[1].toString().c_str());
        }
    };
    class QiFunc_file_exist : public QiFunc
    {
    public:
        QiFunc_file_exist() : QiFunc(1) {}
        QiVar exec(const std::vector<QiVar>& args) const
        {
            return File::PathState(String::toWString(args[0].toString()));
        }
    };
    class QiFunc_file_remove : public QiFunc
    {
    public:
        QiFunc_file_remove() : QiFunc(1) {}
        QiVar exec(const std::vector<QiVar>& args) const
        {
            return bool(DeleteFileW(String::toWString(args[0].toString()).c_str()));
        }
    };
    class QiFunc_csv_read : public QiFunc
    {
    public:
        QiFunc_csv_read() : QiFunc(3) {}
        QiVar exec(const std::vector<QiVar>& args) const
        {
            return CsvTool::read(args[0].toString(), args[1].toInteger(), args[2].toInteger());
        }
    };
    class QiFunc_csv_write : public QiFunc
    {
    public:
        QiFunc_csv_write() : QiFunc(4) {}
        QiVar exec(const std::vector<QiVar>& args) const
        {
            return CsvTool::write(args[0].toString(), args[1].toInteger(), args[2].toInteger(), args[3].toString());
        }
    };
    QiFuncMap()
    {
        insert({ "date", std::make_unique<QiFunc_date>() });
        insert({ "time", std::make_unique<QiFunc_time>() });
        insert({ "time_y", std::make_unique<QiFunc_time_y>() });
        insert({ "time_yd", std::make_unique<QiFunc_time_yd>() });
        insert({ "time_ys", std::make_unique<QiFunc_time_ys>() });
        insert({ "time_m", std::make_unique<QiFunc_time_m>() });
        insert({ "time_w", std::make_unique<QiFunc_time_w>() });
        insert({ "time_d", std::make_unique<QiFunc_time_d>() });
        insert({ "time_dh", std::make_unique<QiFunc_time_dh>() });
        insert({ "time_dm", std::make_unique<QiFunc_time_dm>() });
        insert({ "time_ds", std::make_unique<QiFunc_time_ds>() });
        insert({ "time_ms", std::make_unique<QiFunc_time_ms>() });
        insert({ "str", std::make_unique<QiFunc_str>() });
        insert({ "num", std::make_unique<QiFunc_num>() });
        insert({ "int", std::make_unique<QiFunc_int>() });
        insert({ "rmc", std::make_unique<QiFunc_rmc>() });
        insert({ "rms", std::make_unique<QiFunc_rms>() });
        insert({ "len", std::make_unique<QiFunc_len>() });
        insert({ "pop", std::make_unique<QiFunc_pop>() });
        insert({ "sleep", std::make_unique<QiFunc_sleep>() });
        insert({ "rand", std::make_unique<QiFunc_rand>() });
        insert({ "cur_to", std::make_unique<QiFunc_cur_to>() });
        insert({ "cur_move", std::make_unique<QiFunc_cur_move>() });
        insert({ "sub", std::make_unique<QiFunc_sub>() });
        insert({ "subx", std::make_unique<QiFunc_subx>() });
        insert({ "text_box", std::make_unique<QiFunc_text_box>() });
        insert({ "edit_box", std::make_unique<QiFunc_edit_box>() });
        insert({ "volume", std::make_unique<QiFunc_volume>() });
        insert({ "file_read", std::make_unique<QiFunc_file_read>() });
        insert({ "file_write", std::make_unique<QiFunc_file_write>() });
        insert({ "file_exist", std::make_unique<QiFunc_file_exist>() });
        insert({ "file_remove", std::make_unique<QiFunc_file_remove>() });
        insert({ "csv_read", std::make_unique<QiFunc_csv_read>() });
        insert({ "csv_write", std::make_unique<QiFunc_csv_write>() });
    }
};

class QiScriptInterpreter
{
    enum TokenType
    {
        VARIABLE,
        NUMBER,
        STRING,
        OPERATOR,
        PAREN,
        FUNCTION,
        ARG_SEPARATOR
    };
    struct Token
    {
        TokenType type;
        std::string value;
        int precedence;

        Token(TokenType t, const std::string& v, int p = -1)
            : type(t), value(v), precedence(p)
        {
        }
    };
    const QiFuncMap functions;
    QiVarMap globalVariables;
    std::mutex globalMutex;
    static inline const std::string error_invalid_character = "error_invalid_character";
    static inline const std::string error_not_enough_operands = "error_not_enough_operands";
    static inline const std::string error_invalid_expression = "error_invalid_expression";
    static inline const std::string error_unknown_functions = "error_unknown_functions";
public:
    QiVarMap* varMap()
    {
        return &globalVariables;
    }
    auto trim(const std::string& s) -> std::string
    {
        auto start = s.find_first_not_of(" \t");
        auto end = s.find_last_not_of(" \t");
        return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
    }
    auto tokenize(const std::string& expr) -> std::vector<Token>
    {
        std::vector<Token> tokens;
        size_t pos = 0;
        while (pos < expr.length())
        {
            char c = expr[pos];
            if (isspace(c))
            {
                pos++;
                continue;
            }

            // Functions and Variables
            if (c == '$' || isalpha(c))
            {
                size_t start = pos++;
                while (pos < expr.length() && (isalnum(expr[pos]) || expr[pos] == '_'))
                {
                    pos++;
                }
                std::string identifier = expr.substr(start, pos - start);
                // Functions
                if (pos < expr.length() && expr[pos] == '(')
                {
                    tokens.emplace_back(FUNCTION, identifier, 4);
                    tokens.emplace_back(PAREN, "(");
                    pos++;
                }
                // Variables
                else
                {
                    tokens.emplace_back(VARIABLE, identifier);
                }
                continue;
            }
            else if (isdigit(c) || c == '.')
            {
                size_t start = pos++;
                bool hasDot = (c == '.');
                while (pos < expr.length() && (isdigit(expr[pos]) || (expr[pos] == '.' && !hasDot)))
                {
                    if (expr[pos] == '.')
                        hasDot = true;
                    pos++;
                }
                tokens.emplace_back(NUMBER, expr.substr(start, pos - start));
            }
            // Strings
            else if (c == '\'')
            {
                size_t start = ++pos;
                while (pos < expr.length() && expr[pos] != '\'')
                    pos++;
                tokens.emplace_back(STRING, expr.substr(start, pos - start));
                pos++;
            }
            // Operators
            else if (c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || c == '>' || c == '<' || c == '=' || c == '%')
            {
                if (c == '=' && pos + 1 < expr.length() && expr[pos + 1] == '=')
                {
                    tokens.emplace_back(OPERATOR, "==", 1);
                    pos += 2;
                }
                else
                {
                    std::string op(1, c);
                    int prec = 0;
                    if (c == '*' || c == '/' || c == '%')
                        prec = 3;
                    else if (c == '+' || c == '-')
                        prec = 2;
                    else if (c == '>' || c == '<' || c == '=')
                        prec = 1;
                    else if (c == '^')
                        prec = 0;
                    tokens.emplace_back(OPERATOR, op, prec);
                    pos++;
                }
            }
            // Parentheses
            else if (c == '(' || c == ')')
            {
                tokens.emplace_back(PAREN, std::string(1, c));
                pos++;
            }
            else if (c == ',')
            {
                tokens.emplace_back(ARG_SEPARATOR, ",");
                pos++;
            }
            else
            {
                throw std::runtime_error(error_invalid_character + std::string(1, c));
            }
        }
        return tokens;
    }
    auto infixToPostfix(const std::vector<Token>& tokens) -> std::vector<Token>
    {
        std::vector<Token> output;
        std::vector<Token> opStack;

        for (const auto& token : tokens)
        {
            if (token.type == VARIABLE || token.type == NUMBER || token.type == STRING)
            {
                output.push_back(token);
            }
            else if (token.type == PAREN)
            {
                if (token.value == "(")
                {
                    opStack.push_back(token);
                }
                else
                {
                    while (!opStack.empty() && opStack.back().value != "(")
                    {
                        output.push_back(opStack.back());
                        opStack.pop_back();
                    }
                    if (!opStack.empty())
                    {
                        opStack.pop_back();
                        if (!opStack.empty() && opStack.back().type == FUNCTION)
                        {
                            output.push_back(opStack.back());
                            opStack.pop_back();
                        }
                    }
                }
            }
            else if (token.type == OPERATOR)
            {
                while (!opStack.empty() && opStack.back().type == OPERATOR &&
                    opStack.back().precedence >= token.precedence)
                {
                    output.push_back(opStack.back());
                    opStack.pop_back();
                }
                opStack.push_back(token);
            }
            else if (token.type == FUNCTION)
            {
                opStack.push_back(token);
            }
            else if (token.type == ARG_SEPARATOR)
            {
                while (!opStack.empty() && opStack.back().value != "(")
                {
                    output.push_back(opStack.back());
                    opStack.pop_back();
                }
            }
        }

        while (!opStack.empty())
        {
            output.push_back(opStack.back());
            opStack.pop_back();
        }

        return output;
    }
    auto processInterpolation(const std::string& str, const QiVarMap& localVariables) -> std::string
    {
        std::string result;
        size_t pos = 0;
        while (pos < str.length())
        {
            size_t start = str.find('{', pos);
            if (start == std::string::npos)
            {
                result += str.substr(pos);
                break;
            }
            result += str.substr(pos, start - pos);
            size_t end = str.find('}', start);
            if (end == std::string::npos)
            {
                result += str.substr(start);
                break;
            }

            std::string varName = str.substr(start + 1, end - start - 1);
            bool isGlobal = (!varName.empty() && varName[0] == '$');
            QiVar value;

            if (isGlobal)
            {
                auto it = globalVariables.find(varName.substr(1));
                value = (it != globalVariables.end()) ? it->second : QiVar(0.0);
            }
            else
            {
                auto it = localVariables.find(varName);
                value = (it != localVariables.end()) ? it->second : QiVar(0.0);
            }

            result += value.toString();
            pos = end + 1;
        }
        return result;
    }
    auto evaluatePostfix(const std::vector<Token>& postfix, QiVarMap& localVariables) -> QiVar
    {
        std::vector<QiVar> stack;
        for (const auto& token : postfix)
        {
            if (token.type == VARIABLE)
            {
                bool isGlobal = (token.value[0] == '$');
                std::string varName = isGlobal ? token.value.substr(1) : token.value;
                QiVar value;

                if (isGlobal)
                {
                    auto it = globalVariables.find(varName);
                    value = (it != globalVariables.end()) ? it->second : QiVar(0.0);
                }
                else
                {
                    auto it = localVariables.find(varName);
                    value = (it != localVariables.end()) ? it->second : QiVar(0.0);
                }
                stack.push_back(value);
            }
            else if (token.type == NUMBER)
            {
                stack.push_back(QiVar(QiVar::toNumber(token.value)));
            }
            else if (token.type == STRING)
            {
                stack.push_back(QiVar(processInterpolation(token.value, localVariables)));
            }
            else if (token.type == OPERATOR)
            {
                if (stack.size() < 2) throw std::runtime_error(error_not_enough_operands);

                QiVar right = stack.back();
                stack.pop_back();
                QiVar left = stack.back();
                stack.pop_back();
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
            else if (token.type == FUNCTION)
            {
                QiFuncMap::const_iterator i = functions.find(token.value);
                if (i == functions.end())
                {
                    throw std::runtime_error(error_unknown_functions + std::string(": ") + token.value);
                }
                else
                {
                    const std::unique_ptr<QiFunc>& func = (*i).second;
                    if (func->valid(stack.size()))
                    {
                        stack.assign(1, func->exec(stack));
                    }
                    else
                    {
                        throw std::runtime_error(error_not_enough_operands);
                    }
                }
            }
        }
        if (stack.size() != 1) throw std::runtime_error(error_invalid_expression);
        return stack.back();
    }
    auto splitBySemicolonAndNewline(const std::string& code) -> std::vector<std::string>
    {
        std::vector<std::string> result;
        size_t start = 0;
        size_t end = code.find_first_of(";\n");

        while (end != std::string::npos)
        {
            std::string statement = trim(code.substr(start, end - start));
            if (!statement.empty())
            {
                result.push_back(statement);
            }
            start = end + 1;
            end = code.find_first_of(";\n", start);
        }
        std::string lastStatement = trim(code.substr(start));
        if (!lastStatement.empty())
        {
            result.push_back(lastStatement);
        }

        return result;
    }
    auto execute(const std::string& code, QiVarMap& localVariables) -> QiVar
    {
        std::unique_lock<std::mutex>(globalMutex);
        auto tokens = tokenize(code);
        auto postfix = infixToPostfix(tokens);
        return evaluatePostfix(postfix, localVariables);
    }
    void interpret(const std::string& code, QiVarMap& localVariables)
    {
        std::unique_lock<std::mutex>(globalMutex);
        size_t eqPos = code.find('=');
        if (eqPos == std::string::npos)
        {
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

        if (isGlobal)
        {
            globalVariables[varName] = value;
        }
        else
        {
            localVariables[varName] = value;
        }
    }
    void interpretAll(const std::string& code, QiVarMap& localVariables)
    {
        std::unique_lock<std::mutex>(globalMutex);
        auto operations = splitBySemicolonAndNewline(code);
        for (const auto& op : operations)
        {
            size_t commentPos = op.find('#');
            std::string trimmedOp = (commentPos != std::string::npos) ? trim(op.substr(0, commentPos)) : trim(op);
            if (!trimmedOp.empty())
            {
                interpret(trimmedOp, localVariables);
            }
        }
    }
    auto removeWrap(const std::string str) -> std::string
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
    static auto makeString(const std::string str) -> std::string
    {
        std::string result = str;
        result.erase(std::remove(result.begin(), result.end(), '\''), result.end());
        result = "'" + result + "'";
        return result;
    }
    static bool isValidVariableName(const std::string& name)
    {
        if (name.empty())
            return false;
        char firstChar = name[0];
        if (!std::isalpha(firstChar) && firstChar != '_' && firstChar != '$')
            return false;

        for (size_t i = 1; i < name.size(); ++i)
        {
            char ch = name[i];
            if (!std::isalnum(ch) && ch != '_')
                return false;
        }
        return true;
    }
    static void showError(std::string msg, std::string addition = std::string())
    {
        std::wstring text;
        if (msg.find(QiScriptInterpreter::error_invalid_character) != std::string::npos)
        {
            text = L"语句出现无效的字符";
        }
        else if (msg.find(QiScriptInterpreter::error_invalid_expression) != std::string::npos)
        {
            text = L"语句无效";
        }
        else if (msg.find(QiScriptInterpreter::error_not_enough_operands) != std::string::npos)
        {
            text = L"变量/函数参数缺少或过多";
        }
        else if (msg.find("==") != std::string::npos)
        {
            text = L"== 操作符使用错误";
        }
        else if (msg.find(QiScriptInterpreter::error_unknown_functions) != std::string::npos)
        {
            text = L"未知函数";
        }
        MessageBoxW(nullptr, (String::toWString(msg) + std::wstring(L"\n\n") + text + std::wstring(L"\n\n") + String::toWString(addition)).c_str(), L"Quickinput script", MB_ICONERROR);
    }
};
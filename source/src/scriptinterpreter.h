#pragma once
#include <cmath>
#include <cctype>
#include <string>
#include <vector>
#include <map>
#include <variant>
#include <mutex>
#include <thread>
#include <functional>
#include <stdexcept>
#include <algorithm>
#include <sstream>
#include <regex>
#include <filesystem>
#include <windows.h>
#include <src/thread.h>
#include <src/inc_project.h>

class QiVar
{
public:
	using nul_t = std::nullopt_t;
	using int_t = long long;
	using num_t = double;
	using str_t = std::string;
	using ptr_t = void*;

	using VariantType = std::variant<nul_t, int_t, num_t, str_t, ptr_t, bool>;
	VariantType var;

	static inline const std::string error_invalid_character = "QiVar: type is invalid";
	enum Type
	{
		t_nul,
		t_int,
		t_num,
		t_str,
		t_ptr,
		t_bool
	};

	QiVar() : var(std::nullopt) {}
	QiVar(int val) : var(int_t(val)) {}
	QiVar(unsigned int val) : var(int_t(val)) {}
	QiVar(long val) : var(int_t(val)) {}
	QiVar(unsigned long val) : var(int_t(val)) {}
	QiVar(long long val) : var(int_t(val)) {}
	QiVar(unsigned long long val) : var(int_t(val)) {}
	QiVar(double val) : var(num_t(val)) {}
	QiVar(const char* str) : var(str_t(str)) {}
	QiVar(const std::string& str) : var(str_t(str)) {}
	QiVar(const std::wstring& str) : var(str_t(String::toString(str))) {}
	QiVar(void* ptr) : var(ptr_t(ptr)) {}
	QiVar(bool val) : var(bool(val)) {}

	Type type() const { return static_cast<Type>(var.index()); }

	str_t type_name() const
	{
		switch (type())
		{
		case t_nul: return "null";
		case t_int: return "int";
		case t_num: return "num";
		case t_str: return "str";
		case t_ptr: return "ptr";
		case t_bool: return "bool";
		}
		return "null";
	}

	bool isNull() const { return type() == t_nul; }
	bool isInteger() const
	{
		if (type() == t_int) return true;
		if (type() == t_num) return isInteger(get<num_t>(var));
		if (type() == t_str) return isInteger(get<str_t>(var));
		return false;
	}
	bool isNumber() const
	{
		if (type() == t_int) return true;
		if (type() == t_num) return true;
		if (type() == t_str) return isNumber(get<str_t>(var));
		return false;
	}
	bool isString() const { return type() == t_str; }
	bool isPointer() const { return type() == t_ptr; }
	bool isBool() const { return type() == t_bool; }

	bool toBool() const
	{
		if (type() == t_nul) return false;
		if (type() == t_int) return std::get<int_t>(var);
		if (type() == t_num) return std::get<num_t>(var);
		if (type() == t_str) return toBool(std::get<str_t>(var));
		if (type() == t_ptr) return std::get<ptr_t>(var);
		if (type() == t_bool) return std::get<bool>(var);
		return false;
	}
	int_t toInteger() const
	{
		if (type() == t_nul) return 0LL;
		if (type() == t_int) return std::get<int_t>(var);
		if (type() == t_num) return get<num_t>(var);
		if (type() == t_str) return toInteger(get<str_t>(var));
		if (type() == t_ptr) return reinterpret_cast<int_t>(get<ptr_t>(var));
		if (type() == t_bool) return std::get<bool>(var);
		return 0LL;
	}
	num_t toNumber() const
	{
		if (type() == t_nul) return 0.0;
		if (type() == t_int) return std::get<int_t>(var);
		if (type() == t_num) return get<num_t>(var);
		if (type() == t_str) return toNumber(get<str_t>(var));
		if (type() == t_ptr) return reinterpret_cast<int_t>(get<ptr_t>(var));
		if (type() == t_bool) return std::get<bool>(var);
		return 0.0;
	}
	str_t toString() const
	{
		if (type() == t_nul) return str_t();
		if (type() == t_int) return toString(std::get<int_t>(var));
		if (type() == t_num) return toString(get<num_t>(var));
		if (type() == t_str) return get<str_t>(var);
		if (type() == t_ptr) return toString(get<ptr_t>(var));
		if (type() == t_bool) return toString(std::get<bool>(var));
		return str_t();
	}
	ptr_t toPointer() const
	{
		if (type() == t_nul) return nullptr;
		if (type() == t_int) return reinterpret_cast<ptr_t>(std::get<int_t>(var));
		if (type() == t_num) return nullptr;
		if (type() == t_str) return nullptr;
		if (type() == t_ptr) return get<ptr_t>(var);
		if (type() == t_bool) return nullptr;
		return nullptr;
	}
	std::wstring toWString() const
	{
		return String::toWString(toString());
	}

	int_t len() const
	{
		if (type() == t_nul) return 0LL;
		if (type() == t_int) return toString(std::get<int_t>(var)).length();
		if (type() == t_num) return toString(get<num_t>(var)).length();
		if (type() == t_str) return get<str_t>(var).length();
		if (type() == t_ptr) return toString(get<ptr_t>(var)).length();
		if (type() == t_bool) return toString(get<bool>(var)).length();
		return 0LL;
	}

	QiVar merge(const QiVar& other) const
	{
		return toString() + other.toString();
	}
	QiVar operator+(const QiVar& other) const
	{
		if (isPointer() && (other.isInteger() || other.isPointer())) return reinterpret_cast<ptr_t>(toInteger() + other.toInteger());
		if (isInteger() && (other.isInteger() || other.isPointer())) return toInteger() + other.toInteger();
		return toNumber() + other.toNumber();
	}
	QiVar operator-(const QiVar& other) const
	{
		if (isPointer() && (other.isInteger() || other.isPointer())) return reinterpret_cast<ptr_t>(toInteger() - other.toInteger());
		if (isInteger() && (other.isInteger() || other.isPointer())) return toInteger() - other.toInteger();
		return toNumber() - other.toNumber();
	}
	QiVar operator*(const QiVar& other) const
	{
		if ((isPointer() || isInteger()) && (other.isInteger() || other.isPointer())) return toInteger() * other.toInteger();
		return toNumber() * other.toNumber();
	}
	QiVar operator/(const QiVar& other) const
	{
		if (other.toNumber() == 0.0) return 0.0;
		if ((isPointer() || isInteger()) && (other.isInteger() || other.isPointer())) return toInteger() / other.toInteger();
		return toNumber() / other.toNumber();
	}
	QiVar operator%(const QiVar& other) const
	{
		int_t i = other.toInteger();
		return i == 0LL ? 0LL : toInteger() % i;
	}

	QiVar operator~() const
	{
		return ~toInteger();
	}
	QiVar operator&(const QiVar& other) const
	{
		return toInteger() & other.toInteger();
	}
	QiVar operator|(const QiVar& other) const
	{
		return toInteger() | other.toInteger();
	}
	QiVar operator^(const QiVar& other) const
	{
		return toInteger() ^ other.toInteger();
	}
	QiVar operator<<(const QiVar& other) const
	{
		return toInteger() << other.toInteger();
	}
	QiVar operator>>(const QiVar& other) const
	{
		return toInteger() >> other.toInteger();
	}

	bool operator>(const QiVar& other) const
	{
		return toNumber() > other.toNumber();
	}
	bool operator>=(const QiVar& other) const
	{
		return toNumber() >= other.toNumber();
	}
	bool operator<(const QiVar& other) const
	{
		return toNumber() < other.toNumber();
	}
	bool operator<=(const QiVar& other) const
	{
		return toNumber() <= other.toNumber();
	}
	bool operator==(const QiVar& other) const
	{
		if (isString() || other.isString()) return toString() == other.toString();
		return toNumber() == other.toNumber();
	}
	bool operator!=(const QiVar& other) const
	{
		if (isString() || other.isString()) return toString() != other.toString();
		return toNumber() != other.toNumber();
	}

	bool operator!() const
	{
		return !toBool();
	}
	bool operator&&(const QiVar& other) const
	{
		return toBool() && other.toBool();
	}
	bool operator||(const QiVar& other) const
	{
		return toBool() || other.toBool();
	}

	QiVar& operator+=(const QiVar& other)
	{
		*this = operator+(other); return *this;
	}
	QiVar& operator-=(const QiVar& other)
	{
		*this = operator-(other); return *this;
	}
	QiVar& operator*=(const QiVar& other)
	{
		*this = operator*(other); return *this;
	}
	QiVar& operator/=(const QiVar& other)
	{
		*this = operator/(other); return *this;
	}

	static std::string ch(const std::string& str, size_t where)
	{
		if (str.empty()) return std::string();
		size_t i = 0;
		std::string c;
		iterateStrX(str, [&c, &i, where](const std::string& ch) -> bool {
			if (where == i)
			{
				c = ch;
				return false;
			}
			i++;
			return true;
			});
		return c;
	}
	static std::string sub(const std::string& str, size_t where, size_t length = ~size_t(0))
	{
		if (str.empty()) return std::string();
		if (where > str.size()) where = str.size() - 1;
		return str.substr(where, length);
	}
	static std::string subx(const std::string& str, size_t where, size_t length = ~size_t(0))
	{
		if (str.empty() || (!where && !length)) return std::string();
		size_t end = str.size() - where - 1;
		if (end >= str.size()) return std::string();
		size_t begin = end - length + 1;
		if (begin >= str.size()) begin = 0;
		return str.substr(begin, end - begin + 1);
	}
	static void iterateStr(const std::string& str, std::function<void(const std::string& ch)> call) {
		for (size_t i = 0; i < str.size(); )
		{
			const unsigned char& c = str[i];
			size_t charLen = 0;
			if (c < 0x80) charLen = 1;
			else if ((c & 0xE0) == 0xC0) charLen = 2;
			else if ((c & 0xF0) == 0xE0) charLen = 3;
			else if ((c & 0xF8) == 0xF0) charLen = 4;
			else
			{
				i++;
				continue;
			}
			call(str.substr(i, charLen));
			i += charLen;
		}
	}
	static bool iterateStrX(const std::string& str, std::function<bool(const std::string& ch)> call) {
		for (size_t i = 0; i < str.size(); )
		{
			const unsigned char& c = str[i];
			size_t charLen = 0;
			if (c < 0x80) charLen = 1;
			else if ((c & 0xE0) == 0xC0) charLen = 2;
			else if ((c & 0xF0) == 0xE0) charLen = 3;
			else if ((c & 0xF8) == 0xF0) charLen = 4;
			else
			{
				i++;
				continue;
			}
			if (!call(str.substr(i, charLen))) return false;
			i += charLen;
		}
		return true;
	}

	static std::string removeStrs(const std::string& str, const std::vector<std::string>& strs_rm)
	{
		std::string result = str;
		for (const std::string& i : strs_rm)
		{
			if (i.empty()) continue;
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
		iterateStr(chs_rm, [&result](const std::string& ch) {
			if (ch.empty()) return;
			size_t pos = 0;
			while ((pos = result.find(ch, pos)) != std::string::npos) result.erase(pos, ch.length());
			});
		return result;
	}
	static std::string removeChars(const std::string& str) {
		std::string result;
		iterateStr(str, [&result](const std::string& ch)
			{
				if (ch.size() == 1 && std::isdigit(ch[0])) result += ch;
			});
		return result;
	}
	static std::string removeNums(const std::string& str) {
		std::string result;
		iterateStr(str, [&result](const std::string& ch)
			{
				if (ch.size() == 1 && std::isdigit(ch[0])) return;
				result += ch;
			});
		return result;
	}
	static std::string removeCommas(const std::string& str)
	{
		std::string result = str;
		result.erase(std::remove(result.begin(), result.end(), ','), result.end());
		return result;
	}
	static std::string removePoints(const std::string& str)
	{
		std::string result = str;
		result.erase(std::remove(result.begin(), result.end(), '.'), result.end());
		return result;
	}

	static bool isInteger(const std::string& str, bool commas = false)
	{
		if (str.empty()) return false;
		try
		{
			std::string cleanedStr = commas ? removeCommas(str) : str;
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
	static bool isNumber(const std::string& str, bool commas = false)
	{
		if (str.empty()) return false;
		try
		{
			std::string cleanedStr = commas ? removeCommas(str) : str;
			std::size_t pos;
			std::stod(cleanedStr, &pos);
			return pos == cleanedStr.size();
		}
		catch (...)
		{
			return false;
		}
	}

	static bool toBool(const std::string& str)
	{
		return str != "0" && stricmp(str.c_str(), "false") != 0;
	}
	static int toInteger(const std::string& str)
	{
		try
		{
			std::string cleanedStr = removeCommas(str);
			size_t pos;
			double result = std::stoll(cleanedStr, &pos);
			return pos == cleanedStr.length() ? result : 0.0;
		}
		catch (...)
		{
			return 0LL;
		}
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

	static std::string toString(int val) { return std::to_string(val); }
	static std::string toString(long long val) { return std::to_string(val); }
	static std::string toString(double val)
	{
		std::string str = std::to_string(val);
		if (std::fmod(val, 1.0) == 0.0) return std::to_string((long long)(val));
		str.erase(str.find_last_not_of('0') + 1, std::string::npos);
		if (!str.empty() && str.back() == '.') str.pop_back();
		return str;
	}
	static std::string toString(void* val) { return std::to_string(reinterpret_cast<unsigned long long>(val)); }
	static std::string toString(bool val) { return val ? std::string("true") : std::string("false"); }
	static std::string replace(const std::string& str, const std::string& str_rp, const std::string& str_new)
	{
		std::string result(str);
		std::string::size_type pos = 0;
		while ((pos = result.find(str_rp)) != std::string::npos) result.replace(pos, str_rp.length(), str_new);
		return result;
	}
	static std::string replace_u8(const std::string& str, const std::string& str_rp, const std::string& str_new)
	{
		if (str_rp.length() > 1) return replace(str, str_rp, str_new);
		std::string result;
		iterateStr(str, [&](const std::string& ch) {
			if (ch == str_rp) result += str_new;
			else result += ch;
			});
		return result;
	}
	static std::string replace_escape(const std::string& str)
	{
		std::string result;
		bool escapeMode = false;
		iterateStr(str, [&](const std::string& ch) {
			if (escapeMode)
			{
				if (ch == "n") result += '\n';
				else if (ch == "r") result += '\r';
				else if (ch == "\\") result += '\\';
				else
				{
					result += '\\';
					result += ch;
				}
				escapeMode = false;
			}
			else
			{
				if (ch == "\\") escapeMode = true;
				else result += ch;
			}
			});
		if (escapeMode) result += '\\';
		return result;
	}
};
using QiVarMap = std::map<std::string, QiVar>;

class QiScriptInterpreter;
struct QiFunc
{
	const size_t m_args;
	const size_t m_args_max;
	QiFunc() = delete;
	QiFunc(size_t args, size_t args_max = 0) : m_args(args), m_args_max(args_max ? args_max : args) {}
	bool valid(size_t args) const { return args >= m_args && args <= m_args_max; }
	virtual QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter* interpreter = nullptr, QiVarMap* global = nullptr, QiVarMap* local = nullptr, QiWorker* worker = nullptr) const = 0;
};
class QiFuncMap : public std::map<std::string, std::unique_ptr<QiFunc>> { public: QiFuncMap(); };

class QiScriptInterpreter
{
public:
	static inline const std::string var_count = "_count";
	static inline const std::string var_index = "_index";
	static inline const std::string var_macro_name = "_macro_name";
	static inline const std::string var_rand_last = "_rand_last";
	static inline const std::string var_cur_last_x = "_cur_last_x";
	static inline const std::string var_cur_last_y = "_cur_last_y";
	static inline const std::string var_cur_last_ax = "_cur_last_ax";
	static inline const std::string var_cur_last_ay = "_cur_last_ay";

private:
	struct StatementType
	{
		enum
		{
			NODE,
			IF,
			LOOP,
			FUNCTION_DEF,
			RETURN
		};
	};

	class Statement;
	using StatementList = std::vector<Statement>;

	using Node = std::string;

	struct If
	{
		std::string condition;
		StatementList trueBlock;
		StatementList falseBlock;
	};
	struct Loop
	{
		std::string condition;
		StatementList body;
	};
	struct FunctionDef
	{
		std::string name;
		std::vector<std::string> parameters;
		StatementList body;
	};
	struct ReturnStmt
	{
		std::string expression;
	};

	using StatementVariant = std::variant<Node, If, Loop, FunctionDef, ReturnStmt>;
	struct Statement : public StatementVariant
	{
		using StatementVariant::StatementVariant;
	};

	enum TokenType
	{
		VARIABLE,
		NULLOPT,
		NUMBER,
		STRING,
		TRUE_,
		FALSE_,
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
		int argCount;
		Token(TokenType t, const std::string& v = std::string(), int p = -1, int ac = 0) : type(t), value(v), precedence(p), argCount(ac) {}
	};

	struct ReturnException : public std::exception
	{
		QiVar returnValue;
		ReturnException(const QiVar& value) : returnValue(value) {}
	};

	struct QiCustomFunc
	{
		std::vector<std::string> parameters;
		StatementList statementList;
		QiCustomFunc() = delete;
		QiCustomFunc(const std::vector<std::string>& params, const StatementList& body) : parameters(params), statementList(body) {}
	};

	using QiCustomFuncMap = std::map<std::string, std::unique_ptr<QiCustomFunc>>;

	static inline const std::string error_invalid_character = "error_invalid_character";
	static inline const std::string error_not_enough_operands = "error_not_enough_operands";
	static inline const std::string error_functions_parameter_invalid = "error_functions_parameter_invalid";
	static inline const std::string error_unknown_operator = "error_unknown_operator";
	static inline const std::string error_invalid_expression = "error_invalid_expression";
	static inline const std::string error_unknown_functions = "error_unknown_functions";

	static inline const QiFuncMap functions;
	static inline QiVarMap globalVariables;
	static inline QiCustomFuncMap customFunctions;
	static inline std::mutex globalVariablesMutex;
	static inline std::mutex customFunctionsMutex;

	QiVarMap localVariables;
	std::mutex localVariablesMutex;
	QiWorker* workerPtr = nullptr;

	auto trim(const std::string& s) -> std::string
	{
		auto start = s.find_first_not_of(" \t");
		auto end = s.find_last_not_of(" \t");
		return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
	}
	auto splitLines(const std::string& code) -> std::vector<std::string>
	{
		std::vector<std::string> lines;
		std::istringstream stream(code);
		std::string line;
		while (std::getline(stream, line)) {
			line = trim(line);
			if (!line.empty()) {
				lines.push_back(line);
			}
		}
		return lines;
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

	auto tokenize(const std::string& expr) -> std::vector<Token>
	{
		std::vector<Token> tokens;
		size_t pos = 0;
		while (pos < expr.length())
		{
			char c = expr[pos];
			if (std::isspace(c)) pos++;
			// Functions and Variables
			else if (c == '$' || isValidVariableFirst(c))
			{
				size_t start = pos++;
				while (pos < expr.length() && isValidVariableChar(expr[pos])) pos++;
				std::string identifier = expr.substr(start, pos - start);
				// key words
				if (identifier[0] != '$')
				{
					if (identifier == "null") { tokens.emplace_back(NULLOPT); continue; }
					else if (identifier == "true") { tokens.emplace_back(TRUE_, "true"); continue; }
					else if (identifier == "false") { tokens.emplace_back(FALSE_, "false"); continue; }
					else if (identifier == "return") { tokens.emplace_back(OPERATOR, "return", 4); continue; }
				}
				// Functions
				if (pos < expr.length() && expr[pos] == '(')
				{
					int argCount = 0;
					int parenCount = 1;
					size_t currentPos = pos + 1;
					bool inString = false;
					bool hasNonSpaceContent = false;

					while (currentPos < expr.length() && parenCount > 0)
					{
						if (expr[currentPos] == '\'' && !inString) inString = true;
						else if (expr[currentPos] == '\'' && inString) inString = false;

						if (!inString)
						{
							if (expr[currentPos] == '(') parenCount++;
							else if (expr[currentPos] == ')') parenCount--;
							else if (expr[currentPos] == ',' && parenCount == 1) argCount++;

							if (parenCount == 1 && !std::isspace(expr[currentPos]) &&
								expr[currentPos] != ',' && expr[currentPos] != ')') {
								hasNonSpaceContent = true;
							}
						}
						currentPos++;
					}

					if (parenCount == 0)
					{
						if (argCount > 0 || hasNonSpaceContent) argCount++;
						tokens.emplace_back(FUNCTION, identifier, 4, argCount);
						continue;
					}
					else tokens.emplace_back(VARIABLE, identifier);
				}
				// Variables
				else tokens.emplace_back(VARIABLE, identifier);
			}
			// Numbers
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
				while (pos < expr.length() && expr[pos] != '\'') pos++;
				tokens.emplace_back(STRING, QiVar::replace_escape(expr.substr(start, pos - start)));
				pos++;
			}
			// Operators
			else if (c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || c == '>' || c == '<' || c == '=' || c == '%' || c == '~' || c == '&' || c == '|' || c == '!')
			{
				bool isUnaryMinus = (c == '-') && (tokens.empty() || tokens.back().type == OPERATOR || tokens.back().value == "(" || tokens.back().type == ARG_SEPARATOR);
				if (isUnaryMinus)
				{
					tokens.emplace_back(OPERATOR, "_-", 4);
					pos++;
					continue;
				}

				std::string op(1, c);
				int prec = 0;
				bool dual_char = false;

				if (pos + 1 < expr.length())
				{
					char next_c = expr[pos + 1];
					std::string dual_op = op + next_c;

					if ((dual_op == "<<" || dual_op == ">>") || (dual_op == ">=" || dual_op == "<=" || dual_op == "!=" || dual_op == "==" || dual_op == "&&" || dual_op == "||" || dual_op == "^^"))
					{
						op = dual_op;
						pos++;
						dual_char = true;
					}
				}

				if (op == "!" || op == "~" || op == "^")
					prec = 4;
				else if (op == "*" || op == "/" || op == "%")
					prec = 3;
				else if (op == "+" || op == "-")
					prec = 2;
				else if (op == "<<" || op == ">>")
					prec = 1;
				else if (op == "|" || op == "&" || op == "^^")
					prec = 1;
				else if (op == "==" || op == "!=" || op == ">" || op == "<" || op == ">=" || op == "<=")
					prec = 1;
				else if (op == "||" || op == "&&")
					prec = 0;

				tokens.emplace_back(OPERATOR, op, prec);
				pos++;
			}
			// Parentheses
			else if (c == '(' || c == ')')
			{
				tokens.emplace_back(PAREN, std::string(1, c));
				pos++;
			}
			// Args
			else if (c == ',')
			{
				tokens.emplace_back(ARG_SEPARATOR, ",");
				pos++;
			}
			else throw std::runtime_error(error_invalid_character + std::string(": ") + std::string(1, c) + std::string("\n\nat: ") + expr);
		}
		return tokens;
	}
	auto infixToPostfix(const std::vector<Token>& tokens) -> std::vector<Token>
	{
		std::vector<Token> output;
		std::vector<Token> opStack;

		for (const auto& token : tokens)
		{
			if (token.type == VARIABLE || token.type == NULLOPT || token.type == NUMBER || token.type == STRING || token.type == TRUE_ || token.type == FALSE_)
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
				if (token.value == "return") { opStack.push_back(token); continue; }

				while (!opStack.empty() && opStack.back().type == OPERATOR && opStack.back().precedence >= token.precedence)
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
	auto processInterpolation(const std::string& str, QiVarMap* local) -> std::string
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
			result += value(str.substr(start + 1, end - start - 1), local).toString();
			pos = end + 1;
		}
		return result;
	}
	auto evaluatePostfix(const std::vector<Token>& postfix, QiVarMap* local) -> QiVar
	{
		std::vector<QiVar> stack;
		for (const auto& token : postfix)
		{
			if (token.type == VARIABLE) stack.push_back(value(token.value, local));
			else if (token.type == NULLOPT) stack.push_back(QiVar());
			else if (token.type == NUMBER)
			{
				if (QiVar::isInteger(token.value, false)) stack.push_back(QiVar(QiVar::toInteger(token.value)));
				else stack.push_back(QiVar(QiVar::toNumber(token.value)));
			}
			else if (token.type == STRING) stack.push_back(QiVar(processInterpolation(token.value, local)));
			else if (token.type == TRUE_) stack.push_back(QiVar(true));
			else if (token.type == FALSE_) stack.push_back(QiVar(false));
			else if (token.type == OPERATOR)
			{
				QiVar result;
				try
				{
					if (token.value == "return")
					{
						QiVar right;
						if (!stack.empty())
						{
							right = stack.back();
							stack.pop_back();
						}
						throw ReturnException(right);
					}
					else if (token.value == "!" || token.value == "~" || token.value == "_-")
					{
						if (stack.size() < 1) throw std::runtime_error(error_not_enough_operands + std::string(": ") + token.value);
						QiVar right = stack.back();
						stack.pop_back();

						if (token.value == "!") result = !right;
						else if (token.value == "~") result = ~right;
						else if (token.value == "_-") result = (right.type() == QiVar::t_num) ? QiVar(0.0) - right : QiVar(0) - right;
					}
					else
					{
						if (stack.size() < 2) throw std::runtime_error(error_not_enough_operands + std::string(": ") + token.value);
						QiVar right = stack.back();
						stack.pop_back();
						QiVar left = stack.back();
						stack.pop_back();

						if (token.value == "^") result = left.merge(right);
						else if (token.value == "+") result = left + right;
						else if (token.value == "-") result = left - right;
						else if (token.value == "*") result = left * right;
						else if (token.value == "/") result = left / right;
						else if (token.value == "%") result = left % right;
						else if (token.value == "&") result = left & right;
						else if (token.value == "|") result = left | right;
						else if (token.value == "^^") result = left ^ right;
						else if (token.value == "<<") result = left << right;
						else if (token.value == ">>") result = left >> right;
						else if (token.value == ">") result = left > right;
						else if (token.value == "<") result = left < right;
						else if (token.value == ">=") result = left >= right;
						else if (token.value == "<=") result = left <= right;
						else if (token.value == "==") result = left == right;
						else if (token.value == "!=") result = left != right;
						else if (token.value == "&&") result = left && right;
						else if (token.value == "||") result = left || right;
						else throw std::runtime_error(error_unknown_operator + std::string(": ") + token.value);
					}
				}
				catch (const ReturnException&)
				{
					throw;
				}
				catch (const std::runtime_error&)
				{
					throw;
				}
				stack.push_back(result);
			}
			else if (token.type == FUNCTION)
			{
				QiFuncMap::const_iterator i = functions.find(token.value);
				if (i != functions.end())
				{
					const std::unique_ptr<QiFunc>& func = i->second;

					std::vector<QiVar> args;
					for (int j = 0; j < token.argCount; j++)
					{
						if (stack.empty()) throw std::runtime_error(error_functions_parameter_invalid + std::string(": ") + token.value);
						args.insert(args.begin(), stack.back());
						stack.pop_back();
					}

					if (func->valid(args.size()))
					{
						QiVar result = func->exec(args, this, &globalVariables, &localVariables, workerPtr);
						stack.push_back(result);
					}
					else throw std::runtime_error(error_functions_parameter_invalid + std::string(": ") + token.value);
				}
				else
				{
					std::unique_lock<std::mutex> lock(customFunctionsMutex);
					auto customIt = customFunctions.find(token.value);
					if (customIt != customFunctions.end())
					{
						const std::unique_ptr<QiCustomFunc>& func = customIt->second;
						if (stack.size() >= func->parameters.size())
						{
							QiVarMap funcLocal;
							for (int i = func->parameters.size() - 1; i >= 0; i--) {
								funcLocal[func->parameters[i]] = stack.back();
								stack.pop_back();
							}

							try
							{
								executeStatementList(func->statementList, &funcLocal);
								stack.push_back(QiVar(0));
							}
							catch (const ReturnException& e)
							{
								stack.push_back(e.returnValue);
							}
						}
						else throw std::runtime_error(error_functions_parameter_invalid + std::string(": ") + token.value);
					}
					else throw std::runtime_error(error_unknown_functions + std::string(": ") + token.value);
				}
			}
		}
		if (stack.size() != 1)
		{
			std::string exc; for (const auto& i : postfix) exc += i.value;
			throw std::runtime_error(error_invalid_expression + std::string(": ") + exc);
		}
		return stack.back();
	}

	std::pair<StatementList, size_t> parseTopLevel(const std::vector<std::string>& lines, size_t start_index) {
		StatementList block;
		size_t index = start_index;
		while (index < lines.size()) {
			auto [stmt, next_index] = parseStatement(lines, index);
			block.push_back(stmt);
			index = next_index;
		}
		return { block, index };
	}
	std::pair<Statement, size_t> parseStatement(const std::vector<std::string>& lines, size_t start_index) {
		if (start_index >= lines.size()) {
			return { Node(""), start_index };
		}

		std::string line = lines[start_index];

		if (line.substr(0, 2) == "fn") {
			size_t end_pos = line.find('(', 2);
			if (end_pos == std::string::npos) {
				return { Node(line), start_index + 1 };
			}

			std::string func_name = trim(line.substr(2, end_pos - 2));
			std::string params_str = line.substr(end_pos);

			size_t paren_start = params_str.find('(');
			size_t paren_end = params_str.find(')');
			if (paren_start == std::string::npos || paren_end == std::string::npos) {
				return { Node(line), start_index + 1 };
			}

			std::string params = params_str.substr(paren_start + 1, paren_end - paren_start - 1);
			std::vector<std::string> parameters;
			std::istringstream param_stream(params);
			std::string param;
			while (std::getline(param_stream, param, ',')) {
				parameters.push_back(trim(param));
			}

			if (start_index + 1 >= lines.size() || lines[start_index + 1] != "{") {
				return { Node(line), start_index + 1 };
			}

			auto [body, next_index] = parseBlock(lines, start_index + 2);
			return { FunctionDef{func_name, parameters, body}, next_index };
		}
		else if (line.substr(0, 6) == "return") {
			std::string expr = trim(line.substr(6));
			return { ReturnStmt{expr}, start_index + 1 };
		}
		else if (line.substr(0, 2) == "if") {
			size_t start_paren = line.find('(');
			if (start_paren == std::string::npos) {
				return { Node(line), start_index + 1 };
			}

			int paren_count = 1;
			size_t current_pos = start_paren + 1;
			size_t end_paren = std::string::npos;

			while (current_pos < line.length()) {
				if (line[current_pos] == '(') {
					paren_count++;
				}
				else if (line[current_pos] == ')') {
					paren_count--;
					if (paren_count == 0) {
						end_paren = current_pos;
						break;
					}
				}
				current_pos++;
			}

			if (end_paren == std::string::npos) {
				return { Node(line), start_index + 1 };
			}

			std::string condition = line.substr(start_paren + 1, end_paren - start_paren - 1);

			if (start_index + 1 >= lines.size() || lines[start_index + 1] != "{") {
				return { Node(line), start_index + 1 };
			}

			auto [trueBlock, index_after_true] = parseBlock(lines, start_index + 2);
			StatementList falseBlock;
			size_t next_index = index_after_true;

			if (index_after_true < lines.size() && lines[index_after_true] == "else") {
				if (index_after_true + 1 < lines.size() && lines[index_after_true + 1] == "{") {
					auto [fb, idx] = parseBlock(lines, index_after_true + 2);
					falseBlock = fb;
					next_index = idx;
				}
				else {
					next_index = index_after_true + 1;
				}
			}

			return { If{ condition, trueBlock, falseBlock }, next_index };
		}
		else if (line.substr(0, 4) == "loop") {
			size_t start_paren = line.find('(');
			if (start_paren == std::string::npos) {
				return { Node(line), start_index + 1 };
			}

			int paren_count = 1;
			size_t current_pos = start_paren + 1;
			size_t end_paren = std::string::npos;

			while (current_pos < line.length()) {
				if (line[current_pos] == '(') {
					paren_count++;
				}
				else if (line[current_pos] == ')') {
					paren_count--;
					if (paren_count == 0) {
						end_paren = current_pos;
						break;
					}
				}
				current_pos++;
			}

			if (end_paren == std::string::npos) {
				return { Node(line), start_index + 1 };
			}

			std::string condition = line.substr(start_paren + 1, end_paren - start_paren - 1);

			if (start_index + 1 >= lines.size() || lines[start_index + 1] != "{") {
				return { Node(line), start_index + 1 };
			}

			auto [body, next_index] = parseBlock(lines, start_index + 2);
			return { Loop{ condition, body }, next_index };
		}
		else {
			return { Node(line), start_index + 1 };
		}
	}
	std::pair<StatementList, size_t> parseBlock(const std::vector<std::string>& lines, size_t start_index) {
		StatementList block;
		size_t index = start_index;
		while (index < lines.size()) {
			if (lines[index] == "}") {
				return { block, index + 1 };
			}
			auto [stmt, next_index] = parseStatement(lines, index);
			block.push_back(stmt);
			index = next_index;
		}
		return { block, index };
	}
	void executeStatementList(const StatementList& statements, QiVarMap* local) {
		for (const auto& stmt : statements) {
			if (std::holds_alternative<Node>(stmt)) {
				std::string code = std::get<Node>(stmt);
				if (!code.empty()) interpret(code, local);
			}
			else if (std::holds_alternative<If>(stmt)) {
				const If& ifStmt = std::get<If>(stmt);
				if (execute(ifStmt.condition, local).toBool()) executeStatementList(ifStmt.trueBlock, local);
				else executeStatementList(ifStmt.falseBlock, local);
			}
			else if (std::holds_alternative<Loop>(stmt)) {
				const Loop& loopStmt = std::get<Loop>(stmt);
				while ((workerPtr ? (!workerPtr->m_stop) : true) && execute(loopStmt.condition).toBool()) executeStatementList(loopStmt.body, local);
			}
			else if (std::holds_alternative<FunctionDef>(stmt)) {
				const FunctionDef& funcDef = std::get<FunctionDef>(stmt);
				std::unique_lock<std::mutex> lock(customFunctionsMutex);
				customFunctions[funcDef.name] = std::make_unique<QiCustomFunc>(funcDef.parameters, funcDef.body);
			}
			else if (std::holds_alternative<ReturnStmt>(stmt)) {
				const ReturnStmt& returnStmt = std::get<ReturnStmt>(stmt);
				throw ReturnException(returnStmt.expression.empty() ? QiVar() : execute(returnStmt.expression, local));
			}
		}
	}

public:
	QiScriptInterpreter() {}
	QiScriptInterpreter(QiScriptInterpreter&& right)
	{
		std::unique_lock<std::mutex> lock(localVariablesMutex);
		localVariables = std::move(right.localVariables);
		workerPtr = right.workerPtr;
		workerPtr = nullptr;
	}
	QiScriptInterpreter(const QiScriptInterpreter&) = delete;
	QiScriptInterpreter& operator=(QiScriptInterpreter&& right)
	{
		std::unique_lock<std::mutex> lock(localVariablesMutex);
		localVariables = std::move(right.localVariables);
		workerPtr = right.workerPtr;
		workerPtr = nullptr;
		return *this;
	}
	QiScriptInterpreter& operator=(const QiScriptInterpreter&) = delete;

	auto execute(const std::string& code, QiVarMap* local = nullptr) -> QiVar
	{
		auto tokens = tokenize(code);
		auto postfix = infixToPostfix(tokens);
		try { return evaluatePostfix(postfix, local); }
		catch (const ReturnException& e) { return e.returnValue; }
	}
	void interpret(const std::string& code, QiVarMap* local = nullptr)
	{
		std::string trimmedCode = trim(code);
		size_t incPos = trimmedCode.find("++");
		size_t decPos = trimmedCode.find("--");
		if (incPos != std::string::npos || decPos != std::string::npos)
		{
			std::string varName;
			// front
			if (trimmedCode.substr(0, 2) == "++")
			{
				varName = trim(trimmedCode.substr(2));
				setValue(varName, value(varName, local) + 1, local);
			}
			else if (trimmedCode.substr(0, 2) == "--")
			{
				varName = trim(trimmedCode.substr(2));
				setValue(varName, value(varName, local) - 1, local);
			}
			// back
			else if (trimmedCode.substr(trimmedCode.size() - 2) == "++")
			{
				varName = trim(trimmedCode.substr(0, trimmedCode.size() - 2));
				setValue(varName, value(varName, local) + 1, local);
			}
			else if (trimmedCode.substr(trimmedCode.size() - 2) == "--")
			{
				varName = trim(trimmedCode.substr(0, trimmedCode.size() - 2));
				setValue(varName, value(varName, local) - 1, local);
			}
			return;
		}

		size_t oper = 0;
		size_t eqPos = trimmedCode.find('=');
		if (eqPos == std::string::npos)
		{
			execute(trimmedCode, local);
			return;
		}
		else
		{
			if (eqPos + 1 < trimmedCode.size() && trimmedCode[eqPos + 1] == '=')
			{
				execute(trimmedCode, local);
				return;
			}
			if (eqPos > 0)
			{
				if (trimmedCode[eqPos - 1] == '+') oper = 1;
				if (trimmedCode[eqPos - 1] == '-') oper = 2;
				if (trimmedCode[eqPos - 1] == '*') oper = 3;
				if (trimmedCode[eqPos - 1] == '/') oper = 4;
				if (trimmedCode[eqPos - 1] == '^') oper = 5;
			}
		}

		auto tokens = tokenize(trimmedCode.substr(eqPos + 1));
		auto postfix = infixToPostfix(tokens);
		QiVar result;
		try { result = evaluatePostfix(postfix, local); }
		catch (const ReturnException& e) { result = e.returnValue; }

		std::string left = trim(trimmedCode.substr(0, oper ? eqPos - 1 : eqPos));
		if (!left.empty())
		{
			switch (oper)
			{
			case 1: result = value(left, local) + result; break;
			case 2:	result = value(left, local) - result; break;
			case 3:	result = value(left, local) * result; break;
			case 4:	result = value(left, local) / result; break;
			case 5:	result = value(left, local).merge(result); break;
			}
			setValue(left, result, local);
		}
	}
	void interpretAll(const std::string& code, QiVarMap* local = nullptr)
	{
		if (code.empty()) return;

		std::vector<std::string> rawLines;
		std::istringstream codeStream(code);
		std::string rawLine;
		while (std::getline(codeStream, rawLine)) rawLines.push_back(rawLine);

		std::vector<std::string> lines;
		for (const auto& rawLine : rawLines) {
			std::string line = rawLine;
			bool in_string = false;
			bool escape_next = false;

			for (size_t i = 0; i < line.length(); i++) {
				char c = line[i];
				if (in_string) {
					if (escape_next) {
						escape_next = false;
					}
					else {
						if (c == '\\') {
							escape_next = true;
						}
						else if (c == '\'') {
							in_string = false;
						}
					}
				}
				else {
					if (c == '\'') {
						in_string = true;
					}
					else if (c == '#') {
						line = line.substr(0, i);
						break;
					}
				}
			}

			if (!line.empty()) {
				std::vector<std::string> statements;
				size_t start = 0;
				size_t end = line.find(';');
				while (end != std::string::npos) {
					std::string stmt = trim(line.substr(start, end - start));
					if (!stmt.empty()) {
						statements.push_back(stmt);
					}
					start = end + 1;
					end = line.find(';', start);
				}
				std::string lastStmt = trim(line.substr(start));
				if (!lastStmt.empty()) statements.push_back(lastStmt);

				for (const auto& stmt : statements) lines.push_back(stmt);
			}
		}
		try { executeStatementList(parseTopLevel(lines, 0).first, local); }
		catch (const ReturnException&) {}
	}

	bool exist(const std::string& var, const QiVarMap* local = nullptr)
	{
		if (isValidVariableName(var))
		{
			if (isGlobalVariable(var)) return globalExist(var);
			else return localExist(var, local);
		}
		return false;
	}
	bool globalExist(const std::string& var)
	{
		std::unique_lock<std::mutex> lock(globalVariablesMutex);
		return globalVariables.find(var) != globalVariables.end();
	}
	bool localExist(const std::string& var, const QiVarMap* local = nullptr)
	{
		if (local)
		{
			return local->find(var) != local->end();
		}
		else
		{
			std::unique_lock<std::mutex> lock(localVariablesMutex);
			return localVariables.find(var) != localVariables.end();
		}
	}

	QiVar value(const std::string& var, const QiVarMap* local = nullptr)
	{
		if (isValidVariableName(var))
		{
			if (isGlobalVariable(var)) return globalValue(var);
			else return localValue(var, local);
		}
		return QiVar();
	}
	void setValue(const std::string& var, const QiVar& value, QiVarMap* local = nullptr)
	{
		if (isValidVariableName(var))
		{
			if (isGlobalVariable(var))
			{
				std::unique_lock<std::mutex> lock(globalVariablesMutex);
				globalVariables[var] = value;
			}
			else
			{
				if (local) (*local)[var] = value;
				else
				{
					std::unique_lock<std::mutex> lock(localVariablesMutex);
					localVariables[var] = value;
				}
			}
		}
	}

	static QiVar globalValue(const std::string& var)
	{
		std::unique_lock<std::mutex> lock(globalVariablesMutex);
		const auto& i = globalVariables.find(var);
		if (i != globalVariables.end()) return i->second;
		return QiVar();
	}
	static void setGlobalValue(const std::string& var, const QiVar& value)
	{
		std::unique_lock<std::mutex> lock(globalVariablesMutex);
		globalVariables[var] = value;
	}

	QiVar localValue(const std::string& var, const QiVarMap* local = nullptr)
	{
		if (local)
		{
			const auto& i = local->find(var);
			if (i != local->end()) return i->second;
		}
		else
		{
			std::unique_lock<std::mutex> lock(localVariablesMutex);
			const auto& i = localVariables.find(var);
			if (i != localVariables.end()) return i->second;
		}
		return QiVar();
	}
	void setLocalValue(const std::string& var, const QiVar& value, QiVarMap* local = nullptr)
	{
		if (local) (*local)[var] = value;
		else
		{
			std::unique_lock<std::mutex> lock(localVariablesMutex);
			localVariables[var] = value;
		}
	}

	QiWorker* worker() { return workerPtr; }
	void setWorker(QiWorker* worker) { workerPtr = worker; }

	static QiVarMap* global() { return &globalVariables; }
	QiVarMap* local() { return &localVariables; }

	static std::mutex* globalMutex() { return &globalVariablesMutex; }
	std::mutex* localMutex() { return &localVariablesMutex; }

	void clearGlobal() { std::unique_lock<std::mutex> lock(globalVariablesMutex); globalVariables.clear(); }
	void clearLocal() { std::unique_lock<std::mutex> lock(localVariablesMutex); localVariables.clear(); }

	static auto makeString(const std::string str) -> std::string
	{
		std::string result = str;
		result.erase(std::remove(result.begin(), result.end(), '\''), result.end());
		result = "'" + result + "'";
		return result;
	}
	static bool isReserveSymbol(unsigned char ch)
	{
		return ch != '_' && ch < 0x80 && !std::isalnum(ch);
	}
	static bool isValidVariableChar(unsigned char ch)
	{
		return ch == '_' || ch > 0x7F || std::isalnum(ch);
	}
	static bool isValidVariableFirst(unsigned char ch)
	{
		return ch == '_' || ch > 0x7F || std::isalpha(ch);
	}
	static bool isValidVariableName(const std::string& name)
	{
		if (name.empty()) return false;
		bool global = name[0] == '$';
		if (global && name.size() < 2) return false;
		if (!isValidVariableFirst(global ? name[1] : name[0])) return false;
		return QiVar::iterateStrX(name.substr(global ? 1 : 0), [](const std::string& ch) {
			if (ch.size() == 1 && !isValidVariableChar(ch[0])) return false;
			return true;
			});
	}
	static bool isGlobalVariable(const std::string& name)
	{
		return isValidVariableName(name) && name.front() == '$';
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
			text = L"变量参数缺少或过多";
		}
		else if (msg.find("==") != std::string::npos)
		{
			text = L"== 操作符使用错误";
		}
		else if (msg.find(QiScriptInterpreter::error_unknown_operator) != std::string::npos)
		{
			text = L"未知运算符号";
		}
		else if (msg.find(QiScriptInterpreter::error_unknown_functions) != std::string::npos)
		{
			text = L"未知函数";
		}
		else if (msg.find(QiScriptInterpreter::error_functions_parameter_invalid) != std::string::npos)
		{
			text = L"函数参数无效";
		}
		MessageBoxW(nullptr, (String::toWString(msg) + std::wstring(L"\n\n") + text + std::wstring(L"\n\n") + String::toWString(addition)).c_str(), L"Quickinput script", MB_ICONERROR | MB_TOPMOST);
	}
};
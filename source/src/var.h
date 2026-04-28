#pragma once
#include <cmath>
#include <cctype>
#include <string>
#include <vector>
#include <map>
#include <variant>
#include <optional>
#include <functional>
#include <stdexcept>
#include <algorithm>
#include <regex>

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
	QiVar(std::nullptr_t) : var(ptr_t(nullptr)) {}
	QiVar(char val) : var(int_t(val)) {}
	QiVar(unsigned char val) : var(int_t(val)) {}
	QiVar(short val) : var(int_t(val)) {}
	QiVar(unsigned short val) : var(int_t(val)) {}
	QiVar(int val) : var(int_t(val)) {}
	QiVar(unsigned int val) : var(int_t(val)) {}
	QiVar(long val) : var(int_t(val)) {}
	QiVar(unsigned long val) : var(int_t(val)) {}
	QiVar(long long val) : var(int_t(val)) {}
	QiVar(unsigned long long val) : var(int_t(val)) {}
	QiVar(float val) : var(num_t(val)) {}
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
	bool isInteger() const { return type() == t_int; }
	bool isNumber() const { return type() == t_int; }
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
		if (type() == t_str) return len(get<str_t>(var));
		if (type() == t_ptr) return toString(get<ptr_t>(var)).length();
		if (type() == t_bool) return toString(get<bool>(var)).length();
		return 0LL;
	}

	int_t size() const
	{
		if (type() == t_nul) return 0LL;
		if (type() == t_int) return sizeof(int_t);
		if (type() == t_num) return sizeof(num_t);
		if (type() == t_str) return get<str_t>(var).size();
		if (type() == t_ptr) return sizeof(ptr_t);
		if (type() == t_bool) return sizeof(bool);
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

	static size_t len(const std::string& str)
	{
		size_t count = 0;
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
			count++;
			i += charLen;
		}
		return count;
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
		if (str.empty() || !length) return {};
		size_t p = 0;
		const size_t e = length == ~size_t(0) ? ~size_t(0) : where + length;
		std::string s;
		iterateStrX(str, [&p, &e, &s, where, length](const std::string& ch) -> bool {
			if (p >= e) return true;;
			if (p >= where) s += ch;
			p++;
			return true;
		});
		return s;
	}
	static std::string subx(const std::string& str, size_t where, size_t length = ~size_t(0))
	{
		if (str.empty() || !length) return {};
		const size_t size = len(str);
		size_t end = size - where - 1;
		if (end >= size) return std::string();
		size_t begin = end - length + 1;
		if (begin >= size) begin = 0;
		return sub(str, begin, end - begin + 1);
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
	// return false to break;
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
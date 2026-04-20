#pragma once
#ifndef TYPE_PACK
#define TYPE_PACK

#include <stdint.h>
#include <stdexcept>
#include <algorithm>
#include <optional>
#include <string>
#include <vector>
#include <map>
#include <variant>

/*

Values
	True/False:
	type(8)

	Number:
	type(8)	value(8~64)

	String/Binary:
	type(8)	length(8~64)	data...

	Object/Array
	type(8)	length(8~64)	items...

Object Pair
	String value + Any value
	key_type(8)	key_length(8~64)	value_type(8)	value...

Serialization represents a value based on its minimum value, which can provide additional space compression

*/

namespace typepack
{
	class value;
	using null = std::nullopt_t;
	using integer = int64_t;
	using uinteger = uint64_t;
	using number = double;
	using string = std::string;
	using binary = std::vector<char>;
	class object : public std::map<string, value>
	{
		using base = std::map<string, value>;
	public:
		binary toBinary() const;
		static object fromBinary(const binary& bin);
		bool exist(const base::key_type& key) const;
		value get(const base::key_type& key) const;
		value get(const base::key_type& key, const value& default_value) const;
		value get(const base::key_type& key, value&& default_value) const;
		void set(const base::key_type& key, const value& value);
		void set(const base::key_type& key, value&& value);
	};
	class array : public std::vector<value>
	{
		using base = std::vector<value>;
	public:
		binary toBinary() const;
		static array fromBinary(const binary& bin);
	};

	class value : public std::variant<null, bool, object, array, integer, uinteger, number, string, binary>
	{
		using base = std::variant<null, bool, object, array, integer, uinteger, number, string, binary>;
	public:
		enum Type
		{
			Null,
			Bool,
			Object,
			Array,
			Int,
			UInt,
			Number,
			String,
			Binary
		};
		value() : base(std::nullopt) {}
		value(bool v) : base(v) {}
		value(const object& v) : base(v) {}
		value(object&& v) : base(std::move(v)) {}
		value(const array& v) : base(v) {}
		value(array&& v) : base(std::move(v)) {}
		value(int8_t v) : base(static_cast<integer>(v)) {}
		value(int16_t v) : base(static_cast<integer>(v)) {}
		value(int32_t v) : base(static_cast<integer>(v)) {}
		value(long v) : base(static_cast<integer>(v)) {}
		value(int64_t v) : base(static_cast<integer>(v)) {}
		value(uint8_t v) : base(static_cast<uinteger>(v)) {}
		value(uint16_t v) : base(static_cast<uinteger>(v)) {}
		value(uint32_t v) : base(static_cast<uinteger>(v)) {}
		value(unsigned long v) : base(static_cast<uinteger>(v)) {}
		value(uint64_t v) : base(static_cast<uinteger>(v)) {}
		value(float v) : base(static_cast<number>(v)) {}
		value(double v) : base(static_cast<number>(v)) {}
		value(const char* v) : base(string(v)) {}
		value(const string& v) : base(v) {}
		value(string&& v) : base(std::move(v)) {}
		value(const binary& v) : base(v) {}
		value(binary&& v) : base(std::move(v)) {}
		value& operator=(bool v) { base::operator=(v); return *this; }
		value& operator=(const object& v) { base::operator=(v); return *this; }
		value& operator=(object&& v) { base::operator=(std::move(v)); return *this; }
		value& operator=(const array& v) { base::operator=(v); return *this; }
		value& operator=(array&& v) { base::operator=(std::move(v)); return *this; }
		value& operator=(int8_t v) { base::operator=(static_cast<integer>(v)); return *this; }
		value& operator=(int16_t v) { base::operator=(static_cast<integer>(v)); return *this; }
		value& operator=(int32_t v) { base::operator=(static_cast<integer>(v)); return *this; }
		value& operator=(long v) { base::operator=(static_cast<integer>(v)); return *this; }
		value& operator=(int64_t v) { base::operator=(static_cast<integer>(v)); return *this; }
		value& operator=(uint8_t v) { base::operator=(static_cast<uinteger>(v)); return *this; }
		value& operator=(uint16_t v) { base::operator=(static_cast<uinteger>(v)); return *this; }
		value& operator=(uint32_t v) { base::operator=(static_cast<uinteger>(v)); return *this; }
		value& operator=(unsigned long v) { base::operator=(static_cast<uinteger>(v)); return *this; }
		value& operator=(uint64_t v) { base::operator=(static_cast<uinteger>(v)); return *this; }
		value& operator=(float v) { base::operator=(static_cast<number>(v)); return *this; }
		value& operator=(double v) { base::operator=(static_cast<number>(v)); return *this; }
		value& operator=(const char* v) { base::operator=(string(v)); return *this; }
		value& operator=(const string& v) { base::operator=(v); return *this; }
		value& operator=(string&& v) { base::operator=(std::move(v)); return *this; }
		value& operator=(const binary& v) { base::operator=(v); return *this; }
		value& operator=(binary&& v) { base::operator=(std::move(v)); return *this; }

		operator bool() const { return index() != Type::Null; }
		bool isNull() const { return index() == Type::Null; }
		bool isObject() const { return index() == Type::Object; }
		bool isArray() const { return index() == Type::Array; }
		bool isBool() const { return index() == Type::Bool; }
		bool isInt() const { return index() == Type::Int; }
		bool isUInt() const { return index() == Type::UInt; }
		bool isNumber() const { return index() == Type::Number; }
		bool isString() const { return index() == Type::String; }
		bool isBinary() const { return index() == Type::Binary; }

		template <typename T>
		const T& to() const { return std::get<T>(*this); }
		template <typename T>
		T& to() { return std::get<T>(*this); }
		object toObject() const
		{
			if (index() == Type::Object) return std::get<object>(*this);
			return {};
		}
		array toArray() const
		{
			if (index() == Type::Array) return std::get<array>(*this);
			return {};
		}
		bool toBool() const
		{
			if (index() == Type::Bool) return std::get<bool>(*this);
			if (index() == Type::Int) return static_cast<bool>(std::get<integer>(*this));
			if (index() == Type::UInt) return static_cast<bool>(std::get<uinteger>(*this));
			if (index() == Type::Number) return static_cast<bool>(std::get<number>(*this));
			return {};
		}
		integer toInt() const
		{
			if (index() == Type::Bool) return static_cast<integer>(std::get<bool>(*this));
			if (index() == Type::Int) return std::get<integer>(*this);
			if (index() == Type::UInt) return static_cast<integer>(std::get<uinteger>(*this));
			if (index() == Type::Number) return static_cast<integer>(std::get<number>(*this));
			return {};
		}
		uinteger toUInt() const
		{
			if (index() == Type::Bool) return static_cast<uinteger>(std::get<bool>(*this));
			if (index() == Type::Int) return static_cast<uinteger>(std::get<integer>(*this));
			if (index() == Type::UInt) return std::get<uinteger>(*this);
			if (index() == Type::Number) return static_cast<uinteger>(std::get<number>(*this));
			return {};
		}
		number toNumber()  const
		{
			if (index() == Type::Bool) return static_cast<number>(std::get<bool>(*this));
			if (index() == Type::Int) return static_cast<number>(std::get<integer>(*this));
			if (index() == Type::UInt) return static_cast<number>(std::get<uinteger>(*this));
			if (index() == Type::Number) return std::get<number>(*this);
			return {};
		}
		string toString()  const
		{
			if (index() == Type::Bool) return toString(std::get<bool>(*this));
			if (index() == Type::Int) return toString(std::get<integer>(*this));
			if (index() == Type::UInt) return toString(std::get<uinteger>(*this));
			if (index() == Type::Number) return toString(std::get<number>(*this));
			if (index() == Type::String) return std::get<string>(*this);
			return {};
		}
		binary toBinary() const
		{
			if (index() == Type::Binary) return std::get<binary>(*this);
			return {};
		}
	private:
		static string toString(bool v)
		{
			static const string t = "true";
			static const string f = "false";
			return v ? t : f;
		}
		static string toString(integer v)
		{
			return std::to_string(v);
		}
		static string toString(uinteger v)
		{
			return std::to_string(v);
		}
		static string toString(number v)
		{
			return std::to_string(v);
		}
	};

	class pack
	{
	public:
		enum Type
		{
			False = 0,
			True = 1,
			Object8 = 0b00000010,
			Object16 = 0b01000010,
			Object32 = 0b10000010,
			Object64 = 0b11000010,
			Array8 = 0b00000011,
			Array16 = 0b01000011,
			Array32 = 0b10000011,
			Array64 = 0b11000011,
			Int8 = 0b00000100,
			Int16 = 0b01000100,
			Int32 = 0b10000100,
			Int64 = 0b11000100,
			UInt8 = 0b00000101,
			UInt16 = 0b01000101,
			UInt32 = 0b10000101,
			UInt64 = 0b11000101,
			Float8 = 0b00000110, // reserve
			Float16 = 0b01000110, // reserve
			Float32 = 0b10000110, // reserve
			Float64 = 0b11000110, // only implement Float64
			String8 = 0b00000111,
			String16 = 0b01000111,
			String32 = 0b10000111,
			String64 = 0b11000111,
			Binary8 = 0b00001000,
			Binary16 = 0b01001000,
			Binary32 = 0b10001000,
			Binary64 = 0b11001000
		};
	private:
		static constexpr size_t tag_bytes(Type t) {
			switch (t) {
			case Type::False: case Type::True: return 0;
			case Type::Object8: case Type::Array8: case Type::Int8: case Type::UInt8:
			case Type::String8: case Type::Binary8: return 1;
			case Type::Object16: case Type::Array16: case Type::Int16: case Type::UInt16:
			case Type::String16: case Type::Binary16: return 2;
			case Type::Object32: case Type::Array32: case Type::Int32: case Type::UInt32:
			case Type::String32: case Type::Binary32: return 4;
			case Type::Object64: case Type::Array64: case Type::Int64: case Type::UInt64:
			case Type::Float64: case Type::String64: case Type::Binary64: return 8;
			}
			return 0;
		}

		static void write_uint64(binary& out, uint64_t value, size_t bytes)
		{
			for (size_t i = 0; i < bytes; ++i) {
				out.push_back(static_cast<char>(value & 0xFF));
				value >>= 8;
			}
		}
		static uint64_t read_uint64(const char*& data, size_t bytes)
		{
			uint64_t result = 0;
			for (size_t i = 0; i < bytes; ++i) result |= (static_cast<uint64_t>(static_cast<uint8_t>(*data++)) << (i * 8));
			return result;
		}

		static Type choose_int_type(int64_t value)
		{
			if (value >= INT8_MIN && value <= INT8_MAX) return Type::Int8;
			if (value >= INT16_MIN && value <= INT16_MAX) return Type::Int16;
			if (value >= INT32_MIN && value <= INT32_MAX) return Type::Int32;
			return Type::Int64;
		}
		static Type choose_uint_type(uint64_t value)
		{
			if (value <= UINT8_MAX) return Type::UInt8;
			if (value <= UINT16_MAX) return Type::UInt16;
			if (value <= UINT32_MAX) return Type::UInt32;
			return Type::UInt64;
		}
		static Type choose_obj_type(size_t count)
		{
			if (count <= UINT8_MAX) return Type::Object8;
			if (count <= UINT16_MAX) return Type::Object16;
			if (count <= UINT32_MAX) return Type::Object32;
			return Type::Object64;
		}
		static Type choose_array_type(size_t count)
		{
			if (count <= UINT8_MAX) return Type::Array8;
			if (count <= UINT16_MAX) return Type::Array16;
			if (count <= UINT32_MAX) return Type::Array32;
			return Type::Array64;
		}
		static Type choose_string_type(size_t len)
		{
			if (len <= UINT8_MAX) return Type::String8;
			if (len <= UINT16_MAX) return Type::String16;
			if (len <= UINT32_MAX) return Type::String32;
			return Type::String64;
		}
		static Type choose_binary_type(size_t len)
		{
			if (len <= UINT8_MAX) return Type::Binary8;
			if (len <= UINT16_MAX) return Type::Binary16;
			if (len <= UINT32_MAX) return Type::Binary32;
			return Type::Binary64;
		}

		static void serialize_string(const string& s, binary& out)
		{
			Type t = choose_string_type(s.size());
			out.push_back(static_cast<char>(t));
			write_uint64(out, s.size(), tag_bytes(t));
			out.insert(out.end(), s.begin(), s.end());
		}
		static void serialize_binary(const binary& b, binary& out)
		{
			Type t = choose_binary_type(b.size());
			out.push_back(static_cast<char>(t));
			write_uint64(out, b.size(), tag_bytes(t));
			out.insert(out.end(), b.begin(), b.end());
		}
		static void serialize_integer(int64_t i, binary& out)
		{
			Type t = choose_int_type(i);
			out.push_back(static_cast<char>(t));
			write_uint64(out, static_cast<uint64_t>(i), tag_bytes(t));
		}
		static void serialize_uinteger(uint64_t u, binary& out)
		{
			Type t = choose_uint_type(u);
			out.push_back(static_cast<char>(t));
			write_uint64(out, u, tag_bytes(t));
		}
		static void serialize_number(double d, binary& out)
		{
			out.push_back(static_cast<char>(Type::Float64));
			write_uint64(out, *reinterpret_cast<uint64_t*>(&d), 8);
		}
		static void serialize(const value& v, binary& out)
		{
			switch (v.index()) {
			case value::Type::Bool:
				out.push_back(static_cast<char>(std::get<bool>(v) ? Type::True : Type::False));
				break;
			case value::Type::Int:
				serialize_integer(std::get<integer>(v), out);
				break;
			case value::Type::UInt:
				serialize_uinteger(std::get<uinteger>(v), out);
				break;
			case value::Type::Number:
				serialize_number(std::get<number>(v), out);
				break;
			case value::Type::String:
				serialize_string(std::get<string>(v), out);
				break;
			case value::Type::Binary:
				serialize_binary(std::get<binary>(v), out);
				break;
			case value::Type::Array: {
				const array& a = std::get<array>(v);
				Type t = choose_array_type(a.size());
				out.push_back(static_cast<char>(t));
				write_uint64(out, a.size(), tag_bytes(t));
				for (const auto& elem : a) serialize(elem, out);
				break;
			}
			case value::Type::Object: {
				const object& o = std::get<object>(v);
				Type t = choose_obj_type(o.size());
				out.push_back(static_cast<char>(t));
				write_uint64(out, o.size(), tag_bytes(t));
				for (const auto& pair : o) {
					serialize_string(pair.first, out);
					serialize(pair.second, out);
				}
				break;
			}
			}
		}

		static std::optional<string> deserialize_string(const char*& data, const char* end)
		{
			if (data >= end) return std::nullopt;
			Type t = static_cast<Type>(static_cast<uint8_t>(*data++));
			size_t bytes = tag_bytes(t);
			if (data + bytes > end) return std::nullopt;
			size_t len = read_uint64(data, bytes);
			if (data + len > end) return std::nullopt;
			string result(data, len);
			data += len;
			return result;
		}
		static std::optional<binary> deserialize_binary(const char*& data, const char* end)
		{
			if (data >= end) return std::nullopt;
			Type t = static_cast<Type>(static_cast<uint8_t>(*data++));
			size_t bytes = tag_bytes(t);
			if (data + bytes > end) return std::nullopt;
			size_t len = read_uint64(data, bytes);
			if (data + len > end) return std::nullopt;
			binary result(data, data + len);
			data += len;
			return result;
		}
		static std::optional<integer> deserialize_integer(const char*& data, const char* end)
		{
			if (data >= end) return std::nullopt;
			Type t = static_cast<Type>(static_cast<uint8_t>(*data++));
			size_t bytes = tag_bytes(t);
			if (data + bytes > end) return std::nullopt;
			uint64_t u = read_uint64(data, bytes);
			switch (t) {
			case Type::Int8:  return static_cast<int8_t>(u);
			case Type::Int16: return static_cast<int16_t>(u);
			case Type::Int32: return static_cast<int32_t>(u);
			case Type::Int64: return static_cast<int64_t>(u);
			default: return std::nullopt;
			}
		}
		static std::optional<uinteger> deserialize_uinteger(const char*& data, const char* end)
		{
			if (data >= end) return std::nullopt;
			Type t = static_cast<Type>(static_cast<uint8_t>(*data++));
			size_t bytes = tag_bytes(t);
			if (data + bytes > end) return std::nullopt;
			uint64_t u = read_uint64(data, bytes);
			return u;
		}
		static std::optional<number> deserialize_number(const char*& data, const char* end)
		{
			if (data >= end) return std::nullopt;
			Type t = static_cast<Type>(static_cast<uint8_t>(*data++));
			if (t != Type::Float64) return std::nullopt;
			if (data + 8 > end) return std::nullopt;
			uint64_t u = read_uint64(data, 8);
			return *reinterpret_cast<double*>(&u);
		}
		static std::optional<value> deserialize(const char*& data, const char* end)
		{
			if (data >= end) return std::nullopt;
			Type t = static_cast<Type>(static_cast<uint8_t>(*data++));
			switch (t) {
			case Type::False: return false;
			case Type::True:  return true;
			case Type::Int8:  case Type::Int16:
			case Type::Int32: case Type::Int64: {
				--data;
				auto val = deserialize_integer(data, end);
				if (!val) return std::nullopt;
				return std::move(*val);
			}
			case Type::UInt8: case Type::UInt16:
			case Type::UInt32: case Type::UInt64: {
				--data;
				auto val = deserialize_uinteger(data, end);
				if (!val) return std::nullopt;
				return std::move(*val);
			}
			case Type::Float64: {
				--data;
				auto val = deserialize_number(data, end);
				if (!val) return std::nullopt;
				return std::move(*val);
			}
			case Type::String8:  case Type::String16:
			case Type::String32: case Type::String64: {
				--data;
				auto val = deserialize_string(data, end);
				if (!val) return std::nullopt;
				return std::move(*val);
			}
			case Type::Binary8:  case Type::Binary16:
			case Type::Binary32: case Type::Binary64: {
				--data;
				auto val = deserialize_binary(data, end);
				if (!val) return std::nullopt;
				return std::move(*val);
			}
			case Type::Array8:  case Type::Array16:
			case Type::Array32: case Type::Array64: {
				size_t bytes = tag_bytes(t);
				if (data + bytes > end) return std::nullopt;
				size_t count = read_uint64(data, bytes);
				array arr;
				arr.reserve(count);
				for (size_t i = 0; i < count; ++i) {
					auto elem = deserialize(data, end);
					if (!elem) return std::nullopt;
					arr.push_back(std::move(*elem));
				}
				return arr;
			}
			case Type::Object8:  case Type::Object16:
			case Type::Object32: case Type::Object64: {
				size_t bytes = tag_bytes(t);
				if (data + bytes > end) return std::nullopt;
				size_t count = read_uint64(data, bytes);
				object obj;
				for (size_t i = 0; i < count; ++i) {
					auto key = deserialize_string(data, end);
					if (!key) return std::nullopt;
					auto val = deserialize(data, end);
					if (!val) return std::nullopt;
					obj[std::move(*key)] = std::move(*val);
				}
				return obj;
			}
			default: return std::nullopt;
			}
		}
	public:
		static binary toBinary(const value& v)
		{
			binary out;
			serialize(v, out);
			return out;
		}

		static value fromBinary(const char*& data, const char* end)
		{
			auto r = deserialize(data, end);
			if (r) return std::move(*r);
			return {};
		}
		static value fromBinary(const binary& bin)
		{
			const char* data = bin.data();
			auto r = deserialize(data, data + bin.size());
			if (r) return std::move(*r);
			return {};
		}
	};
}

#endif TYPE_PACK
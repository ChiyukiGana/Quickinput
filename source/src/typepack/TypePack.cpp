#include "TypePack.h"

namespace typepack
{
	bool object::exist(const base::key_type& key) const
	{
		for (const auto& i : *this) if (i.first == key) return true;
		return false;
	}
	value object::get(const base::key_type& key) const
	{
		for (const auto& i : *this) if (i.first == key) return i.second;
		return {};
	}
	value object::get(const base::key_type& key, const value& default_value) const
	{
		for (const auto& i : *this) if (i.first == key) return i.second;
		return default_value;
	}
	value object::get(const base::key_type& key, value&& default_value) const
	{
		for (const auto& i : *this) if (i.first == key) return i.second;
		return std::move(default_value);
	}
	void object::set(const base::key_type& key, const value& val)
	{
		base::operator[](key) = val;
	}
	void object::set(const base::key_type& key, value&& val)
	{
		base::operator[](key) = std::move(val);
	}
	binary object::toBinary() const
	{
		return pack::toBinary(*this);
	}
	object object::fromBinary(const binary& bin)
	{
		if (bin.empty()) return {};
		if ((static_cast<uint8_t>(bin.front()) & static_cast<uint8_t>(0b00001111)) != static_cast<uint8_t>(pack::Type::Object8)) return {};
		value v = pack::fromBinary(bin);
		if (v.isObject()) return std::move(v.to<object>());
		return {};
	}

	binary array::toBinary() const
	{
		return pack::toBinary(*this);
	}
	array array::fromBinary(const binary& bin)
	{
		if (bin.empty()) return {};
		if ((static_cast<uint8_t>(bin.front()) & static_cast<uint8_t>(0b00001111)) != static_cast<uint8_t>(pack::Type::Array8)) return {};
		value v = pack::fromBinary(bin);
		if (v.isArray()) return std::move(v.to<array>());
		return {};
	}
}
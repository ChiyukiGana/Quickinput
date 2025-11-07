#pragma once
#include <vector>
namespace QiTools
{
	template<class ty>
	class map2d : protected std::vector<ty>
	{
		using vector = std::vector<ty>;
		size_t m_w;
		size_t m_h;
	public:
		using typename vector::iterator;
		using typename vector::const_iterator;
		using typename vector::reverse_iterator;
		using typename vector::const_reverse_iterator;
		using vector::empty;
		using vector::size;
		using vector::begin;
		using vector::end;
		using vector::rbegin;
		using vector::rend;
		using vector::at;
		using vector::front;
		using vector::back;
		using vector::data;
		using vector::operator[];

		map2d() noexcept :m_w(0), m_h(0) {}
		map2d(size_t width, size_t height, const ty& init = ty()) noexcept { assign(width, height, init); }
		map2d(const map2d& other) noexcept { operator=(other); }
		map2d(map2d&& other) noexcept { operator=(std::move(other)); }

		map2d& operator=(const map2d& other) noexcept { vector::operator=(other), m_w = other.m_w, m_h = other.m_h; return *this; }
		map2d& operator=(map2d&& other) noexcept { vector::operator=(std::move(other)), m_w = other.m_w, m_h = other.m_h, other.m_w = other.m_h = 0; return *this; }

		void assign(size_t width, size_t height) noexcept { if (width && height) { if (m_w != width && m_h != height) m_w = width, m_h = height, vector::assign(width * height, ty()); } else clear(); }
		void assign(size_t width, size_t height, const ty& init) noexcept { if (width && height) m_w = width, m_h = height, vector::assign(width * height, init); else clear(); }
		void clear() noexcept { m_w = m_h = 0, vector::clear(); }
		void fill(const ty& value) noexcept { for (auto& i : *this) i = value; }
		void swap(map2d& other) noexcept { std::swap(m_w, other.m_w), std::swap(m_h, other.m_h), vector::swap(other); }

		operator bool() const noexcept { return !empty(); }
		bool not_empty() const noexcept { return !empty(); }
		size_t width() const noexcept { return m_w; }
		size_t height() const noexcept { return m_h; }
		size_t bytes() const noexcept { return size() * sizeof(ty); }

		ty& at(size_t x, size_t y) { return at(m_w * y + x); }
		const ty& at(size_t x, size_t y) const { return at(m_w * y + x); }
	};
}
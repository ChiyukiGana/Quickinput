#pragma once
#include <algorithm>
#include <vector>
#include "base.h"

template<typename T>
void IterateVector(std::vector<T>& v, void(&CallBack)(T&))
{
	for (size_t p = 0; p < v.size(); p++)
	{
		CallBack(v[p]);
	}
}

namespace CG
{
	template<class T>
	class List : public std::vector<T>
	{
		using base_vector = std::vector<T>;
	public:
		static constexpr size_t end_pos = ~((size_t)0);
		using base_vector::base_vector;
		
		T* End()
		{
			if (base_vector::empty()) return nullptr;
			return &base_vector::at(base_vector::size() - 1);
		}
		T* Begin()
		{
			if (base_vector::empty()) return nullptr;
			return &base_vector::at(0);
		}
		T& AddNull()
		{
			Add(std::move(T()), 1);
			return base_vector::at(base_vector::size() - 1);
		}
		void AddNull(const size_t count) 
		{
			Add({}, count);
		}
		T& Add(T&& t)
		{
			base_vector::push_back(std::move(t));
			return base_vector::at(base_vector::size() - 1);
		}
		T& Add(const T& t)
		{
			base_vector::push_back(t);
			return base_vector::at(base_vector::size() - 1);
		}
		void Add(const T& t, size_t count)
		{
			for (size_t c = 0; c < count; c++) base_vector::push_back(t);
		}
		void AddArray(const T* t, size_t size)
		{
			for (size_t p = 0; p < size; p++) base_vector::push_back(t[p]);
		}
		T& InsNull(size_t p)
		{
			Ins(std::move(T()), p);
			return base_vector::at(p);
		}
		void InsNull(size_t p, size_t count)
		{
			Ins(T(), p, count);
		}
		T& Ins(T&& t, size_t p)
		{
			if (p > base_vector::size()) p = base_vector::size();
			base_vector::insert(base_vector::begin() + p, std::move(t));
			return base_vector::at(p);
		}
		T& Ins(const T& t, size_t p)
		{
			if (p > base_vector::size()) p = base_vector::size();
			base_vector::insert(base_vector::begin() + p, t);
			return base_vector::at(p);
		}
		void Ins(const T& t, size_t p, size_t count)
		{
			if (p > base_vector::size()) p = base_vector::size();
			base_vector::insert(base_vector::begin() + p, count, t);
		}
		void Del(const T& rt)
		{
			for (size_t i = 0; i < base_vector::size(); i++)
			{
				if (&base_vector::at(i) == &rt)
				{
					base_vector::erase(base_vector::begin() + i);
					return;
				}
			}
		}
		void Del(const T* pt)
		{
			for (size_t i = 0; i < base_vector::size(); i++)
			{
				if (&base_vector::at(i) == pt)
				{
					base_vector::erase(base_vector::begin() + i);
					return;
				}
			}
		}
		void Del(size_t p)
		{
			base_vector::erase(base_vector::begin() + p);
		}
		void Del(const List<size_t>& ps)
		{
			List<size_t> pl(ps);
			std::sort(pl.begin(), pl.end());
			for (size_t i = ps.size(); i > 0; i--)
			{
				base_vector::erase(base_vector::begin() + pl[i - 1]);
				if (i < 1) break;
			}
		}
		void DelFront(size_t count = 1, size_t p = 0)
		{
			if (p >= base_vector::size()) return;
			if ((p + count) >= base_vector::size()) count = base_vector::size() - p;
			base_vector::erase(base_vector::begin() + p, base_vector::begin() + (p + count));
		}
		void DelBack(size_t count = 1, size_t p = end_pos)
		{
			if (p > base_vector::size()) p = base_vector::size();
			if (count > p) count = p;
			base_vector::erase(base_vector::begin() + (p - count), base_vector::begin() + p);
		}
		size_t DelFind(const T& t)
		{
			size_t n = 0;
			while (true)
			{
				bool none = true;
				for (size_t i = 0; i < base_vector::size(); i++)
				{
					if (t == base_vector::at(i))
					{
						base_vector::erase(base_vector::begin() + i);
						n++;
						none = false;
					}
				}
				if (none) break;
			}
			return n;
		}
		void Swp(size_t p1, size_t p2)
		{
			std::swap(base_vector::at(p1), base_vector::at(p2));
		}
		void Mov(size_t p1, size_t p2)
		{
			if (p1 < p2) for (size_t p = p1; p < p2; p++) Swp(p, p + 1);
			else if (p1 > p2) for (size_t p = p1; p > p2; p--) Swp(p, p - 1);
		}
		void Iterate(void(&CallBack)(T&))
		{
			for (size_t p = 0; p < base_vector::size(); p++) CallBack(base_vector::at(p));
		}
		void Iterate(void(&CallBack)(T&, void*), void* param)
		{
			for (size_t p = 0; p < base_vector::size(); p++) CallBack(base_vector::at(p), param);
		}
		/* return left < right: asc, return left > right: desc */
		void Sort(bool(&CallBack)(const T& left, const T& right))
		{
			size_t size = base_vector::size();
			if (size < 2) return;
			size--;
			bool b;
			do
			{
				b = false;
				for (size_t p = 0; p < size; p++)
				{
					if (CallBack(base_vector::at(p + 1), base_vector::at(p)))
					{
						std::swap(base_vector::at(p + 1), base_vector::at(p));
						b = true;
					}
				}
			} while (b);
		}
	};
}
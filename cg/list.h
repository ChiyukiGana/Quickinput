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
		static const size_t npos = MAXSIZE_T;

		T& End()
		{
			return base_vector::at(base_vector::size() - 1);
		}
		T& AddNull()
		{
			Add({}, 1);
			return base_vector::at(base_vector::size() - 1);
		}
		void AddNull(const size_t count) 
		{
			Add({}, count);
		}
		void Add(const T& t, size_t count = 1)
		{
			for (size_t c = 0; c < count; c++) base_vector::push_back(t);
		}
		void AddArray(const T* t, size_t size)
		{
			for (size_t p = 0; p < size; p++) base_vector::push_back(t[p]);
		}
		T& InsNull(size_t p)
		{
			Ins({}, p, 1);
			return base_vector::at(p);
		}
		void InsNull(size_t p, size_t count)
		{
			Ins({}, p, count);
		}
		void Ins(const T& t, size_t p, size_t count = 1)
		{
			if (p > base_vector::size()) p = base_vector::size();
			base_vector::insert(base_vector::begin() + p, count, t);
		}
		bool Del(size_t p = MAXSIZE_T)
		{
			if (base_vector::size() == 0) return false;
			if (p >= base_vector::size()) p = base_vector::size() - 1;
			base_vector::erase(base_vector::begin() + p);
			return true;
		}
		bool DelBack(size_t count = 1, size_t p = MAXSIZE_T)
		{
			if (base_vector::size() == 0) return false;
			if (p > base_vector::size()) p = base_vector::size();
			if (count > p) count = p;
			base_vector::erase(base_vector::begin() + (p - count), base_vector::begin() + p);
			return true;
		}
		bool DelFront(size_t count = 1, size_t p = 0)
		{
			if (base_vector::size() == 0) return false;
			if (p >= base_vector::size()) return true;
			if (count >= base_vector::size()) count = base_vector::size();
			if ((p + count) >= base_vector::size()) count = base_vector::size() - p;
			base_vector::erase(base_vector::begin() + p, base_vector::begin() + (p + count));
			return true;
		}
		bool Del(List<size_t>& ps)
		{
			if (base_vector::size() == 0) return false;
			std::sort(ps.begin(), ps.end());
			for (size_t n = 0; n < ps.size(); n++) base_vector::erase(base_vector::begin() + ps[n] - n);
			return true;
		}
		bool Swp(size_t p1, size_t p2)
		{
			if (p1 >= base_vector::size() || p2 >= base_vector::size()) return false;
			std::swap(base_vector::at(p1), base_vector::at(p2));
			return true;
		}
		bool Mov(size_t p1, size_t p2)
		{
			if (p1 >= base_vector::size() || p2 >= base_vector::size()) return 0;
			if (p1 < p2)
			{
				for (size_t p = p1; p < p2; p++) Swp(p, p + 1);
				return true;
			}
			else if (p1 > p2)
			{
				for (size_t p = p1; p > p2; p--) Swp(p, p - 1);
				return true;
			}
			return false;
		}
		void Iterate(void(&CallBack)(T&))
		{
			for (size_t p = 0; p < base_vector::size(); p++)
			{
				CallBack(base_vector::at(p));
			}
		}
		void Iterate(void(&CallBack)(T&, void*), void* param)
		{
			for (size_t p = 0; p < base_vector::size(); p++)
			{
				CallBack(base_vector::at(p), param);
			}
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
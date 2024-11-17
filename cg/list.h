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
	public:
		T& AddNull()
		{
			Add({}, 1);
			return std::vector<T>::at(std::vector<T>::size() - 1);
		}
		void AddNull(const size_t count) 
		{
			Add({}, count);
		}
		void Add(const T& t, size_t count = 1)
		{
			for (size_t c = 0; c < count; c++) std::vector<T>::push_back(t);
		}
		void AddArray(const T* t, size_t size)
		{
			for (size_t p = 0; p < size; p++) std::vector<T>::push_back(t[p]);
		}
		T& InsNull(size_t p)
		{
			Ins({}, p, 1);
			return std::vector<T>::at(p);
		}
		void InsNull(size_t p, size_t count)
		{
			Ins({}, p, count);
		}
		void Ins(const T& t, size_t p, size_t count = 1)
		{
			if (p > std::vector<T>::size()) p = std::vector<T>::size();
			std::vector<T>::insert(std::vector<T>::begin() + p, count, t);
		}
		bool Del(size_t p = MAXSIZE_T)
		{
			if (std::vector<T>::size() == 0) return false;
			if (p >= std::vector<T>::size()) p = std::vector<T>::size() - 1;
			std::vector<T>::erase(std::vector<T>::begin() + p);
			return true;
		}
		bool DelBack(size_t count = 1, size_t p = MAXSIZE_T)
		{
			if (std::vector<T>::size() == 0) return false;
			if (p >= std::vector<T>::size()) p = std::vector<T>::size() - 1;
			if (count > p) count = p;
			std::vector<T>::erase(std::vector<T>::begin() + (p - count), std::vector<T>::begin() + p);
			return true;
		}
		bool DelFront(size_t count = 1, size_t p = 0)
		{
			if (std::vector<T>::size() == 0) return false;
			if (p >= std::vector<T>::size()) p = std::vector<T>::size() - 1;
			if ((p + count) >= std::vector<T>::size()) count = std::vector<T>::size() - p;
			std::vector<T>::erase(std::vector<T>::begin() + p, std::vector<T>::begin() + (p + count));
			return true;
		}
		bool Del(List<size_t>& ps)
		{
			if (std::vector<T>::size() == 0) return false;
			std::sort(ps.begin(), ps.end());
			for (size_t n = 0; n < ps.size(); n++) std::vector<T>::erase(std::vector<T>::begin() + ps[n] - n);
			return true;
		}
		bool Swp(size_t p1, size_t p2)
		{
			if (p1 >= std::vector<T>::size() || p2 >= std::vector<T>::size()) return false;
			std::swap(std::vector<T>::at(p1), std::vector<T>::at(p2));
			return true;
		}
		bool Mov(size_t p1, size_t p2)
		{
			if (p1 >= std::vector<T>::size() || p2 >= std::vector<T>::size()) return 0;
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
			for (size_t p = 0; p < std::vector<T>::size(); p++)
			{
				CallBack(std::vector<T>::at(p));
			}
		}
		void Iterate(void(&CallBack)(T&, void*), void* param)
		{
			for (size_t p = 0; p < std::vector<T>::size(); p++)
			{
				CallBack(std::vector<T>::at(p), param);
			}
		}
		/* return left < right: asc, return left > right: desc */
		void Sort(bool(&CallBack)(const T& left, const T& right))
		{
			size_t size = std::vector<T>::size();
			if (size < 2) return;
			size--;
			bool b;
			do
			{
				b = false;
				for (size_t p = 0; p < size; p++)
				{
					if (CallBack(std::vector<T>::at(p + 1), std::vector<T>::at(p)))
					{
						std::swap(std::vector<T>::at(p + 1), std::vector<T>::at(p));
						b = true;
					}
				}
			} while (b);
		}
	};
}
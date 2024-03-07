#pragma once
#include <vector>
#include "base.h"

namespace CG
{
	template<class T>
	class List : public std::vector<T>
	{
	public:
		T& Get(uint32 p = uint32Max)
		{
			if (std::vector<T>::size())
			{
				if (p >= std::vector<T>::size()) p = std::vector<T>::size() - 1;
				return std::vector<T>::operator[](p);
			}
			return std::vector<T>::operator[](0); //error
		}
		T& AddNull(const uint32 count = 1)
		{
			T t = {};
			Add(t, count);
			return Get();
		}
		void Add(const T& t, uint32 count = 1)
		{
			for (uint32 c = 0; c < count; c++) std::vector<T>::push_back(t);
		}
		void AddArray(const T* t, uint32 size)
		{
			for (uint32 p = 0; p < size; p++) std::vector<T>::push_back(t[p]);
		}
		void InsNull(uint32 p, uint32 count = 1)
		{
			T t = {};
			Ins(t, p, count);
		}
		void Ins(const T& t, uint32 p)
		{
			if (p > std::vector<T>::size()) p = std::vector<T>::size();
			std::vector<T>::insert(std::vector<T>::begin() + p, t);
		}
		void Ins(const T& t, uint32 p, uint32 count)
		{
			if (p > std::vector<T>::size()) p = std::vector<T>::size();
			std::vector<T>::insert(std::vector<T>::begin() + p, count, t);
		}
		bool Del(uint32 p = uint32Max)
		{
			if (std::vector<T>::size() == 0) return false;
			if (p >= std::vector<T>::size()) p = std::vector<T>::size() - 1;
			std::vector<T>::erase(std::vector<T>::begin() + p);
			return true;
		}
		bool DelBack(uint32 count = 1, uint32 p = uint32Max)
		{
			if (std::vector<T>::size() == 0) return false;
			if (p >= std::vector<T>::size()) p = std::vector<T>::size();
			if (count > p) count = p;
			std::vector<T>::erase(std::vector<T>::begin() + (p - count), std::vector<T>::begin() + p);
			return true;
		}
		bool DelFront(uint32 count = 1, uint32 p = 0)
		{
			if (std::vector<T>::size() == 0) return false;
			if (p >= std::vector<T>::size()) p = std::vector<T>::size();
			if ((p + count) >= std::vector<T>::size()) count = std::vector<T>::size() - p;
			std::vector<T>::erase(std::vector<T>::begin() + p, std::vector<T>::begin() + (p + count));
			return true;
		}
		bool Swp(uint32 p1, uint32 p2)
		{
			if (p1 >= std::vector<T>::size() || p2 >= std::vector<T>::size()) return false;
			std::swap(std::vector<T>::operator[](p1), std::vector<T>::operator[](p2));
			return true;
		}
		bool Mov(uint32 p1, uint32 p2)
		{
			if (p1 >= std::vector<T>::size() || p2 >= std::vector<T>::size()) return 0;
			if (p1 < p2)
			{
				for (uint32 p = p1; p < p2; p++) Swp(p, p + 1);
				return true;
			}
			else if (p1 > p2)
			{
				for (uint32 p = p1; p > p2; p--) Swp(p, p - 1);
				return true;
			}
			return false;
		}
	};
}
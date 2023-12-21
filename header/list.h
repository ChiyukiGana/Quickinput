#pragma once

#include "base.h"

namespace CG
{
	template <class ITEM>
	class List
	{
	private:

		ITEM* item = 0;
		uint32 length = 0;

	public:

		ITEM& operator[](uint32 u) { return item[u]; }

		void Add()
		{
			ITEM none;
			Add(none);
		}
		void Add(const ITEM& val)
		{
			ITEM* cache = new ITEM[length + 1];
			for (uint32 u = 0; u < length; u++) { cache[u] = item[u]; }
			if (item) { delete[] item; item = 0; }
			cache[length] = val;
			item = cache;
			length++;
		}

		bool Ins(uint32 pos)
		{
			ITEM none;
			return Ins(pos, none);
		}
		bool Ins(uint32 pos, const ITEM& val)
		{
			if (pos > length) return 0;
			ITEM* cache = new ITEM[length + 1];

			for (uint32 u = 0; u < pos && u < length; u++) cache[u] = item[u];
			cache[pos] = val;
			for (uint32 u = pos; u < length; u++) cache[u + 1] = item[u];

			if (item) { delete[] item; item = 0; }
			item = cache;
			length++;
			return 1;
		}

		bool Swp(uint32 p1, uint32 p2)
		{
			if (p1 > (length - 1) || p2 > (length - 1)) return 0;
			if (p1 == p2) return 1;

			ITEM cache;
			cache = item[p1];
			item[p1] = item[p2];
			item[p2] = cache;
			return 1;
		}

		bool Mov(uint32 before, uint32 after)
		{
			if (before > (length - 1) || after > (length - 1)) return 0;

			if (before > after)
			{
				for (uint32 u = before; u > after; u--)
				{
					Swp(u, u - 1);
				}
				return 1;
			}
			else if (before < after)
			{
				for (uint32 u = before; u < after; u++)
				{
					Swp(u, u + 1);
				}
				return 1;
			}
			return 1;
		}

		void Cop(List& list, uint32 begin = 0, uint32 end = UINT32_MAX)
		{
			Emp();
			for (; begin <= end && begin < list.len(); begin++) Add(list.item[begin]);
		}

		void Cut(List& list, uint32 begin = 0, uint32 end = UINT32_MAX)
		{
			for (; begin <= end && begin < list.len(); begin++) Add(list.item[begin]);
		}

		bool Del(uint32 pos)
		{
			if (pos > (length - 1)) return 0;

			if ((length - 1) == 0)
			{
				Emp();
			}
			else
			{
				ITEM* cache = new ITEM[length - 1];
				for (uint32 u = 0; u < length; u++)
				{
					if (u < pos) {
						cache[u] = item[u];
					}
					else if (u > pos) {
						cache[u - 1] = item[u];
					}
				}

				if (item) { delete[] item; item = 0; }
				item = cache;
				length--;
			}
			return 1;
		}

		void Emp()
		{
			if (item) { delete[] item; item = 0; }
			length = 0;
		}

		uint32 len() { return length; }
	};
}
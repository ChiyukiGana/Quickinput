#pragma once
#include <xmemory>
#include <vector>
#include <functional>
namespace QiTools
{
	template<class Ty>
	class QiVector : protected std::vector<Ty>
	{
		using base_vector = std::vector<Ty>;
	public:
		static constexpr size_t end_pos = (size_t)(~0);
		QiVector() {}
		QiVector(const QiVector&) = delete;
		QiVector(QiVector&& right) noexcept : base_vector(std::move(right)) {}
		QiVector(const Ty&& element, size_t count) : base_vector(count, element) {}
		void operator=(const QiVector&) = delete;
		void operator=(QiVector&& right) noexcept
		{
			base_vector::operator=(std::move(right));
		}
		// copy
		void copy(const QiVector& other)
		{
			base_vector::operator=(other);
		}
		void move(QiVector&& other)
		{
			base_vector::operator=(std::move(other));
		}
		// size
		using base_vector::empty;
		bool not_empty() const
		{
			return !base_vector::empty();
		}
		using base_vector::size;
		// access
		using base_vector::operator[];
		using base_vector::at;
		using base_vector::begin;
		using base_vector::end;
		// first and last element
		using base_vector::front;
		using base_vector::back;
		// append
		Ty& append(Ty&& right)
		{
			base_vector::push_back(std::move(right));
			return base_vector::back();
		}
		Ty& append_copy(const Ty& element)
		{
			base_vector::push_back(element);
			return base_vector::back();
		}
		void append_copy(const QiVector& other)
		{
			base_vector::insert(base_vector::end(), other.begin(), other.end());
		}
		void append_copy(const QiVector& other, size_t begin, size_t end)
		{
			base_vector::insert(base_vector::begin(), other.begin() + begin, other.begin() + end);
		}
		// insert
		Ty& insert(size_t where, Ty&& right)
		{
			base_vector::insert(base_vector::begin() + where, std::move(right));
			return base_vector::operator[](where);
		}
		Ty& insert_copy(size_t where, const Ty& element)
		{
			base_vector::insert(base_vector::begin() + where, element);
			return base_vector::operator[](where);
		}
		void insert_copy(size_t where, const QiVector& other)
		{
			base_vector::insert(base_vector::begin() + where, other.begin(), other.end());
		}
		void insert_copy(size_t where, const QiVector& other, size_t begin, size_t end)
		{
			base_vector::insert(base_vector::begin() + where, other.begin() + begin, other.begin() + end);
		}
		// remove
		using base_vector::clear;
		void remove(size_t where)
		{
			base_vector::erase(base_vector::begin() + where);
		}
		void remove(const std::vector<size_t>& positions)
		{
			std::vector<size_t> sortedPositions(positions);
			std::sort(sortedPositions.begin(), sortedPositions.end(), std::greater<size_t>());
			for (size_t i = 0; i < sortedPositions.size(); i++) base_vector::erase(base_vector::begin() + (sortedPositions[i]));
		}
		void remove_of_front(size_t count = 1, size_t where = 0)
		{
			if (where >= base_vector::size()) return;
			if ((where + count) >= base_vector::size()) count = base_vector::size() - where;
			base_vector::erase(base_vector::begin() + where, base_vector::begin() + (where + count));
		}
		void remove_of_back(size_t count = 1, size_t where = end_pos)
		{
			if (where > base_vector::size()) where = base_vector::size();
			if (count > where) count = where;
			base_vector::erase(base_vector::begin() + (where - count), base_vector::begin() + where);
		}
		bool remove_of_find(const Ty* elementOfSelf)
		{
			for (size_t i = 0; i < base_vector::size(); i++)
			{
				if (&base_vector::operator[](i) == elementOfSelf)
				{
					base_vector::erase(base_vector::begin() + i);
					return true;
				}
			}
			return false;
		}
		// move
		void swap(size_t element_1_where, size_t element_2_where)
		{
			Ty element(std::move(base_vector::operator[](element_1_where)));
			base_vector::operator[](element_1_where) = std::move(base_vector::operator[](element_2_where));
			base_vector::operator[](element_2_where) = std::move(element);
		}
		void move(size_t element_where, size_t target_where)
		{
			if (element_where < target_where) for (size_t where = element_where; where < target_where; where++) swap(where, where + 1);
			else if (element_where > target_where) for (size_t where = element_where; where > target_where; where--) swap(where, where - 1);
		}
		// find
		const Ty* find(std::function<bool(const Ty&)> compareCallback) const
		{
			const Ty* result = nullptr;
			for (size_t i = 0; i < base_vector::size(); i++)
			{
				if (compareCallback(base_vector::at(i)))
				{
					result = &base_vector::at(i);
					break;
				}
			}
			return result;
		}
		void sort(std::function<bool(const Ty&, const Ty&)> compareCallback)
		{
			std::sort(base_vector::begin(), base_vector::end(), compareCallback);
		}
	};
}
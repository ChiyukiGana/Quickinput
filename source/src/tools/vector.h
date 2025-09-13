#pragma once
#include <memory>
#include <xmemory>
#include <vector>
#include <algorithm>
#include <functional>
namespace QiTools
{
	using QiVectorIndex = std::vector<size_t>;
	template<class Ty>
	class QiVector : protected std::vector<Ty>
	{
		using base_vector = std::vector<Ty>;
	public:
		static constexpr size_t end_pos = (size_t)(~0);
		using typename base_vector::iterator;
		using typename base_vector::const_iterator;
		using typename base_vector::reverse_iterator;
		using typename base_vector::const_reverse_iterator;
		using base_vector::base_vector;
		using base_vector::assign;
		using base_vector::operator=;
		// size
		operator bool() const { return !base_vector::empty(); }
		using base_vector::empty;
		bool not_empty() const { return !base_vector::empty(); }
		using base_vector::size;
		using base_vector::resize;
		// access
		using base_vector::operator[];
		using base_vector::at;
		using base_vector::begin;
		using base_vector::end;
		// first and last element
		using base_vector::front;
		using base_vector::back;

		std::vector<Ty> toStdVector() const { return std::vector<Ty>(base_vector::begin(), base_vector::end()); }

		// append
		Ty& append(Ty&& right)
		{
			base_vector::push_back(std::move(right));
			return base_vector::back();
		}
		Ty& append(const Ty& element)
		{
			base_vector::push_back(element);
			return base_vector::back();
		}
		void append(const QiVector& other)
		{
			base_vector::insert(base_vector::end(), other.begin(), other.end());
		}
		void append(const QiVector& other, size_t begin, size_t end)
		{
			base_vector::insert(base_vector::begin(), other.begin() + begin, other.begin() + end);
		}
		// insert
		using base_vector::insert;
		Ty& insert(size_t where, Ty&& right)
		{
			base_vector::insert(base_vector::begin() + where, std::move(right));
			return base_vector::operator[](where);
		}
		Ty& insert(size_t where, const Ty& element)
		{
			base_vector::insert(base_vector::begin() + where, element);
			return base_vector::operator[](where);
		}
		void insert(size_t where, const QiVector& other)
		{
			base_vector::insert(base_vector::begin() + where, other.begin(), other.end());
		}
		void insert(size_t where, const QiVector& other, size_t begin, size_t end)
		{
			base_vector::insert(base_vector::begin() + where, other.begin() + begin, other.begin() + end);
		}
		// remove
		using base_vector::clear;
		void remove()
		{
			base_vector::pop_back();
		}
		void remove(size_t where)
		{
			base_vector::erase(base_vector::begin() + where);
		}
		void remove(const QiVectorIndex& positions)
		{
			QiVectorIndex sortedPositions(positions);
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
		bool remove_of_find(const Ty* elementInVector)
		{
			for (size_t i = 0; i < base_vector::size(); i++)
			{
				if (&base_vector::operator[](i) == elementInVector)
				{
					base_vector::erase(base_vector::begin() + i);
					return true;
				}
			}
			return false;
		}
		size_t remove_of_find(std::function<bool(const Ty&)> compareCallback)
		{
			if (base_vector::empty()) return 0;
			size_t count = 0;
			for (size_t i = base_vector::size() - 1; true; i--)
			{
				if (compareCallback(base_vector::operator[](i)))
				{
					base_vector::erase(base_vector::begin() + i);
					count++;
				}
				if (i == 0) break;
			}
			return count;
		}
		size_t remove_of_find(std::function<bool(const Ty&, bool& _break)> compareCallback)
		{
			if (base_vector::empty()) return 0;
			size_t count = 0;
			for (size_t i = base_vector::size() - 1; true; i--)
			{
				bool _break = false;
				if (compareCallback(base_vector::operator[](i), _break))
				{
					base_vector::erase(base_vector::begin() + i);
					count++;
				}
				if (i == 0 || _break) break;
			}
			return count;
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
		Ty* find(std::function<bool(Ty&)> compareCallback)
		{
			Ty* result = nullptr;
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

	template<class Ty>
	class QiHistoryVector : public QiVector<Ty>
	{
		using base_vector = QiVector<Ty>;
	public:
		using typename base_vector::iterator;
		using typename base_vector::const_iterator;
		using typename base_vector::reverse_iterator;
		using typename base_vector::const_reverse_iterator;
		enum class OperationType
		{
			Set,
			Append,
			Insert,
			Remove,
			RemoveRange,
			Clear,
			Resize,
			Assign,
			Swap,
			Move,
			Sort,
		};

		struct OperationRecord
		{
			OperationType type;
			size_t position;
			size_t count;
			std::vector<Ty> elements;
			std::vector<size_t> positions;
			OperationRecord(OperationType t, size_t pos = 0, size_t cnt = 0, const std::vector<Ty>& elems = {}, const std::vector<size_t>& posList = {}) : type(t), position(pos), count(cnt), elements(elems), positions(posList) {}
		};
		using OperationRecords = std::vector<OperationRecord>;

		QiHistoryVector(size_t max_history = 100) : m_max_history(max_history) {}
		QiHistoryVector(QiHistoryVector&& other, size_t max_history = 100) : m_max_history(max_history), base_vector(std::move(other)) {}
		QiHistoryVector(const QiHistoryVector& other, size_t max_history = 100) : m_max_history(max_history), base_vector(other) {}
		QiHistoryVector(base_vector&& other, size_t max_history = 100) : m_max_history(max_history), base_vector(std::move(other)) {}
		QiHistoryVector(const base_vector& other, size_t max_history = 100) : m_max_history(max_history), base_vector(other) {}
		QiHistoryVector(size_t count, size_t max_history) : m_max_history(max_history), base_vector(count) {}
		QiHistoryVector(size_t count, const Ty& init, size_t max_history = 100) : m_max_history(max_history), base_vector(count, init) {}
		QiHistoryVector(std::initializer_list<Ty> init_list, size_t max_history = 100) : m_max_history(max_history), base_vector(init_list) {}
		QiHistoryVector(const_iterator first, const_iterator last, size_t max_history = 100) : m_max_history(max_history), base_vector(first, last) {}
		QiHistoryVector& operator=(std::initializer_list<Ty> init_list) { clearHistory(); base_vector::operator=(init_list); return *this; }
		QiHistoryVector& operator=(QiHistoryVector&& other) { clearHistory(); base_vector::operator=(std::move(other)); return *this; }
		QiHistoryVector& operator=(const QiHistoryVector& other) { clearHistory(); base_vector::operator=(other); return *this; }
		QiHistoryVector& operator=(base_vector&& other) { clearHistory(); base_vector::operator=(std::move(other)); return *this; }
		QiHistoryVector& operator=(const base_vector& other) { clearHistory(); base_vector::operator=(other); return *this; }

		QiVector<Ty> toVector() const { return QiVector<Ty>(base_vector::begin(), base_vector::end()); }
		std::vector<Ty> toStdVector() const { return std::vector<Ty>(base_vector::begin(), base_vector::end()); }

		void setMaxHistory(size_t max_history)
		{
			m_max_history = max_history;
			if (undoStack.size() > m_max_history) undoStack.erase(undoStack.begin(), undoStack.begin() + (undoStack.size() - m_max_history));
		}
		void clearHistory()
		{
			undoStack.clear();
			redoStack.clear();
		}

		const OperationRecords& undos() const { return undoStack; }
		const OperationRecords& redos() const { return redoStack; }

		size_t historyCount() const { return undoStack.size(); }
		size_t redoCount() const { return redoStack.size(); }
		bool canUndo() const { return !undoStack.empty(); }
		bool canRedo() const { return !redoStack.empty(); }

		bool undo()
		{
			if (undoStack.empty()) return false;
			OperationRecord record = undoStack.back();
			undoStack.pop_back();
			redoStack.push_back(record);

			switch (record.type)
			{
			case OperationType::Set:
				if (record.elements.size() >= 2 && record.position < this->size())
				{
					Ty currentValue = std::move(base_vector::operator[](record.position));
					base_vector::operator[](record.position) = std::move(record.elements[0]);
					record.elements[0] = std::move(currentValue);
				}
				break;
			case OperationType::Append:
				if (record.count > 0) {
					base_vector::remove_of_back(record.count);
				}
				break;
			case OperationType::Insert:
				if (record.count > 0 && record.position < this->size()) {
					base_vector::remove_of_front(record.count, record.position);
				}
				break;
			case OperationType::Remove:
				if (!record.elements.empty() && record.position <= this->size()) {
					base_vector::insert(record.position, record.elements[0]);
				}
				break;
			case OperationType::RemoveRange:
				if (!record.elements.empty() && record.position <= this->size()) {
					base_vector::insert(base_vector::begin() + record.position, record.elements.begin(), record.elements.end());
				}
				break;
			case OperationType::Clear:
				for (const auto& elem : record.elements) base_vector::append(elem);
				break;
			case OperationType::Resize:
				this->resize(record.count);
				break;
			case OperationType::Assign:
				this->assign(record.elements.begin(), record.elements.end());
				break;
			case OperationType::Swap:
				if (record.positions.size() == 2) base_vector::swap(record.positions[1], record.positions[0]);
				break;
			case OperationType::Move:
				if (record.positions.size() == 2) base_vector::move(record.positions[1], record.positions[0]);
				break;
			case OperationType::Sort:
				this->assign(record.elements.begin(), record.elements.end());
				break;
			}
			return true;
		}
		bool redo()
		{
			if (redoStack.empty()) return false;
			OperationRecord record = redoStack.back();
			redoStack.pop_back();
			undoStack.push_back(record);

			switch (record.type)
			{
			case OperationType::Set:
				if (record.elements.size() >= 2 && record.position < this->size())
				{
					Ty currentValue = std::move(base_vector::operator[](record.position));
					base_vector::operator[](record.position) = std::move(record.elements[0]);
					record.elements[0] = std::move(currentValue);
				}
				break;
			case OperationType::Append:
				if (!record.elements.empty()) {
					base_vector::insert(base_vector::begin() + this->size(), record.elements.begin(), record.elements.end());
				}
				break;
			case OperationType::Insert:
				if (!record.elements.empty() && record.position <= this->size()) {
					base_vector::insert(base_vector::begin() + record.position, record.elements.begin(), record.elements.end());
				}
				break;
			case OperationType::Remove:
				if (record.position < this->size()) {
					base_vector::remove(record.position);
				}
				break;
			case OperationType::RemoveRange:
				if (record.position < this->size()) {
					base_vector::remove_of_front(record.count, record.position);
				}
				break;
			case OperationType::Clear:
				base_vector::clear();
				break;
			case OperationType::Resize:
				this->resize(record.count);
				break;
			case OperationType::Assign:
				this->assign(record.elements.begin(), record.elements.end());
				break;
			case OperationType::Swap:
				if (record.positions.size() == 2) base_vector::swap(record.positions[0], record.positions[1]);
				break;
			case OperationType::Move:
				if (record.positions.size() == 2) base_vector::move(record.positions[0], record.positions[1]);
				break;
			case OperationType::Sort:
				this->assign(record.elements.begin(), record.elements.end());
				break;
			}
			return true;
		}

		void set(size_t where, const Ty& value)
		{
			if (where < this->size())
			{
				Ty oldValue = this->at(where);
				base_vector::operator[](where) = value;
				pushUndoRecord(OperationRecord(OperationType::Set, where, 1, { oldValue, value }));
			}
		}
		void set(size_t where, Ty&& value)
		{
			if (where < this->size())
			{
				Ty oldValue = this->at(where);
				base_vector::operator[](where) = std::move(value);
				pushUndoRecord(OperationRecord(OperationType::Set, where, 1, { oldValue, base_vector::operator[](where) }));
			}
		}

		Ty& append(Ty&& right)
		{
			pushUndoRecord(OperationRecord(OperationType::Append, this->size(), 1, { right }));
			return base_vector::append(std::move(right));
		}
		Ty& append(const Ty& element)
		{
			pushUndoRecord(OperationRecord(OperationType::Append, this->size(), 1, { element }));
			return base_vector::append(element);
		}
		void append(const base_vector& other)
		{
			std::vector<Ty> elements(other.begin(), other.end());
			pushUndoRecord(OperationRecord(OperationType::Append, this->size(), elements.size(), elements));
			base_vector::append(other);
		}
		void append(const base_vector& other, size_t begin, size_t end)
		{
			std::vector<Ty> elements(other.begin() + begin, other.begin() + end);
			pushUndoRecord(OperationRecord(OperationType::Append, this->size(), elements.size(), elements));
			base_vector::append(other, begin, end);
		}

		Ty& insert(size_t where, Ty&& right)
		{
			pushUndoRecord(OperationRecord(OperationType::Insert, where, 1, { right }));
			return base_vector::insert(where, std::move(right));
		}
		Ty& insert(size_t where, const Ty& element)
		{
			pushUndoRecord(OperationRecord(OperationType::Insert, where, 1, { element }));
			return base_vector::insert(where, element);
		}
		void insert(size_t where, const base_vector& other)
		{
			std::vector<Ty> elements(other.begin(), other.end());
			pushUndoRecord(OperationRecord(OperationType::Insert, where, elements.size(), elements));
			base_vector::insert(where, other);
		}
		void insert(size_t where, const base_vector& other, size_t begin, size_t end)
		{
			std::vector<Ty> elements(other.begin() + begin, other.begin() + end);
			pushUndoRecord(OperationRecord(OperationType::Insert, where, elements.size(), elements));
			base_vector::insert(where, other, begin, end);
		}

		void remove(size_t where)
		{
			if (where < this->size())
			{
				Ty element = this->at(where);
				pushUndoRecord(OperationRecord(OperationType::Remove, where, 1, { element }));
				base_vector::remove(where);
			}
		}
		void remove(const QiVectorIndex& positions)
		{
			QiVectorIndex sortedPositions(positions);
			std::sort(sortedPositions.begin(), sortedPositions.end(), std::greater<size_t>());
			std::vector<Ty> elements;
			for (auto pos : sortedPositions) if (pos < this->size()) elements.push_back(this->at(pos));
			pushUndoRecord(OperationRecord(OperationType::RemoveRange, 0, sortedPositions.size(), elements, sortedPositions));
			base_vector::remove(positions);
		}
		void remove_of_front(size_t count = 1, size_t where = 0)
		{
			if (where < this->size())
			{
				size_t actualCount = (std::min)(count, this->size() - where);
				std::vector<Ty> elements;
				for (size_t i = 0; i < actualCount; ++i) elements.push_back(this->at(where + i));
				pushUndoRecord(OperationRecord(OperationType::RemoveRange, where, actualCount, elements));
				base_vector::remove_of_front(count, where);
			}
		}
		void remove_of_back(size_t count = 1, size_t where = base_vector::end_pos)
		{
			if (where == base_vector::end_pos) where = this->size();
			if (count > where) count = where;
			size_t start = where - count;
			std::vector<Ty> elements;
			for (size_t i = 0; i < count; ++i) elements.push_back(this->at(start + i));
			pushUndoRecord(OperationRecord(OperationType::RemoveRange, start, count, elements));
			base_vector::remove_of_back(count, where);
		}
		size_t remove_of_find(std::function<bool(const Ty&)> compareCallback)
		{
			std::vector<Ty> elements;
			QiVectorIndex positions;
			for (size_t i = this->size() - 1; i != SIZE_MAX; --i)
			{
				if (compareCallback(this->at(i)))
				{
					elements.push_back(this->at(i));
					positions.push_back(i);
				}
			}
			if (!elements.empty())
			{
				pushUndoRecord(OperationRecord(OperationType::RemoveRange, 0, elements.size(), elements, positions));
				return base_vector::remove_of_find(compareCallback);
			}
			return 0;
		}
		size_t remove_of_find(std::function<bool(const Ty&, bool&)> compareCallback)
		{
			std::vector<Ty> elements;
			QiVectorIndex positions;
			for (size_t i = this->size() - 1; i != SIZE_MAX; --i)
			{
				bool breakFlag = false;
				if (compareCallback(this->at(i), breakFlag))
				{
					elements.push_back(this->at(i));
					positions.push_back(i);
				}
				if (breakFlag) break;
			}
			if (!elements.empty())
			{
				pushUndoRecord(OperationRecord(OperationType::RemoveRange, 0, elements.size(), elements, positions));
				return base_vector::remove_of_find(compareCallback);
			}
			return 0;
		}

		void clear()
		{
			std::vector<Ty> elements(this->begin(), this->end());
			pushUndoRecord(OperationRecord(OperationType::Clear, 0, this->size(), elements));
			base_vector::clear();
		}
		void resize(size_t newSize)
		{
			std::vector<Ty> elements(this->begin(), this->end());
			pushUndoRecord(OperationRecord(OperationType::Resize, 0, this->size(), elements));
			base_vector::resize(newSize);
		}
		void assign(size_t count, const Ty& value)
		{
			std::vector<Ty> elements(this->begin(), this->end());
			pushUndoRecord(OperationRecord(OperationType::Assign, 0, this->size(), elements));
			base_vector::assign(count, value);
		}
		void assign(const_iterator first, const_iterator last)
		{
			std::vector<Ty> elements(this->begin(), this->end());
			pushUndoRecord(OperationRecord(OperationType::Assign, 0, this->size(), elements));
			base_vector::assign(first, last);
		}

		void swap(size_t a, size_t b)
		{
			pushUndoRecord(OperationRecord(OperationType::Swap, 0, 0, {}, { a, b }));
			base_vector::swap(a, b);
		}
		void move(size_t from, size_t to)
		{
			pushUndoRecord(OperationRecord(OperationType::Move, 0, 0, {}, { from, to }));
			base_vector::move(from, to);
		}
		void sort(std::function<bool(const Ty&, const Ty&)> compareCallback)
		{
			std::vector<Ty> elements(this->begin(), this->end());
			pushUndoRecord(OperationRecord(OperationType::Sort, 0, this->size(), elements));
			base_vector::sort(compareCallback);
		}
	private:
		OperationRecords undoStack;
		OperationRecords redoStack;
		size_t m_max_history;
		void pushUndoRecord(const OperationRecord& record)
		{
			undoStack.push_back(record);
			if (undoStack.size() > m_max_history) undoStack.erase(undoStack.begin());
			redoStack.clear();
		}
	};
}
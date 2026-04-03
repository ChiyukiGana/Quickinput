#pragma once
#include <string>
#include <mutex>
#include <map>

class QiMutex : protected std::map<std::string, std::shared_ptr<std::mutex>>
{
	using base = std::map<std::string, std::shared_ptr<std::mutex>>;
	std::mutex m_mutex;

	std::shared_ptr<std::mutex>& operator[](const base::key_type& key)
	{
		std::shared_ptr<std::mutex>& mutex = base::operator[](key);
		if (!mutex) mutex = std::make_shared<std::mutex>();
		return mutex;
	}
public:
	void lock(const base::key_type& key)
	{
		m_mutex.lock();
		std::shared_ptr<std::mutex> m = operator[](key);
		m_mutex.unlock();
		m->lock();
	}
	bool try_lock(const base::key_type& key)
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		return operator[](key)->try_lock();
	}
	bool is_lock(const base::key_type& key)
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		auto& m = operator[](key);
		if (m->try_lock())
		{
			m->unlock();
			return false;
		}
		return true;
	}
	void unlock(const base::key_type& key)
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		auto& m = operator[](key);
		m->try_lock();
		m->unlock();
	}
	void clean_up()
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		for (auto i = begin(); i != end();)
		{
			if (!i->second->try_lock()) i->second->unlock();
			if (i->second.use_count() < 2) i = erase(i);
			else i++;
		}
	}
};
#pragma once
#include <mutex>
#include <atomic>
#include <thread>
#include <vector>
#include <condition_variable>

struct QiWorker
{
	std::mutex m_mutex;
	std::atomic_bool m_stop;
	QiWorker() noexcept : m_stop(false) {}
	void sleep(clock_t ms)
	{
		clock_t end = clock() + ms;
		if (ms > 5)
		{
			while (!m_stop && (clock() < end)) std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		else
		{
			while (!m_stop && (clock() < end)) std::this_thread::yield();;
		}
	}
	virtual void run() = 0;
};

template<typename... Args>
struct QiWorkerWithArgs : public QiWorker
{
	std::tuple<Args...> m_args;
	QiWorkerWithArgs(Args&&... args) noexcept : m_args(std::forward<Args>(args)...) {}
	void run() override
	{
		runImpl(std::index_sequence_for<Args...>{});
	}
	template<std::size_t... Is>
	void runImpl(std::index_sequence<Is...>)
	{
		run(std::get<Is>(m_args)...);
	}
	virtual void run(Args... args) = 0;
};

class QiThreadManager
{
protected:
	std::vector<std::shared_ptr<QiWorker>> m_worker_list;
	std::mutex m_mutex;

public:
	QiThreadManager() noexcept {};
	QiThreadManager(QiThreadManager&&) noexcept { stop(); }
	QiThreadManager(const QiThreadManager&) noexcept { stop(); }
	QiThreadManager& operator=(QiThreadManager&&) noexcept { stop(); return *this; }
	QiThreadManager& operator=(const QiThreadManager&) noexcept { stop(); return *this; }

	void lock() { m_mutex.lock(); }
	void unlock() { m_mutex.unlock(); }

	bool active()
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		if (m_worker_list.empty()) return false;
		return !m_worker_list.back()->m_stop;
	}

	template<class QiWorkers, class ...Args>
	void start(Args&&... args)
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		for (auto& i : m_worker_list) i->m_stop = true;

		auto& worker = m_worker_list.emplace_back(new QiWorkers(std::forward<Args>(args)...));
		std::thread([worker_refcopy = worker] { worker_refcopy->run(); worker_refcopy->m_stop = true; }).detach();

		bool exist = false;
		do {
			exist = false;
			for (auto i = m_worker_list.begin(); i != m_worker_list.end(); i++)
			{
				if ((*i)->m_stop)
				{
					m_worker_list.erase(i);
					exist = true;
					break;
				}
			}
		} while (exist);
	}

	void stop()
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		for (auto& i : m_worker_list) i->m_stop = true;
	}

	const QiWorker* worker_last() const
	{
		if (m_worker_list.empty()) return nullptr;
		return m_worker_list.back().get();
	}

	~QiThreadManager()
	{
		stop();
	}
};

class QiMacroThread : public QiThreadManager
{
	void start(struct Macro* self, bool running);
public:
	void run_start(struct Macro* self);
	void end_start(struct Macro* self);
	bool run_active();
	bool end_active();
	using QiThreadManager::active;
};

class QiQuickClickThread : public QiThreadManager
{
public:
	using QiThreadManager::QiThreadManager;
	void start();
};

class QiWindowBindThread : public QiThreadManager
{
public:
	using QiThreadManager::QiThreadManager;
	void start();
};

class QiDisplayUpdateThread : public QiThreadManager
{
public:
	using QiThreadManager::QiThreadManager;
	void start();
};
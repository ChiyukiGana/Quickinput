#pragma once
#include <mutex>
#include <atomic>
#include <thread>
#include <vector>
#include <condition_variable>

struct QiWorker
{
	std::atomic_bool m_stop;
	QiWorker() : m_stop(false) {}
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

	QiWorkerWithArgs(Args&&... args) : m_args(std::forward<Args>(args)...) {}

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

struct QiWorkerThread
{
	std::thread m_thread;
	std::unique_ptr<QiWorker> m_worker;
};

class QiThreadManager
{
	std::vector<std::unique_ptr<QiWorkerThread>> m_thread_list;
	std::mutex m_mutex;

public:
	QiThreadManager() {};
	QiThreadManager(QiThreadManager&& right)
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_thread_list = std::move(right.m_thread_list);
	}
	QiThreadManager(const QiThreadManager&) = delete;
	QiThreadManager& operator=(QiThreadManager&& right)
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_thread_list = std::move(right.m_thread_list);
	}
	QiThreadManager& operator=(const QiThreadManager&) = delete;

	bool active()
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		if (m_thread_list.empty()) return false;
		return !m_thread_list.back()->m_worker->m_stop;
	}

	template<class QiWorkers, class ...Args>
	void start(Args&&... args)
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		if (!m_thread_list.empty()) m_thread_list.back()->m_worker->m_stop = true;

		std::unique_ptr<QiWorkerThread>& worker_thread = m_thread_list.emplace_back(new QiWorkerThread());
		worker_thread->m_worker = std::make_unique<QiWorkers>(std::forward<Args>(args)...);
		worker_thread->m_thread = std::thread([worker = worker_thread->m_worker.get()] {
			worker->run();
			worker->m_stop = true;
			});

		for (size_t i = m_thread_list.size() - 1; true; i--)
		{
			if (!m_thread_list[i]->m_thread.joinable()) m_thread_list.erase(m_thread_list.begin() + i);
			if (i == 0) break;
		}
	}

	void wait()
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		if (!m_thread_list.empty())
		{
			std::unique_ptr<QiWorkerThread>& last = m_thread_list.back();
			if (last->m_thread.joinable()) last->m_thread.join();
		}
	}

	void wait_all()
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		for (const auto& t : m_thread_list)
		{
			if (t->m_thread.joinable()) t->m_thread.join();
		}
	}

	void exit()
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		if (!m_thread_list.empty()) m_thread_list.back()->m_worker->m_stop = true;
	}

	void exit_all()
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		for (const auto& t : m_thread_list)
		{
			t->m_worker->m_stop = true;
		}
	}

	~QiThreadManager()
	{
		exit_all();
		wait_all();
	}
};


class QiMacroThread : public QiThreadManager
{
	bool m_running = false;
public:
	void start(struct Macro* self, bool running);
	bool active(bool runing);
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
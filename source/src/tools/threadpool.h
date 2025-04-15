#pragma once
#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <future>
#include <mutex>
#include <condition_variable>
class ThreadPool
{
	std::vector<std::thread> threads;
	std::queue<std::function<void()>> queue;
	std::mutex mutex;
	std::condition_variable condition;
	bool stop;
public:
	ThreadPool(size_t count) : stop(false)
	{
		for (size_t i = 0; i < count; i++)
		{
			threads.emplace_back(
				[this]
				{
					while (true)
					{
						std::function<void()> task;
						{
							std::unique_lock<std::mutex> lock(this->mutex);
							this->condition.wait(lock, [this] { return this->stop || !this->queue.empty(); });
							if (this->stop && this->queue.empty()) return;
							task = std::move(this->queue.front());
							this->queue.pop();
						}
						task();
					}
				}
			);
		}
	}
	~ThreadPool()
	{
		{
			std::unique_lock<std::mutex> lock(mutex);
			stop = true;
		}
		condition.notify_all();
		for (size_t i = 0; i < threads.size(); i++) threads[i].join();
	}
	template<class Fn, class... Args>
	std::future<std::invoke_result_t<Fn, Args...>> enqueue(Fn&& fn, Args&&... args)
	{
		if (stop) throw std::runtime_error("ThreadPool::enqueue: this thread-pool is stopped");
		auto task = std::make_shared<std::packaged_task<std::invoke_result_t<Fn, Args...>()>>(std::bind(std::forward<Fn>(fn), std::forward<Args>(args)...));
		std::future<std::invoke_result_t<Fn, Args...>> result = task->get_future();
		{
			std::unique_lock<std::mutex> lock(mutex);
			queue.emplace([task] { (*task)(); });
		}
		condition.notify_one();
		return result;
	}
	bool stopped() const
	{
		return stop;
	}
};
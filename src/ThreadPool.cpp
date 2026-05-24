#include "ThreadPool.hpp"
#include <cstdint>
#include <functional>
#include <mutex>

void ThreadPool::Start() {
	const uint32_t num_threads = std::thread::hardware_concurrency();
	for (uint32_t i = 0; i < num_threads; i++) {
		threads.emplace_back(std::thread(&ThreadPool::ThreadLoop, this));
	}
}

// stack overflow: "the scope blocks are used so that when they are exited, the 
// std::unique_lock variables created within go out of scope, unlocking the mutex"

void ThreadPool::ThreadLoop() {
	while (true) {
		std::function<void()> job;
		{
			std::unique_lock<std::mutex> lock(queue_mutex);
			mutex_condition.wait(lock, [this] {
				return !jobs.empty() || should_terminate;
			});
			if (should_terminate) {
				return;
			}
			job = jobs.front();
			jobs.pop();
			active_jobs++;
		}
		job();

		{
			std::unique_lock<std::mutex> lock(queue_mutex);
			active_jobs--;
			if (jobs.empty() && active_jobs == 0) {
				finished_condition.notify_all();
			}
		}
	}
}

void ThreadPool::QueueJob(const std::function<void()>& job) {
	{
		std::unique_lock<std::mutex> lock(queue_mutex);
		jobs.push(job);
	}
	mutex_condition.notify_one();
}

bool ThreadPool::busy() {
	bool poolbusy;
	{
		std::unique_lock<std::mutex> lock(queue_mutex);
		poolbusy = !jobs.empty();
	}
	return poolbusy;
}

void ThreadPool::Stop() {
	{
		std::unique_lock<std::mutex> lock(queue_mutex);
		should_terminate = true;
	}
	mutex_condition.notify_all();
	for (std::thread& active_thread : threads) {
		active_thread.join();
	}
	threads.clear();
}

void ThreadPool::WaitAll() {
	std::unique_lock<std::mutex> lock(queue_mutex);

	finished_condition.wait(lock, [this] {
		return jobs.empty() && active_jobs == 0;
	});
}

// https://stackoverflow.com/questions/15752659/thread-pooling-in-c11
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
public:
	void Start();
	void QueueJob(const std::function<void()>& job);
	void Stop();
	bool busy();
	void WaitAll();
private:
	void ThreadLoop();
	bool should_terminate = false;           // Tells threads to stop looking for jobs
	std::mutex queue_mutex;                  // Prevents data races to the job queue
	std::condition_variable mutex_condition; // Allows threads to wait on new jobs or termination 
	std::vector<std::thread> threads;
	std::queue<std::function<void()>> jobs;

	// for joining
	std::condition_variable finished_condition;
	std::atomic<uint32_t> active_jobs = 0;
};

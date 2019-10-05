#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma once
#include <iostream>
#include <thread>
#include <WinSock2.h>
#include "db.h"
#include "sock.h"






//DB* database;







//thread_pool pool(30);


/*
class thread_pool {
private:
	size_t num_thread;
	std::vector<std::thread> thread_list;
	std::queue<std::function<void()>> job_queue;
	std::mutex mtx_job_queue;
	std::condition_variable cv_job_queue;
	bool stop_all;
public:
	thread_pool(size_t num_threads):num_thread(num_threads), stop_all(false) {
		thread_list.reserve(num_thread);
		//thread_list.assign([this]() {new_thread();  }, num_thread);
		for(size_t i=0;i<num_thread;i++)
			thread_list.emplace_back([this]() {this->new_thread(); });
	}
	~thread_pool() {
		stop_all = true;
		cv_job_queue.notify_all();
		for (auto& tmp : thread_list) tmp.join();
	}
	
	void new_thread() {
		while (1) {
			std::unique_lock<std::mutex>lock(mtx_job_queue);
			cv_job_queue.wait(lock, [this]() {return !this->job_queue.empty() || stop_all; });
			if (stop_all&&this->job_queue.empty()) return;

			std::function<void()> job = std::move(job_queue.front());
			job_queue.pop();
			lock.unlock();

			// 해당 job 을 수행한다 :)
			job();
		}
	}
	template <class F, class... Args>
	std::future <typename std::result_of<F(Args...)>::type> enqueue_job(bool w, F&& f, Args&&... args) {
		if (stop_all) {
			throw std::runtime_error("스레드풀 사용중지됨");
		}
		using return_type = typename std::result_of<F(args...)>::type;
		auto job = std::make_shared<std::packaged_task<return_type() >> (
			std::bind(std::forward<F>(f), std::forward<Args>(args)...));
		std::future<return_type> job_result_future = job->get_future();
		

		std::lock_guard<std::mutex> lock(mtx_job_queue);
			job_queue.push([job]() {(*job)(); });
		cv_job_queue.notify_one();
		if (w) {
			
			return job_result_future;
		}
			
			
	}
};*/

//class Semaphore {
//private:
//	std::mutex mtx;
//	std::condition_variable cond;
//	int sem;
//public:
//	Semaphore(int num = 0) : sem(num) {}
//	void notify() {
//		std::unique_lock<std::mutex> lock(mtx);
//		sem++;
//		cond.notify_one();
//	}
//	void wait() {
//		std::unique_lock<std::mutex> lock(mtx);
//		while (sem == 0) {
//			cond.wait(lock);
//		}
//		sem--;
//	}
//};
/*
inline void showError(const char * msg) {
	std::cout << "에러 : " << msg << std::endl;
	exit(-1);
}*/

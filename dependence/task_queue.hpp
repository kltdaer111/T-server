#ifndef TASK_QUEUE_H
#define TASK_QUEUE_H

#include <deque>
#include <mutex>
#include <condition_variable>
#include <utility>
#include <boost/asio.hpp>

template <typename T>
class TaskQueue
{
public:
	virtual T get() = 0;
	virtual void push(T task) = 0;
};

template <typename T>
class BlockingQueue : public TaskQueue<T>
{
private:
	std::deque<T> event_queue;
	std::condition_variable empty;
	std::mutex mtx;
public:
	T get(){	
		std::unique_lock<std::mutex> lock(mtx);
		if(event_queue.empty())
			empty.wait(lock);
		T tmp = std::move(event_queue.front());
		event_queue.pop_front();
		return tmp;
	}
	void push(T task){
		std::unique_lock<std::mutex> lock(mtx);
		event_queue.push_back(task);
		if(event_queue.size() == 1)
			empty.notify_one();
	}
};

template <typename T>
class NonBlockingQueue : public TaskQueue<T>
{
private:
	std::deque<T> event_queue;
public:
	T get() {
		T tmp = std::move(event_queue.front());
		event_queue.pop_front();
		return tmp;
	}
	void push(T task) {
		event_queue_ptr_->push_back(task);
	}
};
#endif

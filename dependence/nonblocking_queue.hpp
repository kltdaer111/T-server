#ifndef TASK_QUEUE_H
#define TASK_QUEUE_H

#include <deque>
#include <mutex>
#include <utility>
#include <boost/asio.hpp>
#include <condition_variable>

template <typename T>
class NonBlockingQueue
{
private:
	std::shared_ptr<std::deque<T> > event_queue_ptr_;
	std::mutex mtx;
	bool pause_pushing_;
public:
	NonBlockingQueue(){
		event_queue_ptr_.reset(new std::deque<T>);
	}
	std::shared_ptr<std::deque<T> > get_and_flush(){
		std::unique_lock<std::mutex> lock(mtx);
		std::shared_ptr<std::deque<T> > tmp = event_queue_ptr_;
		event_queue_ptr_.reset(new std::deque<T>);
		return tmp;
	}
	void push(T task){
		std::unique_lock<std::mutex> lock(mtx);
		event_queue_ptr_->push_back(task);
	}
};

#endif

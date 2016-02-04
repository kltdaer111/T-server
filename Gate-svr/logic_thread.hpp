#ifndef LOGIC_THREAD
#define LOGIC_THREAD

#include "gate_logic.h"
#include <thread>

class Logic_Thread : public std::enable_shared_from_this<Logic_Thread>
{
public:
	typedef std::shared_ptr<GateLogic> logic_ptr;
	typedef std::shared_ptr<BlockingQueue<std::shared_ptr<TaskData> > > queue_pointer;
	typedef std::shared_ptr<std::thread> thread_ptr;

	Logic_Thread(logic_ptr lp, queue_pointer qp):logic_ptr_(lp), queue_pointer_(qp){}
	void loop(){
		while(true){
			auto taskdata = queue_pointer_->get();
			logic_ptr_->parseBody(taskdata->message_id, taskdata->buf_ptr_, taskdata->connection_id, taskdata->char_id);
		}
	}
	void run(){
		t_.reset(new std::thread(&Logic_Thread::loop, shared_from_this()));
	}
	void join(){
		t_->join();
	}
private:
	logic_ptr logic_ptr_;
	queue_pointer queue_pointer_;
	thread_ptr t_;
};

#endif

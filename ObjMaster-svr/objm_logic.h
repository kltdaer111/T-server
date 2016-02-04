#ifndef OBJM_LOGIC
#define OBJM_LOGIC

#include "../dependence/common_struct.h"
#include "../dependence/nonblocking_queue.hpp"

class ObjM_Logic
{
public:
	typedef NonBlockingQueue<SendMessage> send_queue;
	typedef std::shared_ptr<send_queue> send_queue_ptr;
private:
	send_queue_ptr send_queue_ptr_;

};

#endif

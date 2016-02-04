#ifndef GATE_SERVER
#define GATE_SERVER

#include "gate_logic.h"
#include "logic_thread.hpp"
#include "../network_module/tcp_server.hpp"
#include <boost/thread.hpp>
#include <thread>
#include <boost/bind.hpp>

#define MAX_ID 99999999

class Gate_Server
{
public:
	typedef std::shared_ptr<Tcp_Connection> pointer;
	typedef std::shared_ptr<Auto_ID_Manager<pointer> > manager_pointer;
	typedef std::shared_ptr<Tcp_Server> tcp_server_pointer;
	typedef std::shared_ptr<GateLogic> logic_ptr;
	typedef std::shared_ptr<BlockingQueue<std::shared_ptr<TaskData> > > queue_pointer;
	typedef std::shared_ptr<Logic_Thread> logic_thread_ptr;
	typedef std::shared_ptr<std::thread> thread_ptr;
	typedef std::shared_ptr<boost::thread> bthread_ptr;
	typedef std::shared_ptr<boost::asio::io_service::work> work_ptr;

	Gate_Server(std::vector<short> port_list, int thread_num):port_list_(port_list){
		logic_ptr_.reset(new GateLogic);
		queue_ptr_.reset(new BlockingQueue<std::shared_ptr<TaskData> >);
		manager_pointer_.reset(new Auto_ID_Manager<pointer>(MAX_ID));
		for(int i = 0; i < (int)port_list.size(); ++i){
			io_service_list_.push_back(new boost::asio::io_service);
			tcp_server_list_.push_back(tcp_server_pointer(new Tcp_Server(*(io_service_list_.back()), queue_ptr_, manager_pointer_, port_list[i])));
		}
		for(int i = 0; i < thread_num; ++i){
			logic_thread_list_.push_back(logic_thread_ptr(new Logic_Thread(logic_ptr_, queue_ptr_)));		
		}
	}
	void run(){
		for(int i = 0; i < (int)tcp_server_list_.size(); ++i){
			tcp_server_list_[i]->start_accept();	
		}
		for(int i = 0; i < (int)io_service_list_.size(); ++i){
			work_ptr w_p_(new boost::asio::io_service::work(*(io_service_list_[i])));
			work_list_.push_back(w_p_);
			bthread_ptr t_p_(new boost::thread(boost::bind(&boost::asio::io_service::run, io_service_list_[i])));
			bthread_list_.push_back(t_p_);
		}
		for(int i = 0; i < (int)logic_thread_list_.size(); ++i){
			logic_thread_list_[i]->run();
		}
	}
	void join(){
		for(int i = 0; i < (int)logic_thread_list_.size(); ++i){
			logic_thread_list_[i]->join();
		}
		for(int i = 0; i < (int)bthread_list_.size(); ++i){
			bthread_list_[i]->join();
		}
	}
private:
	manager_pointer manager_pointer_;
	logic_ptr logic_ptr_;
	queue_pointer queue_ptr_;
	std::vector<boost::asio::io_service*> io_service_list_;
	std::vector<short> port_list_;
	std::vector<tcp_server_pointer> tcp_server_list_;
	std::vector<logic_thread_ptr> logic_thread_list_;
	std::vector<std::shared_ptr<boost::thread> > bthread_list_;
	std::vector<std::shared_ptr<boost::asio::io_service::work> > work_list_;
};

#endif

#ifndef TCP_CONNECTION
#define TCP_CONNECTION

#include <boost/asio.hpp>
#include <functional>
#include "../dependence/task_queue.hpp"
#include "../format/message_header.h"
#include "../dependence/auto_id_manager.hpp"
#include "../format/gate_related.pb.h"
#include "../format/message_id.h"
#include "../dependence/buffer.hpp"
#include <cstring>
#include "../dependence/common_struct.h"
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <string>

#define HEADER_LENGTH 64

class Gate_Tcp_Connection : public std::enable_shared_from_this<Gate_Tcp_Connection>
{
public:
	typedef boost::asio::ip::tcp tcp;
	typedef std::shared_ptr<Gate_Tcp_Connection> pointer;
	typedef std::shared_ptr<Auto_ID_Manager<pointer> > manager_pointer;
	typedef std::shared_ptr<EasyBuffer> buf_ptr;

	Gate_Tcp_Connection(boost::asio::io_service& io_service, manager_pointer& m_p, 
		std::map<int, int>* forward_rules, int* obj_id, boost::function<void(int, std::string)> connecting_func):
		socket_(io_service), manager_pointer_(m_p), forward_rules_(forward_rules), 
		p_obj_mgr_id_(obj_id), id_(0), connecting_objmgr_func_(connecting_func){}
	~Gate_Tcp_Connection(){}
	static pointer create(boost::asio::io_service& io_service, manager_pointer& m_p, 
		std::map<int, int>* forward_rules, int* obj_id, boost::function<void(int, std::string)> connecting_func){
		return pointer(new Gate_Tcp_Connection(io_service, m_p, forward_rules, obj_id, connecting_func));
	}
	tcp::socket& socket(){
		return socket_;
	}
	int get_id(){
		return id_;
	}
	void set_id(int id) {
		id_ = id;
	}
	void start(){ 
		do_read_header();
	}
	Header decode_header(){ 
		return *(Header*)header_data_;
	}
	void do_read_header(){
		auto self(shared_from_this());
		boost::asio::async_read(socket_, boost::asio::buffer(header_data_, HEADER_LENGTH),
				[this, self](boost::system::error_code ec, std::size_t){
					if(!ec){
							do_read_body(decode_header());
					}
				}
		);
	}
	void do_read_body(Header header){
		auto self(shared_from_this());
		buf_ptr buf_ptr_(new EasyBuffer);
		buf_ptr_->prepare(sizeof(Header) + header.body_size);
		memcpy(buf_ptr_->get_buffer(), &header, sizeof(Header));
		boost::asio::async_read(socket_, boost::asio::buffer(buf_ptr_->get_buffer(sizeof(Header)), header.body_size),
				[this, self, &buf_ptr_](boost::system::error_code ec, std::size_t){
					if(!ec){
						handle_msg(buf_ptr_);
						do_read_header();
					}
				}
		);
	}
	void do_write(buf_ptr buf){
		boost::asio::async_write(socket_, boost::asio::buffer(buf->get_buffer(), buf->get_size()),[&buf](const boost::system::error_code&, std::size_t){});
	}
	void handle_msg(buf_ptr& buf);
	boost::asio::io_service& get_io_service(){
		return socket_.get_io_service();
	}
private:
	tcp::socket socket_;
	char header_data_[HEADER_LENGTH];
	int id_;
	int* const p_obj_mgr_id_;
	manager_pointer manager_pointer_;
	std::map<int, int>* forward_rules_;
	boost::function<void(int, std::string)> connecting_objmgr_func_;
};

#endif













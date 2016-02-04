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

#define HEADER_LENGTH 64
#define MAX_BODY_LENGTH 2048

class Tcp_Connection : public std::enable_shared_from_this<Tcp_Connection>
{
public:
	typedef boost::asio::ip::tcp tcp;
	typedef std::shared_ptr<Tcp_Connection> pointer;
	typedef std::shared_ptr<TaskQueue<std::shared_ptr<TaskData> > > queue_pointer;
	typedef std::shared_ptr<EasyBuffer> buf_ptr;

	Tcp_Connection(boost::asio::io_service& io_service, queue_pointer task_queue):
				   socket_(io_service), task_queue_(task_queue), id_(0){}
	~Tcp_Connection(){}
	static pointer create(boost::asio::io_service& io_service, queue_pointer qp){
		return pointer(new Tcp_Connection(io_service, qp));
	}
	tcp::socket& socket(){
		return socket_;
	}
	int get_id(){
		return id_;
	}
	void set_id(int id){
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
		buf_ptr buf_ptr_(new EasyBuffer);
		boost::asio::async_read(socket_, boost::asio::buffer(header_data_, sizeof(Header)),
				[this, self](boost::system::error_code ec, std::size_t){
					if(!ec){
							do_read_body(decode_header());
					}
				}
		);
	}
	void do_read_body(Header header){
		auto self(shared_from_this());
		boost::asio::async_read(socket_, boost::asio::buffer(body_data_, header.body_size),
				[this, self, &header](boost::system::error_code ec, std::size_t){
					if(!ec){
						buf_ptr buf(new EasyBuffer);
						buf->put(body_data_, header.body_size);
						std::shared_ptr<TaskData> task_ptr_(new TaskData);
						task_ptr_->message_id = header.message_id;
						task_ptr_->buf_ptr_ = buf;
						task_queue_->push(task_ptr_);
						do_read_header();
					}
				}
		);
	}
	void do_write(buf_ptr buf){
		boost::asio::async_write(socket_, boost::asio::buffer(buf->get_buffer(), buf->get_size()),[](const boost::system::error_code&, std::size_t){});
	}
	void send(buf_ptr buf, short message_id){
		((Header*)write_header_)->message_id = message_id;
		((Header*)write_header_)->body_size = buf->get_size();
		std::memcpy(write_data_, write_header_, sizeof(Header));
		std::memcpy(write_data_ + sizeof(Header), buf->get_buffer(), buf->get_size());
		do_write(buf->get_size() + sizeof(Header));
	}
	boost::asio::io_service& get_io_service(){
		return socket_.get_io_service();
	}
private:
	tcp::socket socket_;
	queue_pointer task_queue_;
	char header_data_[HEADER_LENGTH];
	char body_data_[MAX_BODY_LENGTH];
	char write_header_[HEADER_LENGTH];
	char write_data_[MAX_BODY_LENGTH];
	int id_;
};

#endif













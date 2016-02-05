#ifndef TCP_CONNECTION
#define TCP_CONNECTION

#include <boost/asio.hpp>
#include <functional>
#include "../dependence/blocking_queue.hpp"
#include "../format/message_header.h"
#include "../dependence/auto_id_manager.hpp"
#include "../format/gate_related.pb.h"
#include "../format/message_id.h"
#include "../dependence/buffer.hpp"
#include <cstring>
#include "../dependence/common_struct.h"

#define HEADER_LENGTH 64
#define GATE_HEADER_LENGTH 64
#define MAX_BODY_LENGTH 2048


class Tcp_Connection : public std::enable_shared_from_this<Tcp_Connection>
{
public:
	typedef boost::asio::ip::tcp tcp;
	typedef std::shared_ptr<Tcp_Connection> pointer;
	typedef std::function<void()> functor;
	typedef std::shared_ptr<BlockingQueue<std::shared_ptr<GateLogicTaskData> > > queue_pointer;
	//typedef std::shared_ptr<Auto_ID_Manager<pointer> > manager_pointer;
	typedef std::shared_ptr<EasyBuffer> buf_ptr;

	Tcp_Connection(boost::asio::io_service& io_service, queue_pointer task_queue
		/*manager_pointer manager_p*/) :
		socket_(io_service), task_queue_(task_queue), id_(0)
		/* manager_pointer_(manager_p)*/ {
			//id_ = manager_pointer_->add_to_manager(shared_from_this());

			//start();
	}
	~Tcp_Connection() {
		//manager_pointer_->remove_from_manager(id_);   
	}
	static pointer create(boost::asio::io_service& io_service, queue_pointer qp/* manager_pointer mp*/) {
		return pointer(new Tcp_Connection(io_service, qp));
	}
	tcp::socket& socket() {
		return socket_;
	}
	int get_id() {
		return id_;
	}
	void set_id(int id) {
		id_ = id;
	}
	void start() {
		//((Header*)write_header_)->message_id = msgid::fromgate::notify_connection_id;
		//gr::Notify_Connection_Id msg;
		//msg.set_connection_id(id_);
		//msg.SerializeToArray(write_data_, MAX_BODY_LENGTH);
		//do_write(msg.ByteSize());
		do_read_header();
	}
	Gate_Header decode_header() {
		return *(Gate_Header*)header_data_;
	}
	void do_read_header() {
		auto self(shared_from_this());
		boost::asio::async_read(socket_, boost::asio::buffer(header_data_, HEADER_LENGTH),
			[this, self](boost::system::error_code ec, std::size_t) {
			if (!ec) {
				do_read_body(decode_header());
			}
		}
		);
	}
	void do_read_body(Gate_Header header) {
		auto self(shared_from_this());
		boost::asio::async_read(socket_, boost::asio::buffer(body_data_, header.body_size),
			[this, self, &header](boost::system::error_code ec, std::size_t) {
			if (!ec) {
				buf_ptr buf(new EasyBuffer);
				buf->put(body_data_, header.body_size);
				std::shared_ptr<GateLogicTaskData> task_ptr_(new GateLogicTaskData);
				task_ptr_->char_id = header.char_id;
				task_ptr_->message_id = header.message_id;
				task_ptr_->connection_id = id_;
				task_ptr_->buf_ptr_ = buf;
				task_queue_->push(task_ptr_);
				do_read_header();
			}
		}
		);
	}
	void do_write(int length) {
		boost::asio::async_write(socket_, boost::asio::buffer(write_data_, length), [](const boost::system::error_code&, std::size_t) {});
	}
	void send_to_connection(buf_ptr buf, short message_id, int size) {
		((Header*)write_header_)->message_id = message_id;
		((Header*)write_header_)->body_size = size;
		std::memcpy(write_data_, write_header_, sizeof(Header));
		std::memcpy(write_data_ + sizeof(Header), buf->get_buffer(), size);
		do_write(size + sizeof(Header));
		do_write(size + sizeof(Header));
	}
	boost::asio::io_service& get_io_service() {
		return socket_.get_io_service();
	}
private:
	tcp::socket socket_;
	queue_pointer task_queue_;
	char header_data_[GATE_HEADER_LENGTH];
	char body_data_[MAX_BODY_LENGTH];
	char write_header_[HEADER_LENGTH];
	char write_data_[MAX_BODY_LENGTH];
	int id_;
};

#endif


#ifndef TCP_SERVER
#define TCP_SERVER

#include "tcp_connection.hpp"
#include "../dependence/auto_id_manager.hpp"
#include <boost/bind.hpp>

class Tcp_Server : public std::enable_shared_from_this<Tcp_Server>
{
public:
	typedef std::shared_ptr<Tcp_Connection> pointer;
	typedef std::shared_ptr<Auto_ID_Manager<pointer> > manager_pointer;
	typedef boost::asio::ip::tcp tcp;	
	typedef std::shared_ptr<TaskQueue<std::shared_ptr<TaskData> > > queue_pointer;
	typedef std::shared_ptr<EasyBuffer> buf_ptr;
	
	Tcp_Server(boost::asio::io_service& io_service,queue_pointer qp, short port, int max_connections):
		acceptor_(io_service, tcp::endpoint(tcp::v4(), port)), resolver_(io_service), queue_pointer_(qp), port_(port)
	{
		manager_pointer_.reset(new Auto_ID_Manager<pointer>(max_connections));
	}
	void start_accept(){
		pointer pointer_ = Tcp_Connection::create(acceptor_.get_io_service(), queue_pointer_);
		acceptor_.async_accept(pointer_->socket(), boost::bind(&Tcp_Server::handle_accept, shared_from_this(), pointer_, 
			boost::asio::placeholders::error));
	}
	void do_connect(std::string port, std::string ip, int& connection_id) {
		pointer pointer_ = Tcp_Connection::create(acceptor_.get_io_service(), queue_pointer_);
		boost::asio::async_connect(pointer_->socket(), resolver_.resolve({port.c_str(), ip.c_str()}), boost::bind(&Tcp_Server::handle_connect,
			shared_from_this(), pointer_, connection_id, _2, _3));
	}
private:
	tcp::resolver resolver_;
	tcp::acceptor acceptor_;
	queue_pointer queue_pointer_;	
	manager_pointer manager_pointer_;
	short port_;

	void handle_accept(pointer pointer_, const boost::system::error_code& error){
		if(!error){
			pointer_->set_id(manager_pointer_->add_to_manager(pointer_));		
			pointer_->start();
		}
		start_accept();
	}
	void handle_connect(pointer pointer_, int& connection_id, const boost::system::error_code& error, tcp::resolver::iterator) {
		if (!error) {
			connection_id = manager_pointer_->add_to_manager(pointer_);
			pointer_->set_id(connection_id);
		}
	}
	const manager_pointer get_manager_pointer(){
		return manager_pointer_;
	}
	void send_message(buf_ptr buf, short message_id, int connction_id){
		acceptor_.get_io_service().post(boost::bind(&Tcp_Connection::send, manager_pointer_->get_member(connction_id), buf, message_id));
	}
};

#endif

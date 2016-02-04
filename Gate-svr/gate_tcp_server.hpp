#ifndef TCP_SERVER
#define TCP_SERVER

#include "gate_tcp_connection.hpp"
#include "../dependence/auto_id_manager.hpp"
#include <boost/bind.hpp>
//todo 客户端首先连接gate-mgr,然后gate-mgr给每个gate-server发消息，最先回复的获得转发权。
//因此每个gate-server启动最先做的事情是区gate-mgr那里注册
//gate-server以线程为单位

class Gate_Tcp_Server : public std::enable_shared_from_this<Gate_Tcp_Server>
{
public:
	typedef std::shared_ptr<Gate_Tcp_Connection> pointer;
	typedef std::shared_ptr<Auto_ID_Manager<pointer> > manager_pointer;
	typedef boost::asio::ip::tcp tcp;	
	typedef std::shared_ptr<EasyBuffer> buf_ptr;
	
	Gate_Tcp_Server(boost::asio::io_service& io_service, short port, int max_connections):
		acceptor_(io_service, tcp::endpoint(tcp::v4(), port)), resolver_(io_service), port_(port), 
		obj_mgr_connection_id_(0), gate_mgr_connection_id_(0)
	{
		manager_pointer_.reset(new Auto_ID_Manager<pointer>(max_connections));
	}
	void start(std::string mgr_port, std::string mgr_ip) {
		do_connect(mgr_port, mgr_ip, gate_mgr_connection_id_);
		//
		pointer pointer_ = Gate_Tcp_Connection::create(acceptor_.get_io_service(), manager_pointer_, &forward_rules_, &obj_mgr_connection_id_);
		boost::asio::async_connect(pointer_->socket(),
			resolver_.resolve({ port.c_str(), ip.c_str() }),
			boost::bind(&Gate_Tcp_Server::handle_connect, shared_from_this(), pointer_, connection_id, _2, _3));
		//TODO 在回调里发送注册消息
	}
	void start_accept(){
		pointer pointer_ = Gate_Tcp_Connection::create(acceptor_.get_io_service(), manager_pointer_, &forward_rules_, &obj_mgr_connection_id_);
		acceptor_.async_accept(pointer_->socket(), boost::bind(&Gate_Tcp_Server::handle_accept, shared_from_this(), pointer_,
			boost::asio::placeholders::error));
	}
	void do_connect(std::string port, std::string ip, int& connection_id) {
		pointer pointer_ = Gate_Tcp_Connection::create(acceptor_.get_io_service(), manager_pointer_, &forward_rules_, &obj_mgr_connection_id_);
		boost::asio::async_connect(pointer_->socket(), 
			resolver_.resolve({port.c_str(), ip.c_str()}), 
			boost::bind(&Gate_Tcp_Server::handle_connect, shared_from_this(), pointer_, connection_id, _2, _3));
	}
private:
	tcp::resolver resolver_;
	tcp::acceptor acceptor_;
	manager_pointer manager_pointer_;
	std::map<int, int> forward_rules_;
	int obj_mgr_connection_id_;
	int gate_mgr_connection_id_;
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
};

#endif

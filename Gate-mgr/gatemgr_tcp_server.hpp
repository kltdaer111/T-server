#ifndef TCP_SERVER
#define TCP_SERVER

#include "gatemgr_tcp_connection.hpp"
#include "../dependence/auto_id_manager.hpp"
#include <boost/bind.hpp>

//todo 客户端首先连接gate-mgr,然后gate-mgr给每个gate-server发消息，最先回复的获得转发权。
//因此每个gate-server启动最先做的事情是区gate-mgr那里注册
//gate-server以线程为单位

class Gatemgr_Tcp_Server : public std::enable_shared_from_this<Gatemgr_Tcp_Server>
{
public:
	typedef std::shared_ptr<Gatemgr_Tcp_Connection> pointer;
	typedef std::shared_ptr<Auto_ID_Manager<pointer> > manager_pointer;
	typedef boost::asio::ip::tcp tcp;	
	typedef std::shared_ptr<EasyBuffer> buf_ptr;
	
	Gatemgr_Tcp_Server(boost::asio::io_service& io_service, short port, int max_connections):
		acceptor_(io_service, tcp::endpoint(tcp::v4(), port)), resolver_(io_service), port_(port)
	{
		manager_pointer_.reset(new Auto_ID_Manager<pointer>(max_connections));
	}
	void start_accept(){
		pointer pointer_ = Gatemgr_Tcp_Connection::create(acceptor_.get_io_service(), manager_pointer_, &all_gate_info_, &waiting_clients_);
		acceptor_.async_accept(pointer_->socket(), boost::bind(&Gatemgr_Tcp_Server::handle_accept, shared_from_this(), pointer_,
			boost::asio::placeholders::error));
	}
	void do_connect(std::string port, std::string ip, int& connection_id) {
		pointer pointer_ = Gatemgr_Tcp_Connection::create(acceptor_.get_io_service(), manager_pointer_, &all_gate_info_, &waiting_clients_);
		boost::asio::async_connect(pointer_->socket(), resolver_.resolve({port.c_str(), ip.c_str()}), boost::bind(&Gatemgr_Tcp_Server::handle_connect,
			shared_from_this(), pointer_, connection_id, _2, _3));
	}
private:
	tcp::resolver resolver_;
	tcp::acceptor acceptor_;
	manager_pointer manager_pointer_;
	std::map<int, ServerInfo> all_gate_info_;
	std::set<int> waiting_clients_;
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

#ifndef GATE_SERVER
#define GATE_SERVER

#include "../network_module/tcp_server.hpp"
#include <memory>

class Gate_Thread : public std::enable_shared_from_this<Gate_Thread>
{
public:
	typedef std::shared_ptr<Tcp_Connection> pointer;
	typedef std::shared_ptr<Auto_ID_Manager<pointer> > manager_pointer;
	typedef boost::asio::ip::tcp tcp;
	typedef std::shared_ptr<NonBlockingQueue<std::shared_ptr<TaskData> > > queue_pointer;
	typedef std::shared_ptr<EasyBuffer> buf_ptr;
	typedef std::shared_ptr<Tcp_Server> network_ptr;

	Gate_Thread(boost::asio::io_service& io_service, short port, int max_connections) {
		queue_pointer queue_pointer_(new NonBlockingQueue<std::shared_ptr<TaskData> >);
		network_ptr_.reset(new Tcp_Server(io_service, queue_pointer_, port, max_connections));
	}
private:
	network_ptr network_ptr_;
};

#endif
#ifndef COMMON_STRUCT
#define COMMON_STRUCT

#include <memory>
#include "../dependence/buffer.hpp"

struct SendMessage
{
	short to_where;
	short message_id;
	std::shared_ptr<EasyBuffer> buf_ptr_;
};

struct MessageInSendQueue
{
	short connection_id;
	short message_id;
	std::shared_ptr<EasyBuffer> buf_ptr_;
};

struct TaskData
{
	short message_id;
	std::shared_ptr<EasyBuffer> buf_ptr_;
};

struct ServerInfo
{
	std::string m_ip;
	int m_port;
	ServerInfo() {}
	ServerInfo(std::string ip, int port) :m_ip(ip), m_port(port) {}
};
#endif

#include "gatemgr_tcp_connection.hpp"
#include "../format/message_id.h"
#include "../dependence/order_generator.hpp"

void Gatemgr_Tcp_Connection::handle_msg(buf_ptr& buf)
{
	switch (((Header*)buf->get_buffer())->message_id) {
	case msgid::GateToGatemgr::gate_register:
	{
		gr::Gate_Register msg;
		msg.ParseFromArray(buf->get_buffer(sizeof(Header)), sizeof(msg));
		(*all_gate_info_)[get_id()] = ServerInfo(msg.ip(), msg.port());
	}
	break;
	case msgid::CliToGatemgr::client_apply_connection :
	{
		buf_ptr new_buf(new EasyBuffer);
		gr::Gatemgr_Apply_Fastest_Gate msg;
		new_buf->prepare(sizeof(Header) + sizeof(msg));
		((Header*)new_buf->get_buffer())->message_id = msgid::GatemgrToGate::pick_fastest;
		((Header*)new_buf->get_buffer())->body_size = sizeof(msg);
		msg.set_client_connection_id(get_id());
		msg.SerializeToArray(new_buf->get_buffer(sizeof(Header)), sizeof(msg));
		std::vector<int> all_gate_id;
		for (std::map<int, ServerInfo>::iterator it = all_gate_info_->begin(); it != all_gate_info_->end(); ++it)
			all_gate_id.push_back(it->first);
		generate_order(all_gate_id);
		for (std::vector<int>::iterator it = all_gate_id.begin(); it != all_gate_id.end(); ++it) {
			(*(manager_pointer_->get_member(*it)))->do_write(new_buf);
		}
	}
	break;
	case msgid::GateToGatemgr::pick_fastest:
	{
		gr::Gatemgr_Apply_Fastest_Gate msg;
		if (waiting_clients_->find(msg.client_connection_id()) == waiting_clients_->end())		//has be responsed
			break;
		gr::Gate_Register mesg;
		mesg.set_ip(all_gate_info_->find(get_id())->second.m_ip);
		mesg.set_port(all_gate_info_->find(get_id())->second.m_port);
		buf_ptr new_buf(new EasyBuffer);
		new_buf->prepare(sizeof(Header) + sizeof(mesg));
		((Header*)new_buf->get_buffer())->message_id = msgid::GatemgrToCli::tell_gate_info;
		((Header*)new_buf->get_buffer())->body_size = sizeof(mesg);
		(*(manager_pointer_->get_member(msg.client_connection_id())))->do_write(new_buf);
	}
	break;
	default:
	{

	}
	break;
	//case end
	}
}

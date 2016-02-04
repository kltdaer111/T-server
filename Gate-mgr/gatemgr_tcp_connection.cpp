#include "gatemgr_tcp_connection.hpp"
#include "../format/message_id.h"
#include "../dependence/order_generator.hpp"

void Gatemgr_Tcp_Connection::handle_msg(buf_ptr& buf)
{
	switch (((Header*)buf->get_buffer())->message_id) {
	case msgid::GateToGatemgr::gate_register:
	{
		gr::Server_Info msg;
		msg.ParseFromArray(buf->get_buffer(sizeof(Header)), ((Header*)buf->get_buffer())->body_size);
		(*all_gate_info_)[get_id()] = ServerInfo(msg.ip(), msg.port());
	}
	break;
	case msgid::CliToGatemgr::client_apply_connection :
	{
		waiting_clients_->insert(get_id());
		buf_ptr new_buf(new EasyBuffer);
		gr::Gatemgr_Apply_Fastest_Gate msg;
		msg.set_client_connection_id(get_id());
		new_buf->prepare(sizeof(Header) + msg.ByteSize());
		((Header*)new_buf->get_buffer())->message_id = msgid::GatemgrToGate::pick_fastest;
		((Header*)new_buf->get_buffer())->body_size = msg.ByteSize();
		msg.SerializeToArray(new_buf->get_buffer(sizeof(Header)), msg.ByteSize());
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
		msg.ParseFromArray(buf->get_buffer(sizeof(Header)), ((Header*)buf->get_buffer())->body_size);
		if (waiting_clients_->find(msg.client_connection_id()) == waiting_clients_->end())		//has be responsed
			break;
		waiting_clients_->erase(msg.client_connection_id());

		gr::Server_Info mesg;
		mesg.set_ip(all_gate_info_->find(get_id())->second.m_ip);
		mesg.set_port(all_gate_info_->find(get_id())->second.m_port);
		buf_ptr new_buf(new EasyBuffer);
		new_buf->prepare(sizeof(Header) + mesg.ByteSize());
		((Header*)new_buf->get_buffer())->message_id = msgid::GatemgrToCli::tell_gate_info;
		((Header*)new_buf->get_buffer())->body_size = mesg.ByteSize();
		(*(manager_pointer_->get_member(msg.client_connection_id())))->do_write(new_buf);
	}
	break;
	case msgid::ObjmgrToGatemgr::objmgr_register:
	{
		((Header*)buf->get_buffer())->message_id = msgid::GatemgrToGate::notify_obj_mgr_info;
		for (std::map<int, ServerInfo>::iterator it = all_gate_info_->begin(); it != all_gate_info_->end(); ++it) {
			(*manager_pointer_->get_member(it->first))->do_write(buf);
		}
	}
	break;
	default:
	{

	}
	break;
	//case end
	}
}

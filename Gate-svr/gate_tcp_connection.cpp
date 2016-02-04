#include "gate_tcp_connection.hpp"

void Gate_Tcp_Connection::handle_msg(buf_ptr& buf)
{
	switch (((Header*)buf->get_buffer())->message_id) {
	case msgid::GatemgrToGate::pick_fastest:
	{
		if(*p_obj_mgr_id_ != 0)
			do_write(buf);
	}
	break;
	case msgid::GatemgrToGate::notify_obj_mgr_info:
	{
		gr::Server_Info msg;
		msg.ParseFromArray(buf->get_buffer(sizeof(Header)),((Header*)buf->get_buffer())->body_size);
		connecting_objmgr_func_(msg.port(), msg.ip());
	}
	break;
	default:
	{
		if (forward_rules_->find(get_id()) != forward_rules_->end()) {
			pointer* forward = manager_pointer_->get_member((*forward_rules_)[get_id()]);
			if (forward == NULL) { //找OM在OBJ-S上建立玩家信息
				forward = manager_pointer_->get_member(*p_obj_mgr_id_);
			}
			//执行转发
			(*forward)->do_write(buf);
		}
		break;
	}
	}
}

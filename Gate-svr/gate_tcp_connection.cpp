#include "gate_tcp_connection.hpp"

void Gate_Tcp_Connection::handle_msg(buf_ptr& buf)
{
	switch (((Header*)buf->get_buffer())->message_id) {
	case msgid::GatemgrToGate::pick_fastest:
	{
		do_write(buf);
	}
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

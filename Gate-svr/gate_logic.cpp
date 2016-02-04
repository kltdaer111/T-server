#include "gate_logic.h"

void GateLogic::addPermittedIP(int ip, int now)
{
	permitted_player_ip_list_time_line[now].push_back(ip);
	permitted_player_ip_list.insert(ip);
}

void GateLogic::addOnlinePlayerID(int char_id, int connection_id)
{
	charid_connectionid_map[char_id] = connection_id;
}

void GateLogic::removeOnlinePlayerID(int id)
{
	charid_connectionid_map.erase(id);
}

bool GateLogic::isOnline(int id)
{
	if(charid_connectionid_map.find(id) != charid_connectionid_map.end())
		return true;
	else
		return false;
}

bool GateLogic::isPermited(int ip)
{
	if(permitted_player_ip_list.find(ip) != permitted_player_ip_list.end())
		return true;
	else
		return false;
}

void GateLogic::removeOverduePlayerIP(int now)
{
	for(std::map<int, std::vector<int> >::iterator it = permitted_player_ip_list_time_line.begin();
			it != permitted_player_ip_list_time_line.end();){
		if(it->first + OVERDUE_TIME_SECOND <= now){
			permitted_player_ip_list_time_line.erase(it);
			for(int i = 0; i < (int)it->second.size(); ++i){
				permitted_player_ip_list.erase(it->second[i]);
			}
		}
		else{
			++it;
		}
	}
}

void GateLogic::gateLogicUpdate(int now)
{
	removeOverduePlayerIP(now);
}

GateLogic::connection_ptr GateLogic::getConnection(int id)
{
	return *(cmp_->get_member(id));
}

int GateLogic::getConnectionId(int char_id)
{	
	if(charid_connectionid_map.find(char_id) == charid_connectionid_map.end())
		return 0;
	return charid_connectionid_map[char_id];
}

void GateLogic::setServerId(int server_type, int server_id)
{
	servertype_connectionid_map[server_type] = server_id;
}

void GateLogic::parseBody( short message_id, std::shared_ptr<EasyBuffer> buf_ptr_, int from_connection_id, int char_id)
{
	const char* c = buf_ptr_->get_buffer();
	int size = buf_ptr_->get_size();
	switch(message_id){
		case msgid::togate::player_login:
		{
			gr::Player_Login msg;
			msg.ParseFromArray(c, size);
			int char_id = msg.char_id();
			int ip = msg.ipv4();
			if(isPermited(ip)){
				if(isOnline(char_id)){
					//删除原来的connection
					cmp_->remove_from_manager(getConnectionId(char_id));
				}
				//给obj-master发消息
				int obj_master = 0;
				gr::Notify_Obj_Master_Player_Login send_msg;
				send_msg.set_char_id(char_id);
				std::shared_ptr<EasyBuffer> buf(new EasyBuffer);
				int size = send_msg.ByteSize();
				buf->prepare(size);
				send_msg.SerializeToArray(buf->get_buffer(), size);
				connection_ptr c_p_ = getConnection(obj_master);
				c_p_->get_io_service().post(boost::bind(&Tcp_Connection::send, c_p_, buf, msgid::fromgate::notify_obj_master_player_login, size));
			}
			break;
		}
		case msgid::togate::inter_server_register:
		{
			gr::Inter_Server_Register msg;
			msg.ParseFromArray(c, size);
			setServerId(msg.server_type(),from_connection_id);
			std::shared_ptr<EasyBuffer> buf(new EasyBuffer);
			connection_ptr c_p_ = getConnection(from_connection_id);
			c_p_->get_io_service().post(boost::bind(&Tcp_Connection::send_to_connection, c_p_, buf, msgid::fromgate::server_registered, 0));
			break;
		}
		case msgid::togate::transmission:
		{
			connection_ptr c_p_ = getConnection(getConnectionId(char_id));
			c_p_->get_io_service().post(boost::bind(&Tcp_Connection::send_to_connection, c_p_, buf_ptr_, message_id, buf_ptr_->get_size()));
			break;
		}
	}
}

#ifndef GATE_LOGIC
#define GATE_LOGIC

#include <set>
#include <map>
#include <vector>
#include <utility>
#include "../format/message_id.h"
#include "../format/gate_related.pb.h"
#include "../dependence/buffer.hpp"
#include "../dependence/auto_id_manager.hpp"
#include "../network_module/tcp_connection.hpp"
#include <boost/bind.hpp>
#include "../dependence/common_def.h"

class GateLogic
{
public:
	typedef std::shared_ptr<Tcp_Connection> connection_ptr;
	typedef Auto_ID_Manager<connection_ptr> connection_manager;
	typedef std::shared_ptr<connection_manager> connection_manager_ptr;
	enum{NODEMASTER, PLAYERMASTER, NODE1, PLAYER1};
	enum {OVERDUE_TIME_SECOND = 60};
private:
	std::map<int, std::vector<int> > permitted_player_ip_list_time_line;	// <time, std:vector<ip> >
	std::set<int> permitted_player_ip_list;
	std::map<int, int> charid_connectionid_map;		//<char_id, connection_id>
	std::map<int, int> forward_rule;
	std::map<int, int> servertype_connectionid_map;	//<server_type, connection_id>
public:
	void addPermittedIP(int ip, int now);
	void addOnlinePlayerID(int char_id, int connection_id);
	void removeOnlinePlayerID(int char_id);
	bool isOnline(int id);
	bool isPermited(int ip);
	void removeOverduePlayerIP(int now);
	void gateLogicUpdate(int now);
	void parseBody(short message_id, std::shared_ptr<EasyBuffer> buf_ptr_, int from_connection_id, int char_id); 
	int getConnectionId(int char_id);
	void setServerId(int server_type, int server_id);
};

#endif

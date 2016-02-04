#ifndef MESSAGE_ID
#define MESSAGE_ID

namespace msgid
{
	typedef const short cs;
	namespace togate{
		cs transmission = 100;
		cs player_login = 101;
		cs inter_server_register = 102;
		
	}
	namespace fromgate{
		cs notify_obj_master_player_login = 1000;
		cs server_registered = 1001;
	}
	namespace CliToGatemgr {
		cs client_apply_connection = 50;
	}
	namespace GatemgrToGate {
		cs pick_fastest = 60;
		cs notify_obj_mgr_info = 61;
	}
	namespace GateToGatemgr {
		cs gate_register = 70;
		cs pick_fastest = 80;
	}
	namespace GatemgrToCli {
		cs tell_gate_info = 90;
	}
	namespace ObjmgrToGatemgr {
		cs objmgr_register = 100;
	}
}

#endif

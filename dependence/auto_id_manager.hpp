#ifndef AUTO_ID_MANAGER
#define AUTO_ID_MANAGER

#include <map>
#include <exception>
#include <stdlib.h>
#include <time.h>

template <typename T>
class Auto_ID_Manager
{
private:
	std::map<int, T> manager;
	int max_id_;
public:
	Auto_ID_Manager(int size):max_id_(10 * size){
		srand(time(NULL));
	}
	int genRandom(){
		return rand() % max_id_ + 1;
	}
	int generateID(){
		int num = 1;
		while(manager.find(num) != manager.end())
			num = genRandom();
		return num;
	}
	int add_to_manager(T member){
		int id = generateID();
		manager[id] = member;
		return id;
	}
	void remove_from_manager(int id){
		manager.erase(id);
	}
	T* get_member(int id){
		if(manager.find(id) == manager.end())
			return NULL;
		return &(manager[id]);
	}
};

#endif

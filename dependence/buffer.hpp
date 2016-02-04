#ifndef EASY_BUFFER
#define EASY_BUFFER

#include <cstring>

class EasyBuffer
{
private:
	char* m_buffer_;
	int m_size;
public:
	EasyBuffer(): m_buffer_(NULL), m_size(0){}
	~EasyBuffer(){
		if(m_buffer_)
			delete m_buffer_;
	}
	void put(const char* buf, int size){
		m_size = size;
		m_buffer_ = new char[m_size];
		std::memcpy(m_buffer_, buf, size);		
	}
	char* get_buffer(int offset = 0){
		return m_buffer_ + offset;
	}
	int get_size(){
		return m_size;
	}
	void prepare(int size){
		m_buffer_ = new char[size];
		m_size = size;
	}
	void operator=(EasyBuffer& buf) {
		buf.m_buffer_ = m_buffer_;
		buf.m_size = m_size;
		m_buffer_ = NULL;
		m_size = 0;
	}
};

#endif

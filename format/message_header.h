#ifndef MESSAGE_HEADER
#define MESSAGE_HEADER

struct Header
{
	short message_id;
	int body_size;
};

struct Gate_Header
{
	short message_id;
	int body_size;
	int char_id;
};

#endif

#ifndef _PROTO_H
#define _PROTO_H

//
// Request types
//

struct request_create_session {
	char username[128];
};

struct request_start_session {
	char cmd[128];
};

struct request_post_auth_message_response {
	char response[128];
};

struct request_cancel_session {};

enum request_type {
	request_type_create_session = 1,
	request_type_start_session = 2,
	request_type_post_auth_message_response = 3,
	request_type_cancel_session = 4,	
};

struct request {
	enum request_type request_type;
	union {
		struct request_create_session request_create_session;
		struct request_start_session request_start_session;
		struct request_post_auth_message_response request_post_auth_message_response;
		struct request_cancel_session request_cancel_session;
	} body;
};

//
// Response types
//

struct response_success {};

enum auth_message_type {
	auth_message_type_visible = 1,
	auth_message_type_secret = 2,
	auth_message_type_info = 3,
	auth_message_type_error = 4
};

struct response_auth_message {
	enum auth_message_type auth_message_type;
	char auth_message[128];
};

enum error_type {
	error_type_error,
	error_type_auth,
};

struct response_error {
	enum error_type error_type;
	char description[128];
};

enum response_type {
	response_type_roundtrip_error = -1,
	response_type_success = 1,
	response_type_error = 2,
	response_type_auth_message = 3,
};

struct response {
	enum response_type response_type;
	union {
		struct response_success response_success;
		struct response_error response_error;
		struct response_auth_message response_auth_message;
	} body;
};

struct response roundtrip(struct request req);

#endif
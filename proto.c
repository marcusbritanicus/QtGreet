#define _POSIX_C_SOURCE 200809L

#include <json-c/json_object.h>
#include <json-c/json_tokener.h>

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "proto.h"
#include "window.h"

struct header {
    uint32_t magic;
    uint32_t version;
    uint32_t payload_len;
};

static int write_req(int fd, struct json_object* req) {
    const char* reqstr = json_object_get_string(req);
    uint32_t len = strlen(reqstr);
    char* headerp = (char*)&len;
    ssize_t off = 0;

    while (off < 4) {
        ssize_t n = write(fd, &headerp[off], 4-off);
        if (n < 1) {
            goto error;
        }
        off += n;
    }

    off = 0;
    while (off < len) {
        ssize_t n = write(fd, &reqstr[off], len-off);
        if (n < 1) {
            goto error;
        }
        off += n;
    }
 
    return 0;
error:
    return -1;
}

static struct json_object* read_resp(int fd) {
    struct json_object* resp = NULL;
    char *respstr = NULL;
    uint32_t len;
    ssize_t off = 0;

    while (off < 4) {
        char* headerp = (char*)&len;
        ssize_t n = read(fd, &headerp[off], 4-off);
        if (n < 1) {
            goto end;
        }
        off += n;
    }

    off = 0;
    respstr = (char*)calloc(1,len+1);
    while (off <len) {
        int n = read(fd, &respstr[off],len-off);
        if (n < 1) {
            goto end;
        }
        off += n;
    }

    resp = json_tokener_parse(respstr);

end:
    if (respstr != NULL) {
        free(respstr);
    }
    return resp;
}

static int connectto(const char* path) {
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        fprintf(stderr, "unable to open socket\n");
        return -1;
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, path);
    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        fprintf(stderr, "connect: error: %d\n", errno);
        close(fd);
        return -1;
    }

    return fd;
}

static struct json_object* roundtrip_json(struct json_object* req) {
    struct json_object* resp = NULL;
    int fd;
    char* greetd_sock = getenv("GREETD_SOCK");

    if (greetd_sock == NULL) {
        fprintf(stderr, "GREETD_SOCK not set\n");
        return NULL;
    }

    if ((fd = connectto(greetd_sock)) < 0) {
        fprintf(stderr, "unable to connect to socket\n");
        goto end;
    }

    if (write_req(fd, req) != 0) {
        fprintf(stderr, "unable to write request to socket\n");
        goto end;
    }

    if ((resp = read_resp(fd)) == NULL) {
        fprintf(stderr, "unable to read response from socket\n");
        goto end;
    }

end:
    if (fd > 0) {
        close(fd);
    }
    return resp;
}

const char* json_get_string_from_object(struct json_object* obj, const char* key) {

    struct json_object* val = json_object_object_get(obj, key);
    if (val == NULL) {
        return NULL;
    }
    return json_object_get_string(val);
}

struct response roundtrip(struct request req) {
    struct json_object *json_req = json_object_new_object();
    switch (req.request_type) {
    case request_type_create_session: {
        json_object_object_add(json_req, "type", json_object_new_string("create_session"));
        json_object_object_add(json_req, "username", json_object_new_string(req.body.request_create_session.username));
        break;
    }
    case request_type_start_session: {
        json_object_object_add(json_req, "type", json_object_new_string("start_session"));
        struct json_object* cmd = json_object_new_array();
        json_object_array_add(cmd, json_object_new_string(req.body.request_start_session.cmd));
        json_object_object_add(json_req, "cmd", cmd);
        break;
    }
    case request_type_post_auth_message_response: {
        json_object_object_add(json_req, "type", json_object_new_string("post_auth_message_response"));
        json_object_object_add(json_req, "response", json_object_new_string(req.body.request_post_auth_message_response.response));
        break;
    }
    case request_type_cancel_session: {
        json_object_object_add(json_req, "type", json_object_new_string("cancel_session"));
        break;
    }
    }

    struct response resp = {
        .response_type = response_type_roundtrip_error,
    };
    resp.body.response_error.error_type = error_type_error;

    struct json_object* json_resp = roundtrip_json(json_req);
    if (json_resp == NULL) {
        snprintf(resp.body.response_error.description, 128, "proto: roundtrip failed");
        goto done;
    }

    const char* typestr = json_get_string_from_object(json_resp, "type");
    if (typestr == NULL) {
        snprintf(resp.body.response_error.description, 128, "proto: no type found");
        goto done;
    }


    if (strcmp(typestr, "success") == 0) {
        resp.response_type = response_type_success;
        goto done;
    }

    if (strcmp(typestr, "auth_message") == 0) {
        const char* messagestr = json_get_string_from_object(json_resp, "auth_message");
        if (messagestr == NULL) {
            resp.response_type = response_type_error;
            snprintf(resp.body.response_error.description, 128, "proto: no message");
            goto done;
        }
        const char* auth_message_typestr = json_get_string_from_object(json_resp, "auth_message_type");
        if (auth_message_typestr == NULL) {
            resp.response_type = response_type_error;
            snprintf(resp.body.response_error.description, 128, "proto: no message type");
            goto done;
        }
        resp.response_type = response_type_auth_message;
        if (strcmp(auth_message_typestr, "visible") == 0) {
            resp.body.response_auth_message.auth_message_type = auth_message_type_visible;
        } else if (strcmp(auth_message_typestr, "secret") == 0) {
            resp.body.response_auth_message.auth_message_type = auth_message_type_secret;
        } else if (strcmp(auth_message_typestr, "info") == 0) {
            resp.body.response_auth_message.auth_message_type = auth_message_type_info;
        } else if (strcmp(auth_message_typestr, "error") == 0) {
            resp.body.response_auth_message.auth_message_type = auth_message_type_error;
        } else {
            resp.response_type = response_type_error;
            snprintf(resp.body.response_error.description, 128, "proto: unknown message type");
            goto done;
        }

        strncpy(resp.body.response_auth_message.auth_message, messagestr, sizeof(resp.body.response_auth_message.auth_message)-1);
        goto done;
    }

    if (strcmp(typestr, "error") == 0) {
        const char* descriptionstr = json_get_string_from_object(json_resp, "description");
        if (descriptionstr == NULL) {
            resp.response_type = response_type_error;
            snprintf(resp.body.response_error.description, 128, "proto: no error description");
            goto done;
        }
        resp.response_type = response_type_error;

        const char* errortypestr = json_get_string_from_object(json_resp, "error_type");
        if (strcmp(errortypestr, "auth_error") == 0) {
            resp.body.response_error.error_type = error_type_auth;
        } else {
            resp.body.response_error.error_type = error_type_error;
        }

        strncpy(resp.body.response_error.description, descriptionstr, sizeof(resp.body.response_error.description)-1);
        goto done;
    }

    snprintf(resp.body.response_error.description, 128, "proto: no known type");

done:
    json_object_put(json_resp);
    return resp;
}

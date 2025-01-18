#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>

#define MSG_MAX_SIZE 4096

typedef enum {
    MSG_WELCOME = 0,
    MSG_START_SIM = 1,
    MSG_SIM_UPDATE = 2,
    MSG_SIM_COMPLETE = 3,
    MSG_ERROR = 4,
    MSG_REPLAY = 5,
    MSG_DISCONNECT = 6
} message_type_t;

typedef struct {
    message_type_t type;
    uint32_t length;
    char data[MSG_MAX_SIZE];
} protocol_message_t;

#endif // PROTOCOL_H
//
// Created by Ke Meng on 4/16/15.
//

#ifndef FILTER_PROTOCOL_H
#define FILTER_PROTOCOL_H

typedef struct {
    short Command;
    int Flag;
    size_t Length;

} PROTOCOL_HEADER;

enum {
    CMD_NONE,
    CMD_TEST,
    CMD_RESULT,
    CMD_ADD,
    CMD_DELETE,

    CMD_ERROR=255,
};

#endif //FILTER_PROTOCOL_H

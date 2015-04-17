//
// Created by Ke Meng on 4/16/15.
//

#ifndef FILTER_PROTOCOL_H
#define FILTER_PROTOCOL_H

#define MAX_DATA_LENGTH 0xFFFF
#define PROTOCOL_VERSION 1

typedef struct __attribute__((__packed__)) {
    unsigned char Version;
    unsigned char Command;
    unsigned short Flag;
    unsigned short Length;
} PROTOCOL_HEADER;

typedef struct __attribute__((__packed__)) {
    unsigned short StartPos;
    unsigned short Length;
} RESULT_PAIR;

enum {
    CMD_NONE,
    CMD_TEST,
    CMD_RESULT,
    CMD_ADD,
    CMD_DELETE,

    CMD_ERROR=255,
};

#endif //FILTER_PROTOCOL_H

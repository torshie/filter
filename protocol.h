//
// Created by Ke Meng on 4/16/15.
//

#ifndef FILTER_PROTOCOL_H
#define FILTER_PROTOCOL_H


#define PROTOCOL_HEADER_LENGTH 6
#define MAX_DATA_LENGTH 0xFFFF
#define PROTOCOL_VERSION 1

typedef struct {
    unsigned char Version;
    unsigned char Command;
    unsigned short Flag;
    unsigned short Length;

} PROTOCOL_HEADER;

typedef struct {
    unsigned short StartPos;
    unsigned short Length;
} RESULT_PAIR;
#define RESULT_PAIR_LENGTH 4

enum {
    CMD_NONE,
    CMD_TEST,
    CMD_RESULT,
    CMD_ADD,
    CMD_DELETE,

    CMD_ERROR=255,
};

#endif //FILTER_PROTOCOL_H

Protocol
======

C F L D

* Command  8-bit
* Flag     16-bit    Currently useless
* Length   16-bit
* Data     64K-1 bytes maximum


##Commands
```

enum {
    CMD_NONE,
    CMD_TEST,
    CMD_RESULT,
    CMD_ADD,
    CMD_DELETE,

    CMD_ERROR=255,
};

```
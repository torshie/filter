Protocol
======

V C F L D

* Version  8-bit     1
* Command  8-bit
* Flag     16-bit    Currently useless
* Length   16-bit
* Data     64K-1 bytes maximum

**Multiple-byte fields are all little endian.**

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
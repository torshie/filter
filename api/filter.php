<?php

class filter
{
    const CMD_NONE = 0;
    const CMD_TEST = 1;
    const CMD_RESULT = 2;
    const CMD_ADD = 3;
    const CMD_DELETE = 4;
    const CMD_ERROR=255;

    const VERSION = 1;
    const HEADER_LENGTH = 6;

    const RESULT_PAIR_LENGTH = 4;

    protected $host;
    protected $port;

    protected $fp;

    public function __construct($host, $port)
    {
        $this->host = $host;
        $this->port = intval($port);

        $this->connect();
    }

    public function connect()
    {
        if (empty($this->host)) {
            throw new filterException("host can not be empty", 100001);
        }
        if (empty($this->port) || $this->port < 1 || $this->port > 65535) {
            throw new filterException("Invalid port");
        }

        $ctx = stream_context_create();
        $connect_flag = STREAM_CLIENT_CONNECT | ~STREAM_CLIENT_PERSISTENT;

        $fp = stream_socket_client(
            "tcp://{$this->host}:{$this->port}",
            $errno,
            $error,
            100,
            $connect_flag,
            $ctx
        );

        if (!$fp) {
            throw new filterException("Connection failed. {$error}#{$errno}", 100003);
        }

        stream_set_blocking($fp, 1);

        stream_set_write_buffer($fp, 0);

        $this->fp = $fp;
    }

    public function close()
    {
        fclose($this->fp);
    }

    public function test($text)
    {
        if (isset($text[0x10000]) || empty($text)) {
            throw new Exception("bad length", 1);
        }

        $packet = pack('CC', self::VERSION, self::CMD_TEST);
        $packet .= pack('v', 0);                # flag
        $packet .= pack('v', strlen($text));    # big endian
        $packet .= $text;

        # socket write
        $r = fwrite($this->fp, $packet);

        if (!$r) {
            throw new filterException("write failed", 100004);
        }
        #echo "{$r} bytes written\n";

        $s = fread($this->fp, self::HEADER_LENGTH);
        $p = unpack("Cversion/Ccommand/vflag/vlen", $s);
        #echo "Command = {$p['command']} Length={$p['len']} Flags={$p['flag']}\n";

        if ($p['command'] != self::CMD_RESULT) {
            if ($p['command'] == self::CMD_ERROR) {
                if ($p['len']) {
                    $result = fread($this->fp, $p['len']);
                    echo "Error: $result\n";
                    return false;
                }
            }
            throw new filterException("bad response", 100005);
        }
        if ($p['len']) {
            $result = fread($this->fp, $p['len']);

            $pair_length = self::RESULT_PAIR_LENGTH;
            $start_pos = 0;
            $result_pairs = array();

            do {
                $result_pairs[] = unpack("vpos/vlen", substr($result, $start_pos, $pair_length));
                $start_pos += $pair_length;

            } while(isset($result[$start_pos]));

            var_dump($result_pairs);
        }

    }
}

class filterException extends Exception {}
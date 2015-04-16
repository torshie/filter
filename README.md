# Build
Run script build.sh to build the project. Note .c files under directory
"test" will be ignored.

# Command line options:
* "--listen" Address to listen to. It could to a UNIX socket path or an
Internet address. Internet address must be given in the format of
"a.b.c.d:port". Domain names aren't supported. This option can be
specified more than once.
* "--pattern" path to the file containing the patterns should be censored
* "--thread" number of worker threads to start, optional, default to 10

# Request & response
Create a new connection for every request. Send the text needs to be
checked to the address(es) listened by the program, the response will
be in the following format:
* The 1st line is status code, it will always be zero at the moment.
* The 2nd line is a serial of integer pairs separated by spaces, integers
of a pair are also separated by a space.
* The 1st integer of a pair is the _END_ position of a matched pattern,
the 2nd one is the length of the matched pattern.
* All positions & lengthes are in bytes.

# Online pattern database reloading
Send signal SIGUSR1 (kill -SIGUSR1 <pid>) to the process to reload the
pattern database.

# Change text encoding
Modify source code to redefine type AC_ALPHABET_t to the type you like,
rebuild the program, then encode all characters (pattern database and
text to be checked) into sequences of AC_ALPHABET_t.

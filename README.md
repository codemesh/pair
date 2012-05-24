pair
====

 A duplex pipe. Like a unix pipe, but the output of downstream process is also
piped back to input of upstream process. Can be used to concatenate nc and cat
to make an echo server.

COMPILE
=======
 Run make to compile:
 
 [user@host$]make
 
 Then you should get the executable 'pair'.

INSTALL
=======
 No install is needed.

USAGE
=====
 Pair is used to concatenate two processes and form a duplex pipe between them.
The commands, together with arguments of each, of processes are given directely
on the command line.

 [user@host]$./pair
        usage: ./pair --uargs <cmd1 arg1 arg2 ...> --dargs <cmd2 arg1 ...>

 Arguments between --uargs and --dargs are taken as the
command and args to run the first process, while those following --dargs are
used to run the second one.

EXAMPLE
=======
 To form an echo server with nc and cat, just run the following command:
 
 [user@host]$./pair --uargs nc -l 1234 --dargs cat
 
 Then two commands 'nc -l 1234' and 'cat' will be run and catenated together.
This makes a simple echo server listening at tcp port 1234.
 To test that server, open a connection to it and type something:

 [user@host]$ nc localhost 1234
 hello
 hello
 please show me what you can do
 please show me what you can do

BUGS
====
 Any bug is welcome, drop your message to: 6.283183(at)gmail.com. Be sure
to contain 'pair bug report' in your mail title.


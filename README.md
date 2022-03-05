# A TCP chat written in C++
An example of a TCP/IP server that can be used as a chat.

## Build
There is a Makefile. To build the chat run the command:

```bash
make chat
```

## Use
To run the chat:

```bash
make run
```

or simple:

```bash
./chat
```

By default the port is 6666. To connect to the chat you can use
`telnet` program:

```bash
[roman ~]$ telnet 0 6666
Trying 0.0.0.0...
Connected to 0.
Escape character is '^]'.
Enter your name: Roman
Hello
[Roman] Hello
Anna has entered the chat
[Anna] Hi, everyone
```

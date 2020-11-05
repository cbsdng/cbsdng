# CBSDng
## Protocol

To denotify message dirrection, in the documentation the following arrows will be used:

* `-->` from client to server
* `<--` from server to client

They are not used in the protocol, but are just there to help understanding the messages/protocol.

```
--> <size> <id> <type> ls
<-- <size> <id> <type> <output>
```
```
--> <size> <id> <type> start <jail>
<-- <size> <id> <type> <output>
```

| Name           | Description                                                              |
| :------------- | :----------------------------------------------------------------------- |
| size           | The size of the rest of the message                                      |
| id             | Identification (not used currently)                                      |
| type           | Type of message, multiple types can be ORed together into a single value |
| payload        | Command and arguments, keyboard input or command output                  |

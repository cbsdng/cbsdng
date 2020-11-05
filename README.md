# CBSDng
## Protocol

To denotify message dirrection, in the documentation the following arrows will be used:

* `-->` from client to server
* `<--` from server to client

They are not used in the protocol, but are just there to help understanding the messages/protocol.

```
--> <id> <type> <size> ls
<-- <id> <type> <size> <output>
```
```
--> <id> <type> <size> start <jail>
<-- <id> <type> <size> <output>
```

| Name           | Description                                                              |
| :------------- | :----------------------------------------------------------------------- |
| id             | Identification (not used currently)                                      |
| type           | Type of message, multiple types can be ORed together into a single value |
| size           | The size of the rest of the message (the payload)                        |
| payload        | Command and arguments, keyboard input or command output                  |

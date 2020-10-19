# CBSDng
## Protocol

To denotify message dirrection, in the documentation the following arrows will be used:

* `-->` from client to server
* `<--` from server to client

They are not used in the protocol, but are just there to help understanding the messages/protocol.

```
--> <id> <type> ls
<-- <id> <type> <output>
```
```
--> <id> <type> start <jail>
<-- <id> <type> <output>
```

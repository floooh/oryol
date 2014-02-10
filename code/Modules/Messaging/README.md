## The Oryol Messaging Module

### Overview

The Messaging module implements a simple message-passing framework for the higher-level modules of Oryol. It
is very flexible, easy to use and reasonably fast, but it may be too heavy-weight for some scenarios. Specifically,
if the number of messages per frame is more then a few hundred, another approach might be needed.

Good use cases for the messaging frame-work are:

- handling asynchronous IO requests
- a multiplayer networking system
- simple HTTP services (responding to JSON-RPC calls, etc...)

I wouldn't recommend the messaging framework for offloading heavy compute tasks to threads if this means
a lot of messages need to be passed between the threads.

### Concepts

The Messaging module is based on 3 simple concepts:

- **Message**: 
- Protocol

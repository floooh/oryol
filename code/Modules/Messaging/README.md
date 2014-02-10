## The Oryol Messaging Module

Disclaimer: details may change as the Messaging Module is under development.

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

- **Message**: an actual message object
- **Protocol**: a collection of related messages
- **Port**: a generic message receiver/handler/forwarder object


#### Messages

A Message is a simple, ref-counted C++ object used to hold the message parameters, and to communicate the 
current state of the message. Message classes are usually not written by hand, but their C++ source is 
generated from XML files.

Message classes have the following attributes:

- a numeric message id (auto-generated), message ids are only unique within their protocol
- a protocol id (this is a FourCC uint32)
- a current state (Initial, Pending, Handled)
- a set of member variables with their respecitive setter/getter methods

Messages are generally created through pool-allocators. When a message is created, a pointer is popped from
a lockless forwarded-linked free-list, and placement-new is called on the memory block (so, no allocation 
happens, but a C++ constructor is called). Messages are ref-counted. When a message is destroyed, the
destructor is called on the object, and the memory block is put back into the pool-allocator.

Message classes can be derived from other message classes, the parent message class can be either
from the same Protocol, or from the parent Protocol (see below in the Protocol section).

Messages can serialize/deserialize themselves to and from plain-old-data (POD) representation. Serialization
to POD is only used when the message needs to leave process boundaries, otherwise a (smart-)pointer to the
message is passed around.

Care has been taken, that message-pointers are either forwarded by reference, or moved, so that no
unnecessary copying or ref-count bumping takes place.

The disadvantage with passing messages around by pointers is that - even though they are pooled - once
the content of the message needs to be accessed, a cache miss is likely, since different message objects
can be far away from each other in memory. This is why a more low-level system for computation work 
which works with plain message buffers will be more efficient.

#### Protocols

A Protocol is a group of related messages. Technically it is a C++ class with a bunch of static methods,
and more embedded classes. There are no Protocol objects.

The following classes are embedded in the Protocol class:

- a MessageId class, this holds an enum of all MessageIds in the protocol
- a Factory class, this can create a Message object from its MessageId
- the actual Message classes

Protocols can be derived from one another. The first MessageId of a derived protocol will be the last 
MessageId of the parent protocol + 1, so that MessageIds are unique within a Protocol's ancestor chain.

The complete source code for protocols is usually not written by hand, but generated from XML files. The 
conversion from XML source code to C++ happens transparently during the build process (you 
can simply edit an XML file in your favourite IDE, hit the "Build" button, and the right thing will happen).

The magic behind the source code generation process is handled by python scripts called *generators*
(see the *oryol/generators* directory, and the integration into the build process is handled through
the cmake-scripts under *oryol/cmake*)

Here's a simple message protocol XML file:

```xml
<!-- 
  - type: conversion happens through the MessageProtocol.py python script
  - ns: the C++ namespace of the protocol
  - name: the C++ class name of the protocol
  - id: the protocolId (FourCC code)
-->
<Generator type="MessageProtocol" ns="Messaging" name="TestProtocol" id="TSTP" >
    
    <!-- the required C++ headers for non-trivial types -->
    <Header path="Core/String/StringAtom.h"/>
    
    <!-- 
      A simple message with one 32bit-integer, and one 32-bit-float value.
      - the C++ class name is "TestMsg" and has the following setters/getters:
        - void SetHitpoints(int32 val)
        - int32 GetHitpoints() const
        - void SetDuration(float32 val)
        - float32 GetDuration() const
      - the Hitpoints value will be set to 10 in the constructor
      - the Duration value will be set to 0.0 (0 is the default for all
        simple numeric types
    -->
    <Message name="TestMsg">
        <Attr name="Hitpoints" type="int32" def="10" />
        <Attr name="Duration" type="float32" />
    </Message>
    
    <!-- 
      A derived message with TestMsg as parent class, this only
      adds a StringAtom attribute 'Name'. To set the default value,
      we need to use XML's escape mechanism (&quot; is ")
    -->
    <Message name="DerivedMsg" parent="TestMsg">
        <Attr name="Name" type="Core::StringAtom" def="&quot;Test&quot;"/>
    </Message>

</Generator>
```

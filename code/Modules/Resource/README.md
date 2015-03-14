# Oryol Resources

### The Resource Module

Oryol doesn't provide a centralized resource management system, instead modules
implement their own, localized resource management using the helper classes
in the Resource module as building blocks.

### Resource Management Overview

This is a very general overview of the resource management philosophy in
Oryol. Modules are completely free to implement their own resource
management and don't need to adhere to the basic philosophy, or not
use the Resource classes at all. This overview is more a record of what
did and didn't work in previous attempts, and why the Resource module
classes ended up the way they are.

For specific information on how to create and use resources, please read
the particular module documentation (like the Gfx module doc page).

### Oryol's View of a Resource (What's a Resource)

A resource is a very slim proxy object that adds a standardized interface 
for **creation**, **destruction**, **sharing** and **lifetime management** to some
other object or generally 'piece of data' (e.g. a 3D API resource
like textures, shaders, vertex buffers, or something simple like
a simple memory buffer).

### Resource Sharing

Safe resource sharing is implemented through resource **Locators** and 
resource **Ids**. A **Locator** is (in its usual form) a string name 
that serves as a human-readable sharing identifier, and (for resources
that are loaded from a filesystem) also as an URL. 

A resource **Id** is  a 64-bit identifier that serves as a safe handle to 
a resource object.  Both Locators and Ids are described in detail further down.

The core of resource sharing is the **Lookup** operation, which takes
a Locator object, and returns an Id object (which may be 'invalid' if the
resource doesn't exist or isn't shareable).

There is one interesting feature of Locators besides the string name, which 
is the additional **Signature**. A Locator signature is a 32-bit number
which is used to restrict sharing. Two Locators are only identical if both
the string name and signature match. This can be used to supress resource
sharing even if the string name (e.g. the filename) of two resource matches.

This is for instance useful if a resource (like a mesh, or a texture) must
be loaded from the same data file, but has different usage scenarios (e.g.
one instance of a mesh is used as static geometry for rendering, but
the other instance must be readable by the CPU but is never rendered, but 
both meshes have the same filename).

There is one special "NonShared" signature which disables sharing of
a resource completely, and makes the resource object unique, no matter 
how many other shared or non-shared resources with the same name exist, this
is most useful to enforce private ownership and use of a resource without
having to care about name collisions.


### Resource Lifetime Management

Resources in Oryol are *NOT* use-counted (this changed just recently).
Implementing use-counting for resources became complex because of 
dependencies between resources (e.g. a Gfx DrawState object has references
to shader and mesh resources, meaning the shader and mesh resources need
to exist as long as the draw state object exists). The tipping point that 
use-counting was the wrong approach was when I had to implement a
RAII 'smart-wrapper' for resource Ids (called GfxId). It kinda worked ok,
but the code was complex and ugly.

Use-counting has been replaced with **ResourceLabels** and batch-destruction
by matching resource label. 

If you look at typical use of resources in 3D applications it is apparent that
resources are often created and destroyed in batches or 'onion skin' layers. 
Some basic resources need to exist during the whole lifetime of the application, 
other resources are created at the start of a map (or entering a zone), and
destroyed when the map (or zone) is left, or when a subsystem is created
or destroyed.

A fine-grained per-resource use-count system seems like overkill.

The new resource lifetime management system in Oryol works like this:

Before creating a number of related resources, the creating code pushes
a ResourceLabel on the 'label stack'. All created resources get the label
from the top of the label stack assigned. After the group of related
resources have been created, the label is popped from the label stack and
the creating code keeps a record of the label, because it is needed later
for destruction.

When this specific set of resources is no longer needed, a single
destroy call (taking a resource label as argument) is all that's needed
to destroy all resources with that label.

All resources with that label are **gone** now, any code still trying to use
a resource Id from this batch will fail. What 'fail' exactly means depends
on how an invalid Id is handled by the specific code module: 

There won't be any low-level memory crashes, because resource ids are 
not pointers but actually safe indices into a resource pool. But the module
could for instance throw an assert, or silently ignore the invalid Id.

### Resource States

A resource object is always in one of the following states (if some
states are unclear yet, read on, details are explained further down):

* **Initial**: the resource is in its initial 'uninitalized' state, 
application code usually never sees or has to deal with a resource in the 
initial state
* **Setup**: the resource has a Setup object assigned, but initialization
is pending (this state usually switches immediately to a followup state,
so the application code also never sees a resource in Setup state)
* **Pending**: the resource is currently asynchronously loading, this only
applies for resource types that support asynchronous creation. Application
code can check whether a resource is in pending state, but usually doesn't
need to deal with this (nothing bad will happen if a pending resource is used)
* **Valid**: the resource has been initialized and can be used, this is the
common state of a resource during its lifetime
* **Failed**: resource creation has failed


### Resource Creation and Loading

Resource creation is a complex topic under the hood, but Oryol tries hard
to make it simple, orthogonal and with as little boilerplate code as possible.

The most typical use cases (e.g. loading a texture from disk or http server) 
should be one-liners, but it should still be possible to have very detailed control
over how the resource is actually created.

There is a distinction between Creation and Loading:

* **Creation** always means the resource is created instantly and synchronously
(although please note that 'synchronously' part may
change in later versions when more modern desktop 3D APIs are supported which
make it easier to setup resources in separate threads)
* **Loading** always means that an asynchronous loading operation is involed
(e.g. loading resource data from disk or web)

There may be a 3rd creation type in the future (**Streaming**) which would
be a more restricted type of **Loading**, and be optimized to background-stream
(or generate) data directly into resource memory from separate threads. This
is kinda impossible to do right in OpenGLES2-level APIs, so that's not currently
a high priority goal.

The **Create** operation always follows a simple pattern, regardless
of the type of the resource to be created:

```
Id Create(const SETUP& setup);
```

A Create method takes a 'resource setup object' and returns a resource Id
object.

**A short interlude on Setup objects:**

Setup objects are very small and simple data objects that describe in detail 
how a resource should be created. Typically each resource type has its own
setup class (e.g. in the Gfx module there are classes like TextureSetup
or MeshSetup).

Setup objects exist along with the resource object they describe (the idea is, 
that this will help to restore a 'lost' resource object, although this has not
yet been implemented, and there are some open design issue when generated
data is involved).

A resource **Load** operation is always delegated to a loader object which
knows how to asynchronously setup a resource. The Loader object takes a similar
role to the Setup object for synchronous creation, but a Loader object isn't
a simple data container, but also contains actual loading code (loader objects
typically have a Setup object embedded which describes how exactly the 
resource should be created once the data has been loaded).

Creating a resource through async loading also always follows a simple pattern:

```
Id Load(const Ptr<Loader>& loader);
```

A Load method takes a pointer to a loader object, and immediately returns
a resource Id. The resource Id can be used right away even though the
resource may still be loading in the background. A resource object that
is still loading is in the Pending state. What happens when a pending
resource is used is up to the actual code module, but this should never
be a fatal error. The module could decide to silently ignore operations
that involve pending resources, or it could use a placeholder resource.

One important restriction for Loader objects is that they should only
use publically available resource creation functions of a module, this 
is not enforced anywhere, but it can help to make the required loading code 
simpler and more orthogonal. 

For  instance, a in a former version of the Oryol Gfx resource system, loader 
classes directly made use of OpenGL functionality, which means loader classes
would have to be duplicated for each resource file format **and**
internally supported 3D API.

### Querying Resource State

Actual resource objects are private and opaque, application code will
usually not have direct access to a resource object in form of a
pointer.

Getting information about a resource back is wrapped in one or more
Query operations, taking a resource Id and returning a result (simple
value or a struct):

```
Result QueryXXX(Id resId);
```

Most Query operation are fast (internally resolving a resource Id into
a pointer is a simple array index lookup). Expensive Query operations should
always be marked as such in the function documentation.

### Resource Ids and Pools

Resource objects are typically not allocated one by one on the heap,
but are simple array entries in a **resource pool**. Resource pools
usually cannot grow, instead they are pre-allocated for a maximum
number of resources that can be alive at any one time (although that 
is an implementation detail that may change). Resource objects
are never C++ constructed or destructed while the pool is alive, instead
they only change their resource state (the actual API resource behind the
private resource objects may be created and destroyed though, this depends
on the actual resource implementation in modules).

A **resource Id** consists of 3 components which together form a 64-bit integer:

- a 16 bit index into the resource pool (meaning that at 64k entries is the
maximum size of a resource pool)
- a 16 bit resource type, there is no global resource type enum, instead 
resource types are per-module (e.g. the Gfx module has the GfxResourceType
enum, but the values may collide with other modules), from the view of the
Resource building block classes, the resource type is just a number that is
dragged along in the resource Id
- a 32 bit 'unique stamp': this is a unique number that is incremented for each
created resource of a resource type, the sole purpose of this is to prevent
dangling resource Ids pointing to new resource in the same resource 
pool slot as an old resource the Id was orginally pointing to

### Resource Factories

Resource object are typically initialized by resource **Factories** which
know how to turn a resource from Setup state into Valid (or Failed) state and 
also know how to destroy a resource (bringing it back from the Valid or
Failed state into Initial state).

However, resource factories are not a concept enforced by the Resource
module (there is no resource factory building block class in the Resource
module). Instead, code modules are free to implement resource creation
how ever they want, as long as they adhere to the creation patterns outlined
above (but again, even this is not enforced anywhere).


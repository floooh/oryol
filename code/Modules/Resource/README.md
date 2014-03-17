# Oryol Resources

### Overview

Oryol doesn't have a single, centralizeed resource management system, instead the Resource module
provides generic classes to build small, localized resource management systems inside other modules.

The Oryol resource system provides the following features:

- **resource sharing**: Data which is managed by a resource object only exists once in memory, no
matter how many clients use it. Sharing is optional, it is also possible to force a resource to remain unique.
- **asynchronous or synchronous creation**: Resources can be created asynchronously or synchronously,
asynchronously created resources provide a placeholder as long as the actual resource data is not loaded yet
- **fixed-size object pools**: all resource objects live in fixed-size pools

### Resource Creation

Resource creation is handled with the help of 3 object types:

- **the Setup object**: Setup objects provide all parameters required to create a resource object (for instance
the path to a texture file, or the width, height and pixel format of a render target). The setup object used 
to create a resource will remain valid for the life-time of the resource object, so that the resource data 
can be unloaded and reloaded at later times

- **a Factory object**: A resource factory creates a resource object of a specific type from the creation parameters in a setup object. The actual creation process is delegated to one of multiple loader objects attached to the factory. There is usually exactly one factory object per resource type (e.g. a texture factory, a mesh factory, and so on...)

- **Loader objects**: Loader objects are attached to factories and know how to setup a resource in a specific way
(for instance from a specific file type). Oryol comes with a set of standard loaders, but custom loaders can be
created and attached as well.

### Resource States

A resource object goes through different states during its lifetime:

1. **Initial**: the resource object is in its initial, empty state, and has no Setup object attached yet
2. **Setup**: a Setup object with creation parameters has been attached, but the resource hasn't been 
created/loaded yet
3. **Pending**: a resource goes into the Pending state when it is asynchronously created, and the required
data isn't loaded yet, synchronously created resources never go into the Pending state
4. **Valid**: a resource is valid when it has been successfully created/loaded and is ready to be used
5. **Failed**: a resource goes into the Failed state when it couldn't be created or loaded (for instance because
a required file doesn't exist)

A Valid resource can go back into the Setup state (which basically means it has been unloaded and has
released its resource data). A resource in the Setup state can easily be loaded/created again because all the
required information is still in its Setup object.

Valid or Failed resources can also go back into the Initial state. This happens when a resource is no longer
needed, and its resource pool slot should be freed.

### Ids and Locators

The actual resource objects usually cannot be accessed directly through a pointer, instead they are 
referenced by a small **Id** object which is an opaque handle to a resource. The reason for this is 
that the resource system can return different resource objects for an Id (for instance a placeholder 
as long as a resource is still loading), and that resources live in recycled pool slots, so that a 
simple pointer could point to a slot which no longer holds the original resource object.

Id objects are 8 bytes wide and hold 3 components: a **slot index** into a resource pool,
a **resource type**, and a **unique counter value** which prevents that an old resource id accidentially 
points to a new resource now living in the same pool slot.

**Locators** are essentially strings that are used for resource sharing (resource with identical 
Locators are shared), and if a resource is loaded from a file, the Locator usually also serves
as the filename of the resource.

### Sharing

Resource sharing is implemented in resource **Registry** objects. A Registry is a simple Map with resource Locators as key and resource Ids as value. Entries in a resource registry maintain a use count. If the use count
drop to zero, the resource is discarded and its resource pool slot is freed. Resources that are added to a resource registry can have (a limited number of) dependent resources. The use count of the dependent resources will be incremented and decrement together with the "parent" resource. This basically means that dependent resources are
guaranteed to remain in memory for the lifetime of the parent.

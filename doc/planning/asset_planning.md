### Assets, Resources, Gfx oh my

Resource management in Gfx system is too complex, and too interwoven with
asset loading.

Gfx should only have the following resource features:

- fixed-size resource pools for the various Gfx resource types
- synchronous resource creation from setup objects without data
- synchronous resource creation from setup objects plus data
streams from 'canonical' formats that can be dumped directly
into Gfx resource objects (e.g. raw vertex/index buffer,
raw pixel buffers, raw compressed texture data)

The following Gfx features should be moved out of Gfx:

- asynchronous resource loading
- custom resource loaders
- resource sharing
- resource placeholders

Where to move this to? It would make sense to move this into the
Resource module, but this would mess up module dependencies, 
Gfx would depend on Resource, Resource would depend on Gfx.

Current plan:

NO USE COUNTS ANYMORE.

Use counts are too complicated for dependent resources (e.g.
when a Shader is a dependent resource of a ProgramBundle, 
or an instance mesh is a dependent resource of an instanced mesh.

Instead use resource labels and batch deletion. Each created
resource gets a label slapped on which can be set by
the caller before creating resources. Then there's a DiscardResource(label)
method which deletes all resource matching that label.

- make Resource module even lower level, and introduce a new 
higher level 'Asset' module
- Resource module keeps:
    - resource pools and slots
    - resource base
    - resource factories
    - resource id
    - resource state
- the following current Resource module features are moved out to Asset:
    - resource sharing (ResourceRegistry, Locator, ResourceState)
- the following Resource module concepts are obsolete:
    - loader factories
- Asset will depend on Gfx (and later other subsystems which require 
  shared, asynchronous resources)

The Gfx module needs to expose finer-grained resource functions:
- Id CreateResource(setup): works as before, but only for resources
  that are immediately created (render targets, etc...)
- Id CreateResource(setup, stream): works as before
- Id AllocResource(setup, placeholderId=InvalidId): for asynchronous
  resource creation, allocates a resource slot, optionally takes a 
  placeholder resource which is used as long as the resource is pending
- void SetupResource(id, stream): finish creating an allocated resource


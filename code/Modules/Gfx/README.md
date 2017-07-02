## Gfx Module

### Overview

The Oryol Gfx module is a thin portability wrapper for 3D-APIs and
shader languages. The feature set is defined roughly by GLES3 (WebGL2)
with a fallback to GLES2 (WebGL). The programming model is 
similar to Metal, but with even more simplified resource 
management.

### Platform/API support matrix

Platform   |GL3.3|GLES3|Metal|D3D11
-----------|-----|-----|-----|-----
OSX 10.11+ |YES  |---  |YES  |---  
iOS 9.x+   |---  |YES  |YES  |---  
Window7+   |YES  |---  |---  |YES  
Linux      |YES  |---  |---  |---  
Android    |---  |YES  |---  |---  
HTML5      |---  |YES  |---  |---  
RaspberryPi|---  |YES  |---  |---  

### Sample Code

The most important and detailed documentation of the Gfx module
is the Samples source code.

Looking at those four samples should give you most of the
information necessary to work with the Gfx module:

* [Clear Sample](../../Samples/Clear/Clear.cc)
* [Triangle Sample](../../Samples/Triangle/Triangle.cc)
* [Shapes Sample](../../Samples/Shapes/Shapes.cc)
* [Texture Loading](../../Samples/DDSCubeMap/DDSCubeMap.cc)
* [Offscreen Render Targets](../../Samples/SimpleRenderTarget/SimpleRenderTarget.cc)

### Topics

* [Rendering Backends](doc/RenderBackends.md)
* [A Simple Render Loop](doc/RenderLoop.md)
* [Resource Objects](doc/Resources.md)
    * [Meshes](doc/Meshes.md)
    * [Textures](doc/Textures.md)
    * [Render Passes](doc/RenderPasses.md)
    * [Shaders](doc/Shaders.md)
    * [Pipeline Objects](doc/Pipelines.md)


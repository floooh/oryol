# What's New

- **20-May-2017**: the spirv-tooling branch has been merged into master,
shader code is now generated from a SPIRV intermediate byte code format,
see this blog post for details and required source code changes: http://floooh.github.io/2017/05/15/oryol-spirv.html,
and also check out [the new shader code documentation](../code/Modules/Gfx/doc/Shaders.md).
The change also affects dependencies which include shader code:
  
  - https://github.com/floooh/oryol-imgui
  - https://github.com/floooh/oryol-nuklear
  - https://github.com/floooh/oryol-tbui
  - https://github.com/floooh/oryol-samples
  - https://github.com/floooh/yakc

- **09-Apr-2017**: the big WebGL2/RenderPasses branch has finally been merged,
see this blog post for details:https://floooh.github.io/2017/04/04/oryol-webgl2-merge.html
This change also affects the following git repos:

  - https://github.com/floooh/oryol-imgui
  - https://github.com/floooh/oryol-nuklear
  - https://github.com/floooh/oryol-tbui
  - https://github.com/floooh/oryol-samples
  - https://github.com/floooh/yakc

- **23-Mar-2017**: initial gamepad support on HTML5, OSX, Linux and Windows has
landed, see the Input module README and the new GamepadExplorer sample
for details :)

- **11-Jun-2016**: I have cleaned up the Input module API, unfortunately
this breaks existing code. See the README in code/Modules/Input for 
a complete documentation.

- **29-Apr-2016**: Big Spring Cleanup 2: Exodus

  I have moved some modules and samples out into external
  github repositories, more info here: http://floooh.github.io/2016/04/26/oryol-spring-cleaning.html

- **24-Apr-2016**: Big spring cleanup:
    - the Messaging module has been removed
    - the Synth module has been removed (the recommended audio solution
      for the future is the SoLoud lib)
    - the Time module has been removed and it's classes have been 
      moved into the Core module (under Core/Time)
    - the (empty) OryolClassImpl() macro has been removed
    - I have started to remove the custom integer types like Oryol::uint32,
      since this is cumbersome in external projects, Oryol will use the
      types from stdint.h instead (uint32_t, uint16_t etc)

  All in all this change removed about 4.5k lines of code without loss
  of functionality :)

- **08-Apr-2016**: I have removed the 'git lfs' requirement again, since
it doesn't work with github-pages, and all the big files are in the
github-pages branch, so in the end it didn't make sense 
~~the oryol repo now uses 'git lfs' for some files under
data, and the gh-pages branch for the samples webpage, if you haven't
please install git lfs support (https://git-lfs.github.com/)~~

- **07-Apr-2016**: I have added a new sample which demonstrates how
to integrate the [SoLoud audio lib](http://sol.gfxile.net/soloud/), here's
the web demo: [SoloudTedSid](http://floooh.github.io/oryol/asmjs/SoloudTedSid.html)
 
- **10-Mar-2016**: Some API changes in the Gfx module which break existing 
code have been committed in the last few days, [here's an overview of what
has changed](http://floooh.github.io/2016/03/10/oryol-gfx-changes.html)




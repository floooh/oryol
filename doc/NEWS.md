# What's New

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




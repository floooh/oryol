--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
NVIDIA Texture Tools
README.txt
Version 2.1
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
TABLE OF CONTENTS
--------------------------------------------------------------------------------
I.   Instructions
II.  Contents
III. Compilation Instructions
IV.  Using NVIDIA Texture Tools in your own applications
V.   Known Issues
VI.  Frequently Asked Questions
--------------------------------------------------------------------------------

I.   Introduction
--------------------------------------------------------------------------------

The NVIDIA Texture Tools is a collection of image processing and texture 
manipulation tools, designed to be integrated in game tools and asset 
conditioning pipelines.

The primary features of the library are mipmap and normal map generation, format 
conversion and DXT compression.

DXT compression is based on Simon Brown's squish library. The library also 
contains an alternative GPU-accelerated compressor that uses CUDA and on some
GPUs is one order of magnitude faster.

You can obtain CUDA from our developer site at:

http://developer.nvidia.com/object/cuda.html

The source code of the Texture Tools is being released under the terms of 
the MIT license.

The latest installer is available at the official website:

http://developer.nvidia.com/object/texture_tools.html

Documentation and additional information can be found at the development site:

http://code.google.com/p/nvidia-texture-tools/


II.  Contents
--------------------------------------------------------------------------------

This release contains only the source code of the texture compression library
and several command line applications that serve as examples of how to use the
library. 

Documentation for the texture compression library is available at:

http://code.google.com/p/nvidia-texture-tools/wiki/ApiDocumentation

A brief description of the command line tools can be found at:

http://code.google.com/p/nvidia-texture-tools/wiki/CommandLineTools


III. Compilation Instructions
--------------------------------------------------------------------------------

The compression library and the example can be compiled with Visual Studio 8 on 
Windows using the following solution file:

project\vc8\nvtt.sln

On most other platforms you can also use cmake. For more information about 
cmake, visit:

http://www.cmake.org/

On unix systems you can use the standard build procedure (assuming cmake is 
installed on your system):

$ ./configure
$ make
$ sudo make install


IV.  Using NVIDIA Texture Tools
--------------------------------------------------------------------------------

To use the NVIDIA Texture Tools in your own applications you just have to
include the following header file:

src/nvimage/nvtt/nvtt.h

And include the nvtt library in your projects. 

The following file contains a simple example that shows how to use the library:

src/nvimage/nvtt/compress.cpp

Detailed documentation of the API can be found at:

http://code.google.com/p/nvidia-texture-tools/wiki/ApiDocumentation

The usage of the commandline compression tool is the following:

$ nvcompress [options] infile [outfile]

where 'infile' is and TGA, PNG, PSD, DDS or JPG file, 'outfile' is a DDS file
and 'options' is one or more of the following:

Input options:
  -color     	The input image is a color map (default).
  -alpha     	The input image has an alpha channel used for transparency.
  -normal    	The input image is a normal map.
  -tonormal  	Convert input to normal map.
  -clamp     	Clamp wrapping mode (default).
  -repeat    	Repeat wrapping mode.
  -nomips    	Disable mipmap generation.
  -premula   	Premultiply alpha into color channel.
  -mipfilter 	Mipmap filter. One of the following: box, triangle, kaiser.

Compression options:
  -fast    	Fast compression.
  -nocuda  	Do not use cuda compressor.
  -rgb     	RGBA format
  -bc1     	BC1 format (DXT1)
  -bc1n    	BC1 normal map format (DXT1nm)
  -bc1a    	BC1 format with binary alpha (DXT1a)
  -bc2     	BC2 format (DXT3)
  -bc3     	BC3 format (DXT5)
  -bc3n    	BC3 normal map format (DXT5nm)
  -bc4     	BC4 format (ATI1)
  -bc5     	BC5 format (3Dc/ATI2)

Output options:
  -silent  	Do not output progress messages
  -dds10   	Use DirectX 10 DDS format

In order to run the compiled example on a PC that doesn't have Microsoft Visual 
Studio 2003 installed, you will have to install the Microsoft Visual Studio 2003
redistributable package that you can download at:

http://go.microsoft.com/fwlink/?linkid=65127&clcid=0x409


V.   Known Issues
--------------------------------------------------------------------------------

None so far. Please file suggestions and bug reports at:

http://code.google.com/p/nvidia-texture-tools/issues/list

or contact us directly:

TextureTools@nvidia.com



VI.  Frequently Asked Questions
--------------------------------------------------------------------------------

=== Where can I ask questions? How can I get support? ===
You can ask questions about the usage of the Texture Tools at the [http://developer.nvidia.com/forums/index.php?showforum=9 NVIDIA developer forums]. You can report bugs and request new features in our [http://code.google.com/p/nvidia-texture-tools/issues/list issue database]. If you are a developer and have questions about the API or the source code, feel free to drop by the [http://groups.google.com/group/nvidia-texture-tools developer's mailing list]. If you would like to contact us privately, please send an email to [mailto:TextureTools@nvidia.com TextureTools@nvidia.com].

=== Why is feature XYZ not supported? ===
In order to keep the code small and reduce maintenance costs we have limited the features available in our new texture tools. For this reason, we have also open sourced the code, so that people can modify it and add their own custom features.

=== What platforms do the NVIDIA Texture Tools support? ===
The tools are compiled and tested regularly on Linux, OSX, and Windows. Some platforms are tested more frequently than others and there may be bugs on some uncommon configurations.

=== Is CUDA required? ===
No. The Visual Studio solution file contains a configuration that allows you to compile the texture tools without CUDA support. The cmake scripts automatically detect the CUDA installation and use it only when available.

Even if the texture tools are compiled with CUDA support it's possible to use them on systems that do not support CUDA or that do not have a valid CUDA driver installed.

=== Where can I get CUDA? ===
At [http://developer.nvidia.com/object/cuda.html]

=== Can I use the NVIDIA Texture Tools in my commercial application? ===
Yes, the NVIDIA Texture Tools are licensed under the MIT license.

=== Can I use the NVIDIA Texture Tools in my GPL application? ===
Yes, the MIT license is compatible with the GPL and LGPL licenses.

=== Can I use the NVIDIA Texture Tools in the US? Do I have to obtain a license of the S3TC patent (US patent 5,956,431)? ===
NVIDIA has a license of the S3TC patent that covers all our products, including our Texture Tools. You don't have to obtain a license of the S3TC patent to use any of NVIDIA's products, but certain uses of NVIDIA Texture Tools source code cannot be considered NVIDIA products anymore. Keep in mind that the NVIDIA Texture Tools are licensed under the MIT license and thus are provided without warranty of any kind.

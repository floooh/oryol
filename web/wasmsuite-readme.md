### Oryol Samples

This is the complete set of Oryol samples compiled to wasm, wast and asm.js
with the original emscripten shell-html files which fall back to 
asm.js if wasm is not available.

The samples are built with Binaryen, using the method "wasm-native,asmjs".

Most samples require a WebGL canvas.

The original sample web page is here: http://floooh.github.io/oryol/

#### Testing

The easiest way to test the samples is to start a python web server
inside this directory via **python -m SimpleHTTPServer**, 
point a web browser to http://localhost:8000 and click on one
of the HTML files. 

This tries to load the wasm samples, and if these are not available
falls back to asm.js.

The only demo that runs out of the box in a vanilla JS engine is 'hello.js':

```
> node hello.js
```

#### How to build

Best built on Linux or OSX (Windows might work but is not tested with
WebAssembly builds).

You need **make**, **cmake**, **python** (2.7.x recommended, 3.x should work too).

Get Oryol and setup emscripten with WebAssembly support as described here:

http://floooh.github.io/2016/03/17/oryol-webassembly.html

Build everything from inside the oryol directory with:

```
> ./fips wasmtest build
...
```

Start a browser pointing to the build result with:

```
> ./fips wasmtest serve
...
```

To find the directory with the build result:

```
> cd ../fips-deploy/oryol-wasm-buildsuite/
```


function integrateWasmJS(Module) {
 var method = Module["wasmJSMethod"] || Module["wasmJSMethod"] || "native-wasm" || "native-wasm,wasm-s-parser";
 var wasmTextFile = Module["wasmTextFile"] || "DrawCallExplorer.wasm";
 var wasmBinaryFile = Module["wasmBinaryFile"] || "DrawCallExplorer.wasm";
 var asmjsCodeFile = Module["asmjsCodeFile"] || "DrawCallExplorer.asm.js";
 var asm2wasmImports = {
  "f64-rem": (function(x, y) {
   return x % y;
  }),
  "f64-to-int": (function(x) {
   return x | 0;
  }),
  "debugger": (function() {
   debugger;
  })
 };
 var info = {
  global: null,
  env: null,
  asm2wasm: asm2wasmImports,
  parent: Module
 };
 function lookupImport(mod, base) {
  var lookup = info;
  if (mod.indexOf(".") < 0) {
   lookup = (lookup || {})[mod];
  } else {
   var parts = mod.split(".");
   lookup = (lookup || {})[parts[0]];
   lookup = (lookup || {})[parts[1]];
  }
  if (base) {
   lookup = (lookup || {})[base];
  }
  if (lookup === undefined) {
   abort("bad lookupImport to (" + mod + ")." + base);
  }
  return lookup;
 }
 function mergeMemory(newBuffer) {
  var oldBuffer = Module["buffer"];
  if (newBuffer.byteLength < oldBuffer.byteLength) {
   Module["printErr"]("the new buffer in mergeMemory is smaller than the previous one. in native wasm, we should grow memory here");
  }
  var oldView = new Int8Array(oldBuffer);
  var newView = new Int8Array(newBuffer);
  if (0) {
   oldView.set(newView.subarray(STATIC_BASE, STATIC_BASE + STATIC_BUMP), STATIC_BASE);
  }
  newView.set(oldView);
  updateGlobalBuffer(newBuffer);
  updateGlobalBufferViews();
  Module["reallocBuffer"] = (function(size) {
   var old = Module["buffer"];
   wasmJS["asmExports"]["__growWasmMemory"](size);
   return Module["buffer"] !== old ? Module["buffer"] : null;
  });
 }
 var WasmTypes = {
  none: 0,
  i32: 1,
  i64: 2,
  f32: 3,
  f64: 4
 };
 function applyMappedGlobals(globalsFileBase) {
  var mappedGlobals = JSON.parse(Module["read"](globalsFileBase + ".mappedGlobals"));
  for (var name in mappedGlobals) {
   var global = mappedGlobals[name];
   if (!global.import) continue;
   var value = lookupImport(global.module, global.base);
   var address = global.address;
   switch (global.type) {
   case WasmTypes.i32:
    Module["HEAP32"][address >> 2] = value;
    break;
   case WasmTypes.f32:
    Module["HEAPF32"][address >> 2] = value;
    break;
   case WasmTypes.f64:
    Module["HEAPF64"][address >> 3] = value;
    break;
   default:
    abort();
   }
  }
 }
 function fixImports(imports) {
  if (!0) return imports;
  var ret = {};
  for (var i in imports) {
   var fixed = i;
   if (fixed[0] == "_") fixed = fixed.substr(1);
   ret[fixed] = imports[i];
  }
  return ret;
 }
 function getBinary() {
  var binary;
  if (ENVIRONMENT_IS_WEB || ENVIRONMENT_IS_WORKER) {
   binary = Module["wasmBinary"];
   assert(binary, "on the web, we need the wasm binary to be preloaded and set on Module['wasmBinary']. emcc.py will do that for you when generating HTML (but not JS)");
   binary = new Uint8Array(binary);
  } else {
   binary = Module["readBinary"](wasmBinaryFile);
  }
  return binary;
 }
 function doJustAsm() {
  eval(Module["read"](asmjsCodeFile));
  if (typeof Module["asm"] !== "function") {
   Module["printErr"]("asm evalling did not set the module properly");
   return false;
  }
  return true;
 }
 function doNativeWasm() {
  if (typeof Wasm !== "object") {
   Module["printErr"]("no native wasm support detected");
   return false;
  }
  Module["asm"] = (function(global, env, providedBuffer) {
   global = fixImports(global);
   env = fixImports(env);
   info["global"] = {
    "NaN": NaN,
    "Infinity": Infinity
   };
   info["global.Math"] = global.Math;
   info["env"] = env;
   var instance;
   instance = Wasm.instantiateModule(getBinary(), info);
   mergeMemory(instance.exports.memory);
   applyMappedGlobals(wasmBinaryFile);
   return instance.exports;
  });
  return true;
 }
 function doWasmPolyfill(method) {
  if (typeof WasmJS !== "function") {
   Module["printErr"]("WasmJS not detected - polyfill not bundled?");
   return false;
  }
  var wasmJS = WasmJS({});
  wasmJS["outside"] = Module;
  wasmJS["info"] = info;
  wasmJS["lookupImport"] = lookupImport;
  Module["asm"] = (function(global, env, providedBuffer) {
   global = fixImports(global);
   env = fixImports(env);
   assert(providedBuffer === Module["buffer"]);
   info.global = global;
   info.env = env;
   Module["reallocBuffer"] = (function(size) {
    var old = Module["buffer"];
    wasmJS["asmExports"]["__growWasmMemory"](size);
    return Module["buffer"] !== old ? Module["buffer"] : null;
   });
   wasmJS["providedTotalMemory"] = Module["buffer"].byteLength;
   var code;
   if (method === "wasm-binary") {
    code = getBinary();
   } else {
    code = Module["read"](method == "asm2wasm" ? asmjsCodeFile : wasmTextFile);
   }
   var temp;
   if (method == "asm2wasm") {
    temp = wasmJS["_malloc"](code.length + 1);
    wasmJS["writeAsciiToMemory"](code, temp);
    wasmJS["_load_asm2wasm"](temp);
   } else if (method === "wasm-s-parser") {
    temp = wasmJS["_malloc"](code.length + 1);
    wasmJS["writeAsciiToMemory"](code, temp);
    wasmJS["_load_s_expr2wasm"](temp);
   } else if (method === "wasm-binary") {
    temp = wasmJS["_malloc"](code.length);
    wasmJS["HEAPU8"].set(code, temp);
    wasmJS["_load_binary2wasm"](temp, code.length);
   } else {
    throw "what? " + method;
   }
   wasmJS["_free"](temp);
   wasmJS["_instantiate"](temp);
   if (Module["newBuffer"]) {
    mergeMemory(Module["newBuffer"]);
    Module["newBuffer"] = null;
   }
   if (method == "wasm-s-parser") {
    applyMappedGlobals(wasmTextFile);
   } else if (method == "wasm-binary") {
    applyMappedGlobals(wasmBinaryFile);
   }
   return wasmJS["asmExports"];
  });
  return true;
 }
 var methods = method.split(",");
 for (var i = 0; i < methods.length; i++) {
  var curr = methods[i];
  if (curr === "native-wasm") {
   if (doNativeWasm()) return;
  } else if (curr === "just-asm") {
   if (doJustAsm()) return;
  } else if (curr === "asm2wasm" || curr === "wasm-s-parser" || curr === "wasm-binary") {
   if (doWasmPolyfill(curr)) return;
  } else {
   throw "bad method: " + curr;
  }
 }
 throw "no wasm method succeeded";
}
var Module;
if (!Module) Module = (typeof Module !== "undefined" ? Module : null) || {};
var moduleOverrides = {};
for (var key in Module) {
 if (Module.hasOwnProperty(key)) {
  moduleOverrides[key] = Module[key];
 }
}
var ENVIRONMENT_IS_WEB = false;
var ENVIRONMENT_IS_WORKER = false;
var ENVIRONMENT_IS_NODE = false;
var ENVIRONMENT_IS_SHELL = false;
if (Module["ENVIRONMENT"]) {
 if (Module["ENVIRONMENT"] === "WEB") {
  ENVIRONMENT_IS_WEB = true;
 } else if (Module["ENVIRONMENT"] === "WORKER") {
  ENVIRONMENT_IS_WORKER = true;
 } else if (Module["ENVIRONMENT"] === "NODE") {
  ENVIRONMENT_IS_NODE = true;
 } else if (Module["ENVIRONMENT"] === "SHELL") {
  ENVIRONMENT_IS_SHELL = true;
 } else {
  throw new Error("The provided Module['ENVIRONMENT'] value is not valid. It must be one of: WEB|WORKER|NODE|SHELL.");
 }
} else {
 ENVIRONMENT_IS_WEB = typeof window === "object";
 ENVIRONMENT_IS_WORKER = typeof importScripts === "function";
 ENVIRONMENT_IS_NODE = typeof process === "object" && typeof require === "function" && !ENVIRONMENT_IS_WEB && !ENVIRONMENT_IS_WORKER;
 ENVIRONMENT_IS_SHELL = !ENVIRONMENT_IS_WEB && !ENVIRONMENT_IS_NODE && !ENVIRONMENT_IS_WORKER;
}
if (ENVIRONMENT_IS_NODE) {
 if (!Module["print"]) Module["print"] = console.log;
 if (!Module["printErr"]) Module["printErr"] = console.warn;
 var nodeFS;
 var nodePath;
 Module["read"] = function read(filename, binary) {
  if (!nodeFS) nodeFS = require("fs");
  if (!nodePath) nodePath = require("path");
  filename = nodePath["normalize"](filename);
  var ret = nodeFS["readFileSync"](filename);
  if (!ret && filename != nodePath["resolve"](filename)) {
   filename = path.join(__dirname, "..", "src", filename);
   ret = nodeFS["readFileSync"](filename);
  }
  if (ret && !binary) ret = ret.toString();
  return ret;
 };
 Module["readBinary"] = function readBinary(filename) {
  var ret = Module["read"](filename, true);
  if (!ret.buffer) {
   ret = new Uint8Array(ret);
  }
  assert(ret.buffer);
  return ret;
 };
 Module["load"] = function load(f) {
  globalEval(read(f));
 };
 if (!Module["thisProgram"]) {
  if (process["argv"].length > 1) {
   Module["thisProgram"] = process["argv"][1].replace(/\\/g, "/");
  } else {
   Module["thisProgram"] = "unknown-program";
  }
 }
 Module["arguments"] = process["argv"].slice(2);
 if (typeof module !== "undefined") {
  module["exports"] = Module;
 }
 process["on"]("uncaughtException", (function(ex) {
  if (!(ex instanceof ExitStatus)) {
   throw ex;
  }
 }));
 Module["inspect"] = (function() {
  return "[Emscripten Module object]";
 });
} else if (ENVIRONMENT_IS_SHELL) {
 if (!Module["print"]) Module["print"] = print;
 if (typeof printErr != "undefined") Module["printErr"] = printErr;
 if (typeof read != "undefined") {
  Module["read"] = read;
 } else {
  Module["read"] = function read() {
   throw "no read() available (jsc?)";
  };
 }
 Module["readBinary"] = function readBinary(f) {
  if (typeof readbuffer === "function") {
   return new Uint8Array(readbuffer(f));
  }
  var data = read(f, "binary");
  assert(typeof data === "object");
  return data;
 };
 if (typeof scriptArgs != "undefined") {
  Module["arguments"] = scriptArgs;
 } else if (typeof arguments != "undefined") {
  Module["arguments"] = arguments;
 }
} else if (ENVIRONMENT_IS_WEB || ENVIRONMENT_IS_WORKER) {
 Module["read"] = function read(url) {
  var xhr = new XMLHttpRequest;
  xhr.open("GET", url, false);
  xhr.send(null);
  return xhr.responseText;
 };
 Module["readAsync"] = function readAsync(url, onload, onerror) {
  var xhr = new XMLHttpRequest;
  xhr.open("GET", url, true);
  xhr.responseType = "arraybuffer";
  xhr.onload = function xhr_onload() {
   if (xhr.status == 200 || xhr.status == 0 && xhr.response) {
    onload(xhr.response);
   } else {
    onerror();
   }
  };
  xhr.onerror = onerror;
  xhr.send(null);
 };
 if (typeof arguments != "undefined") {
  Module["arguments"] = arguments;
 }
 if (typeof console !== "undefined") {
  if (!Module["print"]) Module["print"] = function print(x) {
   console.log(x);
  };
  if (!Module["printErr"]) Module["printErr"] = function printErr(x) {
   console.warn(x);
  };
 } else {
  var TRY_USE_DUMP = false;
  if (!Module["print"]) Module["print"] = TRY_USE_DUMP && typeof dump !== "undefined" ? (function(x) {
   dump(x);
  }) : (function(x) {});
 }
 if (ENVIRONMENT_IS_WORKER) {
  Module["load"] = importScripts;
 }
 if (typeof Module["setWindowTitle"] === "undefined") {
  Module["setWindowTitle"] = (function(title) {
   document.title = title;
  });
 }
} else {
 throw "Unknown runtime environment. Where are we?";
}
function globalEval(x) {
 eval.call(null, x);
}
if (!Module["load"] && Module["read"]) {
 Module["load"] = function load(f) {
  globalEval(Module["read"](f));
 };
}
if (!Module["print"]) {
 Module["print"] = (function() {});
}
if (!Module["printErr"]) {
 Module["printErr"] = Module["print"];
}
if (!Module["arguments"]) {
 Module["arguments"] = [];
}
if (!Module["thisProgram"]) {
 Module["thisProgram"] = "./this.program";
}
Module.print = Module["print"];
Module.printErr = Module["printErr"];
Module["preRun"] = [];
Module["postRun"] = [];
for (var key in moduleOverrides) {
 if (moduleOverrides.hasOwnProperty(key)) {
  Module[key] = moduleOverrides[key];
 }
}
moduleOverrides = undefined;
integrateWasmJS(Module);
var Runtime = {
 setTempRet0: (function(value) {
  tempRet0 = value;
 }),
 getTempRet0: (function() {
  return tempRet0;
 }),
 stackSave: (function() {
  return STACKTOP;
 }),
 stackRestore: (function(stackTop) {
  STACKTOP = stackTop;
 }),
 getNativeTypeSize: (function(type) {
  switch (type) {
  case "i1":
  case "i8":
   return 1;
  case "i16":
   return 2;
  case "i32":
   return 4;
  case "i64":
   return 8;
  case "float":
   return 4;
  case "double":
   return 8;
  default:
   {
    if (type[type.length - 1] === "*") {
     return Runtime.QUANTUM_SIZE;
    } else if (type[0] === "i") {
     var bits = parseInt(type.substr(1));
     assert(bits % 8 === 0);
     return bits / 8;
    } else {
     return 0;
    }
   }
  }
 }),
 getNativeFieldSize: (function(type) {
  return Math.max(Runtime.getNativeTypeSize(type), Runtime.QUANTUM_SIZE);
 }),
 STACK_ALIGN: 16,
 prepVararg: (function(ptr, type) {
  if (type === "double" || type === "i64") {
   if (ptr & 7) {
    assert((ptr & 7) === 4);
    ptr += 4;
   }
  } else {
   assert((ptr & 3) === 0);
  }
  return ptr;
 }),
 getAlignSize: (function(type, size, vararg) {
  if (!vararg && (type == "i64" || type == "double")) return 8;
  if (!type) return Math.min(size, 8);
  return Math.min(size || (type ? Runtime.getNativeFieldSize(type) : 0), Runtime.QUANTUM_SIZE);
 }),
 dynCall: (function(sig, ptr, args) {
  if (args && args.length) {
   if (!args.splice) args = Array.prototype.slice.call(args);
   args.splice(0, 0, ptr);
   return Module["dynCall_" + sig].apply(null, args);
  } else {
   return Module["dynCall_" + sig].call(null, ptr);
  }
 }),
 functionPointers: [],
 addFunction: (function(func) {
  for (var i = 0; i < Runtime.functionPointers.length; i++) {
   if (!Runtime.functionPointers[i]) {
    Runtime.functionPointers[i] = func;
    return 2 * (1 + i);
   }
  }
  throw "Finished up all reserved function pointers. Use a higher value for RESERVED_FUNCTION_POINTERS.";
 }),
 removeFunction: (function(index) {
  Runtime.functionPointers[(index - 2) / 2] = null;
 }),
 warnOnce: (function(text) {
  if (!Runtime.warnOnce.shown) Runtime.warnOnce.shown = {};
  if (!Runtime.warnOnce.shown[text]) {
   Runtime.warnOnce.shown[text] = 1;
   Module.printErr(text);
  }
 }),
 funcWrappers: {},
 getFuncWrapper: (function(func, sig) {
  assert(sig);
  if (!Runtime.funcWrappers[sig]) {
   Runtime.funcWrappers[sig] = {};
  }
  var sigCache = Runtime.funcWrappers[sig];
  if (!sigCache[func]) {
   sigCache[func] = function dynCall_wrapper() {
    return Runtime.dynCall(sig, func, arguments);
   };
  }
  return sigCache[func];
 }),
 getCompilerSetting: (function(name) {
  throw "You must build with -s RETAIN_COMPILER_SETTINGS=1 for Runtime.getCompilerSetting or emscripten_get_compiler_setting to work";
 }),
 stackAlloc: (function(size) {
  var ret = STACKTOP;
  STACKTOP = STACKTOP + size | 0;
  STACKTOP = STACKTOP + 15 & -16;
  return ret;
 }),
 staticAlloc: (function(size) {
  var ret = STATICTOP;
  STATICTOP = STATICTOP + size | 0;
  STATICTOP = STATICTOP + 15 & -16;
  return ret;
 }),
 dynamicAlloc: (function(size) {
  var ret = DYNAMICTOP;
  DYNAMICTOP = DYNAMICTOP + size | 0;
  DYNAMICTOP = DYNAMICTOP + 15 & -16;
  if (DYNAMICTOP >= TOTAL_MEMORY) {
   var success = enlargeMemory();
   if (!success) {
    DYNAMICTOP = ret;
    return 0;
   }
  }
  return ret;
 }),
 alignMemory: (function(size, quantum) {
  var ret = size = Math.ceil(size / (quantum ? quantum : 16)) * (quantum ? quantum : 16);
  return ret;
 }),
 makeBigInt: (function(low, high, unsigned) {
  var ret = unsigned ? +(low >>> 0) + +(high >>> 0) * +4294967296 : +(low >>> 0) + +(high | 0) * +4294967296;
  return ret;
 }),
 GLOBAL_BASE: 1024,
 QUANTUM_SIZE: 4,
 __dummy__: 0
};
Module["Runtime"] = Runtime;
var ABORT = false;
var EXITSTATUS = 0;
function assert(condition, text) {
 if (!condition) {
  abort("Assertion failed: " + text);
 }
}
function getCFunc(ident) {
 var func = Module["_" + ident];
 if (!func) {
  try {
   func = eval("_" + ident);
  } catch (e) {}
 }
 assert(func, "Cannot call unknown function " + ident + " (perhaps LLVM optimizations or closure removed it?)");
 return func;
}
var cwrap, ccall;
((function() {
 var JSfuncs = {
  "stackSave": (function() {
   Runtime.stackSave();
  }),
  "stackRestore": (function() {
   Runtime.stackRestore();
  }),
  "arrayToC": (function(arr) {
   var ret = Runtime.stackAlloc(arr.length);
   writeArrayToMemory(arr, ret);
   return ret;
  }),
  "stringToC": (function(str) {
   var ret = 0;
   if (str !== null && str !== undefined && str !== 0) {
    ret = Runtime.stackAlloc((str.length << 2) + 1);
    writeStringToMemory(str, ret);
   }
   return ret;
  })
 };
 var toC = {
  "string": JSfuncs["stringToC"],
  "array": JSfuncs["arrayToC"]
 };
 ccall = function ccallFunc(ident, returnType, argTypes, args, opts) {
  var func = getCFunc(ident);
  var cArgs = [];
  var stack = 0;
  if (args) {
   for (var i = 0; i < args.length; i++) {
    var converter = toC[argTypes[i]];
    if (converter) {
     if (stack === 0) stack = Runtime.stackSave();
     cArgs[i] = converter(args[i]);
    } else {
     cArgs[i] = args[i];
    }
   }
  }
  var ret = func.apply(null, cArgs);
  if (returnType === "string") ret = Pointer_stringify(ret);
  if (stack !== 0) {
   if (opts && opts.async) {
    EmterpreterAsync.asyncFinalizers.push((function() {
     Runtime.stackRestore(stack);
    }));
    return;
   }
   Runtime.stackRestore(stack);
  }
  return ret;
 };
 var sourceRegex = /^function\s*[a-zA-Z$_0-9]*\s*\(([^)]*)\)\s*{\s*([^*]*?)[\s;]*(?:return\s*(.*?)[;\s]*)?}$/;
 function parseJSFunc(jsfunc) {
  var parsed = jsfunc.toString().match(sourceRegex).slice(1);
  return {
   arguments: parsed[0],
   body: parsed[1],
   returnValue: parsed[2]
  };
 }
 var JSsource = null;
 function ensureJSsource() {
  if (!JSsource) {
   JSsource = {};
   for (var fun in JSfuncs) {
    if (JSfuncs.hasOwnProperty(fun)) {
     JSsource[fun] = parseJSFunc(JSfuncs[fun]);
    }
   }
  }
 }
 cwrap = function cwrap(ident, returnType, argTypes) {
  argTypes = argTypes || [];
  var cfunc = getCFunc(ident);
  var numericArgs = argTypes.every((function(type) {
   return type === "number";
  }));
  var numericRet = returnType !== "string";
  if (numericRet && numericArgs) {
   return cfunc;
  }
  var argNames = argTypes.map((function(x, i) {
   return "$" + i;
  }));
  var funcstr = "(function(" + argNames.join(",") + ") {";
  var nargs = argTypes.length;
  if (!numericArgs) {
   ensureJSsource();
   funcstr += "var stack = " + JSsource["stackSave"].body + ";";
   for (var i = 0; i < nargs; i++) {
    var arg = argNames[i], type = argTypes[i];
    if (type === "number") continue;
    var convertCode = JSsource[type + "ToC"];
    funcstr += "var " + convertCode.arguments + " = " + arg + ";";
    funcstr += convertCode.body + ";";
    funcstr += arg + "=(" + convertCode.returnValue + ");";
   }
  }
  var cfuncname = parseJSFunc((function() {
   return cfunc;
  })).returnValue;
  funcstr += "var ret = " + cfuncname + "(" + argNames.join(",") + ");";
  if (!numericRet) {
   var strgfy = parseJSFunc((function() {
    return Pointer_stringify;
   })).returnValue;
   funcstr += "ret = " + strgfy + "(ret);";
  }
  if (!numericArgs) {
   ensureJSsource();
   funcstr += JSsource["stackRestore"].body.replace("()", "(stack)") + ";";
  }
  funcstr += "return ret})";
  return eval(funcstr);
 };
}))();
Module["ccall"] = ccall;
Module["cwrap"] = cwrap;
function setValue(ptr, value, type, noSafe) {
 type = type || "i8";
 if (type.charAt(type.length - 1) === "*") type = "i32";
 switch (type) {
 case "i1":
  HEAP8[ptr >> 0] = value;
  break;
 case "i8":
  HEAP8[ptr >> 0] = value;
  break;
 case "i16":
  HEAP16[ptr >> 1] = value;
  break;
 case "i32":
  HEAP32[ptr >> 2] = value;
  break;
 case "i64":
  tempI64 = [ value >>> 0, (tempDouble = value, +Math_abs(tempDouble) >= +1 ? tempDouble > +0 ? (Math_min(+Math_floor(tempDouble / +4294967296), +4294967295) | 0) >>> 0 : ~~+Math_ceil((tempDouble - +(~~tempDouble >>> 0)) / +4294967296) >>> 0 : 0) ], HEAP32[ptr >> 2] = tempI64[0], HEAP32[ptr + 4 >> 2] = tempI64[1];
  break;
 case "float":
  HEAPF32[ptr >> 2] = value;
  break;
 case "double":
  HEAPF64[ptr >> 3] = value;
  break;
 default:
  abort("invalid type for setValue: " + type);
 }
}
Module["setValue"] = setValue;
function getValue(ptr, type, noSafe) {
 type = type || "i8";
 if (type.charAt(type.length - 1) === "*") type = "i32";
 switch (type) {
 case "i1":
  return HEAP8[ptr >> 0];
 case "i8":
  return HEAP8[ptr >> 0];
 case "i16":
  return HEAP16[ptr >> 1];
 case "i32":
  return HEAP32[ptr >> 2];
 case "i64":
  return HEAP32[ptr >> 2];
 case "float":
  return HEAPF32[ptr >> 2];
 case "double":
  return HEAPF64[ptr >> 3];
 default:
  abort("invalid type for setValue: " + type);
 }
 return null;
}
Module["getValue"] = getValue;
var ALLOC_NORMAL = 0;
var ALLOC_STACK = 1;
var ALLOC_STATIC = 2;
var ALLOC_DYNAMIC = 3;
var ALLOC_NONE = 4;
Module["ALLOC_NORMAL"] = ALLOC_NORMAL;
Module["ALLOC_STACK"] = ALLOC_STACK;
Module["ALLOC_STATIC"] = ALLOC_STATIC;
Module["ALLOC_DYNAMIC"] = ALLOC_DYNAMIC;
Module["ALLOC_NONE"] = ALLOC_NONE;
function allocate(slab, types, allocator, ptr) {
 var zeroinit, size;
 if (typeof slab === "number") {
  zeroinit = true;
  size = slab;
 } else {
  zeroinit = false;
  size = slab.length;
 }
 var singleType = typeof types === "string" ? types : null;
 var ret;
 if (allocator == ALLOC_NONE) {
  ret = ptr;
 } else {
  ret = [ typeof _malloc === "function" ? _malloc : Runtime.staticAlloc, Runtime.stackAlloc, Runtime.staticAlloc, Runtime.dynamicAlloc ][allocator === undefined ? ALLOC_STATIC : allocator](Math.max(size, singleType ? 1 : types.length));
 }
 if (zeroinit) {
  var ptr = ret, stop;
  assert((ret & 3) == 0);
  stop = ret + (size & ~3);
  for (; ptr < stop; ptr += 4) {
   HEAP32[ptr >> 2] = 0;
  }
  stop = ret + size;
  while (ptr < stop) {
   HEAP8[ptr++ >> 0] = 0;
  }
  return ret;
 }
 if (singleType === "i8") {
  if (slab.subarray || slab.slice) {
   HEAPU8.set(slab, ret);
  } else {
   HEAPU8.set(new Uint8Array(slab), ret);
  }
  return ret;
 }
 var i = 0, type, typeSize, previousType;
 while (i < size) {
  var curr = slab[i];
  if (typeof curr === "function") {
   curr = Runtime.getFunctionIndex(curr);
  }
  type = singleType || types[i];
  if (type === 0) {
   i++;
   continue;
  }
  if (type == "i64") type = "i32";
  setValue(ret + i, curr, type);
  if (previousType !== type) {
   typeSize = Runtime.getNativeTypeSize(type);
   previousType = type;
  }
  i += typeSize;
 }
 return ret;
}
Module["allocate"] = allocate;
function getMemory(size) {
 if (!staticSealed) return Runtime.staticAlloc(size);
 if (typeof _sbrk !== "undefined" && !_sbrk.called || !runtimeInitialized) return Runtime.dynamicAlloc(size);
 return _malloc(size);
}
Module["getMemory"] = getMemory;
function Pointer_stringify(ptr, length) {
 if (length === 0 || !ptr) return "";
 var hasUtf = 0;
 var t;
 var i = 0;
 while (1) {
  t = HEAPU8[ptr + i >> 0];
  hasUtf |= t;
  if (t == 0 && !length) break;
  i++;
  if (length && i == length) break;
 }
 if (!length) length = i;
 var ret = "";
 if (hasUtf < 128) {
  var MAX_CHUNK = 1024;
  var curr;
  while (length > 0) {
   curr = String.fromCharCode.apply(String, HEAPU8.subarray(ptr, ptr + Math.min(length, MAX_CHUNK)));
   ret = ret ? ret + curr : curr;
   ptr += MAX_CHUNK;
   length -= MAX_CHUNK;
  }
  return ret;
 }
 return Module["UTF8ToString"](ptr);
}
Module["Pointer_stringify"] = Pointer_stringify;
function AsciiToString(ptr) {
 var str = "";
 while (1) {
  var ch = HEAP8[ptr++ >> 0];
  if (!ch) return str;
  str += String.fromCharCode(ch);
 }
}
Module["AsciiToString"] = AsciiToString;
function stringToAscii(str, outPtr) {
 return writeAsciiToMemory(str, outPtr, false);
}
Module["stringToAscii"] = stringToAscii;
function UTF8ArrayToString(u8Array, idx) {
 var u0, u1, u2, u3, u4, u5;
 var str = "";
 while (1) {
  u0 = u8Array[idx++];
  if (!u0) return str;
  if (!(u0 & 128)) {
   str += String.fromCharCode(u0);
   continue;
  }
  u1 = u8Array[idx++] & 63;
  if ((u0 & 224) == 192) {
   str += String.fromCharCode((u0 & 31) << 6 | u1);
   continue;
  }
  u2 = u8Array[idx++] & 63;
  if ((u0 & 240) == 224) {
   u0 = (u0 & 15) << 12 | u1 << 6 | u2;
  } else {
   u3 = u8Array[idx++] & 63;
   if ((u0 & 248) == 240) {
    u0 = (u0 & 7) << 18 | u1 << 12 | u2 << 6 | u3;
   } else {
    u4 = u8Array[idx++] & 63;
    if ((u0 & 252) == 248) {
     u0 = (u0 & 3) << 24 | u1 << 18 | u2 << 12 | u3 << 6 | u4;
    } else {
     u5 = u8Array[idx++] & 63;
     u0 = (u0 & 1) << 30 | u1 << 24 | u2 << 18 | u3 << 12 | u4 << 6 | u5;
    }
   }
  }
  if (u0 < 65536) {
   str += String.fromCharCode(u0);
  } else {
   var ch = u0 - 65536;
   str += String.fromCharCode(55296 | ch >> 10, 56320 | ch & 1023);
  }
 }
}
Module["UTF8ArrayToString"] = UTF8ArrayToString;
function UTF8ToString(ptr) {
 return UTF8ArrayToString(HEAPU8, ptr);
}
Module["UTF8ToString"] = UTF8ToString;
function stringToUTF8Array(str, outU8Array, outIdx, maxBytesToWrite) {
 if (!(maxBytesToWrite > 0)) return 0;
 var startIdx = outIdx;
 var endIdx = outIdx + maxBytesToWrite - 1;
 for (var i = 0; i < str.length; ++i) {
  var u = str.charCodeAt(i);
  if (u >= 55296 && u <= 57343) u = 65536 + ((u & 1023) << 10) | str.charCodeAt(++i) & 1023;
  if (u <= 127) {
   if (outIdx >= endIdx) break;
   outU8Array[outIdx++] = u;
  } else if (u <= 2047) {
   if (outIdx + 1 >= endIdx) break;
   outU8Array[outIdx++] = 192 | u >> 6;
   outU8Array[outIdx++] = 128 | u & 63;
  } else if (u <= 65535) {
   if (outIdx + 2 >= endIdx) break;
   outU8Array[outIdx++] = 224 | u >> 12;
   outU8Array[outIdx++] = 128 | u >> 6 & 63;
   outU8Array[outIdx++] = 128 | u & 63;
  } else if (u <= 2097151) {
   if (outIdx + 3 >= endIdx) break;
   outU8Array[outIdx++] = 240 | u >> 18;
   outU8Array[outIdx++] = 128 | u >> 12 & 63;
   outU8Array[outIdx++] = 128 | u >> 6 & 63;
   outU8Array[outIdx++] = 128 | u & 63;
  } else if (u <= 67108863) {
   if (outIdx + 4 >= endIdx) break;
   outU8Array[outIdx++] = 248 | u >> 24;
   outU8Array[outIdx++] = 128 | u >> 18 & 63;
   outU8Array[outIdx++] = 128 | u >> 12 & 63;
   outU8Array[outIdx++] = 128 | u >> 6 & 63;
   outU8Array[outIdx++] = 128 | u & 63;
  } else {
   if (outIdx + 5 >= endIdx) break;
   outU8Array[outIdx++] = 252 | u >> 30;
   outU8Array[outIdx++] = 128 | u >> 24 & 63;
   outU8Array[outIdx++] = 128 | u >> 18 & 63;
   outU8Array[outIdx++] = 128 | u >> 12 & 63;
   outU8Array[outIdx++] = 128 | u >> 6 & 63;
   outU8Array[outIdx++] = 128 | u & 63;
  }
 }
 outU8Array[outIdx] = 0;
 return outIdx - startIdx;
}
Module["stringToUTF8Array"] = stringToUTF8Array;
function stringToUTF8(str, outPtr, maxBytesToWrite) {
 return stringToUTF8Array(str, HEAPU8, outPtr, maxBytesToWrite);
}
Module["stringToUTF8"] = stringToUTF8;
function lengthBytesUTF8(str) {
 var len = 0;
 for (var i = 0; i < str.length; ++i) {
  var u = str.charCodeAt(i);
  if (u >= 55296 && u <= 57343) u = 65536 + ((u & 1023) << 10) | str.charCodeAt(++i) & 1023;
  if (u <= 127) {
   ++len;
  } else if (u <= 2047) {
   len += 2;
  } else if (u <= 65535) {
   len += 3;
  } else if (u <= 2097151) {
   len += 4;
  } else if (u <= 67108863) {
   len += 5;
  } else {
   len += 6;
  }
 }
 return len;
}
Module["lengthBytesUTF8"] = lengthBytesUTF8;
function demangle(func) {
 var hasLibcxxabi = !!Module["___cxa_demangle"];
 if (hasLibcxxabi) {
  try {
   var buf = _malloc(func.length);
   writeStringToMemory(func.substr(1), buf);
   var status = _malloc(4);
   var ret = Module["___cxa_demangle"](buf, 0, 0, status);
   if (getValue(status, "i32") === 0 && ret) {
    return Pointer_stringify(ret);
   }
  } catch (e) {
   return func;
  } finally {
   if (buf) _free(buf);
   if (status) _free(status);
   if (ret) _free(ret);
  }
 }
 Runtime.warnOnce("warning: build with  -s DEMANGLE_SUPPORT=1  to link in libcxxabi demangling");
 return func;
}
function demangleAll(text) {
 return text.replace(/__Z[\w\d_]+/g, (function(x) {
  var y = demangle(x);
  return x === y ? x : x + " [" + y + "]";
 }));
}
function jsStackTrace() {
 var err = new Error;
 if (!err.stack) {
  try {
   throw new Error(0);
  } catch (e) {
   err = e;
  }
  if (!err.stack) {
   return "(no stack trace available)";
  }
 }
 return err.stack.toString();
}
function stackTrace() {
 return demangleAll(jsStackTrace());
}
Module["stackTrace"] = stackTrace;
function alignMemoryPage(x) {
 if (x % 4096 > 0) {
  x += 4096 - x % 4096;
 }
 return x;
}
var HEAP;
var buffer;
var HEAP8, HEAPU8, HEAP16, HEAPU16, HEAP32, HEAPU32, HEAPF32, HEAPF64;
function updateGlobalBuffer(buf) {
 Module["buffer"] = buffer = buf;
}
function updateGlobalBufferViews() {
 Module["HEAP8"] = HEAP8 = new Int8Array(buffer);
 Module["HEAP16"] = HEAP16 = new Int16Array(buffer);
 Module["HEAP32"] = HEAP32 = new Int32Array(buffer);
 Module["HEAPU8"] = HEAPU8 = new Uint8Array(buffer);
 Module["HEAPU16"] = HEAPU16 = new Uint16Array(buffer);
 Module["HEAPU32"] = HEAPU32 = new Uint32Array(buffer);
 Module["HEAPF32"] = HEAPF32 = new Float32Array(buffer);
 Module["HEAPF64"] = HEAPF64 = new Float64Array(buffer);
}
var STATIC_BASE = 0, STATICTOP = 0, staticSealed = false;
var STACK_BASE = 0, STACKTOP = 0, STACK_MAX = 0;
var DYNAMIC_BASE = 0, DYNAMICTOP = 0;
function abortOnCannotGrowMemory() {
 abort("Cannot enlarge memory arrays. Either (1) compile with  -s TOTAL_MEMORY=X  with X higher than the current value " + TOTAL_MEMORY + ", (2) compile with  -s ALLOW_MEMORY_GROWTH=1  which adjusts the size at runtime but prevents some optimizations, (3) set Module.TOTAL_MEMORY to a higher value before the program runs, or if you want malloc to return NULL (0) instead of this abort, compile with  -s ABORTING_MALLOC=0 ");
}
function enlargeMemory() {
 abortOnCannotGrowMemory();
}
var TOTAL_STACK = Module["TOTAL_STACK"] || 5242880;
var TOTAL_MEMORY = Module["TOTAL_MEMORY"] || 134217728;
var totalMemory = 64 * 1024;
while (totalMemory < TOTAL_MEMORY || totalMemory < 2 * TOTAL_STACK) {
 if (totalMemory < 16 * 1024 * 1024) {
  totalMemory *= 2;
 } else {
  totalMemory += 16 * 1024 * 1024;
 }
}
if (totalMemory !== TOTAL_MEMORY) {
 TOTAL_MEMORY = totalMemory;
}
if (Module["buffer"]) {
 buffer = Module["buffer"];
} else {
 buffer = new ArrayBuffer(TOTAL_MEMORY);
}
updateGlobalBufferViews();
HEAP32[0] = 255;
if (HEAPU8[0] !== 255 || HEAPU8[3] !== 0) throw "Typed arrays 2 must be run on a little-endian system";
Module["HEAP"] = HEAP;
Module["buffer"] = buffer;
Module["HEAP8"] = HEAP8;
Module["HEAP16"] = HEAP16;
Module["HEAP32"] = HEAP32;
Module["HEAPU8"] = HEAPU8;
Module["HEAPU16"] = HEAPU16;
Module["HEAPU32"] = HEAPU32;
Module["HEAPF32"] = HEAPF32;
Module["HEAPF64"] = HEAPF64;
function callRuntimeCallbacks(callbacks) {
 while (callbacks.length > 0) {
  var callback = callbacks.shift();
  if (typeof callback == "function") {
   callback();
   continue;
  }
  var func = callback.func;
  if (typeof func === "number") {
   if (callback.arg === undefined) {
    Runtime.dynCall("v", func);
   } else {
    Runtime.dynCall("vi", func, [ callback.arg ]);
   }
  } else {
   func(callback.arg === undefined ? null : callback.arg);
  }
 }
}
var __ATPRERUN__ = [];
var __ATINIT__ = [];
var __ATMAIN__ = [];
var __ATEXIT__ = [];
var __ATPOSTRUN__ = [];
var runtimeInitialized = false;
var runtimeExited = false;
function preRun() {
 if (Module["preRun"]) {
  if (typeof Module["preRun"] == "function") Module["preRun"] = [ Module["preRun"] ];
  while (Module["preRun"].length) {
   addOnPreRun(Module["preRun"].shift());
  }
 }
 callRuntimeCallbacks(__ATPRERUN__);
}
function ensureInitRuntime() {
 if (runtimeInitialized) return;
 runtimeInitialized = true;
 callRuntimeCallbacks(__ATINIT__);
}
function preMain() {
 callRuntimeCallbacks(__ATMAIN__);
}
function exitRuntime() {
 callRuntimeCallbacks(__ATEXIT__);
 runtimeExited = true;
}
function postRun() {
 if (Module["postRun"]) {
  if (typeof Module["postRun"] == "function") Module["postRun"] = [ Module["postRun"] ];
  while (Module["postRun"].length) {
   addOnPostRun(Module["postRun"].shift());
  }
 }
 callRuntimeCallbacks(__ATPOSTRUN__);
}
function addOnPreRun(cb) {
 __ATPRERUN__.unshift(cb);
}
Module["addOnPreRun"] = addOnPreRun;
function addOnInit(cb) {
 __ATINIT__.unshift(cb);
}
Module["addOnInit"] = addOnInit;
function addOnPreMain(cb) {
 __ATMAIN__.unshift(cb);
}
Module["addOnPreMain"] = addOnPreMain;
function addOnExit(cb) {
 __ATEXIT__.unshift(cb);
}
Module["addOnExit"] = addOnExit;
function addOnPostRun(cb) {
 __ATPOSTRUN__.unshift(cb);
}
Module["addOnPostRun"] = addOnPostRun;
function intArrayFromString(stringy, dontAddNull, length) {
 var len = length > 0 ? length : lengthBytesUTF8(stringy) + 1;
 var u8array = new Array(len);
 var numBytesWritten = stringToUTF8Array(stringy, u8array, 0, u8array.length);
 if (dontAddNull) u8array.length = numBytesWritten;
 return u8array;
}
Module["intArrayFromString"] = intArrayFromString;
function intArrayToString(array) {
 var ret = [];
 for (var i = 0; i < array.length; i++) {
  var chr = array[i];
  if (chr > 255) {
   chr &= 255;
  }
  ret.push(String.fromCharCode(chr));
 }
 return ret.join("");
}
Module["intArrayToString"] = intArrayToString;
function writeStringToMemory(string, buffer, dontAddNull) {
 var array = intArrayFromString(string, dontAddNull);
 var i = 0;
 while (i < array.length) {
  var chr = array[i];
  HEAP8[buffer + i >> 0] = chr;
  i = i + 1;
 }
}
Module["writeStringToMemory"] = writeStringToMemory;
function writeArrayToMemory(array, buffer) {
 for (var i = 0; i < array.length; i++) {
  HEAP8[buffer++ >> 0] = array[i];
 }
}
Module["writeArrayToMemory"] = writeArrayToMemory;
function writeAsciiToMemory(str, buffer, dontAddNull) {
 for (var i = 0; i < str.length; ++i) {
  HEAP8[buffer++ >> 0] = str.charCodeAt(i);
 }
 if (!dontAddNull) HEAP8[buffer >> 0] = 0;
}
Module["writeAsciiToMemory"] = writeAsciiToMemory;
if (!Math["imul"] || Math["imul"](4294967295, 5) !== -5) Math["imul"] = function imul(a, b) {
 var ah = a >>> 16;
 var al = a & 65535;
 var bh = b >>> 16;
 var bl = b & 65535;
 return al * bl + (ah * bl + al * bh << 16) | 0;
};
Math.imul = Math["imul"];
if (!Math["clz32"]) Math["clz32"] = (function(x) {
 x = x >>> 0;
 for (var i = 0; i < 32; i++) {
  if (x & 1 << 31 - i) return i;
 }
 return 32;
});
Math.clz32 = Math["clz32"];
if (!Math["trunc"]) Math["trunc"] = (function(x) {
 return x < 0 ? Math.ceil(x) : Math.floor(x);
});
Math.trunc = Math["trunc"];
var Math_abs = Math.abs;
var Math_cos = Math.cos;
var Math_sin = Math.sin;
var Math_tan = Math.tan;
var Math_acos = Math.acos;
var Math_asin = Math.asin;
var Math_atan = Math.atan;
var Math_atan2 = Math.atan2;
var Math_exp = Math.exp;
var Math_log = Math.log;
var Math_sqrt = Math.sqrt;
var Math_ceil = Math.ceil;
var Math_floor = Math.floor;
var Math_pow = Math.pow;
var Math_imul = Math.imul;
var Math_fround = Math.fround;
var Math_min = Math.min;
var Math_clz32 = Math.clz32;
var Math_trunc = Math.trunc;
var runDependencies = 0;
var runDependencyWatcher = null;
var dependenciesFulfilled = null;
function addRunDependency(id) {
 runDependencies++;
 if (Module["monitorRunDependencies"]) {
  Module["monitorRunDependencies"](runDependencies);
 }
}
Module["addRunDependency"] = addRunDependency;
function removeRunDependency(id) {
 runDependencies--;
 if (Module["monitorRunDependencies"]) {
  Module["monitorRunDependencies"](runDependencies);
 }
 if (runDependencies == 0) {
  if (runDependencyWatcher !== null) {
   clearInterval(runDependencyWatcher);
   runDependencyWatcher = null;
  }
  if (dependenciesFulfilled) {
   var callback = dependenciesFulfilled;
   dependenciesFulfilled = null;
   callback();
  }
 }
}
Module["removeRunDependency"] = removeRunDependency;
Module["preloadedImages"] = {};
Module["preloadedAudios"] = {};
var memoryInitializer = null;
var ASM_CONSTS = [];
STATIC_BASE = 1024;
STATICTOP = STATIC_BASE + 47968;
__ATINIT__.push({
 func: (function() {
  __GLOBAL__sub_I_DrawCallExplorer_cc();
 })
}, {
 func: (function() {
  __GLOBAL__sub_I_Log_cc();
 })
}, {
 func: (function() {
  __GLOBAL__sub_I_imgui_cpp();
 })
});
memoryInitializer = "DrawCallExplorer.html.mem";
var STATIC_BUMP = 47968;
var tempDoublePtr = STATICTOP;
STATICTOP += 16;
function _atexit(func, arg) {
 __ATEXIT__.unshift({
  func: func,
  arg: arg
 });
}
function ___cxa_atexit() {
 return _atexit.apply(null, arguments);
}
var JSEvents = {
 keyEvent: 0,
 mouseEvent: 0,
 wheelEvent: 0,
 uiEvent: 0,
 focusEvent: 0,
 deviceOrientationEvent: 0,
 deviceMotionEvent: 0,
 fullscreenChangeEvent: 0,
 pointerlockChangeEvent: 0,
 visibilityChangeEvent: 0,
 touchEvent: 0,
 previousFullscreenElement: null,
 previousScreenX: null,
 previousScreenY: null,
 removeEventListenersRegistered: false,
 registerRemoveEventListeners: (function() {
  if (!JSEvents.removeEventListenersRegistered) {
   __ATEXIT__.push((function() {
    for (var i = JSEvents.eventHandlers.length - 1; i >= 0; --i) {
     JSEvents._removeHandler(i);
    }
   }));
   JSEvents.removeEventListenersRegistered = true;
  }
 }),
 findEventTarget: (function(target) {
  if (target) {
   if (typeof target == "number") {
    target = Pointer_stringify(target);
   }
   if (target == "#window") return window; else if (target == "#document") return document; else if (target == "#screen") return window.screen; else if (target == "#canvas") return Module["canvas"];
   if (typeof target == "string") return document.getElementById(target); else return target;
  } else {
   return window;
  }
 }),
 deferredCalls: [],
 deferCall: (function(targetFunction, precedence, argsList) {
  function arraysHaveEqualContent(arrA, arrB) {
   if (arrA.length != arrB.length) return false;
   for (var i in arrA) {
    if (arrA[i] != arrB[i]) return false;
   }
   return true;
  }
  for (var i in JSEvents.deferredCalls) {
   var call = JSEvents.deferredCalls[i];
   if (call.targetFunction == targetFunction && arraysHaveEqualContent(call.argsList, argsList)) {
    return;
   }
  }
  JSEvents.deferredCalls.push({
   targetFunction: targetFunction,
   precedence: precedence,
   argsList: argsList
  });
  JSEvents.deferredCalls.sort((function(x, y) {
   return x.precedence < y.precedence;
  }));
 }),
 removeDeferredCalls: (function(targetFunction) {
  for (var i = 0; i < JSEvents.deferredCalls.length; ++i) {
   if (JSEvents.deferredCalls[i].targetFunction == targetFunction) {
    JSEvents.deferredCalls.splice(i, 1);
    --i;
   }
  }
 }),
 canPerformEventHandlerRequests: (function() {
  return JSEvents.inEventHandler && JSEvents.currentEventHandler.allowsDeferredCalls;
 }),
 runDeferredCalls: (function() {
  if (!JSEvents.canPerformEventHandlerRequests()) {
   return;
  }
  for (var i = 0; i < JSEvents.deferredCalls.length; ++i) {
   var call = JSEvents.deferredCalls[i];
   JSEvents.deferredCalls.splice(i, 1);
   --i;
   call.targetFunction.apply(this, call.argsList);
  }
 }),
 inEventHandler: 0,
 currentEventHandler: null,
 eventHandlers: [],
 isInternetExplorer: (function() {
  return navigator.userAgent.indexOf("MSIE") !== -1 || navigator.appVersion.indexOf("Trident/") > 0;
 }),
 removeAllHandlersOnTarget: (function(target, eventTypeString) {
  for (var i = 0; i < JSEvents.eventHandlers.length; ++i) {
   if (JSEvents.eventHandlers[i].target == target && (!eventTypeString || eventTypeString == JSEvents.eventHandlers[i].eventTypeString)) {
    JSEvents._removeHandler(i--);
   }
  }
 }),
 _removeHandler: (function(i) {
  var h = JSEvents.eventHandlers[i];
  h.target.removeEventListener(h.eventTypeString, h.eventListenerFunc, h.useCapture);
  JSEvents.eventHandlers.splice(i, 1);
 }),
 registerOrRemoveHandler: (function(eventHandler) {
  var jsEventHandler = function jsEventHandler(event) {
   ++JSEvents.inEventHandler;
   JSEvents.currentEventHandler = eventHandler;
   JSEvents.runDeferredCalls();
   eventHandler.handlerFunc(event);
   JSEvents.runDeferredCalls();
   --JSEvents.inEventHandler;
  };
  if (eventHandler.callbackfunc) {
   eventHandler.eventListenerFunc = jsEventHandler;
   eventHandler.target.addEventListener(eventHandler.eventTypeString, jsEventHandler, eventHandler.useCapture);
   JSEvents.eventHandlers.push(eventHandler);
   JSEvents.registerRemoveEventListeners();
  } else {
   for (var i = 0; i < JSEvents.eventHandlers.length; ++i) {
    if (JSEvents.eventHandlers[i].target == eventHandler.target && JSEvents.eventHandlers[i].eventTypeString == eventHandler.eventTypeString) {
     JSEvents._removeHandler(i--);
    }
   }
  }
 }),
 registerKeyEventCallback: (function(target, userData, useCapture, callbackfunc, eventTypeId, eventTypeString) {
  if (!JSEvents.keyEvent) {
   JSEvents.keyEvent = _malloc(164);
  }
  var handlerFunc = (function(event) {
   var e = event || window.event;
   writeStringToMemory(e.key ? e.key : "", JSEvents.keyEvent + 0);
   writeStringToMemory(e.code ? e.code : "", JSEvents.keyEvent + 32);
   HEAP32[JSEvents.keyEvent + 64 >> 2] = e.location;
   HEAP32[JSEvents.keyEvent + 68 >> 2] = e.ctrlKey;
   HEAP32[JSEvents.keyEvent + 72 >> 2] = e.shiftKey;
   HEAP32[JSEvents.keyEvent + 76 >> 2] = e.altKey;
   HEAP32[JSEvents.keyEvent + 80 >> 2] = e.metaKey;
   HEAP32[JSEvents.keyEvent + 84 >> 2] = e.repeat;
   writeStringToMemory(e.locale ? e.locale : "", JSEvents.keyEvent + 88);
   writeStringToMemory(e.char ? e.char : "", JSEvents.keyEvent + 120);
   HEAP32[JSEvents.keyEvent + 152 >> 2] = e.charCode;
   HEAP32[JSEvents.keyEvent + 156 >> 2] = e.keyCode;
   HEAP32[JSEvents.keyEvent + 160 >> 2] = e.which;
   var shouldCancel = Runtime.dynCall("iiii", callbackfunc, [ eventTypeId, JSEvents.keyEvent, userData ]);
   if (shouldCancel) {
    e.preventDefault();
   }
  });
  var eventHandler = {
   target: JSEvents.findEventTarget(target),
   allowsDeferredCalls: JSEvents.isInternetExplorer() ? false : true,
   eventTypeString: eventTypeString,
   callbackfunc: callbackfunc,
   handlerFunc: handlerFunc,
   useCapture: useCapture
  };
  JSEvents.registerOrRemoveHandler(eventHandler);
 }),
 getBoundingClientRectOrZeros: (function(target) {
  return target.getBoundingClientRect ? target.getBoundingClientRect() : {
   left: 0,
   top: 0
  };
 }),
 fillMouseEventData: (function(eventStruct, e, target) {
  HEAPF64[eventStruct >> 3] = JSEvents.tick();
  HEAP32[eventStruct + 8 >> 2] = e.screenX;
  HEAP32[eventStruct + 12 >> 2] = e.screenY;
  HEAP32[eventStruct + 16 >> 2] = e.clientX;
  HEAP32[eventStruct + 20 >> 2] = e.clientY;
  HEAP32[eventStruct + 24 >> 2] = e.ctrlKey;
  HEAP32[eventStruct + 28 >> 2] = e.shiftKey;
  HEAP32[eventStruct + 32 >> 2] = e.altKey;
  HEAP32[eventStruct + 36 >> 2] = e.metaKey;
  HEAP16[eventStruct + 40 >> 1] = e.button;
  HEAP16[eventStruct + 42 >> 1] = e.buttons;
  HEAP32[eventStruct + 44 >> 2] = e["movementX"] || e["mozMovementX"] || e["webkitMovementX"] || e.screenX - JSEvents.previousScreenX;
  HEAP32[eventStruct + 48 >> 2] = e["movementY"] || e["mozMovementY"] || e["webkitMovementY"] || e.screenY - JSEvents.previousScreenY;
  if (Module["canvas"]) {
   var rect = Module["canvas"].getBoundingClientRect();
   HEAP32[eventStruct + 60 >> 2] = e.clientX - rect.left;
   HEAP32[eventStruct + 64 >> 2] = e.clientY - rect.top;
  } else {
   HEAP32[eventStruct + 60 >> 2] = 0;
   HEAP32[eventStruct + 64 >> 2] = 0;
  }
  if (target) {
   var rect = JSEvents.getBoundingClientRectOrZeros(target);
   HEAP32[eventStruct + 52 >> 2] = e.clientX - rect.left;
   HEAP32[eventStruct + 56 >> 2] = e.clientY - rect.top;
  } else {
   HEAP32[eventStruct + 52 >> 2] = 0;
   HEAP32[eventStruct + 56 >> 2] = 0;
  }
  JSEvents.previousScreenX = e.screenX;
  JSEvents.previousScreenY = e.screenY;
 }),
 registerMouseEventCallback: (function(target, userData, useCapture, callbackfunc, eventTypeId, eventTypeString) {
  if (!JSEvents.mouseEvent) {
   JSEvents.mouseEvent = _malloc(72);
  }
  target = JSEvents.findEventTarget(target);
  var handlerFunc = (function(event) {
   var e = event || window.event;
   JSEvents.fillMouseEventData(JSEvents.mouseEvent, e, target);
   var shouldCancel = Runtime.dynCall("iiii", callbackfunc, [ eventTypeId, JSEvents.mouseEvent, userData ]);
   if (shouldCancel) {
    e.preventDefault();
   }
  });
  var eventHandler = {
   target: target,
   allowsDeferredCalls: eventTypeString != "mousemove" && eventTypeString != "mouseenter" && eventTypeString != "mouseleave",
   eventTypeString: eventTypeString,
   callbackfunc: callbackfunc,
   handlerFunc: handlerFunc,
   useCapture: useCapture
  };
  if (JSEvents.isInternetExplorer() && eventTypeString == "mousedown") eventHandler.allowsDeferredCalls = false;
  JSEvents.registerOrRemoveHandler(eventHandler);
 }),
 registerWheelEventCallback: (function(target, userData, useCapture, callbackfunc, eventTypeId, eventTypeString) {
  if (!JSEvents.wheelEvent) {
   JSEvents.wheelEvent = _malloc(104);
  }
  target = JSEvents.findEventTarget(target);
  var wheelHandlerFunc = (function(event) {
   var e = event || window.event;
   JSEvents.fillMouseEventData(JSEvents.wheelEvent, e, target);
   HEAPF64[JSEvents.wheelEvent + 72 >> 3] = e["deltaX"];
   HEAPF64[JSEvents.wheelEvent + 80 >> 3] = e["deltaY"];
   HEAPF64[JSEvents.wheelEvent + 88 >> 3] = e["deltaZ"];
   HEAP32[JSEvents.wheelEvent + 96 >> 2] = e["deltaMode"];
   var shouldCancel = Runtime.dynCall("iiii", callbackfunc, [ eventTypeId, JSEvents.wheelEvent, userData ]);
   if (shouldCancel) {
    e.preventDefault();
   }
  });
  var mouseWheelHandlerFunc = (function(event) {
   var e = event || window.event;
   JSEvents.fillMouseEventData(JSEvents.wheelEvent, e, target);
   HEAPF64[JSEvents.wheelEvent + 72 >> 3] = e["wheelDeltaX"];
   HEAPF64[JSEvents.wheelEvent + 80 >> 3] = -e["wheelDeltaY"];
   HEAPF64[JSEvents.wheelEvent + 88 >> 3] = 0;
   HEAP32[JSEvents.wheelEvent + 96 >> 2] = 0;
   var shouldCancel = Runtime.dynCall("iiii", callbackfunc, [ eventTypeId, JSEvents.wheelEvent, userData ]);
   if (shouldCancel) {
    e.preventDefault();
   }
  });
  var eventHandler = {
   target: target,
   allowsDeferredCalls: true,
   eventTypeString: eventTypeString,
   callbackfunc: callbackfunc,
   handlerFunc: eventTypeString == "wheel" ? wheelHandlerFunc : mouseWheelHandlerFunc,
   useCapture: useCapture
  };
  JSEvents.registerOrRemoveHandler(eventHandler);
 }),
 pageScrollPos: (function() {
  if (window.pageXOffset > 0 || window.pageYOffset > 0) {
   return [ window.pageXOffset, window.pageYOffset ];
  }
  if (typeof document.documentElement.scrollLeft !== "undefined" || typeof document.documentElement.scrollTop !== "undefined") {
   return [ document.documentElement.scrollLeft, document.documentElement.scrollTop ];
  }
  return [ document.body.scrollLeft | 0, document.body.scrollTop | 0 ];
 }),
 registerUiEventCallback: (function(target, userData, useCapture, callbackfunc, eventTypeId, eventTypeString) {
  if (!JSEvents.uiEvent) {
   JSEvents.uiEvent = _malloc(36);
  }
  if (eventTypeString == "scroll" && !target) {
   target = document;
  } else {
   target = JSEvents.findEventTarget(target);
  }
  var handlerFunc = (function(event) {
   var e = event || window.event;
   if (e.target != target) {
    return;
   }
   var scrollPos = JSEvents.pageScrollPos();
   HEAP32[JSEvents.uiEvent >> 2] = e.detail;
   HEAP32[JSEvents.uiEvent + 4 >> 2] = document.body.clientWidth;
   HEAP32[JSEvents.uiEvent + 8 >> 2] = document.body.clientHeight;
   HEAP32[JSEvents.uiEvent + 12 >> 2] = window.innerWidth;
   HEAP32[JSEvents.uiEvent + 16 >> 2] = window.innerHeight;
   HEAP32[JSEvents.uiEvent + 20 >> 2] = window.outerWidth;
   HEAP32[JSEvents.uiEvent + 24 >> 2] = window.outerHeight;
   HEAP32[JSEvents.uiEvent + 28 >> 2] = scrollPos[0];
   HEAP32[JSEvents.uiEvent + 32 >> 2] = scrollPos[1];
   var shouldCancel = Runtime.dynCall("iiii", callbackfunc, [ eventTypeId, JSEvents.uiEvent, userData ]);
   if (shouldCancel) {
    e.preventDefault();
   }
  });
  var eventHandler = {
   target: target,
   allowsDeferredCalls: false,
   eventTypeString: eventTypeString,
   callbackfunc: callbackfunc,
   handlerFunc: handlerFunc,
   useCapture: useCapture
  };
  JSEvents.registerOrRemoveHandler(eventHandler);
 }),
 getNodeNameForTarget: (function(target) {
  if (!target) return "";
  if (target == window) return "#window";
  if (target == window.screen) return "#screen";
  return target && target.nodeName ? target.nodeName : "";
 }),
 registerFocusEventCallback: (function(target, userData, useCapture, callbackfunc, eventTypeId, eventTypeString) {
  if (!JSEvents.focusEvent) {
   JSEvents.focusEvent = _malloc(256);
  }
  var handlerFunc = (function(event) {
   var e = event || window.event;
   var nodeName = JSEvents.getNodeNameForTarget(e.target);
   var id = e.target.id ? e.target.id : "";
   writeStringToMemory(nodeName, JSEvents.focusEvent + 0);
   writeStringToMemory(id, JSEvents.focusEvent + 128);
   var shouldCancel = Runtime.dynCall("iiii", callbackfunc, [ eventTypeId, JSEvents.focusEvent, userData ]);
   if (shouldCancel) {
    e.preventDefault();
   }
  });
  var eventHandler = {
   target: JSEvents.findEventTarget(target),
   allowsDeferredCalls: false,
   eventTypeString: eventTypeString,
   callbackfunc: callbackfunc,
   handlerFunc: handlerFunc,
   useCapture: useCapture
  };
  JSEvents.registerOrRemoveHandler(eventHandler);
 }),
 tick: (function() {
  if (window["performance"] && window["performance"]["now"]) return window["performance"]["now"](); else return Date.now();
 }),
 registerDeviceOrientationEventCallback: (function(target, userData, useCapture, callbackfunc, eventTypeId, eventTypeString) {
  if (!JSEvents.deviceOrientationEvent) {
   JSEvents.deviceOrientationEvent = _malloc(40);
  }
  var handlerFunc = (function(event) {
   var e = event || window.event;
   HEAPF64[JSEvents.deviceOrientationEvent >> 3] = JSEvents.tick();
   HEAPF64[JSEvents.deviceOrientationEvent + 8 >> 3] = e.alpha;
   HEAPF64[JSEvents.deviceOrientationEvent + 16 >> 3] = e.beta;
   HEAPF64[JSEvents.deviceOrientationEvent + 24 >> 3] = e.gamma;
   HEAP32[JSEvents.deviceOrientationEvent + 32 >> 2] = e.absolute;
   var shouldCancel = Runtime.dynCall("iiii", callbackfunc, [ eventTypeId, JSEvents.deviceOrientationEvent, userData ]);
   if (shouldCancel) {
    e.preventDefault();
   }
  });
  var eventHandler = {
   target: JSEvents.findEventTarget(target),
   allowsDeferredCalls: false,
   eventTypeString: eventTypeString,
   callbackfunc: callbackfunc,
   handlerFunc: handlerFunc,
   useCapture: useCapture
  };
  JSEvents.registerOrRemoveHandler(eventHandler);
 }),
 registerDeviceMotionEventCallback: (function(target, userData, useCapture, callbackfunc, eventTypeId, eventTypeString) {
  if (!JSEvents.deviceMotionEvent) {
   JSEvents.deviceMotionEvent = _malloc(80);
  }
  var handlerFunc = (function(event) {
   var e = event || window.event;
   HEAPF64[JSEvents.deviceOrientationEvent >> 3] = JSEvents.tick();
   HEAPF64[JSEvents.deviceMotionEvent + 8 >> 3] = e.acceleration.x;
   HEAPF64[JSEvents.deviceMotionEvent + 16 >> 3] = e.acceleration.y;
   HEAPF64[JSEvents.deviceMotionEvent + 24 >> 3] = e.acceleration.z;
   HEAPF64[JSEvents.deviceMotionEvent + 32 >> 3] = e.accelerationIncludingGravity.x;
   HEAPF64[JSEvents.deviceMotionEvent + 40 >> 3] = e.accelerationIncludingGravity.y;
   HEAPF64[JSEvents.deviceMotionEvent + 48 >> 3] = e.accelerationIncludingGravity.z;
   HEAPF64[JSEvents.deviceMotionEvent + 56 >> 3] = e.rotationRate.alpha;
   HEAPF64[JSEvents.deviceMotionEvent + 64 >> 3] = e.rotationRate.beta;
   HEAPF64[JSEvents.deviceMotionEvent + 72 >> 3] = e.rotationRate.gamma;
   var shouldCancel = Runtime.dynCall("iiii", callbackfunc, [ eventTypeId, JSEvents.deviceMotionEvent, userData ]);
   if (shouldCancel) {
    e.preventDefault();
   }
  });
  var eventHandler = {
   target: JSEvents.findEventTarget(target),
   allowsDeferredCalls: false,
   eventTypeString: eventTypeString,
   callbackfunc: callbackfunc,
   handlerFunc: handlerFunc,
   useCapture: useCapture
  };
  JSEvents.registerOrRemoveHandler(eventHandler);
 }),
 screenOrientation: (function() {
  if (!window.screen) return undefined;
  return window.screen.orientation || window.screen.mozOrientation || window.screen.webkitOrientation || window.screen.msOrientation;
 }),
 fillOrientationChangeEventData: (function(eventStruct, e) {
  var orientations = [ "portrait-primary", "portrait-secondary", "landscape-primary", "landscape-secondary" ];
  var orientations2 = [ "portrait", "portrait", "landscape", "landscape" ];
  var orientationString = JSEvents.screenOrientation();
  var orientation = orientations.indexOf(orientationString);
  if (orientation == -1) {
   orientation = orientations2.indexOf(orientationString);
  }
  HEAP32[eventStruct >> 2] = 1 << orientation;
  HEAP32[eventStruct + 4 >> 2] = window.orientation;
 }),
 registerOrientationChangeEventCallback: (function(target, userData, useCapture, callbackfunc, eventTypeId, eventTypeString) {
  if (!JSEvents.orientationChangeEvent) {
   JSEvents.orientationChangeEvent = _malloc(8);
  }
  if (!target) {
   target = window.screen;
  } else {
   target = JSEvents.findEventTarget(target);
  }
  var handlerFunc = (function(event) {
   var e = event || window.event;
   JSEvents.fillOrientationChangeEventData(JSEvents.orientationChangeEvent, e);
   var shouldCancel = Runtime.dynCall("iiii", callbackfunc, [ eventTypeId, JSEvents.orientationChangeEvent, userData ]);
   if (shouldCancel) {
    e.preventDefault();
   }
  });
  if (eventTypeString == "orientationchange" && window.screen.mozOrientation !== undefined) {
   eventTypeString = "mozorientationchange";
  }
  var eventHandler = {
   target: target,
   allowsDeferredCalls: false,
   eventTypeString: eventTypeString,
   callbackfunc: callbackfunc,
   handlerFunc: handlerFunc,
   useCapture: useCapture
  };
  JSEvents.registerOrRemoveHandler(eventHandler);
 }),
 fullscreenEnabled: (function() {
  return document.fullscreenEnabled || document.mozFullscreenEnabled || document.mozFullScreenEnabled || document.webkitFullscreenEnabled || document.msFullscreenEnabled;
 }),
 fillFullscreenChangeEventData: (function(eventStruct, e) {
  var fullscreenElement = document.fullscreenElement || document.mozFullScreenElement || document.webkitFullscreenElement || document.msFullscreenElement;
  var isFullscreen = !!fullscreenElement;
  HEAP32[eventStruct >> 2] = isFullscreen;
  HEAP32[eventStruct + 4 >> 2] = JSEvents.fullscreenEnabled();
  var reportedElement = isFullscreen ? fullscreenElement : JSEvents.previousFullscreenElement;
  var nodeName = JSEvents.getNodeNameForTarget(reportedElement);
  var id = reportedElement && reportedElement.id ? reportedElement.id : "";
  writeStringToMemory(nodeName, eventStruct + 8);
  writeStringToMemory(id, eventStruct + 136);
  HEAP32[eventStruct + 264 >> 2] = reportedElement ? reportedElement.clientWidth : 0;
  HEAP32[eventStruct + 268 >> 2] = reportedElement ? reportedElement.clientHeight : 0;
  HEAP32[eventStruct + 272 >> 2] = screen.width;
  HEAP32[eventStruct + 276 >> 2] = screen.height;
  if (isFullscreen) {
   JSEvents.previousFullscreenElement = fullscreenElement;
  }
 }),
 registerFullscreenChangeEventCallback: (function(target, userData, useCapture, callbackfunc, eventTypeId, eventTypeString) {
  if (!JSEvents.fullscreenChangeEvent) {
   JSEvents.fullscreenChangeEvent = _malloc(280);
  }
  if (!target) {
   target = document;
  } else {
   target = JSEvents.findEventTarget(target);
  }
  var handlerFunc = (function(event) {
   var e = event || window.event;
   JSEvents.fillFullscreenChangeEventData(JSEvents.fullscreenChangeEvent, e);
   var shouldCancel = Runtime.dynCall("iiii", callbackfunc, [ eventTypeId, JSEvents.fullscreenChangeEvent, userData ]);
   if (shouldCancel) {
    e.preventDefault();
   }
  });
  var eventHandler = {
   target: target,
   allowsDeferredCalls: false,
   eventTypeString: eventTypeString,
   callbackfunc: callbackfunc,
   handlerFunc: handlerFunc,
   useCapture: useCapture
  };
  JSEvents.registerOrRemoveHandler(eventHandler);
 }),
 resizeCanvasForFullscreen: (function(target, strategy) {
  var restoreOldStyle = __registerRestoreOldStyle(target);
  var cssWidth = strategy.softFullscreen ? window.innerWidth : screen.width;
  var cssHeight = strategy.softFullscreen ? window.innerHeight : screen.height;
  var rect = target.getBoundingClientRect();
  var windowedCssWidth = rect.right - rect.left;
  var windowedCssHeight = rect.bottom - rect.top;
  var windowedRttWidth = target.width;
  var windowedRttHeight = target.height;
  if (strategy.scaleMode == 3) {
   __setLetterbox(target, (cssHeight - windowedCssHeight) / 2, (cssWidth - windowedCssWidth) / 2);
   cssWidth = windowedCssWidth;
   cssHeight = windowedCssHeight;
  } else if (strategy.scaleMode == 2) {
   if (cssWidth * windowedRttHeight < windowedRttWidth * cssHeight) {
    var desiredCssHeight = windowedRttHeight * cssWidth / windowedRttWidth;
    __setLetterbox(target, (cssHeight - desiredCssHeight) / 2, 0);
    cssHeight = desiredCssHeight;
   } else {
    var desiredCssWidth = windowedRttWidth * cssHeight / windowedRttHeight;
    __setLetterbox(target, 0, (cssWidth - desiredCssWidth) / 2);
    cssWidth = desiredCssWidth;
   }
  }
  if (!target.style.backgroundColor) target.style.backgroundColor = "black";
  if (!document.body.style.backgroundColor) document.body.style.backgroundColor = "black";
  target.style.width = cssWidth + "px";
  target.style.height = cssHeight + "px";
  if (strategy.filteringMode == 1) {
   target.style.imageRendering = "optimizeSpeed";
   target.style.imageRendering = "-moz-crisp-edges";
   target.style.imageRendering = "-o-crisp-edges";
   target.style.imageRendering = "-webkit-optimize-contrast";
   target.style.imageRendering = "optimize-contrast";
   target.style.imageRendering = "crisp-edges";
   target.style.imageRendering = "pixelated";
  }
  var dpiScale = strategy.canvasResolutionScaleMode == 2 ? window.devicePixelRatio : 1;
  if (strategy.canvasResolutionScaleMode != 0) {
   target.width = cssWidth * dpiScale;
   target.height = cssHeight * dpiScale;
   if (target.GLctxObject) target.GLctxObject.GLctx.viewport(0, 0, target.width, target.height);
  }
  return restoreOldStyle;
 }),
 requestFullscreen: (function(target, strategy) {
  if (strategy.scaleMode != 0 || strategy.canvasResolutionScaleMode != 0) {
   JSEvents.resizeCanvasForFullscreen(target, strategy);
  }
  if (target.requestFullscreen) {
   target.requestFullscreen();
  } else if (target.msRequestFullscreen) {
   target.msRequestFullscreen();
  } else if (target.mozRequestFullScreen) {
   target.mozRequestFullScreen();
  } else if (target.mozRequestFullscreen) {
   target.mozRequestFullscreen();
  } else if (target.webkitRequestFullscreen) {
   target.webkitRequestFullscreen(Element.ALLOW_KEYBOARD_INPUT);
  } else {
   if (typeof JSEvents.fullscreenEnabled() === "undefined") {
    return -1;
   } else {
    return -3;
   }
  }
  if (strategy.canvasResizedCallback) {
   Runtime.dynCall("iiii", strategy.canvasResizedCallback, [ 37, 0, strategy.canvasResizedCallbackUserData ]);
  }
  return 0;
 }),
 fillPointerlockChangeEventData: (function(eventStruct, e) {
  var pointerLockElement = document.pointerLockElement || document.mozPointerLockElement || document.webkitPointerLockElement || document.msPointerLockElement;
  var isPointerlocked = !!pointerLockElement;
  HEAP32[eventStruct >> 2] = isPointerlocked;
  var nodeName = JSEvents.getNodeNameForTarget(pointerLockElement);
  var id = pointerLockElement && pointerLockElement.id ? pointerLockElement.id : "";
  writeStringToMemory(nodeName, eventStruct + 4);
  writeStringToMemory(id, eventStruct + 132);
 }),
 registerPointerlockChangeEventCallback: (function(target, userData, useCapture, callbackfunc, eventTypeId, eventTypeString) {
  if (!JSEvents.pointerlockChangeEvent) {
   JSEvents.pointerlockChangeEvent = _malloc(260);
  }
  if (!target) {
   target = document;
  } else {
   target = JSEvents.findEventTarget(target);
  }
  var handlerFunc = (function(event) {
   var e = event || window.event;
   JSEvents.fillPointerlockChangeEventData(JSEvents.pointerlockChangeEvent, e);
   var shouldCancel = Runtime.dynCall("iiii", callbackfunc, [ eventTypeId, JSEvents.pointerlockChangeEvent, userData ]);
   if (shouldCancel) {
    e.preventDefault();
   }
  });
  var eventHandler = {
   target: target,
   allowsDeferredCalls: false,
   eventTypeString: eventTypeString,
   callbackfunc: callbackfunc,
   handlerFunc: handlerFunc,
   useCapture: useCapture
  };
  JSEvents.registerOrRemoveHandler(eventHandler);
 }),
 requestPointerLock: (function(target) {
  if (target.requestPointerLock) {
   target.requestPointerLock();
  } else if (target.mozRequestPointerLock) {
   target.mozRequestPointerLock();
  } else if (target.webkitRequestPointerLock) {
   target.webkitRequestPointerLock();
  } else if (target.msRequestPointerLock) {
   target.msRequestPointerLock();
  } else {
   if (document.body.requestPointerLock || document.body.mozRequestPointerLock || document.body.webkitRequestPointerLock || document.body.msRequestPointerLock) {
    return -3;
   } else {
    return -1;
   }
  }
  return 0;
 }),
 fillVisibilityChangeEventData: (function(eventStruct, e) {
  var visibilityStates = [ "hidden", "visible", "prerender", "unloaded" ];
  var visibilityState = visibilityStates.indexOf(document.visibilityState);
  HEAP32[eventStruct >> 2] = document.hidden;
  HEAP32[eventStruct + 4 >> 2] = visibilityState;
 }),
 registerVisibilityChangeEventCallback: (function(target, userData, useCapture, callbackfunc, eventTypeId, eventTypeString) {
  if (!JSEvents.visibilityChangeEvent) {
   JSEvents.visibilityChangeEvent = _malloc(8);
  }
  if (!target) {
   target = document;
  } else {
   target = JSEvents.findEventTarget(target);
  }
  var handlerFunc = (function(event) {
   var e = event || window.event;
   JSEvents.fillVisibilityChangeEventData(JSEvents.visibilityChangeEvent, e);
   var shouldCancel = Runtime.dynCall("iiii", callbackfunc, [ eventTypeId, JSEvents.visibilityChangeEvent, userData ]);
   if (shouldCancel) {
    e.preventDefault();
   }
  });
  var eventHandler = {
   target: target,
   allowsDeferredCalls: false,
   eventTypeString: eventTypeString,
   callbackfunc: callbackfunc,
   handlerFunc: handlerFunc,
   useCapture: useCapture
  };
  JSEvents.registerOrRemoveHandler(eventHandler);
 }),
 registerTouchEventCallback: (function(target, userData, useCapture, callbackfunc, eventTypeId, eventTypeString) {
  if (!JSEvents.touchEvent) {
   JSEvents.touchEvent = _malloc(1684);
  }
  target = JSEvents.findEventTarget(target);
  var handlerFunc = (function(event) {
   var e = event || window.event;
   var touches = {};
   for (var i = 0; i < e.touches.length; ++i) {
    var touch = e.touches[i];
    touches[touch.identifier] = touch;
   }
   for (var i = 0; i < e.changedTouches.length; ++i) {
    var touch = e.changedTouches[i];
    touches[touch.identifier] = touch;
    touch.changed = true;
   }
   for (var i = 0; i < e.targetTouches.length; ++i) {
    var touch = e.targetTouches[i];
    touches[touch.identifier].onTarget = true;
   }
   var ptr = JSEvents.touchEvent;
   HEAP32[ptr + 4 >> 2] = e.ctrlKey;
   HEAP32[ptr + 8 >> 2] = e.shiftKey;
   HEAP32[ptr + 12 >> 2] = e.altKey;
   HEAP32[ptr + 16 >> 2] = e.metaKey;
   ptr += 20;
   var canvasRect = Module["canvas"] ? Module["canvas"].getBoundingClientRect() : undefined;
   var targetRect = JSEvents.getBoundingClientRectOrZeros(target);
   var numTouches = 0;
   for (var i in touches) {
    var t = touches[i];
    HEAP32[ptr >> 2] = t.identifier;
    HEAP32[ptr + 4 >> 2] = t.screenX;
    HEAP32[ptr + 8 >> 2] = t.screenY;
    HEAP32[ptr + 12 >> 2] = t.clientX;
    HEAP32[ptr + 16 >> 2] = t.clientY;
    HEAP32[ptr + 20 >> 2] = t.pageX;
    HEAP32[ptr + 24 >> 2] = t.pageY;
    HEAP32[ptr + 28 >> 2] = t.changed;
    HEAP32[ptr + 32 >> 2] = t.onTarget;
    if (canvasRect) {
     HEAP32[ptr + 44 >> 2] = t.clientX - canvasRect.left;
     HEAP32[ptr + 48 >> 2] = t.clientY - canvasRect.top;
    } else {
     HEAP32[ptr + 44 >> 2] = 0;
     HEAP32[ptr + 48 >> 2] = 0;
    }
    HEAP32[ptr + 36 >> 2] = t.clientX - targetRect.left;
    HEAP32[ptr + 40 >> 2] = t.clientY - targetRect.top;
    ptr += 52;
    if (++numTouches >= 32) {
     break;
    }
   }
   HEAP32[JSEvents.touchEvent >> 2] = numTouches;
   var shouldCancel = Runtime.dynCall("iiii", callbackfunc, [ eventTypeId, JSEvents.touchEvent, userData ]);
   if (shouldCancel) {
    e.preventDefault();
   }
  });
  var eventHandler = {
   target: target,
   allowsDeferredCalls: false,
   eventTypeString: eventTypeString,
   callbackfunc: callbackfunc,
   handlerFunc: handlerFunc,
   useCapture: useCapture
  };
  JSEvents.registerOrRemoveHandler(eventHandler);
 }),
 fillGamepadEventData: (function(eventStruct, e) {
  HEAPF64[eventStruct >> 3] = e.timestamp;
  for (var i = 0; i < e.axes.length; ++i) {
   HEAPF64[eventStruct + i * 8 + 16 >> 3] = e.axes[i];
  }
  for (var i = 0; i < e.buttons.length; ++i) {
   if (typeof e.buttons[i] === "object") {
    HEAPF64[eventStruct + i * 8 + 528 >> 3] = e.buttons[i].value;
   } else {
    HEAPF64[eventStruct + i * 8 + 528 >> 3] = e.buttons[i];
   }
  }
  for (var i = 0; i < e.buttons.length; ++i) {
   if (typeof e.buttons[i] === "object") {
    HEAP32[eventStruct + i * 4 + 1040 >> 2] = e.buttons[i].pressed;
   } else {
    HEAP32[eventStruct + i * 4 + 1040 >> 2] = e.buttons[i] == 1;
   }
  }
  HEAP32[eventStruct + 1296 >> 2] = e.connected;
  HEAP32[eventStruct + 1300 >> 2] = e.index;
  HEAP32[eventStruct + 8 >> 2] = e.axes.length;
  HEAP32[eventStruct + 12 >> 2] = e.buttons.length;
  writeStringToMemory(e.id, eventStruct + 1304);
  writeStringToMemory(e.mapping, eventStruct + 1368);
 }),
 registerGamepadEventCallback: (function(target, userData, useCapture, callbackfunc, eventTypeId, eventTypeString) {
  if (!JSEvents.gamepadEvent) {
   JSEvents.gamepadEvent = _malloc(1432);
  }
  var handlerFunc = (function(event) {
   var e = event || window.event;
   JSEvents.fillGamepadEventData(JSEvents.gamepadEvent, e.gamepad);
   var shouldCancel = Runtime.dynCall("iiii", callbackfunc, [ eventTypeId, JSEvents.gamepadEvent, userData ]);
   if (shouldCancel) {
    e.preventDefault();
   }
  });
  var eventHandler = {
   target: JSEvents.findEventTarget(target),
   allowsDeferredCalls: true,
   eventTypeString: eventTypeString,
   callbackfunc: callbackfunc,
   handlerFunc: handlerFunc,
   useCapture: useCapture
  };
  JSEvents.registerOrRemoveHandler(eventHandler);
 }),
 registerBeforeUnloadEventCallback: (function(target, userData, useCapture, callbackfunc, eventTypeId, eventTypeString) {
  var handlerFunc = (function(event) {
   var e = event || window.event;
   var confirmationMessage = Runtime.dynCall("iiii", callbackfunc, [ eventTypeId, 0, userData ]);
   if (confirmationMessage) {
    confirmationMessage = Pointer_stringify(confirmationMessage);
   }
   if (confirmationMessage) {
    e.preventDefault();
    e.returnValue = confirmationMessage;
    return confirmationMessage;
   }
  });
  var eventHandler = {
   target: JSEvents.findEventTarget(target),
   allowsDeferredCalls: false,
   eventTypeString: eventTypeString,
   callbackfunc: callbackfunc,
   handlerFunc: handlerFunc,
   useCapture: useCapture
  };
  JSEvents.registerOrRemoveHandler(eventHandler);
 }),
 battery: (function() {
  return navigator.battery || navigator.mozBattery || navigator.webkitBattery;
 }),
 fillBatteryEventData: (function(eventStruct, e) {
  HEAPF64[eventStruct >> 3] = e.chargingTime;
  HEAPF64[eventStruct + 8 >> 3] = e.dischargingTime;
  HEAPF64[eventStruct + 16 >> 3] = e.level;
  HEAP32[eventStruct + 24 >> 2] = e.charging;
 }),
 registerBatteryEventCallback: (function(target, userData, useCapture, callbackfunc, eventTypeId, eventTypeString) {
  if (!JSEvents.batteryEvent) {
   JSEvents.batteryEvent = _malloc(32);
  }
  var handlerFunc = (function(event) {
   var e = event || window.event;
   JSEvents.fillBatteryEventData(JSEvents.batteryEvent, JSEvents.battery());
   var shouldCancel = Runtime.dynCall("iiii", callbackfunc, [ eventTypeId, JSEvents.batteryEvent, userData ]);
   if (shouldCancel) {
    e.preventDefault();
   }
  });
  var eventHandler = {
   target: JSEvents.findEventTarget(target),
   allowsDeferredCalls: false,
   eventTypeString: eventTypeString,
   callbackfunc: callbackfunc,
   handlerFunc: handlerFunc,
   useCapture: useCapture
  };
  JSEvents.registerOrRemoveHandler(eventHandler);
 }),
 registerWebGlEventCallback: (function(target, userData, useCapture, callbackfunc, eventTypeId, eventTypeString) {
  if (!target) {
   target = Module["canvas"];
  }
  var handlerFunc = (function(event) {
   var e = event || window.event;
   var shouldCancel = Runtime.dynCall("iiii", callbackfunc, [ eventTypeId, 0, userData ]);
   if (shouldCancel) {
    e.preventDefault();
   }
  });
  var eventHandler = {
   target: JSEvents.findEventTarget(target),
   allowsDeferredCalls: false,
   eventTypeString: eventTypeString,
   callbackfunc: callbackfunc,
   handlerFunc: handlerFunc,
   useCapture: useCapture
  };
  JSEvents.registerOrRemoveHandler(eventHandler);
 })
};
function _emscripten_set_mousedown_callback(target, userData, useCapture, callbackfunc) {
 JSEvents.registerMouseEventCallback(target, userData, useCapture, callbackfunc, 5, "mousedown");
 return 0;
}
var GL = {
 counter: 1,
 lastError: 0,
 buffers: [],
 mappedBuffers: {},
 programs: [],
 framebuffers: [],
 renderbuffers: [],
 textures: [],
 uniforms: [],
 shaders: [],
 vaos: [],
 contexts: [],
 currentContext: null,
 byteSizeByTypeRoot: 5120,
 byteSizeByType: [ 1, 1, 2, 2, 4, 4, 4, 2, 3, 4, 8 ],
 programInfos: {},
 stringCache: {},
 packAlignment: 4,
 unpackAlignment: 4,
 init: (function() {
  GL.miniTempBuffer = new Float32Array(GL.MINI_TEMP_BUFFER_SIZE);
  for (var i = 0; i < GL.MINI_TEMP_BUFFER_SIZE; i++) {
   GL.miniTempBufferViews[i] = GL.miniTempBuffer.subarray(0, i + 1);
  }
 }),
 recordError: function recordError(errorCode) {
  if (!GL.lastError) {
   GL.lastError = errorCode;
  }
 },
 getNewId: (function(table) {
  var ret = GL.counter++;
  for (var i = table.length; i < ret; i++) {
   table[i] = null;
  }
  return ret;
 }),
 MINI_TEMP_BUFFER_SIZE: 256,
 miniTempBuffer: null,
 miniTempBufferViews: [ 0 ],
 getSource: (function(shader, count, string, length) {
  var source = "";
  for (var i = 0; i < count; ++i) {
   var frag;
   if (length) {
    var len = HEAP32[length + i * 4 >> 2];
    if (len < 0) {
     frag = Pointer_stringify(HEAP32[string + i * 4 >> 2]);
    } else {
     frag = Pointer_stringify(HEAP32[string + i * 4 >> 2], len);
    }
   } else {
    frag = Pointer_stringify(HEAP32[string + i * 4 >> 2]);
   }
   source += frag;
  }
  return source;
 }),
 createContext: (function(canvas, webGLContextAttributes) {
  if (typeof webGLContextAttributes.majorVersion === "undefined" && typeof webGLContextAttributes.minorVersion === "undefined") {
   webGLContextAttributes.majorVersion = 1;
   webGLContextAttributes.minorVersion = 0;
  }
  var ctx;
  var errorInfo = "?";
  function onContextCreationError(event) {
   errorInfo = event.statusMessage || errorInfo;
  }
  try {
   canvas.addEventListener("webglcontextcreationerror", onContextCreationError, false);
   try {
    if (webGLContextAttributes.majorVersion == 1 && webGLContextAttributes.minorVersion == 0) {
     ctx = canvas.getContext("webgl", webGLContextAttributes) || canvas.getContext("experimental-webgl", webGLContextAttributes);
    } else if (webGLContextAttributes.majorVersion == 2 && webGLContextAttributes.minorVersion == 0) {
     ctx = canvas.getContext("webgl2", webGLContextAttributes) || canvas.getContext("experimental-webgl2", webGLContextAttributes);
    } else {
     throw "Unsupported WebGL context version " + majorVersion + "." + minorVersion + "!";
    }
   } finally {
    canvas.removeEventListener("webglcontextcreationerror", onContextCreationError, false);
   }
   if (!ctx) throw ":(";
  } catch (e) {
   Module.print("Could not create canvas: " + [ errorInfo, e, JSON.stringify(webGLContextAttributes) ]);
   return 0;
  }
  if (!ctx) return 0;
  return GL.registerContext(ctx, webGLContextAttributes);
 }),
 registerContext: (function(ctx, webGLContextAttributes) {
  var handle = GL.getNewId(GL.contexts);
  var context = {
   handle: handle,
   version: webGLContextAttributes.majorVersion,
   GLctx: ctx
  };
  if (ctx.canvas) ctx.canvas.GLctxObject = context;
  GL.contexts[handle] = context;
  if (typeof webGLContextAttributes["enableExtensionsByDefault"] === "undefined" || webGLContextAttributes.enableExtensionsByDefault) {
   GL.initExtensions(context);
  }
  return handle;
 }),
 makeContextCurrent: (function(contextHandle) {
  var context = GL.contexts[contextHandle];
  if (!context) return false;
  GLctx = Module.ctx = context.GLctx;
  GL.currentContext = context;
  return true;
 }),
 getContext: (function(contextHandle) {
  return GL.contexts[contextHandle];
 }),
 deleteContext: (function(contextHandle) {
  if (GL.currentContext === GL.contexts[contextHandle]) GL.currentContext = null;
  if (typeof JSEvents === "object") JSEvents.removeAllHandlersOnTarget(GL.contexts[contextHandle].GLctx.canvas);
  if (GL.contexts[contextHandle] && GL.contexts[contextHandle].GLctx.canvas) GL.contexts[contextHandle].GLctx.canvas.GLctxObject = undefined;
  GL.contexts[contextHandle] = null;
 }),
 initExtensions: (function(context) {
  if (!context) context = GL.currentContext;
  if (context.initExtensionsDone) return;
  context.initExtensionsDone = true;
  var GLctx = context.GLctx;
  context.maxVertexAttribs = GLctx.getParameter(GLctx.MAX_VERTEX_ATTRIBS);
  if (context.version < 2) {
   var instancedArraysExt = GLctx.getExtension("ANGLE_instanced_arrays");
   if (instancedArraysExt) {
    GLctx["vertexAttribDivisor"] = (function(index, divisor) {
     instancedArraysExt["vertexAttribDivisorANGLE"](index, divisor);
    });
    GLctx["drawArraysInstanced"] = (function(mode, first, count, primcount) {
     instancedArraysExt["drawArraysInstancedANGLE"](mode, first, count, primcount);
    });
    GLctx["drawElementsInstanced"] = (function(mode, count, type, indices, primcount) {
     instancedArraysExt["drawElementsInstancedANGLE"](mode, count, type, indices, primcount);
    });
   }
   var vaoExt = GLctx.getExtension("OES_vertex_array_object");
   if (vaoExt) {
    GLctx["createVertexArray"] = (function() {
     return vaoExt["createVertexArrayOES"]();
    });
    GLctx["deleteVertexArray"] = (function(vao) {
     vaoExt["deleteVertexArrayOES"](vao);
    });
    GLctx["bindVertexArray"] = (function(vao) {
     vaoExt["bindVertexArrayOES"](vao);
    });
    GLctx["isVertexArray"] = (function(vao) {
     return vaoExt["isVertexArrayOES"](vao);
    });
   }
   var drawBuffersExt = GLctx.getExtension("WEBGL_draw_buffers");
   if (drawBuffersExt) {
    GLctx["drawBuffers"] = (function(n, bufs) {
     drawBuffersExt["drawBuffersWEBGL"](n, bufs);
    });
   }
  }
  var automaticallyEnabledExtensions = [ "OES_texture_float", "OES_texture_half_float", "OES_standard_derivatives", "OES_vertex_array_object", "WEBGL_compressed_texture_s3tc", "WEBGL_depth_texture", "OES_element_index_uint", "EXT_texture_filter_anisotropic", "ANGLE_instanced_arrays", "OES_texture_float_linear", "OES_texture_half_float_linear", "WEBGL_compressed_texture_atc", "WEBGL_compressed_texture_pvrtc", "EXT_color_buffer_half_float", "WEBGL_color_buffer_float", "EXT_frag_depth", "EXT_sRGB", "WEBGL_draw_buffers", "WEBGL_shared_resources", "EXT_shader_texture_lod" ];
  var exts = GLctx.getSupportedExtensions();
  if (exts && exts.length > 0) {
   GLctx.getSupportedExtensions().forEach((function(ext) {
    if (automaticallyEnabledExtensions.indexOf(ext) != -1) {
     GLctx.getExtension(ext);
    }
   }));
  }
 }),
 populateUniformTable: (function(program) {
  var p = GL.programs[program];
  GL.programInfos[program] = {
   uniforms: {},
   maxUniformLength: 0,
   maxAttributeLength: -1
  };
  var ptable = GL.programInfos[program];
  var utable = ptable.uniforms;
  var numUniforms = GLctx.getProgramParameter(p, GLctx.ACTIVE_UNIFORMS);
  for (var i = 0; i < numUniforms; ++i) {
   var u = GLctx.getActiveUniform(p, i);
   var name = u.name;
   ptable.maxUniformLength = Math.max(ptable.maxUniformLength, name.length + 1);
   if (name.indexOf("]", name.length - 1) !== -1) {
    var ls = name.lastIndexOf("[");
    name = name.slice(0, ls);
   }
   var loc = GLctx.getUniformLocation(p, name);
   var id = GL.getNewId(GL.uniforms);
   utable[name] = [ u.size, id ];
   GL.uniforms[id] = loc;
   for (var j = 1; j < u.size; ++j) {
    var n = name + "[" + j + "]";
    loc = GLctx.getUniformLocation(p, n);
    id = GL.getNewId(GL.uniforms);
    GL.uniforms[id] = loc;
   }
  }
 })
};
function _glBlendColor(x0, x1, x2, x3) {
 GLctx.blendColor(x0, x1, x2, x3);
}
Module["_i64Subtract"] = _i64Subtract;
function _glClearColor(x0, x1, x2, x3) {
 GLctx.clearColor(x0, x1, x2, x3);
}
Module["_i64Add"] = _i64Add;
Module["_roundf"] = _roundf;
function __ZSt18uncaught_exceptionv() {
 return !!__ZSt18uncaught_exceptionv.uncaught_exception;
}
var EXCEPTIONS = {
 last: 0,
 caught: [],
 infos: {},
 deAdjust: (function(adjusted) {
  if (!adjusted || EXCEPTIONS.infos[adjusted]) return adjusted;
  for (var ptr in EXCEPTIONS.infos) {
   var info = EXCEPTIONS.infos[ptr];
   if (info.adjusted === adjusted) {
    return ptr;
   }
  }
  return adjusted;
 }),
 addRef: (function(ptr) {
  if (!ptr) return;
  var info = EXCEPTIONS.infos[ptr];
  info.refcount++;
 }),
 decRef: (function(ptr) {
  if (!ptr) return;
  var info = EXCEPTIONS.infos[ptr];
  assert(info.refcount > 0);
  info.refcount--;
  if (info.refcount === 0) {
   if (info.destructor) {
    Runtime.dynCall("vi", info.destructor, [ ptr ]);
   }
   delete EXCEPTIONS.infos[ptr];
   ___cxa_free_exception(ptr);
  }
 }),
 clearRef: (function(ptr) {
  if (!ptr) return;
  var info = EXCEPTIONS.infos[ptr];
  info.refcount = 0;
 })
};
function ___resumeException(ptr) {
 if (!EXCEPTIONS.last) {
  EXCEPTIONS.last = ptr;
 }
 EXCEPTIONS.clearRef(EXCEPTIONS.deAdjust(ptr));
 throw ptr + " - Exception catching is disabled, this exception cannot be caught. Compile with -s DISABLE_EXCEPTION_CATCHING=0 or DISABLE_EXCEPTION_CATCHING=2 to catch.";
}
function ___cxa_find_matching_catch() {
 var thrown = EXCEPTIONS.last;
 if (!thrown) {
  return (asm["setTempRet0"](0), 0) | 0;
 }
 var info = EXCEPTIONS.infos[thrown];
 var throwntype = info.type;
 if (!throwntype) {
  return (asm["setTempRet0"](0), thrown) | 0;
 }
 var typeArray = Array.prototype.slice.call(arguments);
 var pointer = Module["___cxa_is_pointer_type"](throwntype);
 if (!___cxa_find_matching_catch.buffer) ___cxa_find_matching_catch.buffer = _malloc(4);
 HEAP32[___cxa_find_matching_catch.buffer >> 2] = thrown;
 thrown = ___cxa_find_matching_catch.buffer;
 for (var i = 0; i < typeArray.length; i++) {
  if (typeArray[i] && Module["___cxa_can_catch"](typeArray[i], throwntype, thrown)) {
   thrown = HEAP32[thrown >> 2];
   info.adjusted = thrown;
   return (asm["setTempRet0"](typeArray[i]), thrown) | 0;
  }
 }
 thrown = HEAP32[thrown >> 2];
 return (asm["setTempRet0"](throwntype), thrown) | 0;
}
function ___cxa_throw(ptr, type, destructor) {
 EXCEPTIONS.infos[ptr] = {
  ptr: ptr,
  adjusted: ptr,
  type: type,
  destructor: destructor,
  refcount: 0
 };
 EXCEPTIONS.last = ptr;
 if (!("uncaught_exception" in __ZSt18uncaught_exceptionv)) {
  __ZSt18uncaught_exceptionv.uncaught_exception = 1;
 } else {
  __ZSt18uncaught_exceptionv.uncaught_exception++;
 }
 throw ptr + " - Exception catching is disabled, this exception cannot be caught. Compile with -s DISABLE_EXCEPTION_CATCHING=0 or DISABLE_EXCEPTION_CATCHING=2 to catch.";
}
function _emscripten_set_touchmove_callback(target, userData, useCapture, callbackfunc) {
 JSEvents.registerTouchEventCallback(target, userData, useCapture, callbackfunc, 24, "touchmove");
 return 0;
}
function _emscripten_set_touchend_callback(target, userData, useCapture, callbackfunc) {
 JSEvents.registerTouchEventCallback(target, userData, useCapture, callbackfunc, 23, "touchend");
 return 0;
}
function _glStencilMaskSeparate(x0, x1) {
 GLctx.stencilMaskSeparate(x0, x1);
}
function _glDisableVertexAttribArray(index) {
 GLctx.disableVertexAttribArray(index);
}
function _glLinkProgram(program) {
 GLctx.linkProgram(GL.programs[program]);
 GL.programInfos[program] = null;
 GL.populateUniformTable(program);
}
function _glShaderSource(shader, count, string, length) {
 var source = GL.getSource(shader, count, string, length);
 GLctx.shaderSource(GL.shaders[shader], source);
}
function _glBindTexture(target, texture) {
 GLctx.bindTexture(target, texture ? GL.textures[texture] : null);
}
function _glDeleteRenderbuffers(n, renderbuffers) {
 for (var i = 0; i < n; i++) {
  var id = HEAP32[renderbuffers + i * 4 >> 2];
  var renderbuffer = GL.renderbuffers[id];
  if (!renderbuffer) continue;
  GLctx.deleteRenderbuffer(renderbuffer);
  renderbuffer.name = 0;
  GL.renderbuffers[id] = null;
 }
}
function _glDeleteFramebuffers(n, framebuffers) {
 for (var i = 0; i < n; ++i) {
  var id = HEAP32[framebuffers + i * 4 >> 2];
  var framebuffer = GL.framebuffers[id];
  if (!framebuffer) continue;
  GLctx.deleteFramebuffer(framebuffer);
  framebuffer.name = 0;
  GL.framebuffers[id] = null;
 }
}
function _glDrawArrays(mode, first, count) {
 GLctx.drawArrays(mode, first, count);
}
function _emscripten_set_touchstart_callback(target, userData, useCapture, callbackfunc) {
 JSEvents.registerTouchEventCallback(target, userData, useCapture, callbackfunc, 22, "touchstart");
 return 0;
}
function _emscripten_set_deviceorientation_callback(userData, useCapture, callbackfunc) {
 JSEvents.registerDeviceOrientationEventCallback(window, userData, useCapture, callbackfunc, 16, "deviceorientation");
 return 0;
}
function _glGetString(name_) {
 if (GL.stringCache[name_]) return GL.stringCache[name_];
 var ret;
 switch (name_) {
 case 7936:
 case 7937:
 case 7938:
  ret = allocate(intArrayFromString(GLctx.getParameter(name_)), "i8", ALLOC_NORMAL);
  break;
 case 7939:
  var exts = GLctx.getSupportedExtensions();
  var gl_exts = [];
  for (var i in exts) {
   gl_exts.push(exts[i]);
   gl_exts.push("GL_" + exts[i]);
  }
  ret = allocate(intArrayFromString(gl_exts.join(" ")), "i8", ALLOC_NORMAL);
  break;
 case 35724:
  var glslVersion = GLctx.getParameter(GLctx.SHADING_LANGUAGE_VERSION);
  if (glslVersion.indexOf("WebGL GLSL ES 1.0") != -1) glslVersion = "OpenGL ES GLSL ES 1.00 (WebGL)";
  ret = allocate(intArrayFromString(glslVersion), "i8", ALLOC_NORMAL);
  break;
 default:
  GL.recordError(1280);
  return 0;
 }
 GL.stringCache[name_] = ret;
 return ret;
}
function _emscripten_set_keypress_callback(target, userData, useCapture, callbackfunc) {
 JSEvents.registerKeyEventCallback(target, userData, useCapture, callbackfunc, 1, "keypress");
 return 0;
}
function _usleep(useconds) {
 var msec = useconds / 1e3;
 if ((ENVIRONMENT_IS_WEB || ENVIRONMENT_IS_WORKER) && self["performance"] && self["performance"]["now"]) {
  var start = self["performance"]["now"]();
  while (self["performance"]["now"]() - start < msec) {}
 } else {
  var start = Date.now();
  while (Date.now() - start < msec) {}
 }
 return 0;
}
function _nanosleep(rqtp, rmtp) {
 var seconds = HEAP32[rqtp >> 2];
 var nanoseconds = HEAP32[rqtp + 4 >> 2];
 if (rmtp !== 0) {
  HEAP32[rmtp >> 2] = 0;
  HEAP32[rmtp + 4 >> 2] = 0;
 }
 return _usleep(seconds * 1e6 + nanoseconds / 1e3);
}
function _glClear(x0) {
 GLctx.clear(x0);
}
function _glUniform2f(location, v0, v1) {
 location = GL.uniforms[location];
 GLctx.uniform2f(location, v0, v1);
}
function _glActiveTexture(x0) {
 GLctx.activeTexture(x0);
}
function _emscripten_get_now() {
 if (!_emscripten_get_now.actual) {
  if (ENVIRONMENT_IS_NODE) {
   _emscripten_get_now.actual = function _emscripten_get_now_actual() {
    var t = process["hrtime"]();
    return t[0] * 1e3 + t[1] / 1e6;
   };
  } else if (typeof dateNow !== "undefined") {
   _emscripten_get_now.actual = dateNow;
  } else if (typeof self === "object" && self["performance"] && typeof self["performance"]["now"] === "function") {
   _emscripten_get_now.actual = function _emscripten_get_now_actual() {
    return self["performance"]["now"]();
   };
  } else if (typeof performance === "object" && typeof performance["now"] === "function") {
   _emscripten_get_now.actual = function _emscripten_get_now_actual() {
    return performance["now"]();
   };
  } else {
   _emscripten_get_now.actual = Date.now;
  }
 }
 return _emscripten_get_now.actual();
}
function _emscripten_set_mouseup_callback(target, userData, useCapture, callbackfunc) {
 JSEvents.registerMouseEventCallback(target, userData, useCapture, callbackfunc, 6, "mouseup");
 return 0;
}
function _glBindBuffer(target, buffer) {
 var bufferObj = buffer ? GL.buffers[buffer] : null;
 GLctx.bindBuffer(target, bufferObj);
}
function _emscripten_webgl_destroy_context(contextHandle) {
 GL.deleteContext(contextHandle);
}
function _glEnableVertexAttribArray(index) {
 GLctx.enableVertexAttribArray(index);
}
function _glCompileShader(shader) {
 GLctx.compileShader(GL.shaders[shader]);
}
function _emscripten_set_main_loop_timing(mode, value) {
 Browser.mainLoop.timingMode = mode;
 Browser.mainLoop.timingValue = value;
 if (!Browser.mainLoop.func) {
  return 1;
 }
 if (mode == 0) {
  Browser.mainLoop.scheduler = function Browser_mainLoop_scheduler_setTimeout() {
   setTimeout(Browser.mainLoop.runner, value);
  };
  Browser.mainLoop.method = "timeout";
 } else if (mode == 1) {
  Browser.mainLoop.scheduler = function Browser_mainLoop_scheduler_rAF() {
   Browser.requestAnimationFrame(Browser.mainLoop.runner);
  };
  Browser.mainLoop.method = "rAF";
 } else if (mode == 2) {
  if (!window["setImmediate"]) {
   var setImmediates = [];
   var emscriptenMainLoopMessageId = "__emcc";
   function Browser_setImmediate_messageHandler(event) {
    if (event.source === window && event.data === emscriptenMainLoopMessageId) {
     event.stopPropagation();
     setImmediates.shift()();
    }
   }
   window.addEventListener("message", Browser_setImmediate_messageHandler, true);
   window["setImmediate"] = function Browser_emulated_setImmediate(func) {
    setImmediates.push(func);
    window.postMessage(emscriptenMainLoopMessageId, "*");
   };
  }
  Browser.mainLoop.scheduler = function Browser_mainLoop_scheduler_setImmediate() {
   window["setImmediate"](Browser.mainLoop.runner);
  };
  Browser.mainLoop.method = "immediate";
 }
 return 0;
}
function _emscripten_set_main_loop(func, fps, simulateInfiniteLoop, arg, noSetTiming) {
 Module["noExitRuntime"] = true;
 assert(!Browser.mainLoop.func, "emscripten_set_main_loop: there can only be one main loop function at once: call emscripten_cancel_main_loop to cancel the previous one before setting a new one with different parameters.");
 Browser.mainLoop.func = func;
 Browser.mainLoop.arg = arg;
 var thisMainLoopId = Browser.mainLoop.currentlyRunningMainloop;
 Browser.mainLoop.runner = function Browser_mainLoop_runner() {
  if (ABORT) return;
  if (Browser.mainLoop.queue.length > 0) {
   var start = Date.now();
   var blocker = Browser.mainLoop.queue.shift();
   blocker.func(blocker.arg);
   if (Browser.mainLoop.remainingBlockers) {
    var remaining = Browser.mainLoop.remainingBlockers;
    var next = remaining % 1 == 0 ? remaining - 1 : Math.floor(remaining);
    if (blocker.counted) {
     Browser.mainLoop.remainingBlockers = next;
    } else {
     next = next + .5;
     Browser.mainLoop.remainingBlockers = (8 * remaining + next) / 9;
    }
   }
   console.log('main loop blocker "' + blocker.name + '" took ' + (Date.now() - start) + " ms");
   Browser.mainLoop.updateStatus();
   if (thisMainLoopId < Browser.mainLoop.currentlyRunningMainloop) return;
   setTimeout(Browser.mainLoop.runner, 0);
   return;
  }
  if (thisMainLoopId < Browser.mainLoop.currentlyRunningMainloop) return;
  Browser.mainLoop.currentFrameNumber = Browser.mainLoop.currentFrameNumber + 1 | 0;
  if (Browser.mainLoop.timingMode == 1 && Browser.mainLoop.timingValue > 1 && Browser.mainLoop.currentFrameNumber % Browser.mainLoop.timingValue != 0) {
   Browser.mainLoop.scheduler();
   return;
  }
  if (Browser.mainLoop.method === "timeout" && Module.ctx) {
   Module.printErr("Looks like you are rendering without using requestAnimationFrame for the main loop. You should use 0 for the frame rate in emscripten_set_main_loop in order to use requestAnimationFrame, as that can greatly improve your frame rates!");
   Browser.mainLoop.method = "";
  }
  Browser.mainLoop.runIter((function() {
   if (typeof arg !== "undefined") {
    Runtime.dynCall("vi", func, [ arg ]);
   } else {
    Runtime.dynCall("v", func);
   }
  }));
  if (thisMainLoopId < Browser.mainLoop.currentlyRunningMainloop) return;
  if (typeof SDL === "object" && SDL.audio && SDL.audio.queueNewAudioData) SDL.audio.queueNewAudioData();
  Browser.mainLoop.scheduler();
 };
 if (!noSetTiming) {
  if (fps && fps > 0) _emscripten_set_main_loop_timing(0, 1e3 / fps); else _emscripten_set_main_loop_timing(1, 1);
  Browser.mainLoop.scheduler();
 }
 if (simulateInfiniteLoop) {
  throw "SimulateInfiniteLoop";
 }
}
var Browser = {
 mainLoop: {
  scheduler: null,
  method: "",
  currentlyRunningMainloop: 0,
  func: null,
  arg: 0,
  timingMode: 0,
  timingValue: 0,
  currentFrameNumber: 0,
  queue: [],
  pause: (function() {
   Browser.mainLoop.scheduler = null;
   Browser.mainLoop.currentlyRunningMainloop++;
  }),
  resume: (function() {
   Browser.mainLoop.currentlyRunningMainloop++;
   var timingMode = Browser.mainLoop.timingMode;
   var timingValue = Browser.mainLoop.timingValue;
   var func = Browser.mainLoop.func;
   Browser.mainLoop.func = null;
   _emscripten_set_main_loop(func, 0, false, Browser.mainLoop.arg, true);
   _emscripten_set_main_loop_timing(timingMode, timingValue);
   Browser.mainLoop.scheduler();
  }),
  updateStatus: (function() {
   if (Module["setStatus"]) {
    var message = Module["statusMessage"] || "Please wait...";
    var remaining = Browser.mainLoop.remainingBlockers;
    var expected = Browser.mainLoop.expectedBlockers;
    if (remaining) {
     if (remaining < expected) {
      Module["setStatus"](message + " (" + (expected - remaining) + "/" + expected + ")");
     } else {
      Module["setStatus"](message);
     }
    } else {
     Module["setStatus"]("");
    }
   }
  }),
  runIter: (function(func) {
   if (ABORT) return;
   if (Module["preMainLoop"]) {
    var preRet = Module["preMainLoop"]();
    if (preRet === false) {
     return;
    }
   }
   try {
    func();
   } catch (e) {
    if (e instanceof ExitStatus) {
     return;
    } else {
     if (e && typeof e === "object" && e.stack) Module.printErr("exception thrown: " + [ e, e.stack ]);
     throw e;
    }
   }
   if (Module["postMainLoop"]) Module["postMainLoop"]();
  })
 },
 isFullScreen: false,
 pointerLock: false,
 moduleContextCreatedCallbacks: [],
 workers: [],
 init: (function() {
  if (!Module["preloadPlugins"]) Module["preloadPlugins"] = [];
  if (Browser.initted) return;
  Browser.initted = true;
  try {
   new Blob;
   Browser.hasBlobConstructor = true;
  } catch (e) {
   Browser.hasBlobConstructor = false;
   console.log("warning: no blob constructor, cannot create blobs with mimetypes");
  }
  Browser.BlobBuilder = typeof MozBlobBuilder != "undefined" ? MozBlobBuilder : typeof WebKitBlobBuilder != "undefined" ? WebKitBlobBuilder : !Browser.hasBlobConstructor ? console.log("warning: no BlobBuilder") : null;
  Browser.URLObject = typeof window != "undefined" ? window.URL ? window.URL : window.webkitURL : undefined;
  if (!Module.noImageDecoding && typeof Browser.URLObject === "undefined") {
   console.log("warning: Browser does not support creating object URLs. Built-in browser image decoding will not be available.");
   Module.noImageDecoding = true;
  }
  var imagePlugin = {};
  imagePlugin["canHandle"] = function imagePlugin_canHandle(name) {
   return !Module.noImageDecoding && /\.(jpg|jpeg|png|bmp)$/i.test(name);
  };
  imagePlugin["handle"] = function imagePlugin_handle(byteArray, name, onload, onerror) {
   var b = null;
   if (Browser.hasBlobConstructor) {
    try {
     b = new Blob([ byteArray ], {
      type: Browser.getMimetype(name)
     });
     if (b.size !== byteArray.length) {
      b = new Blob([ (new Uint8Array(byteArray)).buffer ], {
       type: Browser.getMimetype(name)
      });
     }
    } catch (e) {
     Runtime.warnOnce("Blob constructor present but fails: " + e + "; falling back to blob builder");
    }
   }
   if (!b) {
    var bb = new Browser.BlobBuilder;
    bb.append((new Uint8Array(byteArray)).buffer);
    b = bb.getBlob();
   }
   var url = Browser.URLObject.createObjectURL(b);
   var img = new Image;
   img.onload = function img_onload() {
    assert(img.complete, "Image " + name + " could not be decoded");
    var canvas = document.createElement("canvas");
    canvas.width = img.width;
    canvas.height = img.height;
    var ctx = canvas.getContext("2d");
    ctx.drawImage(img, 0, 0);
    Module["preloadedImages"][name] = canvas;
    Browser.URLObject.revokeObjectURL(url);
    if (onload) onload(byteArray);
   };
   img.onerror = function img_onerror(event) {
    console.log("Image " + url + " could not be decoded");
    if (onerror) onerror();
   };
   img.src = url;
  };
  Module["preloadPlugins"].push(imagePlugin);
  var audioPlugin = {};
  audioPlugin["canHandle"] = function audioPlugin_canHandle(name) {
   return !Module.noAudioDecoding && name.substr(-4) in {
    ".ogg": 1,
    ".wav": 1,
    ".mp3": 1
   };
  };
  audioPlugin["handle"] = function audioPlugin_handle(byteArray, name, onload, onerror) {
   var done = false;
   function finish(audio) {
    if (done) return;
    done = true;
    Module["preloadedAudios"][name] = audio;
    if (onload) onload(byteArray);
   }
   function fail() {
    if (done) return;
    done = true;
    Module["preloadedAudios"][name] = new Audio;
    if (onerror) onerror();
   }
   if (Browser.hasBlobConstructor) {
    try {
     var b = new Blob([ byteArray ], {
      type: Browser.getMimetype(name)
     });
    } catch (e) {
     return fail();
    }
    var url = Browser.URLObject.createObjectURL(b);
    var audio = new Audio;
    audio.addEventListener("canplaythrough", (function() {
     finish(audio);
    }), false);
    audio.onerror = function audio_onerror(event) {
     if (done) return;
     console.log("warning: browser could not fully decode audio " + name + ", trying slower base64 approach");
     function encode64(data) {
      var BASE = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
      var PAD = "=";
      var ret = "";
      var leftchar = 0;
      var leftbits = 0;
      for (var i = 0; i < data.length; i++) {
       leftchar = leftchar << 8 | data[i];
       leftbits += 8;
       while (leftbits >= 6) {
        var curr = leftchar >> leftbits - 6 & 63;
        leftbits -= 6;
        ret += BASE[curr];
       }
      }
      if (leftbits == 2) {
       ret += BASE[(leftchar & 3) << 4];
       ret += PAD + PAD;
      } else if (leftbits == 4) {
       ret += BASE[(leftchar & 15) << 2];
       ret += PAD;
      }
      return ret;
     }
     audio.src = "data:audio/x-" + name.substr(-3) + ";base64," + encode64(byteArray);
     finish(audio);
    };
    audio.src = url;
    Browser.safeSetTimeout((function() {
     finish(audio);
    }), 1e4);
   } else {
    return fail();
   }
  };
  Module["preloadPlugins"].push(audioPlugin);
  var canvas = Module["canvas"];
  function pointerLockChange() {
   Browser.pointerLock = document["pointerLockElement"] === canvas || document["mozPointerLockElement"] === canvas || document["webkitPointerLockElement"] === canvas || document["msPointerLockElement"] === canvas;
  }
  if (canvas) {
   canvas.requestPointerLock = canvas["requestPointerLock"] || canvas["mozRequestPointerLock"] || canvas["webkitRequestPointerLock"] || canvas["msRequestPointerLock"] || (function() {});
   canvas.exitPointerLock = document["exitPointerLock"] || document["mozExitPointerLock"] || document["webkitExitPointerLock"] || document["msExitPointerLock"] || (function() {});
   canvas.exitPointerLock = canvas.exitPointerLock.bind(document);
   document.addEventListener("pointerlockchange", pointerLockChange, false);
   document.addEventListener("mozpointerlockchange", pointerLockChange, false);
   document.addEventListener("webkitpointerlockchange", pointerLockChange, false);
   document.addEventListener("mspointerlockchange", pointerLockChange, false);
   if (Module["elementPointerLock"]) {
    canvas.addEventListener("click", (function(ev) {
     if (!Browser.pointerLock && canvas.requestPointerLock) {
      canvas.requestPointerLock();
      ev.preventDefault();
     }
    }), false);
   }
  }
 }),
 createContext: (function(canvas, useWebGL, setInModule, webGLContextAttributes) {
  if (useWebGL && Module.ctx && canvas == Module.canvas) return Module.ctx;
  var ctx;
  var contextHandle;
  if (useWebGL) {
   var contextAttributes = {
    antialias: false,
    alpha: false
   };
   if (webGLContextAttributes) {
    for (var attribute in webGLContextAttributes) {
     contextAttributes[attribute] = webGLContextAttributes[attribute];
    }
   }
   contextHandle = GL.createContext(canvas, contextAttributes);
   if (contextHandle) {
    ctx = GL.getContext(contextHandle).GLctx;
   }
   canvas.style.backgroundColor = "black";
  } else {
   ctx = canvas.getContext("2d");
  }
  if (!ctx) return null;
  if (setInModule) {
   if (!useWebGL) assert(typeof GLctx === "undefined", "cannot set in module if GLctx is used, but we are a non-GL context that would replace it");
   Module.ctx = ctx;
   if (useWebGL) GL.makeContextCurrent(contextHandle);
   Module.useWebGL = useWebGL;
   Browser.moduleContextCreatedCallbacks.forEach((function(callback) {
    callback();
   }));
   Browser.init();
  }
  return ctx;
 }),
 destroyContext: (function(canvas, useWebGL, setInModule) {}),
 fullScreenHandlersInstalled: false,
 lockPointer: undefined,
 resizeCanvas: undefined,
 requestFullScreen: (function(lockPointer, resizeCanvas, vrDevice) {
  Browser.lockPointer = lockPointer;
  Browser.resizeCanvas = resizeCanvas;
  Browser.vrDevice = vrDevice;
  if (typeof Browser.lockPointer === "undefined") Browser.lockPointer = true;
  if (typeof Browser.resizeCanvas === "undefined") Browser.resizeCanvas = false;
  if (typeof Browser.vrDevice === "undefined") Browser.vrDevice = null;
  var canvas = Module["canvas"];
  function fullScreenChange() {
   Browser.isFullScreen = false;
   var canvasContainer = canvas.parentNode;
   if ((document["webkitFullScreenElement"] || document["webkitFullscreenElement"] || document["mozFullScreenElement"] || document["mozFullscreenElement"] || document["fullScreenElement"] || document["fullscreenElement"] || document["msFullScreenElement"] || document["msFullscreenElement"] || document["webkitCurrentFullScreenElement"]) === canvasContainer) {
    canvas.cancelFullScreen = document["cancelFullScreen"] || document["mozCancelFullScreen"] || document["webkitCancelFullScreen"] || document["msExitFullscreen"] || document["exitFullscreen"] || (function() {});
    canvas.cancelFullScreen = canvas.cancelFullScreen.bind(document);
    if (Browser.lockPointer) canvas.requestPointerLock();
    Browser.isFullScreen = true;
    if (Browser.resizeCanvas) Browser.setFullScreenCanvasSize();
   } else {
    canvasContainer.parentNode.insertBefore(canvas, canvasContainer);
    canvasContainer.parentNode.removeChild(canvasContainer);
    if (Browser.resizeCanvas) Browser.setWindowedCanvasSize();
   }
   if (Module["onFullScreen"]) Module["onFullScreen"](Browser.isFullScreen);
   Browser.updateCanvasDimensions(canvas);
  }
  if (!Browser.fullScreenHandlersInstalled) {
   Browser.fullScreenHandlersInstalled = true;
   document.addEventListener("fullscreenchange", fullScreenChange, false);
   document.addEventListener("mozfullscreenchange", fullScreenChange, false);
   document.addEventListener("webkitfullscreenchange", fullScreenChange, false);
   document.addEventListener("MSFullscreenChange", fullScreenChange, false);
  }
  var canvasContainer = document.createElement("div");
  canvas.parentNode.insertBefore(canvasContainer, canvas);
  canvasContainer.appendChild(canvas);
  canvasContainer.requestFullScreen = canvasContainer["requestFullScreen"] || canvasContainer["mozRequestFullScreen"] || canvasContainer["msRequestFullscreen"] || (canvasContainer["webkitRequestFullScreen"] ? (function() {
   canvasContainer["webkitRequestFullScreen"](Element["ALLOW_KEYBOARD_INPUT"]);
  }) : null);
  if (vrDevice) {
   canvasContainer.requestFullScreen({
    vrDisplay: vrDevice
   });
  } else {
   canvasContainer.requestFullScreen();
  }
 }),
 nextRAF: 0,
 fakeRequestAnimationFrame: (function(func) {
  var now = Date.now();
  if (Browser.nextRAF === 0) {
   Browser.nextRAF = now + 1e3 / 60;
  } else {
   while (now + 2 >= Browser.nextRAF) {
    Browser.nextRAF += 1e3 / 60;
   }
  }
  var delay = Math.max(Browser.nextRAF - now, 0);
  setTimeout(func, delay);
 }),
 requestAnimationFrame: function requestAnimationFrame(func) {
  if (typeof window === "undefined") {
   Browser.fakeRequestAnimationFrame(func);
  } else {
   if (!window.requestAnimationFrame) {
    window.requestAnimationFrame = window["requestAnimationFrame"] || window["mozRequestAnimationFrame"] || window["webkitRequestAnimationFrame"] || window["msRequestAnimationFrame"] || window["oRequestAnimationFrame"] || Browser.fakeRequestAnimationFrame;
   }
   window.requestAnimationFrame(func);
  }
 },
 safeCallback: (function(func) {
  return (function() {
   if (!ABORT) return func.apply(null, arguments);
  });
 }),
 allowAsyncCallbacks: true,
 queuedAsyncCallbacks: [],
 pauseAsyncCallbacks: (function() {
  Browser.allowAsyncCallbacks = false;
 }),
 resumeAsyncCallbacks: (function() {
  Browser.allowAsyncCallbacks = true;
  if (Browser.queuedAsyncCallbacks.length > 0) {
   var callbacks = Browser.queuedAsyncCallbacks;
   Browser.queuedAsyncCallbacks = [];
   callbacks.forEach((function(func) {
    func();
   }));
  }
 }),
 safeRequestAnimationFrame: (function(func) {
  return Browser.requestAnimationFrame((function() {
   if (ABORT) return;
   if (Browser.allowAsyncCallbacks) {
    func();
   } else {
    Browser.queuedAsyncCallbacks.push(func);
   }
  }));
 }),
 safeSetTimeout: (function(func, timeout) {
  Module["noExitRuntime"] = true;
  return setTimeout((function() {
   if (ABORT) return;
   if (Browser.allowAsyncCallbacks) {
    func();
   } else {
    Browser.queuedAsyncCallbacks.push(func);
   }
  }), timeout);
 }),
 safeSetInterval: (function(func, timeout) {
  Module["noExitRuntime"] = true;
  return setInterval((function() {
   if (ABORT) return;
   if (Browser.allowAsyncCallbacks) {
    func();
   }
  }), timeout);
 }),
 getMimetype: (function(name) {
  return {
   "jpg": "image/jpeg",
   "jpeg": "image/jpeg",
   "png": "image/png",
   "bmp": "image/bmp",
   "ogg": "audio/ogg",
   "wav": "audio/wav",
   "mp3": "audio/mpeg"
  }[name.substr(name.lastIndexOf(".") + 1)];
 }),
 getUserMedia: (function(func) {
  if (!window.getUserMedia) {
   window.getUserMedia = navigator["getUserMedia"] || navigator["mozGetUserMedia"];
  }
  window.getUserMedia(func);
 }),
 getMovementX: (function(event) {
  return event["movementX"] || event["mozMovementX"] || event["webkitMovementX"] || 0;
 }),
 getMovementY: (function(event) {
  return event["movementY"] || event["mozMovementY"] || event["webkitMovementY"] || 0;
 }),
 getMouseWheelDelta: (function(event) {
  var delta = 0;
  switch (event.type) {
  case "DOMMouseScroll":
   delta = event.detail;
   break;
  case "mousewheel":
   delta = event.wheelDelta;
   break;
  case "wheel":
   delta = event["deltaY"];
   break;
  default:
   throw "unrecognized mouse wheel event: " + event.type;
  }
  return delta;
 }),
 mouseX: 0,
 mouseY: 0,
 mouseMovementX: 0,
 mouseMovementY: 0,
 touches: {},
 lastTouches: {},
 calculateMouseEvent: (function(event) {
  if (Browser.pointerLock) {
   if (event.type != "mousemove" && "mozMovementX" in event) {
    Browser.mouseMovementX = Browser.mouseMovementY = 0;
   } else {
    Browser.mouseMovementX = Browser.getMovementX(event);
    Browser.mouseMovementY = Browser.getMovementY(event);
   }
   if (typeof SDL != "undefined") {
    Browser.mouseX = SDL.mouseX + Browser.mouseMovementX;
    Browser.mouseY = SDL.mouseY + Browser.mouseMovementY;
   } else {
    Browser.mouseX += Browser.mouseMovementX;
    Browser.mouseY += Browser.mouseMovementY;
   }
  } else {
   var rect = Module["canvas"].getBoundingClientRect();
   var cw = Module["canvas"].width;
   var ch = Module["canvas"].height;
   var scrollX = typeof window.scrollX !== "undefined" ? window.scrollX : window.pageXOffset;
   var scrollY = typeof window.scrollY !== "undefined" ? window.scrollY : window.pageYOffset;
   if (event.type === "touchstart" || event.type === "touchend" || event.type === "touchmove") {
    var touch = event.touch;
    if (touch === undefined) {
     return;
    }
    var adjustedX = touch.pageX - (scrollX + rect.left);
    var adjustedY = touch.pageY - (scrollY + rect.top);
    adjustedX = adjustedX * (cw / rect.width);
    adjustedY = adjustedY * (ch / rect.height);
    var coords = {
     x: adjustedX,
     y: adjustedY
    };
    if (event.type === "touchstart") {
     Browser.lastTouches[touch.identifier] = coords;
     Browser.touches[touch.identifier] = coords;
    } else if (event.type === "touchend" || event.type === "touchmove") {
     var last = Browser.touches[touch.identifier];
     if (!last) last = coords;
     Browser.lastTouches[touch.identifier] = last;
     Browser.touches[touch.identifier] = coords;
    }
    return;
   }
   var x = event.pageX - (scrollX + rect.left);
   var y = event.pageY - (scrollY + rect.top);
   x = x * (cw / rect.width);
   y = y * (ch / rect.height);
   Browser.mouseMovementX = x - Browser.mouseX;
   Browser.mouseMovementY = y - Browser.mouseY;
   Browser.mouseX = x;
   Browser.mouseY = y;
  }
 }),
 asyncLoad: (function(url, onload, onerror, noRunDep) {
  Module["readAsync"](url, (function(arrayBuffer) {
   assert(arrayBuffer, 'Loading data file "' + url + '" failed (no arrayBuffer).');
   onload(new Uint8Array(arrayBuffer));
   if (!noRunDep) removeRunDependency("al " + url);
  }), (function(event) {
   if (onerror) {
    onerror();
   } else {
    throw 'Loading data file "' + url + '" failed.';
   }
  }));
  if (!noRunDep) addRunDependency("al " + url);
 }),
 resizeListeners: [],
 updateResizeListeners: (function() {
  var canvas = Module["canvas"];
  Browser.resizeListeners.forEach((function(listener) {
   listener(canvas.width, canvas.height);
  }));
 }),
 setCanvasSize: (function(width, height, noUpdates) {
  var canvas = Module["canvas"];
  Browser.updateCanvasDimensions(canvas, width, height);
  if (!noUpdates) Browser.updateResizeListeners();
 }),
 windowedWidth: 0,
 windowedHeight: 0,
 setFullScreenCanvasSize: (function() {
  if (typeof SDL != "undefined") {
   var flags = HEAPU32[SDL.screen + Runtime.QUANTUM_SIZE * 0 >> 2];
   flags = flags | 8388608;
   HEAP32[SDL.screen + Runtime.QUANTUM_SIZE * 0 >> 2] = flags;
  }
  Browser.updateResizeListeners();
 }),
 setWindowedCanvasSize: (function() {
  if (typeof SDL != "undefined") {
   var flags = HEAPU32[SDL.screen + Runtime.QUANTUM_SIZE * 0 >> 2];
   flags = flags & ~8388608;
   HEAP32[SDL.screen + Runtime.QUANTUM_SIZE * 0 >> 2] = flags;
  }
  Browser.updateResizeListeners();
 }),
 updateCanvasDimensions: (function(canvas, wNative, hNative) {
  if (wNative && hNative) {
   canvas.widthNative = wNative;
   canvas.heightNative = hNative;
  } else {
   wNative = canvas.widthNative;
   hNative = canvas.heightNative;
  }
  var w = wNative;
  var h = hNative;
  if (Module["forcedAspectRatio"] && Module["forcedAspectRatio"] > 0) {
   if (w / h < Module["forcedAspectRatio"]) {
    w = Math.round(h * Module["forcedAspectRatio"]);
   } else {
    h = Math.round(w / Module["forcedAspectRatio"]);
   }
  }
  if ((document["webkitFullScreenElement"] || document["webkitFullscreenElement"] || document["mozFullScreenElement"] || document["mozFullscreenElement"] || document["fullScreenElement"] || document["fullscreenElement"] || document["msFullScreenElement"] || document["msFullscreenElement"] || document["webkitCurrentFullScreenElement"]) === canvas.parentNode && typeof screen != "undefined") {
   var factor = Math.min(screen.width / w, screen.height / h);
   w = Math.round(w * factor);
   h = Math.round(h * factor);
  }
  if (Browser.resizeCanvas) {
   if (canvas.width != w) canvas.width = w;
   if (canvas.height != h) canvas.height = h;
   if (typeof canvas.style != "undefined") {
    canvas.style.removeProperty("width");
    canvas.style.removeProperty("height");
   }
  } else {
   if (canvas.width != wNative) canvas.width = wNative;
   if (canvas.height != hNative) canvas.height = hNative;
   if (typeof canvas.style != "undefined") {
    if (w != wNative || h != hNative) {
     canvas.style.setProperty("width", w + "px", "important");
     canvas.style.setProperty("height", h + "px", "important");
    } else {
     canvas.style.removeProperty("width");
     canvas.style.removeProperty("height");
    }
   }
  }
 }),
 wgetRequests: {},
 nextWgetRequestHandle: 0,
 getNextWgetRequestHandle: (function() {
  var handle = Browser.nextWgetRequestHandle;
  Browser.nextWgetRequestHandle++;
  return handle;
 })
};
function _emscripten_cancel_main_loop() {
 Browser.mainLoop.pause();
 Browser.mainLoop.func = null;
}
function _glUniform4f(location, v0, v1, v2, v3) {
 location = GL.uniforms[location];
 GLctx.uniform4f(location, v0, v1, v2, v3);
}
function ___gxx_personality_v0() {}
function _glDeleteTextures(n, textures) {
 for (var i = 0; i < n; i++) {
  var id = HEAP32[textures + i * 4 >> 2];
  var texture = GL.textures[id];
  if (!texture) continue;
  GLctx.deleteTexture(texture);
  texture.name = 0;
  GL.textures[id] = null;
 }
}
function _emscripten_request_pointerlock(target, deferUntilInEventHandler) {
 if (!target) target = "#canvas";
 target = JSEvents.findEventTarget(target);
 if (!target) return -4;
 if (!target.requestPointerLock && !target.mozRequestPointerLock && !target.webkitRequestPointerLock && !target.msRequestPointerLock) {
  return -1;
 }
 var canPerformRequests = JSEvents.canPerformEventHandlerRequests();
 if (!canPerformRequests) {
  if (deferUntilInEventHandler) {
   JSEvents.deferCall(JSEvents.requestPointerLock, 2, [ target ]);
   return 1;
  } else {
   return -2;
  }
 }
 return JSEvents.requestPointerLock(target);
}
function _glStencilOpSeparate(x0, x1, x2, x3) {
 GLctx.stencilOpSeparate(x0, x1, x2, x3);
}
function _glUniformMatrix3fv(location, count, transpose, value) {
 location = GL.uniforms[location];
 var view;
 if (9 * count <= GL.MINI_TEMP_BUFFER_SIZE) {
  view = GL.miniTempBufferViews[9 * count - 1];
  for (var i = 0; i < 9 * count; i += 9) {
   view[i] = HEAPF32[value + 4 * i >> 2];
   view[i + 1] = HEAPF32[value + (4 * i + 4) >> 2];
   view[i + 2] = HEAPF32[value + (4 * i + 8) >> 2];
   view[i + 3] = HEAPF32[value + (4 * i + 12) >> 2];
   view[i + 4] = HEAPF32[value + (4 * i + 16) >> 2];
   view[i + 5] = HEAPF32[value + (4 * i + 20) >> 2];
   view[i + 6] = HEAPF32[value + (4 * i + 24) >> 2];
   view[i + 7] = HEAPF32[value + (4 * i + 28) >> 2];
   view[i + 8] = HEAPF32[value + (4 * i + 32) >> 2];
  }
 } else {
  view = HEAPF32.subarray(value >> 2, value + count * 36 >> 2);
 }
 GLctx.uniformMatrix3fv(location, transpose, view);
}
function _glStencilFuncSeparate(x0, x1, x2, x3) {
 GLctx.stencilFuncSeparate(x0, x1, x2, x3);
}
function _pthread_cleanup_push(routine, arg) {
 __ATEXIT__.push((function() {
  Runtime.dynCall("vi", routine, [ arg ]);
 }));
 _pthread_cleanup_push.level = __ATEXIT__.length;
}
function _emscripten_webgl_create_context(target, attributes) {
 var contextAttributes = {};
 contextAttributes.alpha = !!HEAP32[attributes >> 2];
 contextAttributes.depth = !!HEAP32[attributes + 4 >> 2];
 contextAttributes.stencil = !!HEAP32[attributes + 8 >> 2];
 contextAttributes.antialias = !!HEAP32[attributes + 12 >> 2];
 contextAttributes.premultipliedAlpha = !!HEAP32[attributes + 16 >> 2];
 contextAttributes.preserveDrawingBuffer = !!HEAP32[attributes + 20 >> 2];
 contextAttributes.preferLowPowerToHighPerformance = !!HEAP32[attributes + 24 >> 2];
 contextAttributes.failIfMajorPerformanceCaveat = !!HEAP32[attributes + 28 >> 2];
 contextAttributes.majorVersion = HEAP32[attributes + 32 >> 2];
 contextAttributes.minorVersion = HEAP32[attributes + 36 >> 2];
 if (!target) {
  target = Module["canvas"];
 } else {
  target = JSEvents.findEventTarget(target);
 }
 var contextHandle = GL.createContext(target, contextAttributes);
 return contextHandle;
}
function _glClearDepthf(x0) {
 GLctx.clearDepth(x0);
}
function _glStencilOp(x0, x1, x2) {
 GLctx.stencilOp(x0, x1, x2);
}
function _emscripten_set_keydown_callback(target, userData, useCapture, callbackfunc) {
 JSEvents.registerKeyEventCallback(target, userData, useCapture, callbackfunc, 2, "keydown");
 return 0;
}
function _glVertexAttribPointer(index, size, type, normalized, stride, ptr) {
 GLctx.vertexAttribPointer(index, size, type, normalized, stride, ptr);
}
function _emscripten_memcpy_big(dest, src, num) {
 HEAPU8.set(HEAPU8.subarray(src, src + num), dest);
 return dest;
}
Module["_memcpy"] = _memcpy;
function _emscripten_set_devicemotion_callback(userData, useCapture, callbackfunc) {
 JSEvents.registerDeviceMotionEventCallback(window, userData, useCapture, callbackfunc, 17, "devicemotion");
 return 0;
}
function _sbrk(bytes) {
 var self = _sbrk;
 if (!self.called) {
  DYNAMICTOP = alignMemoryPage(DYNAMICTOP);
  self.called = true;
  assert(Runtime.dynamicAlloc);
  self.alloc = Runtime.dynamicAlloc;
  Runtime.dynamicAlloc = (function() {
   abort("cannot dynamically allocate, sbrk now has control");
  });
 }
 var ret = DYNAMICTOP;
 if (bytes != 0) {
  var success = self.alloc(bytes);
  if (!success) return -1 >>> 0;
 }
 return ret;
}
Module["_memmove"] = _memmove;
function _glGenTextures(n, textures) {
 for (var i = 0; i < n; i++) {
  var texture = GLctx.createTexture();
  if (!texture) {
   GL.recordError(1282);
   while (i < n) HEAP32[textures + i++ * 4 >> 2] = 0;
   return;
  }
  var id = GL.getNewId(GL.textures);
  texture.name = id;
  GL.textures[id] = texture;
  HEAP32[textures + i * 4 >> 2] = id;
 }
}
function _glStencilFunc(x0, x1, x2) {
 GLctx.stencilFunc(x0, x1, x2);
}
function _glDepthFunc(x0) {
 GLctx.depthFunc(x0);
}
function emscriptenWebGLGet(name_, p, type) {
 if (!p) {
  GL.recordError(1281);
  return;
 }
 var ret = undefined;
 switch (name_) {
 case 36346:
  ret = 1;
  break;
 case 36344:
  if (type !== "Integer" && type !== "Integer64") {
   GL.recordError(1280);
  }
  return;
 case 36345:
  ret = 0;
  break;
 case 34466:
  var formats = GLctx.getParameter(34467);
  ret = formats.length;
  break;
 case 35738:
  ret = 5121;
  break;
 case 35739:
  ret = 6408;
  break;
 }
 if (ret === undefined) {
  var result = GLctx.getParameter(name_);
  switch (typeof result) {
  case "number":
   ret = result;
   break;
  case "boolean":
   ret = result ? 1 : 0;
   break;
  case "string":
   GL.recordError(1280);
   return;
  case "object":
   if (result === null) {
    switch (name_) {
    case 34964:
    case 35725:
    case 34965:
    case 36006:
    case 36007:
    case 32873:
    case 34068:
     {
      ret = 0;
      break;
     }
    default:
     {
      GL.recordError(1280);
      return;
     }
    }
   } else if (result instanceof Float32Array || result instanceof Uint32Array || result instanceof Int32Array || result instanceof Array) {
    for (var i = 0; i < result.length; ++i) {
     switch (type) {
     case "Integer":
      HEAP32[p + i * 4 >> 2] = result[i];
      break;
     case "Float":
      HEAPF32[p + i * 4 >> 2] = result[i];
      break;
     case "Boolean":
      HEAP8[p + i >> 0] = result[i] ? 1 : 0;
      break;
     default:
      throw "internal glGet error, bad type: " + type;
     }
    }
    return;
   } else if (result instanceof WebGLBuffer || result instanceof WebGLProgram || result instanceof WebGLFramebuffer || result instanceof WebGLRenderbuffer || result instanceof WebGLTexture) {
    ret = result.name | 0;
   } else {
    GL.recordError(1280);
    return;
   }
   break;
  default:
   GL.recordError(1280);
   return;
  }
 }
 switch (type) {
 case "Integer64":
  tempI64 = [ ret >>> 0, (tempDouble = ret, +Math_abs(tempDouble) >= +1 ? tempDouble > +0 ? (Math_min(+Math_floor(tempDouble / +4294967296), +4294967295) | 0) >>> 0 : ~~+Math_ceil((tempDouble - +(~~tempDouble >>> 0)) / +4294967296) >>> 0 : 0) ], HEAP32[p >> 2] = tempI64[0], HEAP32[p + 4 >> 2] = tempI64[1];
  break;
 case "Integer":
  HEAP32[p >> 2] = ret;
  break;
 case "Float":
  HEAPF32[p >> 2] = ret;
  break;
 case "Boolean":
  HEAP8[p >> 0] = ret ? 1 : 0;
  break;
 default:
  throw "internal glGet error, bad type: " + type;
 }
}
function _glGetIntegerv(name_, p) {
 emscriptenWebGLGet(name_, p, "Integer");
}
var SYSCALLS = {
 varargs: 0,
 get: (function(varargs) {
  SYSCALLS.varargs += 4;
  var ret = HEAP32[SYSCALLS.varargs - 4 >> 2];
  return ret;
 }),
 getStr: (function() {
  var ret = Pointer_stringify(SYSCALLS.get());
  return ret;
 }),
 get64: (function() {
  var low = SYSCALLS.get(), high = SYSCALLS.get();
  if (low >= 0) assert(high === 0); else assert(high === -1);
  return low;
 }),
 getZero: (function() {
  assert(SYSCALLS.get() === 0);
 })
};
function ___syscall54(which, varargs) {
 SYSCALLS.varargs = varargs;
 try {
  return 0;
 } catch (e) {
  if (typeof FS === "undefined" || !(e instanceof FS.ErrnoError)) abort(e);
  return -e.errno;
 }
}
function _glUniform1f(location, v0) {
 location = GL.uniforms[location];
 GLctx.uniform1f(location, v0);
}
function _glDepthMask(x0) {
 GLctx.depthMask(x0);
}
function _glDeleteBuffers(n, buffers) {
 for (var i = 0; i < n; i++) {
  var id = HEAP32[buffers + i * 4 >> 2];
  var buffer = GL.buffers[id];
  if (!buffer) continue;
  GLctx.deleteBuffer(buffer);
  buffer.name = 0;
  GL.buffers[id] = null;
  if (id == GL.currArrayBuffer) GL.currArrayBuffer = 0;
  if (id == GL.currElementArrayBuffer) GL.currElementArrayBuffer = 0;
 }
}
function _glCreateShader(shaderType) {
 var id = GL.getNewId(GL.shaders);
 GL.shaders[id] = GLctx.createShader(shaderType);
 return id;
}
function _glFrontFace(x0) {
 GLctx.frontFace(x0);
}
function _glUniform1i(location, v0) {
 location = GL.uniforms[location];
 GLctx.uniform1i(location, v0);
}
function _glUniformMatrix2fv(location, count, transpose, value) {
 location = GL.uniforms[location];
 var view;
 if (4 * count <= GL.MINI_TEMP_BUFFER_SIZE) {
  view = GL.miniTempBufferViews[4 * count - 1];
  for (var i = 0; i < 4 * count; i += 4) {
   view[i] = HEAPF32[value + 4 * i >> 2];
   view[i + 1] = HEAPF32[value + (4 * i + 4) >> 2];
   view[i + 2] = HEAPF32[value + (4 * i + 8) >> 2];
   view[i + 3] = HEAPF32[value + (4 * i + 12) >> 2];
  }
 } else {
  view = HEAPF32.subarray(value >> 2, value + count * 16 >> 2);
 }
 GLctx.uniformMatrix2fv(location, transpose, view);
}
function _glUseProgram(program) {
 GLctx.useProgram(program ? GL.programs[program] : null);
}
function _glCompressedTexImage2D(target, level, internalFormat, width, height, border, imageSize, data) {
 var heapView;
 if (data) {
  heapView = HEAPU8.subarray(data, data + imageSize);
 } else {
  heapView = null;
 }
 GLctx["compressedTexImage2D"](target, level, internalFormat, width, height, border, heapView);
}
function emscriptenWebGLComputeImageSize(width, height, sizePerPixel, alignment) {
 function roundedToNextMultipleOf(x, y) {
  return Math.floor((x + y - 1) / y) * y;
 }
 var plainRowSize = width * sizePerPixel;
 var alignedRowSize = roundedToNextMultipleOf(plainRowSize, alignment);
 return height <= 0 ? 0 : (height - 1) * alignedRowSize + plainRowSize;
}
function emscriptenWebGLGetTexPixelData(type, format, width, height, pixels, internalFormat) {
 var sizePerPixel;
 var numChannels;
 switch (format) {
 case 6406:
 case 6409:
 case 6402:
 case 6403:
  numChannels = 1;
  break;
 case 6410:
 case 33319:
  numChannels = 2;
  break;
 case 6407:
 case 35904:
  numChannels = 3;
  break;
 case 6408:
 case 35906:
  numChannels = 4;
  break;
 default:
  GL.recordError(1280);
  return {
   pixels: null,
   internalFormat: 0
  };
 }
 switch (type) {
 case 5121:
  sizePerPixel = numChannels * 1;
  break;
 case 5123:
 case 36193:
  sizePerPixel = numChannels * 2;
  break;
 case 5125:
 case 5126:
  sizePerPixel = numChannels * 4;
  break;
 case 34042:
  sizePerPixel = 4;
  break;
 case 33635:
 case 32819:
 case 32820:
  sizePerPixel = 2;
  break;
 default:
  GL.recordError(1280);
  return {
   pixels: null,
   internalFormat: 0
  };
 }
 var bytes = emscriptenWebGLComputeImageSize(width, height, sizePerPixel, GL.unpackAlignment);
 if (type == 5121) {
  pixels = HEAPU8.subarray(pixels, pixels + bytes);
 } else if (type == 5126) {
  pixels = HEAPF32.subarray(pixels >> 2, pixels + bytes >> 2);
 } else if (type == 5125 || type == 34042) {
  pixels = HEAPU32.subarray(pixels >> 2, pixels + bytes >> 2);
 } else {
  pixels = HEAPU16.subarray(pixels >> 1, pixels + bytes >> 1);
 }
 return {
  pixels: pixels,
  internalFormat: internalFormat
 };
}
function _glTexImage2D(target, level, internalFormat, width, height, border, format, type, pixels) {
 var pixelData;
 if (pixels) {
  var data = emscriptenWebGLGetTexPixelData(type, format, width, height, pixels, internalFormat);
  pixelData = data.pixels;
  internalFormat = data.internalFormat;
 } else {
  pixelData = null;
 }
 GLctx.texImage2D(target, level, internalFormat, width, height, border, format, type, pixelData);
}
function _glDisable(x0) {
 GLctx.disable(x0);
}
function _glBlendFuncSeparate(x0, x1, x2, x3) {
 GLctx.blendFuncSeparate(x0, x1, x2, x3);
}
function _glStencilMask(x0) {
 GLctx.stencilMask(x0);
}
Module["_memset"] = _memset;
Module["_bitshift64Lshr"] = _bitshift64Lshr;
function _glGetProgramiv(program, pname, p) {
 if (!p) {
  GL.recordError(1281);
  return;
 }
 if (pname == 35716) {
  var log = GLctx.getProgramInfoLog(GL.programs[program]);
  if (log === null) log = "(unknown error)";
  HEAP32[p >> 2] = log.length + 1;
 } else if (pname == 35719) {
  var ptable = GL.programInfos[program];
  if (ptable) {
   HEAP32[p >> 2] = ptable.maxUniformLength;
   return;
  } else if (program < GL.counter) {
   GL.recordError(1282);
  } else {
   GL.recordError(1281);
  }
 } else if (pname == 35722) {
  var ptable = GL.programInfos[program];
  if (ptable) {
   if (ptable.maxAttributeLength == -1) {
    var program = GL.programs[program];
    var numAttribs = GLctx.getProgramParameter(program, GLctx.ACTIVE_ATTRIBUTES);
    ptable.maxAttributeLength = 0;
    for (var i = 0; i < numAttribs; ++i) {
     var activeAttrib = GLctx.getActiveAttrib(program, i);
     ptable.maxAttributeLength = Math.max(ptable.maxAttributeLength, activeAttrib.name.length + 1);
    }
   }
   HEAP32[p >> 2] = ptable.maxAttributeLength;
   return;
  } else if (program < GL.counter) {
   GL.recordError(1282);
  } else {
   GL.recordError(1281);
  }
 } else {
  HEAP32[p >> 2] = GLctx.getProgramParameter(GL.programs[program], pname);
 }
}
function _glColorMask(x0, x1, x2, x3) {
 GLctx.colorMask(x0, x1, x2, x3);
}
function _emscripten_exit_pointerlock() {
 JSEvents.removeDeferredCalls(JSEvents.requestPointerLock);
 if (document.exitPointerLock) {
  document.exitPointerLock();
 } else if (document.msExitPointerLock) {
  document.msExitPointerLock();
 } else if (document.mozExitPointerLock) {
  document.mozExitPointerLock();
 } else if (document.webkitExitPointerLock) {
  document.webkitExitPointerLock();
 } else {
  return -1;
 }
 return 0;
}
function _abort() {
 Module["abort"]();
}
function _emscripten_set_wheel_callback(target, userData, useCapture, callbackfunc) {
 target = JSEvents.findEventTarget(target);
 if (typeof target.onwheel !== "undefined") {
  JSEvents.registerWheelEventCallback(target, userData, useCapture, callbackfunc, 9, "wheel");
  return 0;
 } else if (typeof target.onmousewheel !== "undefined") {
  JSEvents.registerWheelEventCallback(target, userData, useCapture, callbackfunc, 9, "mousewheel");
  return 0;
 } else {
  return -1;
 }
}
function _glGetUniformLocation(program, name) {
 name = Pointer_stringify(name);
 var arrayOffset = 0;
 if (name.indexOf("]", name.length - 1) !== -1) {
  var ls = name.lastIndexOf("[");
  var arrayIndex = name.slice(ls + 1, -1);
  if (arrayIndex.length > 0) {
   arrayOffset = parseInt(arrayIndex);
   if (arrayOffset < 0) {
    return -1;
   }
  }
  name = name.slice(0, ls);
 }
 var ptable = GL.programInfos[program];
 if (!ptable) {
  return -1;
 }
 var utable = ptable.uniforms;
 var uniformInfo = utable[name];
 if (uniformInfo && arrayOffset < uniformInfo[0]) {
  return uniformInfo[1] + arrayOffset;
 } else {
  return -1;
 }
}
function _emscripten_set_touchcancel_callback(target, userData, useCapture, callbackfunc) {
 JSEvents.registerTouchEventCallback(target, userData, useCapture, callbackfunc, 25, "touchcancel");
 return 0;
}
function _glBindFramebuffer(target, framebuffer) {
 GLctx.bindFramebuffer(target, framebuffer ? GL.framebuffers[framebuffer] : null);
}
function ___lock() {}
function ___unlock() {}
function _glCullFace(x0) {
 GLctx.cullFace(x0);
}
function _glScissor(x0, x1, x2, x3) {
 GLctx.scissor(x0, x1, x2, x3);
}
function _glEnable(x0) {
 GLctx.enable(x0);
}
function _glUniform4fv(location, count, value) {
 location = GL.uniforms[location];
 var view;
 if (4 * count <= GL.MINI_TEMP_BUFFER_SIZE) {
  view = GL.miniTempBufferViews[4 * count - 1];
  for (var i = 0; i < 4 * count; i += 4) {
   view[i] = HEAPF32[value + 4 * i >> 2];
   view[i + 1] = HEAPF32[value + (4 * i + 4) >> 2];
   view[i + 2] = HEAPF32[value + (4 * i + 8) >> 2];
   view[i + 3] = HEAPF32[value + (4 * i + 12) >> 2];
  }
 } else {
  view = HEAPF32.subarray(value >> 2, value + count * 16 >> 2);
 }
 GLctx.uniform4fv(location, view);
}
var _llvm_fabs_f32 = Math_abs;
function _emscripten_set_keyup_callback(target, userData, useCapture, callbackfunc) {
 JSEvents.registerKeyEventCallback(target, userData, useCapture, callbackfunc, 3, "keyup");
 return 0;
}
function _glBlendEquationSeparate(x0, x1) {
 GLctx.blendEquationSeparate(x0, x1);
}
function _glGenBuffers(n, buffers) {
 for (var i = 0; i < n; i++) {
  var buffer = GLctx.createBuffer();
  if (!buffer) {
   GL.recordError(1282);
   while (i < n) HEAP32[buffers + i++ * 4 >> 2] = 0;
   return;
  }
  var id = GL.getNewId(GL.buffers);
  buffer.name = id;
  GL.buffers[id] = buffer;
  HEAP32[buffers + i * 4 >> 2] = id;
 }
}
function _glDeleteProgram(id) {
 if (!id) return;
 var program = GL.programs[id];
 if (!program) {
  GL.recordError(1281);
  return;
 }
 GLctx.deleteProgram(program);
 program.name = 0;
 GL.programs[id] = null;
 GL.programInfos[id] = null;
}
function _glVertexAttribDivisor(index, divisor) {
 GLctx["vertexAttribDivisor"](index, divisor);
}
function _glAttachShader(program, shader) {
 GLctx.attachShader(GL.programs[program], GL.shaders[shader]);
}
function _malloc(bytes) {
 var ptr = Runtime.dynamicAlloc(bytes + 8);
 return ptr + 8 & 4294967288;
}
Module["_malloc"] = _malloc;
function ___cxa_allocate_exception(size) {
 return _malloc(size);
}
function _glDeleteShader(id) {
 if (!id) return;
 var shader = GL.shaders[id];
 if (!shader) {
  GL.recordError(1281);
  return;
 }
 GLctx.deleteShader(shader);
 GL.shaders[id] = null;
}
function _glCreateProgram() {
 var id = GL.getNewId(GL.programs);
 var program = GLctx.createProgram();
 program.name = id;
 GL.programs[id] = program;
 return id;
}
function _emscripten_set_canvas_size(width, height) {
 Browser.setCanvasSize(width, height);
}
function _emscripten_set_mousemove_callback(target, userData, useCapture, callbackfunc) {
 JSEvents.registerMouseEventCallback(target, userData, useCapture, callbackfunc, 8, "mousemove");
 return 0;
}
function _glViewport(x0, x1, x2, x3) {
 GLctx.viewport(x0, x1, x2, x3);
}
function _glUniform3f(location, v0, v1, v2) {
 location = GL.uniforms[location];
 GLctx.uniform3f(location, v0, v1, v2);
}
function _glBindAttribLocation(program, index, name) {
 name = Pointer_stringify(name);
 GLctx.bindAttribLocation(GL.programs[program], index, name);
}
function _glDrawElements(mode, count, type, indices) {
 GLctx.drawElements(mode, count, type, indices);
}
function _emscripten_webgl_init_context_attributes(attributes) {
 HEAP32[attributes >> 2] = 1;
 HEAP32[attributes + 4 >> 2] = 1;
 HEAP32[attributes + 8 >> 2] = 0;
 HEAP32[attributes + 12 >> 2] = 1;
 HEAP32[attributes + 16 >> 2] = 1;
 HEAP32[attributes + 20 >> 2] = 0;
 HEAP32[attributes + 24 >> 2] = 0;
 HEAP32[attributes + 28 >> 2] = 0;
 HEAP32[attributes + 32 >> 2] = 1;
 HEAP32[attributes + 36 >> 2] = 0;
 HEAP32[attributes + 40 >> 2] = 1;
}
Module["_bitshift64Shl"] = _bitshift64Shl;
var _llvm_fabs_f64 = Math_abs;
function ___syscall5(which, varargs) {
 SYSCALLS.varargs = varargs;
 try {
  var pathname = SYSCALLS.getStr(), flags = SYSCALLS.get(), mode = SYSCALLS.get();
  var stream = FS.open(pathname, flags, mode);
  return stream.fd;
 } catch (e) {
  if (typeof FS === "undefined" || !(e instanceof FS.ErrnoError)) abort(e);
  return -e.errno;
 }
}
function _glUniformMatrix4fv(location, count, transpose, value) {
 location = GL.uniforms[location];
 var view;
 if (16 * count <= GL.MINI_TEMP_BUFFER_SIZE) {
  view = GL.miniTempBufferViews[16 * count - 1];
  for (var i = 0; i < 16 * count; i += 16) {
   view[i] = HEAPF32[value + 4 * i >> 2];
   view[i + 1] = HEAPF32[value + (4 * i + 4) >> 2];
   view[i + 2] = HEAPF32[value + (4 * i + 8) >> 2];
   view[i + 3] = HEAPF32[value + (4 * i + 12) >> 2];
   view[i + 4] = HEAPF32[value + (4 * i + 16) >> 2];
   view[i + 5] = HEAPF32[value + (4 * i + 20) >> 2];
   view[i + 6] = HEAPF32[value + (4 * i + 24) >> 2];
   view[i + 7] = HEAPF32[value + (4 * i + 28) >> 2];
   view[i + 8] = HEAPF32[value + (4 * i + 32) >> 2];
   view[i + 9] = HEAPF32[value + (4 * i + 36) >> 2];
   view[i + 10] = HEAPF32[value + (4 * i + 40) >> 2];
   view[i + 11] = HEAPF32[value + (4 * i + 44) >> 2];
   view[i + 12] = HEAPF32[value + (4 * i + 48) >> 2];
   view[i + 13] = HEAPF32[value + (4 * i + 52) >> 2];
   view[i + 14] = HEAPF32[value + (4 * i + 56) >> 2];
   view[i + 15] = HEAPF32[value + (4 * i + 60) >> 2];
  }
 } else {
  view = HEAPF32.subarray(value >> 2, value + count * 64 >> 2);
 }
 GLctx.uniformMatrix4fv(location, transpose, view);
}
function ___syscall6(which, varargs) {
 SYSCALLS.varargs = varargs;
 try {
  var stream = SYSCALLS.getStreamFromFD();
  FS.close(stream);
  return 0;
 } catch (e) {
  if (typeof FS === "undefined" || !(e instanceof FS.ErrnoError)) abort(e);
  return -e.errno;
 }
}
function _glBufferSubData(target, offset, size, data) {
 GLctx.bufferSubData(target, offset, HEAPU8.subarray(data, data + size));
}
function _emscripten_set_fullscreenchange_callback(target, userData, useCapture, callbackfunc) {
 if (typeof JSEvents.fullscreenEnabled() === "undefined") return -1;
 if (!target) target = document; else {
  target = JSEvents.findEventTarget(target);
  if (!target) return -4;
 }
 JSEvents.registerFullscreenChangeEventCallback(target, userData, useCapture, callbackfunc, 19, "fullscreenchange");
 JSEvents.registerFullscreenChangeEventCallback(target, userData, useCapture, callbackfunc, 19, "mozfullscreenchange");
 JSEvents.registerFullscreenChangeEventCallback(target, userData, useCapture, callbackfunc, 19, "webkitfullscreenchange");
 JSEvents.registerFullscreenChangeEventCallback(target, userData, useCapture, callbackfunc, 19, "msfullscreenchange");
 return 0;
}
function _glTexParameteri(x0, x1, x2) {
 GLctx.texParameteri(x0, x1, x2);
}
function _emscripten_webgl_make_context_current(contextHandle) {
 var success = GL.makeContextCurrent(contextHandle);
 return success ? 0 : -5;
}
function _pthread_cleanup_pop() {
 assert(_pthread_cleanup_push.level == __ATEXIT__.length, "cannot pop if something else added meanwhile!");
 __ATEXIT__.pop();
 _pthread_cleanup_push.level = __ATEXIT__.length;
}
function _glBufferData(target, size, data, usage) {
 switch (usage) {
 case 35041:
 case 35042:
  usage = 35040;
  break;
 case 35045:
 case 35046:
  usage = 35044;
  break;
 case 35049:
 case 35050:
  usage = 35048;
  break;
 }
 if (!data) {
  GLctx.bufferData(target, size, usage);
 } else {
  GLctx.bufferData(target, HEAPU8.subarray(data, data + size), usage);
 }
}
function _llvm_trap() {
 abort("trap!");
}
function _glGetShaderiv(shader, pname, p) {
 if (!p) {
  GL.recordError(1281);
  return;
 }
 if (pname == 35716) {
  var log = GLctx.getShaderInfoLog(GL.shaders[shader]);
  if (log === null) log = "(unknown error)";
  HEAP32[p >> 2] = log.length + 1;
 } else {
  HEAP32[p >> 2] = GLctx.getShaderParameter(GL.shaders[shader], pname);
 }
}
Module["_pthread_self"] = _pthread_self;
function ___syscall140(which, varargs) {
 SYSCALLS.varargs = varargs;
 try {
  var stream = SYSCALLS.getStreamFromFD(), offset_high = SYSCALLS.get(), offset_low = SYSCALLS.get(), result = SYSCALLS.get(), whence = SYSCALLS.get();
  var offset = offset_low;
  assert(offset_high === 0);
  FS.llseek(stream, offset, whence);
  HEAP32[result >> 2] = stream.position;
  if (stream.getdents && offset === 0 && whence === 0) stream.getdents = null;
  return 0;
 } catch (e) {
  if (typeof FS === "undefined" || !(e instanceof FS.ErrnoError)) abort(e);
  return -e.errno;
 }
}
function ___syscall146(which, varargs) {
 SYSCALLS.varargs = varargs;
 try {
  var stream = SYSCALLS.get(), iov = SYSCALLS.get(), iovcnt = SYSCALLS.get();
  var ret = 0;
  if (!___syscall146.buffer) {
   ___syscall146.buffers = [ null, [], [] ];
   ___syscall146.printChar = (function(stream, curr) {
    var buffer = ___syscall146.buffers[stream];
    assert(buffer);
    if (curr === 0 || curr === 10) {
     (stream === 1 ? Module["print"] : Module["printErr"])(UTF8ArrayToString(buffer, 0));
     buffer.length = 0;
    } else {
     buffer.push(curr);
    }
   });
  }
  for (var i = 0; i < iovcnt; i++) {
   var ptr = HEAP32[iov + i * 8 >> 2];
   var len = HEAP32[iov + (i * 8 + 4) >> 2];
   for (var j = 0; j < len; j++) {
    ___syscall146.printChar(stream, HEAPU8[ptr + j]);
   }
   ret += len;
  }
  return ret;
 } catch (e) {
  if (typeof FS === "undefined" || !(e instanceof FS.ErrnoError)) abort(e);
  return -e.errno;
 }
}
function ___setErrNo(value) {
 if (Module["___errno_location"]) HEAP32[Module["___errno_location"]() >> 2] = value;
 return value;
}
function ___syscall221(which, varargs) {
 SYSCALLS.varargs = varargs;
 try {
  var stream = SYSCALLS.getStreamFromFD(), cmd = SYSCALLS.get();
  switch (cmd) {
  case 0:
   {
    var arg = SYSCALLS.get();
    if (arg < 0) {
     return -ERRNO_CODES.EINVAL;
    }
    var newStream;
    newStream = FS.open(stream.path, stream.flags, 0, arg);
    return newStream.fd;
   }
  case 1:
  case 2:
   return 0;
  case 3:
   return stream.flags;
  case 4:
   {
    var arg = SYSCALLS.get();
    stream.flags |= arg;
    return 0;
   }
  case 12:
  case 12:
   {
    var arg = SYSCALLS.get();
    var offset = 0;
    HEAP16[arg + offset >> 1] = 2;
    return 0;
   }
  case 13:
  case 14:
  case 13:
  case 14:
   return 0;
  case 16:
  case 8:
   return -ERRNO_CODES.EINVAL;
  case 9:
   ___setErrNo(ERRNO_CODES.EINVAL);
   return -1;
  default:
   {
    return -ERRNO_CODES.EINVAL;
   }
  }
 } catch (e) {
  if (typeof FS === "undefined" || !(e instanceof FS.ErrnoError)) abort(e);
  return -e.errno;
 }
}
function ___syscall145(which, varargs) {
 SYSCALLS.varargs = varargs;
 try {
  var stream = SYSCALLS.getStreamFromFD(), iov = SYSCALLS.get(), iovcnt = SYSCALLS.get();
  return SYSCALLS.doReadv(stream, iov, iovcnt);
 } catch (e) {
  if (typeof FS === "undefined" || !(e instanceof FS.ErrnoError)) abort(e);
  return -e.errno;
 }
}
var ___dso_handle = STATICTOP;
STATICTOP += 16;
var GLctx;
GL.init();
Module["requestFullScreen"] = function Module_requestFullScreen(lockPointer, resizeCanvas, vrDevice) {
 Browser.requestFullScreen(lockPointer, resizeCanvas, vrDevice);
};
Module["requestAnimationFrame"] = function Module_requestAnimationFrame(func) {
 Browser.requestAnimationFrame(func);
};
Module["setCanvasSize"] = function Module_setCanvasSize(width, height, noUpdates) {
 Browser.setCanvasSize(width, height, noUpdates);
};
Module["pauseMainLoop"] = function Module_pauseMainLoop() {
 Browser.mainLoop.pause();
};
Module["resumeMainLoop"] = function Module_resumeMainLoop() {
 Browser.mainLoop.resume();
};
Module["getUserMedia"] = function Module_getUserMedia() {
 Browser.getUserMedia();
};
Module["createContext"] = function Module_createContext(canvas, useWebGL, setInModule, webGLContextAttributes) {
 return Browser.createContext(canvas, useWebGL, setInModule, webGLContextAttributes);
};
__ATEXIT__.push((function() {
 var fflush = Module["_fflush"];
 if (fflush) fflush(0);
 var printChar = ___syscall146.printChar;
 if (!printChar) return;
 var buffers = ___syscall146.buffers;
 if (buffers[1].length) printChar(1, 10);
 if (buffers[2].length) printChar(2, 10);
}));
STACK_BASE = STACKTOP = Runtime.alignMemory(STATICTOP);
staticSealed = true;
STACK_MAX = STACK_BASE + TOTAL_STACK;
DYNAMIC_BASE = DYNAMICTOP = Runtime.alignMemory(STACK_MAX);
var cttz_i8 = allocate([ 8, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0 ], "i8", ALLOC_DYNAMIC);
function invoke_iiii(index, a1, a2, a3) {
 try {
  return Module["dynCall_iiii"](index, a1, a2, a3);
 } catch (e) {
  if (typeof e !== "number" && e !== "longjmp") throw e;
  asm["setThrew"](1, 0);
 }
}
function invoke_viiiii(index, a1, a2, a3, a4, a5) {
 try {
  Module["dynCall_viiiii"](index, a1, a2, a3, a4, a5);
 } catch (e) {
  if (typeof e !== "number" && e !== "longjmp") throw e;
  asm["setThrew"](1, 0);
 }
}
function invoke_i(index) {
 try {
  return Module["dynCall_i"](index);
 } catch (e) {
  if (typeof e !== "number" && e !== "longjmp") throw e;
  asm["setThrew"](1, 0);
 }
}
function invoke_vi(index, a1) {
 try {
  Module["dynCall_vi"](index, a1);
 } catch (e) {
  if (typeof e !== "number" && e !== "longjmp") throw e;
  asm["setThrew"](1, 0);
 }
}
function invoke_vii(index, a1, a2) {
 try {
  Module["dynCall_vii"](index, a1, a2);
 } catch (e) {
  if (typeof e !== "number" && e !== "longjmp") throw e;
  asm["setThrew"](1, 0);
 }
}
function invoke_ii(index, a1) {
 try {
  return Module["dynCall_ii"](index, a1);
 } catch (e) {
  if (typeof e !== "number" && e !== "longjmp") throw e;
  asm["setThrew"](1, 0);
 }
}
function invoke_v(index) {
 try {
  Module["dynCall_v"](index);
 } catch (e) {
  if (typeof e !== "number" && e !== "longjmp") throw e;
  asm["setThrew"](1, 0);
 }
}
function invoke_viiiiii(index, a1, a2, a3, a4, a5, a6) {
 try {
  Module["dynCall_viiiiii"](index, a1, a2, a3, a4, a5, a6);
 } catch (e) {
  if (typeof e !== "number" && e !== "longjmp") throw e;
  asm["setThrew"](1, 0);
 }
}
function invoke_iii(index, a1, a2) {
 try {
  return Module["dynCall_iii"](index, a1, a2);
 } catch (e) {
  if (typeof e !== "number" && e !== "longjmp") throw e;
  asm["setThrew"](1, 0);
 }
}
function invoke_viiii(index, a1, a2, a3, a4) {
 try {
  Module["dynCall_viiii"](index, a1, a2, a3, a4);
 } catch (e) {
  if (typeof e !== "number" && e !== "longjmp") throw e;
  asm["setThrew"](1, 0);
 }
}
Module.asmGlobalArg = {
 "Math": Math,
 "Int8Array": Int8Array,
 "Int16Array": Int16Array,
 "Int32Array": Int32Array,
 "Uint8Array": Uint8Array,
 "Uint16Array": Uint16Array,
 "Uint32Array": Uint32Array,
 "Float32Array": Float32Array,
 "Float64Array": Float64Array,
 "NaN": NaN,
 "Infinity": Infinity
};
Module.asmLibraryArg = {
 "abort": abort,
 "assert": assert,
 "invoke_iiii": invoke_iiii,
 "invoke_viiiii": invoke_viiiii,
 "invoke_i": invoke_i,
 "invoke_vi": invoke_vi,
 "invoke_vii": invoke_vii,
 "invoke_ii": invoke_ii,
 "invoke_v": invoke_v,
 "invoke_viiiiii": invoke_viiiiii,
 "invoke_iii": invoke_iii,
 "invoke_viiii": invoke_viiii,
 "_glUseProgram": _glUseProgram,
 "___syscall221": ___syscall221,
 "_glUniformMatrix3fv": _glUniformMatrix3fv,
 "_glUniformMatrix2fv": _glUniformMatrix2fv,
 "_emscripten_set_fullscreenchange_callback": _emscripten_set_fullscreenchange_callback,
 "_glStencilFunc": _glStencilFunc,
 "_glUniformMatrix4fv": _glUniformMatrix4fv,
 "_llvm_fabs_f64": _llvm_fabs_f64,
 "_glDeleteProgram": _glDeleteProgram,
 "__ZSt18uncaught_exceptionv": __ZSt18uncaught_exceptionv,
 "_glBindBuffer": _glBindBuffer,
 "_glCreateProgram": _glCreateProgram,
 "_emscripten_set_touchmove_callback": _emscripten_set_touchmove_callback,
 "_emscripten_set_main_loop_timing": _emscripten_set_main_loop_timing,
 "_sbrk": _sbrk,
 "_glDisableVertexAttribArray": _glDisableVertexAttribArray,
 "_emscripten_memcpy_big": _emscripten_memcpy_big,
 "_emscripten_set_mousedown_callback": _emscripten_set_mousedown_callback,
 "_emscripten_set_touchstart_callback": _emscripten_set_touchstart_callback,
 "_glStencilOp": _glStencilOp,
 "_glUniform4f": _glUniform4f,
 "___resumeException": ___resumeException,
 "_emscripten_webgl_make_context_current": _emscripten_webgl_make_context_current,
 "_glGenBuffers": _glGenBuffers,
 "_glShaderSource": _glShaderSource,
 "___cxa_atexit": ___cxa_atexit,
 "_pthread_cleanup_push": _pthread_cleanup_push,
 "___syscall140": ___syscall140,
 "___syscall145": ___syscall145,
 "___syscall146": ___syscall146,
 "_pthread_cleanup_pop": _pthread_cleanup_pop,
 "_glVertexAttribPointer": _glVertexAttribPointer,
 "___cxa_find_matching_catch": ___cxa_find_matching_catch,
 "_glDrawElements": _glDrawElements,
 "_glDepthMask": _glDepthMask,
 "_glBufferSubData": _glBufferSubData,
 "_glViewport": _glViewport,
 "___setErrNo": ___setErrNo,
 "_glDeleteTextures": _glDeleteTextures,
 "_glDepthFunc": _glDepthFunc,
 "_glStencilOpSeparate": _glStencilOpSeparate,
 "_emscripten_set_canvas_size": _emscripten_set_canvas_size,
 "_glUniform3f": _glUniform3f,
 "_nanosleep": _nanosleep,
 "_glCompressedTexImage2D": _glCompressedTexImage2D,
 "_glEnable": _glEnable,
 "_llvm_trap": _llvm_trap,
 "_glGenTextures": _glGenTextures,
 "_glGetIntegerv": _glGetIntegerv,
 "_glGetString": _glGetString,
 "emscriptenWebGLGet": emscriptenWebGLGet,
 "_glStencilMaskSeparate": _glStencilMaskSeparate,
 "_emscripten_get_now": _emscripten_get_now,
 "_glAttachShader": _glAttachShader,
 "___lock": ___lock,
 "emscriptenWebGLGetTexPixelData": emscriptenWebGLGetTexPixelData,
 "___syscall6": ___syscall6,
 "___syscall5": ___syscall5,
 "_glBindFramebuffer": _glBindFramebuffer,
 "_emscripten_set_devicemotion_callback": _emscripten_set_devicemotion_callback,
 "_glUniform2f": _glUniform2f,
 "_glCullFace": _glCullFace,
 "_emscripten_set_keypress_callback": _emscripten_set_keypress_callback,
 "_glDeleteFramebuffers": _glDeleteFramebuffers,
 "_emscripten_webgl_create_context": _emscripten_webgl_create_context,
 "_emscripten_set_deviceorientation_callback": _emscripten_set_deviceorientation_callback,
 "___cxa_allocate_exception": ___cxa_allocate_exception,
 "_emscripten_set_keyup_callback": _emscripten_set_keyup_callback,
 "_glBlendFuncSeparate": _glBlendFuncSeparate,
 "_emscripten_set_mouseup_callback": _emscripten_set_mouseup_callback,
 "_glClearDepthf": _glClearDepthf,
 "_glClearColor": _glClearColor,
 "_glBindTexture": _glBindTexture,
 "_glUniform1f": _glUniform1f,
 "_glUniform1i": _glUniform1i,
 "_glDrawArrays": _glDrawArrays,
 "_glCreateShader": _glCreateShader,
 "_llvm_fabs_f32": _llvm_fabs_f32,
 "_emscripten_webgl_init_context_attributes": _emscripten_webgl_init_context_attributes,
 "_glActiveTexture": _glActiveTexture,
 "_glFrontFace": _glFrontFace,
 "_glCompileShader": _glCompileShader,
 "_emscripten_exit_pointerlock": _emscripten_exit_pointerlock,
 "_glEnableVertexAttribArray": _glEnableVertexAttribArray,
 "_abort": _abort,
 "_glDeleteBuffers": _glDeleteBuffers,
 "_glBufferData": _glBufferData,
 "_glTexImage2D": _glTexImage2D,
 "_glDeleteShader": _glDeleteShader,
 "_glGetProgramiv": _glGetProgramiv,
 "_glScissor": _glScissor,
 "_emscripten_request_pointerlock": _emscripten_request_pointerlock,
 "emscriptenWebGLComputeImageSize": emscriptenWebGLComputeImageSize,
 "_emscripten_set_keydown_callback": _emscripten_set_keydown_callback,
 "___gxx_personality_v0": ___gxx_personality_v0,
 "_emscripten_set_touchcancel_callback": _emscripten_set_touchcancel_callback,
 "_emscripten_set_mousemove_callback": _emscripten_set_mousemove_callback,
 "_glDeleteRenderbuffers": _glDeleteRenderbuffers,
 "_usleep": _usleep,
 "_glLinkProgram": _glLinkProgram,
 "_emscripten_set_touchend_callback": _emscripten_set_touchend_callback,
 "_glGetShaderiv": _glGetShaderiv,
 "_glGetUniformLocation": _glGetUniformLocation,
 "_emscripten_cancel_main_loop": _emscripten_cancel_main_loop,
 "_glClear": _glClear,
 "_glUniform4fv": _glUniform4fv,
 "_glBindAttribLocation": _glBindAttribLocation,
 "_emscripten_webgl_destroy_context": _emscripten_webgl_destroy_context,
 "_glVertexAttribDivisor": _glVertexAttribDivisor,
 "_emscripten_set_wheel_callback": _emscripten_set_wheel_callback,
 "___syscall54": ___syscall54,
 "___unlock": ___unlock,
 "_emscripten_set_main_loop": _emscripten_set_main_loop,
 "___cxa_throw": ___cxa_throw,
 "_glColorMask": _glColorMask,
 "_glDisable": _glDisable,
 "_glTexParameteri": _glTexParameteri,
 "_glBlendColor": _glBlendColor,
 "_atexit": _atexit,
 "_glStencilMask": _glStencilMask,
 "_glBlendEquationSeparate": _glBlendEquationSeparate,
 "_glStencilFuncSeparate": _glStencilFuncSeparate,
 "STACKTOP": STACKTOP,
 "STACK_MAX": STACK_MAX,
 "tempDoublePtr": tempDoublePtr,
 "ABORT": ABORT,
 "cttz_i8": cttz_i8,
 "___dso_handle": ___dso_handle
};
// EMSCRIPTEN_START_ASM

var asm =Module["asm"]// EMSCRIPTEN_END_ASM
(Module.asmGlobalArg, Module.asmLibraryArg, buffer);
var runPostSets = Module["runPostSets"] = asm["runPostSets"];
var _i64Subtract = Module["_i64Subtract"] = asm["_i64Subtract"];
var __GLOBAL__sub_I_DrawCallExplorer_cc = Module["__GLOBAL__sub_I_DrawCallExplorer_cc"] = asm["__GLOBAL__sub_I_DrawCallExplorer_cc"];
var _free = Module["_free"] = asm["_free"];
var _main = Module["_main"] = asm["_main"];
var _i64Add = Module["_i64Add"] = asm["_i64Add"];
var _memmove = Module["_memmove"] = asm["_memmove"];
var _pthread_self = Module["_pthread_self"] = asm["_pthread_self"];
var _roundf = Module["_roundf"] = asm["_roundf"];
var _memset = Module["_memset"] = asm["_memset"];
var _malloc = Module["_malloc"] = asm["_malloc"];
var __GLOBAL__sub_I_Log_cc = Module["__GLOBAL__sub_I_Log_cc"] = asm["__GLOBAL__sub_I_Log_cc"];
var _memcpy = Module["_memcpy"] = asm["_memcpy"];
var _bitshift64Lshr = Module["_bitshift64Lshr"] = asm["_bitshift64Lshr"];
var __GLOBAL__sub_I_imgui_cpp = Module["__GLOBAL__sub_I_imgui_cpp"] = asm["__GLOBAL__sub_I_imgui_cpp"];
var _bitshift64Shl = Module["_bitshift64Shl"] = asm["_bitshift64Shl"];
var dynCall_iiii = Module["dynCall_iiii"] = asm["dynCall_iiii"];
var dynCall_viiiii = Module["dynCall_viiiii"] = asm["dynCall_viiiii"];
var dynCall_i = Module["dynCall_i"] = asm["dynCall_i"];
var dynCall_vi = Module["dynCall_vi"] = asm["dynCall_vi"];
var dynCall_vii = Module["dynCall_vii"] = asm["dynCall_vii"];
var dynCall_ii = Module["dynCall_ii"] = asm["dynCall_ii"];
var dynCall_v = Module["dynCall_v"] = asm["dynCall_v"];
var dynCall_viiiiii = Module["dynCall_viiiiii"] = asm["dynCall_viiiiii"];
var dynCall_iii = Module["dynCall_iii"] = asm["dynCall_iii"];
var dynCall_viiii = Module["dynCall_viiii"] = asm["dynCall_viiii"];
Runtime.stackAlloc = asm["stackAlloc"];
Runtime.stackSave = asm["stackSave"];
Runtime.stackRestore = asm["stackRestore"];
Runtime.establishStackSpace = asm["establishStackSpace"];
Runtime.setTempRet0 = asm["setTempRet0"];
Runtime.getTempRet0 = asm["getTempRet0"];
if (memoryInitializer) {
 if (typeof Module["locateFile"] === "function") {
  memoryInitializer = Module["locateFile"](memoryInitializer);
 } else if (Module["memoryInitializerPrefixURL"]) {
  memoryInitializer = Module["memoryInitializerPrefixURL"] + memoryInitializer;
 }
 if (ENVIRONMENT_IS_NODE || ENVIRONMENT_IS_SHELL) {
  var data = Module["readBinary"](memoryInitializer);
  HEAPU8.set(data, Runtime.GLOBAL_BASE);
 } else {
  addRunDependency("memory initializer");
  var applyMemoryInitializer = (function(data) {
   if (data.byteLength) data = new Uint8Array(data);
   HEAPU8.set(data, Runtime.GLOBAL_BASE);
   if (Module["memoryInitializerRequest"]) delete Module["memoryInitializerRequest"].response;
   removeRunDependency("memory initializer");
  });
  function doBrowserLoad() {
   Module["readAsync"](memoryInitializer, applyMemoryInitializer, (function() {
    throw "could not load memory initializer " + memoryInitializer;
   }));
  }
  if (Module["memoryInitializerRequest"]) {
   function useRequest() {
    var request = Module["memoryInitializerRequest"];
    if (request.status !== 200 && request.status !== 0) {
     console.warn("a problem seems to have happened with Module.memoryInitializerRequest, status: " + request.status + ", retrying " + memoryInitializer);
     doBrowserLoad();
     return;
    }
    applyMemoryInitializer(request.response);
   }
   if (Module["memoryInitializerRequest"].response) {
    setTimeout(useRequest, 0);
   } else {
    Module["memoryInitializerRequest"].addEventListener("load", useRequest);
   }
  } else {
   doBrowserLoad();
  }
 }
}
function ExitStatus(status) {
 this.name = "ExitStatus";
 this.message = "Program terminated with exit(" + status + ")";
 this.status = status;
}
ExitStatus.prototype = new Error;
ExitStatus.prototype.constructor = ExitStatus;
var initialStackTop;
var preloadStartTime = null;
var calledMain = false;
dependenciesFulfilled = function runCaller() {
 if (!Module["calledRun"]) run();
 if (!Module["calledRun"]) dependenciesFulfilled = runCaller;
};
Module["callMain"] = Module.callMain = function callMain(args) {
 args = args || [];
 ensureInitRuntime();
 var argc = args.length + 1;
 function pad() {
  for (var i = 0; i < 4 - 1; i++) {
   argv.push(0);
  }
 }
 var argv = [ allocate(intArrayFromString(Module["thisProgram"]), "i8", ALLOC_NORMAL) ];
 pad();
 for (var i = 0; i < argc - 1; i = i + 1) {
  argv.push(allocate(intArrayFromString(args[i]), "i8", ALLOC_NORMAL));
  pad();
 }
 argv.push(0);
 argv = allocate(argv, "i32", ALLOC_NORMAL);
 try {
  var ret = Module["_main"](argc, argv, 0);
  exit(ret, true);
 } catch (e) {
  if (e instanceof ExitStatus) {
   return;
  } else if (e == "SimulateInfiniteLoop") {
   Module["noExitRuntime"] = true;
   return;
  } else {
   if (e && typeof e === "object" && e.stack) Module.printErr("exception thrown: " + [ e, e.stack ]);
   throw e;
  }
 } finally {
  calledMain = true;
 }
};
function run(args) {
 args = args || Module["arguments"];
 if (preloadStartTime === null) preloadStartTime = Date.now();
 if (runDependencies > 0) {
  return;
 }
 preRun();
 if (runDependencies > 0) return;
 if (Module["calledRun"]) return;
 function doRun() {
  if (Module["calledRun"]) return;
  Module["calledRun"] = true;
  if (ABORT) return;
  ensureInitRuntime();
  preMain();
  if (Module["onRuntimeInitialized"]) Module["onRuntimeInitialized"]();
  if (Module["_main"] && shouldRunNow) Module["callMain"](args);
  postRun();
 }
 if (Module["setStatus"]) {
  Module["setStatus"]("Running...");
  setTimeout((function() {
   setTimeout((function() {
    Module["setStatus"]("");
   }), 1);
   doRun();
  }), 1);
 } else {
  doRun();
 }
}
Module["run"] = Module.run = run;
function exit(status, implicit) {
 if (implicit && Module["noExitRuntime"]) {
  return;
 }
 if (Module["noExitRuntime"]) {} else {
  ABORT = true;
  EXITSTATUS = status;
  STACKTOP = initialStackTop;
  exitRuntime();
  if (Module["onExit"]) Module["onExit"](status);
 }
 if (ENVIRONMENT_IS_NODE) {
  process["exit"](status);
 } else if (ENVIRONMENT_IS_SHELL && typeof quit === "function") {
  quit(status);
 }
 throw new ExitStatus(status);
}
Module["exit"] = Module.exit = exit;
var abortDecorators = [];
function abort(what) {
 if (what !== undefined) {
  Module.print(what);
  Module.printErr(what);
  what = JSON.stringify(what);
 } else {
  what = "";
 }
 ABORT = true;
 EXITSTATUS = 1;
 var extra = "\nIf this abort() is unexpected, build with -s ASSERTIONS=1 which can give more information.";
 var output = "abort(" + what + ") at " + stackTrace() + extra;
 if (abortDecorators) {
  abortDecorators.forEach((function(decorator) {
   output = decorator(output, what);
  }));
 }
 throw output;
}
Module["abort"] = Module.abort = abort;
if (Module["preInit"]) {
 if (typeof Module["preInit"] == "function") Module["preInit"] = [ Module["preInit"] ];
 while (Module["preInit"].length > 0) {
  Module["preInit"].pop()();
 }
}
var shouldRunNow = true;
if (Module["noInitialRun"]) {
 shouldRunNow = false;
}
run();





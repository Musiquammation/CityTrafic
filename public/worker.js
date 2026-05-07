// client/src/worker/Chunk.ts
var Chunk = class _Chunk {
  static SIZE = 32;
  cells;
  constructor() {
    this.cells = new Uint16Array(_Chunk.SIZE * _Chunk.SIZE);
  }
  get(x, y) {
    return this.cells[y * _Chunk.SIZE + x];
  }
  set(x, y, v) {
    this.cells[y * _Chunk.SIZE + x] = v;
  }
};

// client/src/worker/MapHandler.ts
var MapHandler = class {
  chunks = /* @__PURE__ */ new Map();
  chunkKey(cx, cy) {
    return cx << 16 ^ cy & 65535;
  }
  getChunk(cx, cy) {
    const key = this.chunkKey(cx, cy);
    let chunk = this.chunks.get(key);
    if (!chunk) {
      chunk = new Chunk();
      this.chunks.set(key, chunk);
    }
    return chunk;
  }
  getChunkAt(wx, wy) {
    const { cx, cy, lx, ly } = this.worldToChunk(wx, wy);
    const chunk = this.getChunk(cx, cy);
    return { chunk, lx, ly };
  }
  worldToChunk(x, y) {
    return {
      cx: Math.floor(x / Chunk.SIZE),
      cy: Math.floor(y / Chunk.SIZE),
      lx: (x % Chunk.SIZE + Chunk.SIZE) % Chunk.SIZE,
      ly: (y % Chunk.SIZE + Chunk.SIZE) % Chunk.SIZE
    };
  }
  getCell(x, y) {
    const { chunk, lx, ly } = this.getChunkAt(x, y);
    return chunk.get(lx, ly);
  }
  setCell(x, y, data) {
    const { chunk, lx, ly } = this.getChunkAt(x, y);
    chunk.set(lx, ly, data);
  }
  *getChunks(viewX, viewY, rangeW, rangeH) {
    const halfW = Math.floor(rangeW / 2);
    const halfH = Math.floor(rangeH / 2);
    const minX = viewX - halfW;
    const minY = viewY - halfH;
    const maxX = viewX + halfW;
    const maxY = viewY + halfH;
    const minCX = Math.floor(minX / Chunk.SIZE) - 1;
    const minCY = Math.floor(minY / Chunk.SIZE) - 1;
    const maxCX = Math.floor(maxX / Chunk.SIZE) + 1;
    const maxCY = Math.floor(maxY / Chunk.SIZE) + 1;
    for (let cy = minCY; cy <= maxCY; cy++) {
      for (let cx = minCX; cx <= maxCX; cx++) {
        yield {
          x: cx * Chunk.SIZE,
          y: cy * Chunk.SIZE,
          cells: this.getChunk(cx, cy).cells
        };
      }
    }
  }
  *getLineBuffer(x, y, w) {
    if (w <= 0) return;
    let remaining = w;
    let wx = x;
    while (remaining > 0) {
      const { cx, cy, lx, ly } = this.worldToChunk(wx, y);
      const chunk = this.getChunk(cx, cy);
      const available = Chunk.SIZE - lx;
      const take = Math.min(remaining, available);
      const start = ly * Chunk.SIZE + lx;
      const end = start + take;
      yield chunk.cells.subarray(start, end);
      wx += take;
      remaining -= take;
    }
  }
  clear() {
    this.chunks.clear();
  }
};

// client/src/worker/ClientApi.ts
function getChunkBounds(x, y, w, h) {
  const bx = Math.floor(x / Chunk.SIZE) * Chunk.SIZE;
  const by = Math.floor(y / Chunk.SIZE) * Chunk.SIZE;
  const lx = Math.floor((x + w + (Chunk.SIZE - 1)) / Chunk.SIZE);
  const ly = Math.floor((y + h + (Chunk.SIZE - 1)) / Chunk.SIZE);
  return {
    x: bx,
    y: by,
    w: lx * Chunk.SIZE - bx,
    h: ly * Chunk.SIZE - by
  };
}
var ClientApi = class {
  module = null;
  apiPtr = 0;
  map = new MapHandler();
  cameraTimeout = -1;
  setModule(module) {
    if (this.module) {
      throw new Error("Module was already defined");
    }
    this.module = module;
    this.apiPtr = module._Api_createApi(0, 0);
    this.run(7 /* PUSH_LAYER */);
    this.createCellGrid();
  }
  isModuleMissing() {
    return this.module === null;
  }
  cleanup() {
    if (!this.module || !this.apiPtr) return;
    this.module._Api_deleteSession(this.apiPtr, 0);
    this.module._Api_deleteApi(this.apiPtr);
    this.apiPtr = 0;
    this.module = null;
    if (this.cameraTimeout >= 0)
      clearTimeout(this.cameraTimeout);
  }
  freeBuffer() {
    this.run(-1, 0 /* FREE_BUFFER */);
  }
  createSession() {
    this.module._Api_createSession(this.apiPtr);
  }
  deleteSession() {
    this.module._Api_deleteSession(this.apiPtr, 0);
  }
  run(code, args = 0) {
    return this.module._Api_take(this.apiPtr, 0, code, args);
  }
  takeCoords() {
    const ptr = this.run(3 /* COPY_COORDS */) >> 2;
    const x = this.module.HEAP32[ptr + 0];
    const y = this.module.HEAP32[ptr + 1];
    const w = this.module.HEAP32[ptr + 2];
    const h = this.module.HEAP32[ptr + 3];
    return { x, y, w, h };
  }
  createCellGrid() {
    this.map.clear();
    const rect = this.takeCoords();
    const arg = this.module._malloc(4 * 4);
    const view = this.module.HEAPU32.subarray(arg >> 2);
    view[0] = rect.x;
    view[1] = rect.y;
    view[2] = rect.w;
    view[3] = rect.h;
    const ptr = this.run(1 /* MAKE_MAP */, arg) >> 1;
    this.module._free(arg);
    const viewCells = this.module.HEAPU16.subarray(
      ptr,
      ptr + rect.w * rect.h
    );
    for (let y = 0; y < rect.h; y++) {
      for (let x = 0; x < rect.w; x++) {
        const wx = rect.x + x;
        const wy = rect.y + y;
        const { chunk, lx, ly } = this.map.getChunkAt(wx, wy);
        chunk.set(lx, ly, viewCells[y * rect.w + x]);
      }
    }
    this.module._free(ptr);
  }
  updateCells(viewX, viewY, viewW, viewH) {
    const bounds = getChunkBounds(
      viewX,
      viewY,
      viewW,
      viewH
    );
    const argPtr = this.module._malloc(4 * 5);
    const arg = this.module.HEAPU32.subarray(argPtr >> 2);
    arg[0] = bounds.x;
    arg[1] = bounds.y;
    arg[2] = bounds.w;
    arg[3] = bounds.h;
    arg[4] = 0;
    const ptr = this.run(4 /* MAKE_MAP_EDITS */, argPtr) >> 2;
    this.module._free(argPtr);
    let cursor = ptr + 1;
    for (let rangeCount = this.module.HEAPU32[cursor++]; rangeCount; rangeCount--) {
      const x0 = this.module.HEAP32[cursor++];
      const y0 = this.module.HEAP32[cursor++];
      for (let count = this.module.HEAPU32[cursor++]; count; count--) {
        const packed = this.module.HEAPU32[cursor++];
        const dx = packed >> 24 & 255;
        const dy = packed >> 16 & 255;
        const data = packed & 65535;
        const wx = x0 + dx;
        const wy = y0 + dy;
        const { chunk, lx, ly } = this.map.getChunkAt(wx, wy);
        chunk.set(lx, ly, data);
      }
    }
  }
  setArea(x0, y0, w, h, buffer) {
    const reader = new Uint16Array(buffer);
    let offset = 0;
    let buffers = new Array(h);
    for (let y = y0, yf = y0 + h; y < yf; y++) {
      const buffer2 = new Uint16Array(w);
      buffers[y - y0] = buffer2;
      let i = 0;
      for (const line of this.map.getLineBuffer(x0, y, w)) {
        for (let j = 0; j < line.length; j++) {
          const data = reader[offset++];
          line[j] = data;
          buffer2[i++] = data;
        }
      }
    }
    const rect = this.takeCoords();
    const rx = rect.x;
    const ry = rect.x;
    const rw = rect.w;
    const mapPtr = this.run(6 /* TAKE_MAP_PTR */) >> 1;
    const heap = this.module.HEAPU16;
    for (let y = y0, yf = y0 + h; y < yf; y++) {
      const buffer2 = buffers[y - y0];
      const dstOffset = mapPtr + (y - ry) * rw + (x0 - rx);
      heap.set(buffer2, dstOffset);
    }
  }
  placeRoad(x, y) {
    const arg = this.module._malloc(4 * 2);
    const view = this.module.HEAPU32.subarray(arg >> 2);
    view[0] = x;
    view[1] = y;
    this.run(9 /* PLACE_SINGLE_ROAD */, arg);
    this.module._free(arg);
  }
  takeCars() {
    const srcPtr = this.run(2 /* COPY_CARS */) >> 2;
    const number = this.module.HEAPU32[srcPtr];
  }
  getChunks(viewX, viewY, rangeW, rangeH) {
    const chunks = [];
    const transfered = [];
    for (const chunk of this.map.getChunks(viewX, viewY, rangeW, rangeH)) {
      const cells = new Uint16Array(chunk.cells);
      chunks.push({
        x: chunk.x,
        y: chunk.y,
        cells
      });
      transfered.push(cells.buffer);
    }
    chunks.sort((a, b) => {
      if (a.x !== b.x) return a.x - b.x;
      return a.y - b.y;
    });
    return {
      result: chunks,
      transfered
    };
  }
  applyEdits(array) {
    const argPtr = this.module._malloc(array.length * 4);
    this.module.HEAPU32.set(array, argPtr >> 2);
    this.run(10 /* APPLY_EDITS */, argPtr);
    this.module._free(argPtr);
  }
  performGameCommand(data) {
    const argPtr = this.module._malloc(data.byteLength);
    this.module.HEAPU16.set(new Uint16Array(data), argPtr >> 1);
    this.run(11 /* GAME_COMMAND */, argPtr);
    this.module._free(argPtr);
  }
  readEntities(array) {
    const argPtr = this.module._malloc(array.length);
    this.module.HEAPU8.set(array, argPtr);
    this.run(13 /* READ_ENTITIES */, argPtr);
    this.module._free(argPtr);
  }
};

// client/wasm/api.js
var Module = (() => {
  var _scriptDir = import.meta.url;
  return (function(Module2) {
    Module2 = Module2 || {};
    var Module2 = typeof Module2 != "undefined" ? Module2 : {};
    var readyPromiseResolve, readyPromiseReject;
    Module2["ready"] = new Promise(function(resolve, reject) {
      readyPromiseResolve = resolve;
      readyPromiseReject = reject;
    });
    var moduleOverrides = Object.assign({}, Module2);
    var arguments_ = [];
    var thisProgram = "./this.program";
    var quit_ = (status, toThrow) => {
      throw toThrow;
    };
    var ENVIRONMENT_IS_WEB = typeof window == "object";
    var ENVIRONMENT_IS_WORKER = typeof importScripts == "function";
    var ENVIRONMENT_IS_NODE = typeof process == "object" && typeof process.versions == "object" && typeof process.versions.node == "string";
    var scriptDirectory = "";
    function locateFile(path) {
      if (Module2["locateFile"]) {
        return Module2["locateFile"](path, scriptDirectory);
      }
      return scriptDirectory + path;
    }
    var read_, readAsync, readBinary, setWindowTitle;
    if (ENVIRONMENT_IS_WEB || ENVIRONMENT_IS_WORKER) {
      if (ENVIRONMENT_IS_WORKER) {
        scriptDirectory = self.location.href;
      } else if (typeof document != "undefined" && document.currentScript) {
        scriptDirectory = document.currentScript.src;
      }
      if (_scriptDir) {
        scriptDirectory = _scriptDir;
      }
      if (scriptDirectory.indexOf("blob:") !== 0) {
        scriptDirectory = scriptDirectory.substr(0, scriptDirectory.replace(/[?#].*/, "").lastIndexOf("/") + 1);
      } else {
        scriptDirectory = "";
      }
      {
        read_ = ((url) => {
          var xhr = new XMLHttpRequest();
          xhr.open("GET", url, false);
          xhr.send(null);
          return xhr.responseText;
        });
        if (ENVIRONMENT_IS_WORKER) {
          readBinary = ((url) => {
            var xhr = new XMLHttpRequest();
            xhr.open("GET", url, false);
            xhr.responseType = "arraybuffer";
            xhr.send(null);
            return new Uint8Array(xhr.response);
          });
        }
        readAsync = ((url, onload, onerror) => {
          var xhr = new XMLHttpRequest();
          xhr.open("GET", url, true);
          xhr.responseType = "arraybuffer";
          xhr.onload = (() => {
            if (xhr.status == 200 || xhr.status == 0 && xhr.response) {
              onload(xhr.response);
              return;
            }
            onerror();
          });
          xhr.onerror = onerror;
          xhr.send(null);
        });
      }
      setWindowTitle = ((title) => document.title = title);
    } else {
    }
    var out = Module2["print"] || console.log.bind(console);
    var err = Module2["printErr"] || console.warn.bind(console);
    Object.assign(Module2, moduleOverrides);
    moduleOverrides = null;
    if (Module2["arguments"]) arguments_ = Module2["arguments"];
    if (Module2["thisProgram"]) thisProgram = Module2["thisProgram"];
    if (Module2["quit"]) quit_ = Module2["quit"];
    var wasmBinary;
    if (Module2["wasmBinary"]) wasmBinary = Module2["wasmBinary"];
    var noExitRuntime = Module2["noExitRuntime"] || true;
    if (typeof WebAssembly != "object") {
      abort("no native wasm support detected");
    }
    var wasmMemory;
    var ABORT = false;
    var EXITSTATUS;
    function getCFunc(ident) {
      var func = Module2["_" + ident];
      return func;
    }
    function ccall(ident, returnType, argTypes, args, opts) {
      var toC = { "string": function(str) {
        var ret2 = 0;
        if (str !== null && str !== void 0 && str !== 0) {
          var len = (str.length << 2) + 1;
          ret2 = stackAlloc(len);
          stringToUTF8(str, ret2, len);
        }
        return ret2;
      }, "array": function(arr) {
        var ret2 = stackAlloc(arr.length);
        writeArrayToMemory(arr, ret2);
        return ret2;
      } };
      function convertReturnValue(ret2) {
        if (returnType === "string") return UTF8ToString(ret2);
        if (returnType === "boolean") return Boolean(ret2);
        return ret2;
      }
      var func = getCFunc(ident);
      var cArgs = [];
      var stack = 0;
      if (args) {
        for (var i = 0; i < args.length; i++) {
          var converter = toC[argTypes[i]];
          if (converter) {
            if (stack === 0) stack = stackSave();
            cArgs[i] = converter(args[i]);
          } else {
            cArgs[i] = args[i];
          }
        }
      }
      var ret = func.apply(null, cArgs);
      function onDone(ret2) {
        if (stack !== 0) stackRestore(stack);
        return convertReturnValue(ret2);
      }
      ret = onDone(ret);
      return ret;
    }
    function cwrap(ident, returnType, argTypes, opts) {
      argTypes = argTypes || [];
      var numericArgs = argTypes.every(function(type) {
        return type === "number";
      });
      var numericRet = returnType !== "string";
      if (numericRet && numericArgs && !opts) {
        return getCFunc(ident);
      }
      return function() {
        return ccall(ident, returnType, argTypes, arguments, opts);
      };
    }
    var UTF8Decoder = typeof TextDecoder != "undefined" ? new TextDecoder("utf8") : void 0;
    function UTF8ArrayToString(heap, idx, maxBytesToRead) {
      var endIdx = idx + maxBytesToRead;
      var endPtr = idx;
      while (heap[endPtr] && !(endPtr >= endIdx)) ++endPtr;
      if (endPtr - idx > 16 && heap.subarray && UTF8Decoder) {
        return UTF8Decoder.decode(heap.subarray(idx, endPtr));
      } else {
        var str = "";
        while (idx < endPtr) {
          var u0 = heap[idx++];
          if (!(u0 & 128)) {
            str += String.fromCharCode(u0);
            continue;
          }
          var u1 = heap[idx++] & 63;
          if ((u0 & 224) == 192) {
            str += String.fromCharCode((u0 & 31) << 6 | u1);
            continue;
          }
          var u2 = heap[idx++] & 63;
          if ((u0 & 240) == 224) {
            u0 = (u0 & 15) << 12 | u1 << 6 | u2;
          } else {
            u0 = (u0 & 7) << 18 | u1 << 12 | u2 << 6 | heap[idx++] & 63;
          }
          if (u0 < 65536) {
            str += String.fromCharCode(u0);
          } else {
            var ch = u0 - 65536;
            str += String.fromCharCode(55296 | ch >> 10, 56320 | ch & 1023);
          }
        }
      }
      return str;
    }
    function UTF8ToString(ptr, maxBytesToRead) {
      return ptr ? UTF8ArrayToString(HEAPU8, ptr, maxBytesToRead) : "";
    }
    function stringToUTF8Array(str, heap, outIdx, maxBytesToWrite) {
      if (!(maxBytesToWrite > 0)) return 0;
      var startIdx = outIdx;
      var endIdx = outIdx + maxBytesToWrite - 1;
      for (var i = 0; i < str.length; ++i) {
        var u = str.charCodeAt(i);
        if (u >= 55296 && u <= 57343) {
          var u1 = str.charCodeAt(++i);
          u = 65536 + ((u & 1023) << 10) | u1 & 1023;
        }
        if (u <= 127) {
          if (outIdx >= endIdx) break;
          heap[outIdx++] = u;
        } else if (u <= 2047) {
          if (outIdx + 1 >= endIdx) break;
          heap[outIdx++] = 192 | u >> 6;
          heap[outIdx++] = 128 | u & 63;
        } else if (u <= 65535) {
          if (outIdx + 2 >= endIdx) break;
          heap[outIdx++] = 224 | u >> 12;
          heap[outIdx++] = 128 | u >> 6 & 63;
          heap[outIdx++] = 128 | u & 63;
        } else {
          if (outIdx + 3 >= endIdx) break;
          heap[outIdx++] = 240 | u >> 18;
          heap[outIdx++] = 128 | u >> 12 & 63;
          heap[outIdx++] = 128 | u >> 6 & 63;
          heap[outIdx++] = 128 | u & 63;
        }
      }
      heap[outIdx] = 0;
      return outIdx - startIdx;
    }
    function stringToUTF8(str, outPtr, maxBytesToWrite) {
      return stringToUTF8Array(str, HEAPU8, outPtr, maxBytesToWrite);
    }
    function lengthBytesUTF8(str) {
      var len = 0;
      for (var i = 0; i < str.length; ++i) {
        var u = str.charCodeAt(i);
        if (u >= 55296 && u <= 57343) u = 65536 + ((u & 1023) << 10) | str.charCodeAt(++i) & 1023;
        if (u <= 127) ++len;
        else if (u <= 2047) len += 2;
        else if (u <= 65535) len += 3;
        else len += 4;
      }
      return len;
    }
    function writeArrayToMemory(array, buffer2) {
      HEAP8.set(array, buffer2);
    }
    function writeAsciiToMemory(str, buffer2, dontAddNull) {
      for (var i = 0; i < str.length; ++i) {
        HEAP8[buffer2++ >> 0] = str.charCodeAt(i);
      }
      if (!dontAddNull) HEAP8[buffer2 >> 0] = 0;
    }
    var buffer, HEAP8, HEAPU8, HEAP16, HEAPU16, HEAP32, HEAPU32, HEAPF32, HEAPF64;
    function updateGlobalBufferAndViews(buf) {
      buffer = buf;
      Module2["HEAP8"] = HEAP8 = new Int8Array(buf);
      Module2["HEAP16"] = HEAP16 = new Int16Array(buf);
      Module2["HEAP32"] = HEAP32 = new Int32Array(buf);
      Module2["HEAPU8"] = HEAPU8 = new Uint8Array(buf);
      Module2["HEAPU16"] = HEAPU16 = new Uint16Array(buf);
      Module2["HEAPU32"] = HEAPU32 = new Uint32Array(buf);
      Module2["HEAPF32"] = HEAPF32 = new Float32Array(buf);
      Module2["HEAPF64"] = HEAPF64 = new Float64Array(buf);
    }
    var INITIAL_MEMORY = Module2["INITIAL_MEMORY"] || 16777216;
    var wasmTable;
    var __ATPRERUN__ = [];
    var __ATINIT__ = [];
    var __ATPOSTRUN__ = [];
    var runtimeInitialized = false;
    function preRun() {
      if (Module2["preRun"]) {
        if (typeof Module2["preRun"] == "function") Module2["preRun"] = [Module2["preRun"]];
        while (Module2["preRun"].length) {
          addOnPreRun(Module2["preRun"].shift());
        }
      }
      callRuntimeCallbacks(__ATPRERUN__);
    }
    function initRuntime() {
      runtimeInitialized = true;
      callRuntimeCallbacks(__ATINIT__);
    }
    function postRun() {
      if (Module2["postRun"]) {
        if (typeof Module2["postRun"] == "function") Module2["postRun"] = [Module2["postRun"]];
        while (Module2["postRun"].length) {
          addOnPostRun(Module2["postRun"].shift());
        }
      }
      callRuntimeCallbacks(__ATPOSTRUN__);
    }
    function addOnPreRun(cb) {
      __ATPRERUN__.unshift(cb);
    }
    function addOnInit(cb) {
      __ATINIT__.unshift(cb);
    }
    function addOnPostRun(cb) {
      __ATPOSTRUN__.unshift(cb);
    }
    var runDependencies = 0;
    var runDependencyWatcher = null;
    var dependenciesFulfilled = null;
    function addRunDependency(id) {
      runDependencies++;
      if (Module2["monitorRunDependencies"]) {
        Module2["monitorRunDependencies"](runDependencies);
      }
    }
    function removeRunDependency(id) {
      runDependencies--;
      if (Module2["monitorRunDependencies"]) {
        Module2["monitorRunDependencies"](runDependencies);
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
    Module2["preloadedImages"] = {};
    Module2["preloadedAudios"] = {};
    function abort(what) {
      {
        if (Module2["onAbort"]) {
          Module2["onAbort"](what);
        }
      }
      what = "Aborted(" + what + ")";
      err(what);
      ABORT = true;
      EXITSTATUS = 1;
      what += ". Build with -s ASSERTIONS=1 for more info.";
      var e = new WebAssembly.RuntimeError(what);
      readyPromiseReject(e);
      throw e;
    }
    var dataURIPrefix = "data:application/octet-stream;base64,";
    function isDataURI(filename) {
      return filename.startsWith(dataURIPrefix);
    }
    var wasmBinaryFile;
    if (Module2["locateFile"]) {
      wasmBinaryFile = "api.wasm";
      if (!isDataURI(wasmBinaryFile)) {
        wasmBinaryFile = locateFile(wasmBinaryFile);
      }
    } else {
      wasmBinaryFile = new URL("api.wasm", import.meta.url).toString();
    }
    function getBinary(file) {
      try {
        if (file == wasmBinaryFile && wasmBinary) {
          return new Uint8Array(wasmBinary);
        }
        if (readBinary) {
          return readBinary(file);
        } else {
          throw "both async and sync fetching of the wasm failed";
        }
      } catch (err2) {
        abort(err2);
      }
    }
    function getBinaryPromise() {
      if (!wasmBinary && (ENVIRONMENT_IS_WEB || ENVIRONMENT_IS_WORKER)) {
        if (typeof fetch == "function") {
          return fetch(wasmBinaryFile, { credentials: "same-origin" }).then(function(response) {
            if (!response["ok"]) {
              throw "failed to load wasm binary file at '" + wasmBinaryFile + "'";
            }
            return response["arrayBuffer"]();
          }).catch(function() {
            return getBinary(wasmBinaryFile);
          });
        }
      }
      return Promise.resolve().then(function() {
        return getBinary(wasmBinaryFile);
      });
    }
    function createWasm() {
      var info = { "a": asmLibraryArg };
      function receiveInstance(instance, module) {
        var exports2 = instance.exports;
        Module2["asm"] = exports2;
        wasmMemory = Module2["asm"]["k"];
        updateGlobalBufferAndViews(wasmMemory.buffer);
        wasmTable = Module2["asm"]["u"];
        addOnInit(Module2["asm"]["l"]);
        removeRunDependency("wasm-instantiate");
      }
      addRunDependency("wasm-instantiate");
      function receiveInstantiationResult(result) {
        receiveInstance(result["instance"]);
      }
      function instantiateArrayBuffer(receiver) {
        return getBinaryPromise().then(function(binary) {
          return WebAssembly.instantiate(binary, info);
        }).then(function(instance) {
          return instance;
        }).then(receiver, function(reason) {
          err("failed to asynchronously prepare wasm: " + reason);
          abort(reason);
        });
      }
      function instantiateAsync() {
        if (!wasmBinary && typeof WebAssembly.instantiateStreaming == "function" && !isDataURI(wasmBinaryFile) && typeof fetch == "function") {
          return fetch(wasmBinaryFile, { credentials: "same-origin" }).then(function(response) {
            var result = WebAssembly.instantiateStreaming(response, info);
            return result.then(receiveInstantiationResult, function(reason) {
              err("wasm streaming compile failed: " + reason);
              err("falling back to ArrayBuffer instantiation");
              return instantiateArrayBuffer(receiveInstantiationResult);
            });
          });
        } else {
          return instantiateArrayBuffer(receiveInstantiationResult);
        }
      }
      if (Module2["instantiateWasm"]) {
        try {
          var exports = Module2["instantiateWasm"](info, receiveInstance);
          return exports;
        } catch (e) {
          err("Module.instantiateWasm callback failed with error: " + e);
          return false;
        }
      }
      instantiateAsync().catch(readyPromiseReject);
      return {};
    }
    function callRuntimeCallbacks(callbacks) {
      while (callbacks.length > 0) {
        var callback = callbacks.shift();
        if (typeof callback == "function") {
          callback(Module2);
          continue;
        }
        var func = callback.func;
        if (typeof func == "number") {
          if (callback.arg === void 0) {
            getWasmTableEntry(func)();
          } else {
            getWasmTableEntry(func)(callback.arg);
          }
        } else {
          func(callback.arg === void 0 ? null : callback.arg);
        }
      }
    }
    var wasmTableMirror = [];
    function getWasmTableEntry(funcPtr) {
      var func = wasmTableMirror[funcPtr];
      if (!func) {
        if (funcPtr >= wasmTableMirror.length) wasmTableMirror.length = funcPtr + 1;
        wasmTableMirror[funcPtr] = func = wasmTable.get(funcPtr);
      }
      return func;
    }
    function ___cxa_allocate_exception(size) {
      return _malloc(size + 16) + 16;
    }
    function ExceptionInfo(excPtr) {
      this.excPtr = excPtr;
      this.ptr = excPtr - 16;
      this.set_type = function(type) {
        HEAP32[this.ptr + 4 >> 2] = type;
      };
      this.get_type = function() {
        return HEAP32[this.ptr + 4 >> 2];
      };
      this.set_destructor = function(destructor) {
        HEAP32[this.ptr + 8 >> 2] = destructor;
      };
      this.get_destructor = function() {
        return HEAP32[this.ptr + 8 >> 2];
      };
      this.set_refcount = function(refcount) {
        HEAP32[this.ptr >> 2] = refcount;
      };
      this.set_caught = function(caught) {
        caught = caught ? 1 : 0;
        HEAP8[this.ptr + 12 >> 0] = caught;
      };
      this.get_caught = function() {
        return HEAP8[this.ptr + 12 >> 0] != 0;
      };
      this.set_rethrown = function(rethrown) {
        rethrown = rethrown ? 1 : 0;
        HEAP8[this.ptr + 13 >> 0] = rethrown;
      };
      this.get_rethrown = function() {
        return HEAP8[this.ptr + 13 >> 0] != 0;
      };
      this.init = function(type, destructor) {
        this.set_type(type);
        this.set_destructor(destructor);
        this.set_refcount(0);
        this.set_caught(false);
        this.set_rethrown(false);
      };
      this.add_ref = function() {
        var value = HEAP32[this.ptr >> 2];
        HEAP32[this.ptr >> 2] = value + 1;
      };
      this.release_ref = function() {
        var prev = HEAP32[this.ptr >> 2];
        HEAP32[this.ptr >> 2] = prev - 1;
        return prev === 1;
      };
    }
    var exceptionLast = 0;
    var uncaughtExceptionCount = 0;
    function ___cxa_throw(ptr, type, destructor) {
      var info = new ExceptionInfo(ptr);
      info.init(type, destructor);
      exceptionLast = ptr;
      uncaughtExceptionCount++;
      throw ptr;
    }
    function _abort() {
      abort("");
    }
    var _emscripten_get_now;
    _emscripten_get_now = (() => performance.now());
    var _emscripten_get_now_is_monotonic = true;
    function setErrNo(value) {
      HEAP32[___errno_location() >> 2] = value;
      return value;
    }
    function _clock_gettime(clk_id, tp) {
      var now;
      if (clk_id === 0) {
        now = Date.now();
      } else if ((clk_id === 1 || clk_id === 4) && _emscripten_get_now_is_monotonic) {
        now = _emscripten_get_now();
      } else {
        setErrNo(28);
        return -1;
      }
      HEAP32[tp >> 2] = now / 1e3 | 0;
      HEAP32[tp + 4 >> 2] = now % 1e3 * 1e3 * 1e3 | 0;
      return 0;
    }
    function _emscripten_memcpy_big(dest, src, num) {
      HEAPU8.copyWithin(dest, src, src + num);
    }
    function _emscripten_get_heap_max() {
      return 2147483648;
    }
    function emscripten_realloc_buffer(size) {
      try {
        wasmMemory.grow(size - buffer.byteLength + 65535 >>> 16);
        updateGlobalBufferAndViews(wasmMemory.buffer);
        return 1;
      } catch (e) {
      }
    }
    function _emscripten_resize_heap(requestedSize) {
      var oldSize = HEAPU8.length;
      requestedSize = requestedSize >>> 0;
      var maxHeapSize = _emscripten_get_heap_max();
      if (requestedSize > maxHeapSize) {
        return false;
      }
      let alignUp = (x, multiple) => x + (multiple - x % multiple) % multiple;
      for (var cutDown = 1; cutDown <= 4; cutDown *= 2) {
        var overGrownHeapSize = oldSize * (1 + 0.2 / cutDown);
        overGrownHeapSize = Math.min(overGrownHeapSize, requestedSize + 100663296);
        var newSize = Math.min(maxHeapSize, alignUp(Math.max(requestedSize, overGrownHeapSize), 65536));
        var replacement = emscripten_realloc_buffer(newSize);
        if (replacement) {
          return true;
        }
      }
      return false;
    }
    var ENV = {};
    function getExecutableName() {
      return thisProgram || "./this.program";
    }
    function getEnvStrings() {
      if (!getEnvStrings.strings) {
        var lang = (typeof navigator == "object" && navigator.languages && navigator.languages[0] || "C").replace("-", "_") + ".UTF-8";
        var env = { "USER": "web_user", "LOGNAME": "web_user", "PATH": "/", "PWD": "/", "HOME": "/home/web_user", "LANG": lang, "_": getExecutableName() };
        for (var x in ENV) {
          if (ENV[x] === void 0) delete env[x];
          else env[x] = ENV[x];
        }
        var strings = [];
        for (var x in env) {
          strings.push(x + "=" + env[x]);
        }
        getEnvStrings.strings = strings;
      }
      return getEnvStrings.strings;
    }
    var SYSCALLS = { buffers: [null, [], []], printChar: function(stream, curr) {
      var buffer2 = SYSCALLS.buffers[stream];
      if (curr === 0 || curr === 10) {
        (stream === 1 ? out : err)(UTF8ArrayToString(buffer2, 0));
        buffer2.length = 0;
      } else {
        buffer2.push(curr);
      }
    }, varargs: void 0, get: function() {
      SYSCALLS.varargs += 4;
      var ret = HEAP32[SYSCALLS.varargs - 4 >> 2];
      return ret;
    }, getStr: function(ptr) {
      var ret = UTF8ToString(ptr);
      return ret;
    }, get64: function(low, high) {
      return low;
    } };
    function _environ_get(__environ, environ_buf) {
      var bufSize = 0;
      getEnvStrings().forEach(function(string, i) {
        var ptr = environ_buf + bufSize;
        HEAP32[__environ + i * 4 >> 2] = ptr;
        writeAsciiToMemory(string, ptr);
        bufSize += string.length + 1;
      });
      return 0;
    }
    function _environ_sizes_get(penviron_count, penviron_buf_size) {
      var strings = getEnvStrings();
      HEAP32[penviron_count >> 2] = strings.length;
      var bufSize = 0;
      strings.forEach(function(string) {
        bufSize += string.length + 1;
      });
      HEAP32[penviron_buf_size >> 2] = bufSize;
      return 0;
    }
    function _fd_write(fd, iov, iovcnt, pnum) {
      var num = 0;
      for (var i = 0; i < iovcnt; i++) {
        var ptr = HEAP32[iov >> 2];
        var len = HEAP32[iov + 4 >> 2];
        iov += 8;
        for (var j = 0; j < len; j++) {
          SYSCALLS.printChar(fd, HEAPU8[ptr + j]);
        }
        num += len;
      }
      HEAP32[pnum >> 2] = num;
      return 0;
    }
    function __isLeapYear(year) {
      return year % 4 === 0 && (year % 100 !== 0 || year % 400 === 0);
    }
    function __arraySum(array, index) {
      var sum = 0;
      for (var i = 0; i <= index; sum += array[i++]) {
      }
      return sum;
    }
    var __MONTH_DAYS_LEAP = [31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31];
    var __MONTH_DAYS_REGULAR = [31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31];
    function __addDays(date, days) {
      var newDate = new Date(date.getTime());
      while (days > 0) {
        var leap = __isLeapYear(newDate.getFullYear());
        var currentMonth = newDate.getMonth();
        var daysInCurrentMonth = (leap ? __MONTH_DAYS_LEAP : __MONTH_DAYS_REGULAR)[currentMonth];
        if (days > daysInCurrentMonth - newDate.getDate()) {
          days -= daysInCurrentMonth - newDate.getDate() + 1;
          newDate.setDate(1);
          if (currentMonth < 11) {
            newDate.setMonth(currentMonth + 1);
          } else {
            newDate.setMonth(0);
            newDate.setFullYear(newDate.getFullYear() + 1);
          }
        } else {
          newDate.setDate(newDate.getDate() + days);
          return newDate;
        }
      }
      return newDate;
    }
    function _strftime(s, maxsize, format, tm) {
      var tm_zone = HEAP32[tm + 40 >> 2];
      var date = { tm_sec: HEAP32[tm >> 2], tm_min: HEAP32[tm + 4 >> 2], tm_hour: HEAP32[tm + 8 >> 2], tm_mday: HEAP32[tm + 12 >> 2], tm_mon: HEAP32[tm + 16 >> 2], tm_year: HEAP32[tm + 20 >> 2], tm_wday: HEAP32[tm + 24 >> 2], tm_yday: HEAP32[tm + 28 >> 2], tm_isdst: HEAP32[tm + 32 >> 2], tm_gmtoff: HEAP32[tm + 36 >> 2], tm_zone: tm_zone ? UTF8ToString(tm_zone) : "" };
      var pattern = UTF8ToString(format);
      var EXPANSION_RULES_1 = { "%c": "%a %b %d %H:%M:%S %Y", "%D": "%m/%d/%y", "%F": "%Y-%m-%d", "%h": "%b", "%r": "%I:%M:%S %p", "%R": "%H:%M", "%T": "%H:%M:%S", "%x": "%m/%d/%y", "%X": "%H:%M:%S", "%Ec": "%c", "%EC": "%C", "%Ex": "%m/%d/%y", "%EX": "%H:%M:%S", "%Ey": "%y", "%EY": "%Y", "%Od": "%d", "%Oe": "%e", "%OH": "%H", "%OI": "%I", "%Om": "%m", "%OM": "%M", "%OS": "%S", "%Ou": "%u", "%OU": "%U", "%OV": "%V", "%Ow": "%w", "%OW": "%W", "%Oy": "%y" };
      for (var rule in EXPANSION_RULES_1) {
        pattern = pattern.replace(new RegExp(rule, "g"), EXPANSION_RULES_1[rule]);
      }
      var WEEKDAYS = ["Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"];
      var MONTHS = ["January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"];
      function leadingSomething(value, digits, character) {
        var str = typeof value == "number" ? value.toString() : value || "";
        while (str.length < digits) {
          str = character[0] + str;
        }
        return str;
      }
      function leadingNulls(value, digits) {
        return leadingSomething(value, digits, "0");
      }
      function compareByDay(date1, date2) {
        function sgn(value) {
          return value < 0 ? -1 : value > 0 ? 1 : 0;
        }
        var compare;
        if ((compare = sgn(date1.getFullYear() - date2.getFullYear())) === 0) {
          if ((compare = sgn(date1.getMonth() - date2.getMonth())) === 0) {
            compare = sgn(date1.getDate() - date2.getDate());
          }
        }
        return compare;
      }
      function getFirstWeekStartDate(janFourth) {
        switch (janFourth.getDay()) {
          case 0:
            return new Date(janFourth.getFullYear() - 1, 11, 29);
          case 1:
            return janFourth;
          case 2:
            return new Date(janFourth.getFullYear(), 0, 3);
          case 3:
            return new Date(janFourth.getFullYear(), 0, 2);
          case 4:
            return new Date(janFourth.getFullYear(), 0, 1);
          case 5:
            return new Date(janFourth.getFullYear() - 1, 11, 31);
          case 6:
            return new Date(janFourth.getFullYear() - 1, 11, 30);
        }
      }
      function getWeekBasedYear(date2) {
        var thisDate = __addDays(new Date(date2.tm_year + 1900, 0, 1), date2.tm_yday);
        var janFourthThisYear = new Date(thisDate.getFullYear(), 0, 4);
        var janFourthNextYear = new Date(thisDate.getFullYear() + 1, 0, 4);
        var firstWeekStartThisYear = getFirstWeekStartDate(janFourthThisYear);
        var firstWeekStartNextYear = getFirstWeekStartDate(janFourthNextYear);
        if (compareByDay(firstWeekStartThisYear, thisDate) <= 0) {
          if (compareByDay(firstWeekStartNextYear, thisDate) <= 0) {
            return thisDate.getFullYear() + 1;
          } else {
            return thisDate.getFullYear();
          }
        } else {
          return thisDate.getFullYear() - 1;
        }
      }
      var EXPANSION_RULES_2 = { "%a": function(date2) {
        return WEEKDAYS[date2.tm_wday].substring(0, 3);
      }, "%A": function(date2) {
        return WEEKDAYS[date2.tm_wday];
      }, "%b": function(date2) {
        return MONTHS[date2.tm_mon].substring(0, 3);
      }, "%B": function(date2) {
        return MONTHS[date2.tm_mon];
      }, "%C": function(date2) {
        var year = date2.tm_year + 1900;
        return leadingNulls(year / 100 | 0, 2);
      }, "%d": function(date2) {
        return leadingNulls(date2.tm_mday, 2);
      }, "%e": function(date2) {
        return leadingSomething(date2.tm_mday, 2, " ");
      }, "%g": function(date2) {
        return getWeekBasedYear(date2).toString().substring(2);
      }, "%G": function(date2) {
        return getWeekBasedYear(date2);
      }, "%H": function(date2) {
        return leadingNulls(date2.tm_hour, 2);
      }, "%I": function(date2) {
        var twelveHour = date2.tm_hour;
        if (twelveHour == 0) twelveHour = 12;
        else if (twelveHour > 12) twelveHour -= 12;
        return leadingNulls(twelveHour, 2);
      }, "%j": function(date2) {
        return leadingNulls(date2.tm_mday + __arraySum(__isLeapYear(date2.tm_year + 1900) ? __MONTH_DAYS_LEAP : __MONTH_DAYS_REGULAR, date2.tm_mon - 1), 3);
      }, "%m": function(date2) {
        return leadingNulls(date2.tm_mon + 1, 2);
      }, "%M": function(date2) {
        return leadingNulls(date2.tm_min, 2);
      }, "%n": function() {
        return "\n";
      }, "%p": function(date2) {
        if (date2.tm_hour >= 0 && date2.tm_hour < 12) {
          return "AM";
        } else {
          return "PM";
        }
      }, "%S": function(date2) {
        return leadingNulls(date2.tm_sec, 2);
      }, "%t": function() {
        return "	";
      }, "%u": function(date2) {
        return date2.tm_wday || 7;
      }, "%U": function(date2) {
        var janFirst = new Date(date2.tm_year + 1900, 0, 1);
        var firstSunday = janFirst.getDay() === 0 ? janFirst : __addDays(janFirst, 7 - janFirst.getDay());
        var endDate = new Date(date2.tm_year + 1900, date2.tm_mon, date2.tm_mday);
        if (compareByDay(firstSunday, endDate) < 0) {
          var februaryFirstUntilEndMonth = __arraySum(__isLeapYear(endDate.getFullYear()) ? __MONTH_DAYS_LEAP : __MONTH_DAYS_REGULAR, endDate.getMonth() - 1) - 31;
          var firstSundayUntilEndJanuary = 31 - firstSunday.getDate();
          var days = firstSundayUntilEndJanuary + februaryFirstUntilEndMonth + endDate.getDate();
          return leadingNulls(Math.ceil(days / 7), 2);
        }
        return compareByDay(firstSunday, janFirst) === 0 ? "01" : "00";
      }, "%V": function(date2) {
        var janFourthThisYear = new Date(date2.tm_year + 1900, 0, 4);
        var janFourthNextYear = new Date(date2.tm_year + 1901, 0, 4);
        var firstWeekStartThisYear = getFirstWeekStartDate(janFourthThisYear);
        var firstWeekStartNextYear = getFirstWeekStartDate(janFourthNextYear);
        var endDate = __addDays(new Date(date2.tm_year + 1900, 0, 1), date2.tm_yday);
        if (compareByDay(endDate, firstWeekStartThisYear) < 0) {
          return "53";
        }
        if (compareByDay(firstWeekStartNextYear, endDate) <= 0) {
          return "01";
        }
        var daysDifference;
        if (firstWeekStartThisYear.getFullYear() < date2.tm_year + 1900) {
          daysDifference = date2.tm_yday + 32 - firstWeekStartThisYear.getDate();
        } else {
          daysDifference = date2.tm_yday + 1 - firstWeekStartThisYear.getDate();
        }
        return leadingNulls(Math.ceil(daysDifference / 7), 2);
      }, "%w": function(date2) {
        return date2.tm_wday;
      }, "%W": function(date2) {
        var janFirst = new Date(date2.tm_year, 0, 1);
        var firstMonday = janFirst.getDay() === 1 ? janFirst : __addDays(janFirst, janFirst.getDay() === 0 ? 1 : 7 - janFirst.getDay() + 1);
        var endDate = new Date(date2.tm_year + 1900, date2.tm_mon, date2.tm_mday);
        if (compareByDay(firstMonday, endDate) < 0) {
          var februaryFirstUntilEndMonth = __arraySum(__isLeapYear(endDate.getFullYear()) ? __MONTH_DAYS_LEAP : __MONTH_DAYS_REGULAR, endDate.getMonth() - 1) - 31;
          var firstMondayUntilEndJanuary = 31 - firstMonday.getDate();
          var days = firstMondayUntilEndJanuary + februaryFirstUntilEndMonth + endDate.getDate();
          return leadingNulls(Math.ceil(days / 7), 2);
        }
        return compareByDay(firstMonday, janFirst) === 0 ? "01" : "00";
      }, "%y": function(date2) {
        return (date2.tm_year + 1900).toString().substring(2);
      }, "%Y": function(date2) {
        return date2.tm_year + 1900;
      }, "%z": function(date2) {
        var off = date2.tm_gmtoff;
        var ahead = off >= 0;
        off = Math.abs(off) / 60;
        off = off / 60 * 100 + off % 60;
        return (ahead ? "+" : "-") + String("0000" + off).slice(-4);
      }, "%Z": function(date2) {
        return date2.tm_zone;
      }, "%%": function() {
        return "%";
      } };
      pattern = pattern.replace(/%%/g, "\0\0");
      for (var rule in EXPANSION_RULES_2) {
        if (pattern.includes(rule)) {
          pattern = pattern.replace(new RegExp(rule, "g"), EXPANSION_RULES_2[rule](date));
        }
      }
      pattern = pattern.replace(/\0\0/g, "%");
      var bytes = intArrayFromString(pattern, false);
      if (bytes.length > maxsize) {
        return 0;
      }
      writeArrayToMemory(bytes, s);
      return bytes.length - 1;
    }
    function _strftime_l(s, maxsize, format, tm) {
      return _strftime(s, maxsize, format, tm);
    }
    function intArrayFromString(stringy, dontAddNull, length) {
      var len = length > 0 ? length : lengthBytesUTF8(stringy) + 1;
      var u8array = new Array(len);
      var numBytesWritten = stringToUTF8Array(stringy, u8array, 0, u8array.length);
      if (dontAddNull) u8array.length = numBytesWritten;
      return u8array;
    }
    var asmLibraryArg = { "b": ___cxa_allocate_exception, "a": ___cxa_throw, "c": _abort, "h": _clock_gettime, "j": _emscripten_memcpy_big, "i": _emscripten_resize_heap, "f": _environ_get, "g": _environ_sizes_get, "d": _fd_write, "e": _strftime_l };
    var asm = createWasm();
    var ___wasm_call_ctors = Module2["___wasm_call_ctors"] = function() {
      return (___wasm_call_ctors = Module2["___wasm_call_ctors"] = Module2["asm"]["l"]).apply(null, arguments);
    };
    var _free = Module2["_free"] = function() {
      return (_free = Module2["_free"] = Module2["asm"]["m"]).apply(null, arguments);
    };
    var _malloc = Module2["_malloc"] = function() {
      return (_malloc = Module2["_malloc"] = Module2["asm"]["n"]).apply(null, arguments);
    };
    var _Api_createApi = Module2["_Api_createApi"] = function() {
      return (_Api_createApi = Module2["_Api_createApi"] = Module2["asm"]["o"]).apply(null, arguments);
    };
    var _Api_deleteApi = Module2["_Api_deleteApi"] = function() {
      return (_Api_deleteApi = Module2["_Api_deleteApi"] = Module2["asm"]["p"]).apply(null, arguments);
    };
    var _Api_createSession = Module2["_Api_createSession"] = function() {
      return (_Api_createSession = Module2["_Api_createSession"] = Module2["asm"]["q"]).apply(null, arguments);
    };
    var _Api_deleteSession = Module2["_Api_deleteSession"] = function() {
      return (_Api_deleteSession = Module2["_Api_deleteSession"] = Module2["asm"]["r"]).apply(null, arguments);
    };
    var _Api_take = Module2["_Api_take"] = function() {
      return (_Api_take = Module2["_Api_take"] = Module2["asm"]["s"]).apply(null, arguments);
    };
    var _Api_runFrames = Module2["_Api_runFrames"] = function() {
      return (_Api_runFrames = Module2["_Api_runFrames"] = Module2["asm"]["t"]).apply(null, arguments);
    };
    var ___errno_location = Module2["___errno_location"] = function() {
      return (___errno_location = Module2["___errno_location"] = Module2["asm"]["v"]).apply(null, arguments);
    };
    var stackSave = Module2["stackSave"] = function() {
      return (stackSave = Module2["stackSave"] = Module2["asm"]["w"]).apply(null, arguments);
    };
    var stackRestore = Module2["stackRestore"] = function() {
      return (stackRestore = Module2["stackRestore"] = Module2["asm"]["x"]).apply(null, arguments);
    };
    var stackAlloc = Module2["stackAlloc"] = function() {
      return (stackAlloc = Module2["stackAlloc"] = Module2["asm"]["y"]).apply(null, arguments);
    };
    Module2["ccall"] = ccall;
    Module2["cwrap"] = cwrap;
    var calledRun;
    dependenciesFulfilled = function runCaller() {
      if (!calledRun) run();
      if (!calledRun) dependenciesFulfilled = runCaller;
    };
    function run(args) {
      args = args || arguments_;
      if (runDependencies > 0) {
        return;
      }
      preRun();
      if (runDependencies > 0) {
        return;
      }
      function doRun() {
        if (calledRun) return;
        calledRun = true;
        Module2["calledRun"] = true;
        if (ABORT) return;
        initRuntime();
        readyPromiseResolve(Module2);
        if (Module2["onRuntimeInitialized"]) Module2["onRuntimeInitialized"]();
        postRun();
      }
      if (Module2["setStatus"]) {
        Module2["setStatus"]("Running...");
        setTimeout(function() {
          setTimeout(function() {
            Module2["setStatus"]("");
          }, 1);
          doRun();
        }, 1);
      } else {
        doRun();
      }
    }
    Module2["run"] = run;
    if (Module2["preInit"]) {
      if (typeof Module2["preInit"] == "function") Module2["preInit"] = [Module2["preInit"]];
      while (Module2["preInit"].length > 0) {
        Module2["preInit"].pop()();
      }
    }
    run();
    return Module2.ready;
  });
})();
var api_default = Module;

// client/src/worker/clientWorker.ts
var api = new ClientApi();
var moduleReadyResolvers = [];
function initModule(wasmPath) {
  api_default({
    locateFile(path) {
      if (path.endsWith(".wasm")) {
        return new URL(wasmPath, import.meta.url).href;
      }
      return path;
    }
  }).then((module) => {
    api.setModule(module);
    console.log("wasm module has been loaded");
    for (let resolve of moduleReadyResolvers) {
      resolve();
    }
    moduleReadyResolvers.length = 0;
  });
}
self.onmessage = async (event) => {
  const { requestId, method } = event.data;
  const args = event.data.args;
  if (method === "init") {
    initModule(event.data.wasmPath);
    return;
  }
  if (method === "shutdown") {
    api.cleanup();
    self.close();
    return;
  }
  if (api.isModuleMissing()) {
    await new Promise((resolve) => {
      moduleReadyResolvers.push(resolve);
    });
  }
  const result = api[method](...args);
  api.freeBuffer();
  if (result === void 0) {
    self.postMessage({ requestId });
  } else if (result.transfered && result.result) {
    self.postMessage({
      requestId,
      result: result.result
    }, result.transfered);
  } else {
    self.postMessage({ requestId, result });
  }
};

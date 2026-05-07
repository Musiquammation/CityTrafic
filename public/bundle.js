const GAME_WIDTH = 1600;
const GAME_HEIGHT = 900;
class Keydown {
  constructor() {
    this.turnLeft = false;
    this.turnRight = false;
    this.yieldIns = false;
    this.light = false;
    this.fastView = false;
    this.altern = false;
  }
}
class KeyboardCollector {
  constructor() {
    this.turnLeft = 0;
    this.turnRight = 0;
    this.yieldIns = 0;
    this.light = 0;
    this.fastView = 0;
    this.altern = 0;
  }
}
const _InputHandler = class _InputHandler {
  constructor(mode) {
    this.collectedKeys = new KeyboardCollector();
    this.keysDown = new Keydown();
    this.firstPress = new Keydown();
    this.killedPress = new Keydown();
    this.onMouseUp = (e) => {
    };
    this.onMouseDown = (e) => {
    };
    this.onMouseMove = (e) => {
    };
    this.onScroll = (e) => {
    };
    this.onTouchStart = (e) => {
    };
    this.onTouchEnd = (e) => {
    };
    this.onTouchMove = (e) => {
    };
    this.onKeydown = (event) => {
      const e = event;
      const control = this.keyMap[e.code];
      if (control) {
        this.applyKeydown(control);
      }
    };
    this.onKeyup = (event) => {
      const e = event;
      const control = this.keyMap[e.code];
      if (control) {
        this.applyKeyup(control);
      }
    };
    this.onButtonTouchStart = (control, element) => {
      element.classList.add("high");
      if (control === "special") {
        return;
      }
      switch (this.collectedKeys[control]) {
        case 0:
          this.collectedKeys[control] = 1;
          break;
        case 1:
          break;
        case 2:
          this.collectedKeys[control] = 4;
          break;
        case 3:
          this.collectedKeys[control] = 4;
          break;
        case 4:
          this.collectedKeys[control] = 4;
          break;
      }
    };
    this.onButtonTouchEnd = (control, element) => {
      element.classList.remove("high");
      if (control === "special") {
        return;
      }
      switch (this.collectedKeys[control]) {
        case 0:
          this.collectedKeys[control] = 2;
          break;
        case 1:
          this.collectedKeys[control] = 3;
          break;
        case 2:
          break;
        case 3:
          this.collectedKeys[control] = 3;
          break;
        case 4:
          this.collectedKeys[control] = 3;
          break;
      }
    };
    this.keyMap = _InputHandler.KEYBOARDS[mode];
  }
  applyKeydown(control) {
    switch (this.collectedKeys[control]) {
      case 0:
        this.collectedKeys[control] = 1;
        break;
      case 1:
        break;
      case 2:
        this.collectedKeys[control] = 4;
        break;
      case 3:
        this.collectedKeys[control] = 4;
        break;
      case 4:
        this.collectedKeys[control] = 4;
        break;
    }
  }
  applyKeyup(control) {
    switch (this.collectedKeys[control]) {
      case 0:
        this.collectedKeys[control] = 2;
        break;
      case 1:
        this.collectedKeys[control] = 3;
        break;
      case 2:
        break;
      case 3:
        this.collectedKeys[control] = 3;
        break;
      case 4:
        this.collectedKeys[control] = 3;
        break;
    }
  }
  startKeydownListeners(target) {
    target.addEventListener("keydown", this.onKeydown);
    target.addEventListener("keyup", this.onKeyup);
  }
  startMouseListeners(target) {
    target.addEventListener("mouseup", (e) => {
      this.onMouseUp(e);
    });
    target.addEventListener("mousedown", (e) => {
      this.onMouseDown(e);
    });
    target.addEventListener("mousemove", (e) => {
      this.onMouseMove(e);
    });
    target.addEventListener("wheel", (e) => {
      const we = e;
      if (we.ctrlKey) {
        we.preventDefault();
      }
      this.onScroll(we);
    }, { passive: false });
    target.addEventListener("touchstart", (e) => {
      this.onTouchStart(e);
    });
    target.addEventListener("touchend", (e) => {
      this.onTouchEnd(e);
    });
    target.addEventListener("touchmove", (e) => {
      this.onTouchMove(e);
    });
  }
  removeListeners(target) {
    target.removeEventListener("keydown", this.onKeydown);
    target.removeEventListener("keyup", this.onKeyup);
  }
  update() {
    for (const control of _InputHandler.CONTROLS) {
      this.play(control, this.collectedKeys[control]);
      this.collectedKeys[control] = 0;
    }
  }
  play(control, action) {
    switch (action) {
      case 0:
        this.firstPress[control] = false;
        this.killedPress[control] = false;
        break;
      case 1:
        if (this.keysDown[control]) {
          this.firstPress[control] = false;
        } else {
          this.firstPress[control] = true;
          this.keysDown[control] = true;
        }
        this.killedPress[control] = false;
        break;
      case 2:
        if (this.keysDown[control]) {
          this.firstPress[control] = false;
          this.keysDown[control] = false;
          this.killedPress[control] = true;
        } else {
          this.firstPress[control] = false;
          this.killedPress[control] = false;
        }
        break;
      case 3:
        if (this.keysDown[control]) {
          this.firstPress[control] = false;
          this.keysDown[control] = false;
        } else {
          this.firstPress[control] = true;
        }
        this.killedPress[control] = true;
        break;
      case 4:
        if (this.keysDown[control]) {
          this.firstPress[control] = false;
          this.keysDown[control] = false;
          this.killedPress[control] = true;
        } else {
          this.firstPress[control] = false;
          this.killedPress[control] = false;
        }
        if (this.keysDown[control]) {
          this.firstPress[control] = false;
        } else {
          this.firstPress[control] = true;
          this.keysDown[control] = true;
        }
        this.killedPress[control] = false;
        break;
    }
  }
  press(control) {
    return this.firstPress[control] || this.keysDown[control];
  }
  first(control) {
    return this.firstPress[control];
  }
  killed(control) {
    return this.killedPress[control];
  }
  kill(control, removeFirstPress = false) {
    this.keysDown[control] = false;
    if (removeFirstPress) {
      this.firstPress[control] = false;
    }
  }
};
_InputHandler.CONTROLS = ["turnLeft", "turnRight", "yieldIns", "light", "fastView", "altern"];
_InputHandler.CONTROL_STACK_SIZE = 256;
_InputHandler.KEYBOARDS = {
  zqsd: {
    KeyE: "turnLeft",
    KeyR: "turnRight",
    KeyP: "yieldIns",
    KeyL: "light",
    KeyC: "fastView",
    KeyS: "altern"
  },
  wasd: {
    KeyE: "turnLeft",
    KeyR: "turnRight",
    KeyP: "yieldIns",
    KeyL: "light",
    KeyC: "fastView",
    KeyS: "altern"
  }
};
let InputHandler = _InputHandler;
class ImageLoader {
  constructor(pathRoot) {
    this.folders = {};
    this.pathRoot = pathRoot;
    const size = 2;
    const canvas = document.createElement("canvas");
    canvas.width = size;
    canvas.height = size;
    const ctx = canvas.getContext("2d");
    ctx.imageSmoothingEnabled = false;
    ctx.fillStyle = "violet";
    ctx.fillRect(0, 0, size / 2, size / 2);
    ctx.fillRect(size / 2, size / 2, size / 2, size / 2);
    ctx.fillStyle = "white";
    ctx.fillRect(size / 2, 0, size / 2, size / 2);
    ctx.fillRect(0, size / 2, size / 2, size / 2);
    this.placeholder = canvas;
  }
  hexToRgb(hex) {
    const clean = hex.replace("#", "");
    const r = parseInt(clean.substring(0, 2), 16);
    const g = parseInt(clean.substring(2, 4), 16);
    const b = parseInt(clean.substring(4, 6), 16);
    return [r, g, b];
  }
  recolorImage(img, checked, target) {
    const canvas = document.createElement("canvas");
    canvas.width = img.width;
    canvas.height = img.height;
    const ctx = canvas.getContext("2d");
    ctx.imageSmoothingEnabled = false;
    ctx.drawImage(img, 0, 0);
    const [cr, cg, cb] = this.hexToRgb(checked);
    const [tr, tg, tb] = this.hexToRgb(target);
    const imageData = ctx.getImageData(0, 0, canvas.width, canvas.height);
    const data = imageData.data;
    for (let i = 0; i < data.length; i += 4) {
      if (data[i] === cr && data[i + 1] === cg && data[i + 2] === cb) {
        data[i] = tr;
        data[i + 1] = tg;
        data[i + 2] = tb;
      }
    }
    ctx.putImageData(imageData, 0, 0);
    return canvas;
  }
  async load(list) {
    if (!this.folders["default"]) this.folders["default"] = {};
    const entries = Object.entries(list);
    const promises = entries.map(async ([name, path]) => {
      if (this.folders["default"][name]) {
        return;
      }
      try {
        const res = await fetch(this.pathRoot + path);
        if (!res.ok) throw new Error("Failed to fetch " + path);
        const blob = await res.blob();
        const objectUrl = URL.createObjectURL(blob);
        const img = await new Promise((resolve, reject) => {
          const i = new Image();
          i.onload = () => {
            URL.revokeObjectURL(objectUrl);
            resolve(i);
          };
          i.onerror = (e) => reject(e);
          i.src = objectUrl;
        });
        this.folders["default"][name] = [img];
      } catch (err) {
        console.warn("Error with:", path, err);
      }
    });
    await Promise.all(promises);
  }
  async loadWithColors(checked, colors, list) {
    if (!this.folders["colored"]) this.folders["colored"] = {};
    const entries = Object.entries(list);
    const promises = entries.map(async ([name, path]) => {
      if (this.folders["colored"][name]) {
        return;
      }
      try {
        const res = await fetch(this.pathRoot + path);
        if (!res.ok) throw new Error("Failed to fetch " + path);
        const blob = await res.blob();
        const objectUrl = URL.createObjectURL(blob);
        const img = await new Promise((resolve, reject) => {
          const i = new Image();
          i.onload = () => {
            URL.revokeObjectURL(objectUrl);
            resolve(i);
          };
          i.onerror = (e) => reject(e);
          i.src = objectUrl;
        });
        this.folders["colored"][name] = [];
        for (const color of colors) {
          const recolored = this.recolorImage(img, checked, color);
          this.folders["colored"][name].push(recolored);
        }
      } catch (err) {
        console.warn("Error with:", path, err);
      }
    });
    await Promise.all(promises);
  }
  get(name, color = -1) {
    if (color >= 0) {
      const folder2 = this.folders["colored"];
      if (folder2 && folder2[name] && folder2[name][color])
        return folder2[name][color];
      return this.placeholder;
    }
    const folder = this.folders["default"];
    if (folder && folder[name] && folder[name][0])
      return folder[name][0];
    return this.placeholder;
  }
  getFolders() {
    return this.folders;
  }
}
class GameState {
}
var states;
((states2) => {
  class Home extends GameState {
    enter(data, input, imageLoader) {
      input.onMouseUp = (e) => {
      };
      input.onMouseDown = (e) => {
      };
      input.onMouseMove = (e) => {
      };
      input.onScroll = (e) => {
      };
      input.onTouchStart = (e) => {
      };
      input.onTouchEnd = (e) => {
      };
      input.onTouchMove = (e) => {
      };
    }
    frame(game) {
      return null;
    }
    async draw(args) {
    }
    exit() {
    }
    getCamera() {
      return null;
    }
  }
  states2.Home = Home;
})(states || (states = {}));
class DataReader {
  constructor(buffer) {
    this.offset = 0;
    this.view = new DataView(buffer);
  }
  skip(n) {
    this.offset += n;
  }
  readInt8() {
    const val = this.view.getInt8(this.offset);
    this.offset += 1;
    return val;
  }
  readUint8() {
    const val = this.view.getUint8(this.offset);
    this.offset += 1;
    return val;
  }
  readInt16(littleEndian = true) {
    const val = this.view.getInt16(this.offset, littleEndian);
    this.offset += 2;
    return val;
  }
  readUint16(littleEndian = true) {
    const val = this.view.getUint16(this.offset, littleEndian);
    this.offset += 2;
    return val;
  }
  readInt32(littleEndian = true) {
    const val = this.view.getInt32(this.offset, littleEndian);
    this.offset += 4;
    return val;
  }
  readUint32(littleEndian = true) {
    const val = this.view.getUint32(this.offset, littleEndian);
    this.offset += 4;
    return val;
  }
  readInt64(littleEndian = true) {
    const val = this.view.getBigInt64(this.offset, littleEndian);
    this.offset += 8;
    return val;
  }
  readUint64(littleEndian = true) {
    const val = this.view.getBigUint64(this.offset, littleEndian);
    this.offset += 8;
    return val;
  }
  readFloat32(littleEndian = true) {
    const val = this.view.getFloat32(this.offset, littleEndian);
    this.offset += 4;
    return val;
  }
  readFloat64(littleEndian = true) {
    const val = this.view.getFloat64(this.offset, littleEndian);
    this.offset += 8;
    return val;
  }
  read256() {
    const bytes = new Uint8Array(this.view.buffer, this.offset, 8);
    this.offset += 8;
    let hex = "";
    for (const b of bytes) {
      hex += (b >> 4 & 15).toString(16);
      hex += (b & 15).toString(16);
    }
    return hex;
  }
  readUint8Array(length) {
    const array = new Uint8Array(this.view.buffer, this.offset, length);
    this.offset += length;
    return new Uint8Array(array);
  }
  readUint32Array(length) {
    const array = new Uint32Array(this.view.buffer, this.offset, length);
    this.offset += length * 4;
    return new Uint32Array(array);
  }
  getLength() {
    return this.view.byteLength;
  }
  getOffset() {
    return this.offset;
  }
  setOffset(offset) {
    this.offset = offset;
  }
}
var CLIENT_IDS = /* @__PURE__ */ ((CLIENT_IDS2) => {
  CLIENT_IDS2[CLIENT_IDS2["JOIN_CREATED"] = 0] = "JOIN_CREATED";
  CLIENT_IDS2[CLIENT_IDS2["JOIN_ALIVE"] = 1] = "JOIN_ALIVE";
  CLIENT_IDS2[CLIENT_IDS2["AREAS"] = 2] = "AREAS";
  CLIENT_IDS2[CLIENT_IDS2["EDITS"] = 3] = "EDITS";
  CLIENT_IDS2[CLIENT_IDS2["UPDATE"] = 4] = "UPDATE";
  CLIENT_IDS2[CLIENT_IDS2["PANEL"] = 5] = "PANEL";
  CLIENT_IDS2[CLIENT_IDS2["GET_ENTITY"] = 6] = "GET_ENTITY";
  CLIENT_IDS2[CLIENT_IDS2["ERROR"] = 7] = "ERROR";
  return CLIENT_IDS2;
})(CLIENT_IDS || {});
let _gameHandler = null;
function setGameHandler(gameHandler) {
  _gameHandler = gameHandler;
}
function getGameHandler() {
  if (_gameHandler === null)
    throw new Error("Missing gameHandler");
  return _gameHandler;
}
class DataWriter {
  constructor(size = 64) {
    this.offset = 0;
    this.buffer = new ArrayBuffer(size);
    this.view = new DataView(this.buffer);
  }
  skip(n) {
    this.offset += n;
  }
  checkSize(required) {
    const needed = this.offset + required;
    if (needed <= this.buffer.byteLength) return;
    let newSize = this.buffer.byteLength;
    while (newSize < needed) {
      newSize *= 2;
    }
    const newBuffer = new ArrayBuffer(newSize);
    new Uint8Array(newBuffer).set(new Uint8Array(this.buffer));
    this.buffer = newBuffer;
    this.view = new DataView(this.buffer);
  }
  writeInt8(value) {
    this.checkSize(1);
    this.view.setInt8(this.offset, value);
    this.offset += 1;
    return this;
  }
  writeUint8(value) {
    this.checkSize(1);
    this.view.setUint8(this.offset, value);
    this.offset += 1;
    return this;
  }
  writeInt16(value, littleEndian = true) {
    this.checkSize(2);
    this.view.setInt16(this.offset, value, littleEndian);
    this.offset += 2;
    return this;
  }
  writeUint16(value, littleEndian = true) {
    this.checkSize(2);
    this.view.setUint16(this.offset, value, littleEndian);
    this.offset += 2;
    return this;
  }
  writeInt32(value, littleEndian = true) {
    this.checkSize(4);
    this.view.setInt32(this.offset, value, littleEndian);
    this.offset += 4;
    return this;
  }
  writeUint32(value, littleEndian = true) {
    this.checkSize(4);
    this.view.setUint32(this.offset, value, littleEndian);
    this.offset += 4;
    return this;
  }
  writeUint64(value, littleEndian = true) {
    this.checkSize(8);
    this.view.setBigUint64(this.offset, value, littleEndian);
    this.offset += 8;
    return this;
  }
  writeFloat32(value, littleEndian = true) {
    this.checkSize(4);
    this.view.setFloat32(this.offset, value, littleEndian);
    this.offset += 4;
    return this;
  }
  writeFloat64(value, littleEndian = true) {
    this.checkSize(8);
    this.view.setFloat64(this.offset, value, littleEndian);
    this.offset += 8;
    return this;
  }
  static getHex(caracter) {
    switch (caracter) {
      case "0":
        return 0;
      case "1":
        return 1;
      case "2":
        return 2;
      case "3":
        return 3;
      case "4":
        return 4;
      case "5":
        return 5;
      case "6":
        return 6;
      case "7":
        return 7;
      case "8":
        return 8;
      case "9":
        return 9;
      case "a":
        return 10;
      case "b":
        return 11;
      case "c":
        return 12;
      case "d":
        return 13;
      case "e":
        return 14;
      case "f":
        return 15;
      default:
        return 0;
    }
  }
  write256(hex) {
    if (hex === null) {
      this.checkSize(8);
      for (let i = 0; i < 8; i++) {
        this.view.setUint8(this.offset++, 0);
      }
      return;
    }
    if (hex.length !== 16) throw new Error("Hex string must be 16 characters (8 bytes)");
    this.checkSize(8);
    for (let i = 0; i < 16; i += 2) {
      const byte = DataWriter.getHex(hex[i]) << 4 | DataWriter.getHex(hex[i + 1]);
      this.view.setUint8(this.offset++, byte);
    }
  }
  addWriter(writer) {
    const length = writer.getOffset();
    if (length === 0) return;
    this.checkSize(length);
    new Uint8Array(this.buffer, this.offset, length).set(new Uint8Array(writer.toArrayBuffer()));
    this.offset += length;
  }
  addDataView(view) {
    const length = view.byteLength;
    if (length === 0) return;
    this.checkSize(length);
    new Uint8Array(this.buffer, this.offset, length).set(new Uint8Array(view.buffer, view.byteOffset, view.byteLength));
    this.offset += length;
  }
  addArrayBuffer(buffer) {
    const length = buffer.byteLength;
    if (length === 0) return;
    this.checkSize(length);
    new Uint8Array(this.buffer, this.offset, length).set(new Uint8Array(buffer));
    this.offset += length;
  }
  addUint8Array(array) {
    const length = array.length;
    if (length === 0) return;
    this.checkSize(length);
    new Uint8Array(this.buffer, this.offset, length).set(array);
    this.offset += length;
  }
  addUint16Array(array) {
    const length = array.length;
    if (length === 0) return;
    const byteLength = length * 2;
    this.checkSize(byteLength);
    new Uint16Array(this.buffer, this.offset, length).set(array);
    this.offset += byteLength;
  }
  addUint32Array(array) {
    const length = array.length;
    if (length === 0) return;
    const byteLength = length * 4;
    this.checkSize(byteLength);
    new Uint32Array(this.buffer, this.offset, length).set(array);
    this.offset += byteLength;
  }
  toArrayBuffer() {
    return this.buffer.slice(0, this.offset);
  }
  getOffset() {
    return this.offset;
  }
}
var SERVER_IDS = /* @__PURE__ */ ((SERVER_IDS2) => {
  SERVER_IDS2[SERVER_IDS2["CONNECT"] = 0] = "CONNECT";
  SERVER_IDS2[SERVER_IDS2["LISTEN"] = 1] = "LISTEN";
  SERVER_IDS2[SERVER_IDS2["GAME_COMMANDS"] = 2] = "GAME_COMMANDS";
  SERVER_IDS2[SERVER_IDS2["UPDATE"] = 3] = "UPDATE";
  SERVER_IDS2[SERVER_IDS2["PANEL"] = 4] = "PANEL";
  SERVER_IDS2[SERVER_IDS2["GET_CHARACTER"] = 5] = "GET_CHARACTER";
  SERVER_IDS2[SERVER_IDS2["GET_CAR"] = 6] = "GET_CAR";
  SERVER_IDS2[SERVER_IDS2["ERROR"] = 7] = "ERROR";
  return SERVER_IDS2;
})(SERVER_IDS || {});
const _Chunk = class _Chunk {
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
_Chunk.SIZE = 32;
let Chunk = _Chunk;
const worker = new Worker("./dist/worker.js", {
  type: "module"
});
worker.postMessage({
  method: "init",
  requestId: -1,
  wasmPath: window.WASM_PATH
});
const pendings = /* @__PURE__ */ new Map();
let nextRequestId = 0;
worker.onmessage = (event) => {
  const { requestId, result, error } = event.data;
  const pending = pendings.get(requestId);
  if (!pending) return;
  pendings.delete(requestId);
  if (error) pending.reject(error);
  else pending.resolve(result);
};
async function askWorker(method, args, transfered = []) {
  const requestId = nextRequestId++;
  const promise = new Promise((resolve, reject) => {
    pendings.set(requestId, { resolve, reject });
  });
  worker.postMessage({
    requestId,
    method,
    args
  }, transfered);
  return await promise;
}
function postWorker(method, args, transfered = []) {
  worker.postMessage({
    requestId: -1,
    method,
    args
  }, transfered);
}
const queue = [];
let flushTimer = null;
const FLUSH_DELAY = 100;
const MAX_BATCH = 250;
function scheduleFlush() {
  if (flushTimer !== null) return;
  flushTimer = setTimeout(() => {
    flushTimer = null;
    flushQueue();
  }, FLUSH_DELAY);
}
function flushQueue() {
  if (queue.length === 0) return;
  const batchSize = Math.min(queue.length, MAX_BATCH);
  const writer = new DataWriter();
  writer.writeUint8(SERVER_IDS.GAME_COMMANDS);
  writer.writeUint8(batchSize);
  for (let i = 0; i < batchSize; i++) {
    const cmd = queue[i];
    const offset16 = writer.getOffset() % 2;
    if (offset16) {
      writer.skip(1);
    }
    writer.writeUint16(cmd.command);
    const offset32 = writer.getOffset() % 4;
    if (offset32) {
      writer.skip(4 - offset32);
    }
    cmd.fill(writer);
  }
  queue.splice(0, batchSize);
  sendSocket(writer.toArrayBuffer());
  if (queue.length > 0) {
    scheduleFlush();
  }
}
function sendCommand(command, fill) {
  const writer = new DataWriter();
  writer.writeUint16(command);
  writer.skip(2);
  fill(writer);
  const buffer = writer.toArrayBuffer();
  postWorker("performGameCommand", [buffer], [buffer]);
  queue.push({ command, fill });
  if (queue.length >= MAX_BATCH) {
    if (flushTimer !== null) {
      clearTimeout(flushTimer);
      flushTimer = null;
    }
    flushQueue();
    return;
  }
  scheduleFlush();
}
var direction;
((direction2) => {
  function getSide(data, direction3) {
    const dir1 = data >> 10 & 3;
    const dir2 = data >> 12 & 3;
    const side1 = data >> 4 & 7;
    const side2 = data >> 7 & 7;
    if (side1 !== 0 && dir1 === direction3) return side1;
    if (side2 !== 0 && dir2 === direction3) return side2;
    return 0;
  }
  direction2.getSide = getSide;
  function setSide(data, direction3, value) {
    const dir1 = data >> 10 & 3;
    const dir2 = data >> 12 & 3;
    const side1 = data >> 4 & 7;
    const side2 = data >> 7 & 7;
    if (side1 !== 0 && dir1 === direction3) {
      data &= -113;
      data |= (value & 7) << 4;
      return data;
    }
    if (side2 !== 0 && dir2 === direction3) {
      data &= -897;
      data |= (value & 7) << 7;
      return data;
    }
    if (side1 === 0) {
      data &= -113;
      data &= -3073;
      data |= (value & 7) << 4;
      data |= (direction3 & 3) << 10;
      return data;
    }
    if (side2 === 0) {
      data &= -897;
      data &= -12289;
      data |= (value & 7) << 7;
      data |= (direction3 & 3) << 12;
      return data;
    }
    return 0;
  }
  direction2.setSide = setSide;
})(direction || (direction = {}));
function drawInLoop(img, ctx, size, cellWidth, cellHeight) {
  for (let y = 0; y < size.height; y++)
    for (let x = 0; x < size.width; x++)
      ctx.drawImage(img, x, y, 1, 1);
  return false;
}
function drawBuilding(type, ctx, loader, size) {
  switch (type) {
    case 0: {
      ctx.drawImage(loader.get("home"), 0, 0, 2, 2);
      return false;
    }
    case 1: {
      if (!size) {
        return true;
      }
      drawInLoop(
        loader.get("oil"),
        ctx,
        size
      );
      return false;
    }
    case 2: {
      if (!size) {
        return true;
      }
      drawInLoop(
        loader.get("plantation"),
        ctx,
        size
      );
      return true;
    }
    case 3: {
      ctx.drawImage(loader.get("grocery"), 0, 0, 3, 3);
      return false;
    }
    case 4: {
      if (!size) {
        return true;
      }
      drawInLoop(
        loader.get("constructing"),
        ctx,
        size
      );
      return false;
    }
  }
  return false;
}
const DIRECTION_NAMES = ["all", "front", "turn", "turn", "select", "select", "share", "all"];
const DIRECTION_FLIPS = [false, false, false, true, false, true, false];
function drawCell(data, ctx, loader) {
  function drawImage(name, angle, flip = { x: false, y: false, color: -1 }) {
    ctx.save();
    ctx.translate(0.5, 0.5);
    ctx.rotate(-angle);
    ctx.scale(flip.x ? -1 : 1, flip.y ? -1 : 1);
    ctx.imageSmoothingEnabled = false;
    ctx.drawImage(loader.get(name, flip.color), -0.5, -0.5, 1, 1);
    ctx.restore();
  }
  switch (data & 15) {
    case 0:
      break;
    case 1:
      ctx.fillStyle = "#777";
      ctx.fillRect(0, 0, 1, 1);
      break;
    case 2:
      return drawBuilding(data >> 4, ctx, loader, null);
    case 3:
      break;
    case 4:
      ctx.drawImage(loader.get("parking"), 0, 0, 1, 1);
      break;
    case 5: {
      const used = [];
      for (const d of [0, 1, 2, 3]) {
        const s = direction.getSide(data, d);
        if (s !== 0) {
          used.push([s, d]);
        }
      }
      if (used.length === 1) {
        const side = used[0][0];
        const dir = Math.PI / 2 * used[0][1];
        drawImage(DIRECTION_NAMES[side], dir, {
          x: false,
          y: DIRECTION_FLIPS[side],
          color: -1
        });
        break;
      }
      if (used.length === 2) {
        ctx.fillStyle = "#333";
        ctx.fillRect(0, 0, 1, 1);
        break;
      }
      ctx.fillStyle = "#333";
      ctx.fillRect(0, 0, 1, 1);
      break;
    }
  }
  return false;
}
var COMMAND_CODES = /* @__PURE__ */ ((COMMAND_CODES2) => {
  COMMAND_CODES2[COMMAND_CODES2["TEST"] = 0] = "TEST";
  COMMAND_CODES2[COMMAND_CODES2["ERASE"] = 1] = "ERASE";
  COMMAND_CODES2[COMMAND_CODES2["PLACE_SINGLE_ROAD"] = 2] = "PLACE_SINGLE_ROAD";
  COMMAND_CODES2[COMMAND_CODES2["PARKING"] = 3] = "PARKING";
  COMMAND_CODES2[COMMAND_CODES2["DIRECTION"] = 4] = "DIRECTION";
  COMMAND_CODES2[COMMAND_CODES2["PLACE_HOME"] = 5] = "PLACE_HOME";
  COMMAND_CODES2[COMMAND_CODES2["PLACE_GROSSERY"] = 6] = "PLACE_GROSSERY";
  return COMMAND_CODES2;
})(COMMAND_CODES || {});
const NO_PARENT = new Error("Missing parent element");
const htmlParent = document.getElementById("dataPanelList");
class DataPanel {
  constructor() {
    this.realParent = null;
    this.parent = null;
    this.callback = null;
  }
  open() {
    if (this.parent) {
      throw new Error("Parent already defined");
    }
    this.realParent = document.createElement("div");
    this.realParent.classList.add("dataPanel");
    this.realParent.classList.add("loadingDataPanel");
    this.parent = document.createElement("div");
    const loading = document.createElement("div");
    loading.textContent = "Loading...";
    const empty = document.createElement("div");
    const confirm = document.createElement("div");
    confirm.textContent = "Done";
    const cancel = document.createElement("div");
    cancel.textContent = "Cancel";
    confirm.onclick = () => {
      this.confirm();
    };
    cancel.onclick = () => {
      this.cancel();
    };
    this.parent.appendChild(loading);
    this.parent.appendChild(empty);
    this.parent.appendChild(confirm);
    this.parent.appendChild(cancel);
    this.realParent.appendChild(this.parent);
    htmlParent.appendChild(this.realParent);
  }
  append(child, callback) {
    if (!this.parent) {
      throw new Error("Parent already defined");
    }
    this.parent.classList.remove("loadingDataPanel");
    this.parent.children[0].classList.add("hidden");
    this.parent.children[1].appendChild(child);
    this.callback = callback;
  }
  confirm() {
    this.callback(true);
    this.close();
  }
  cancel() {
    this.callback(false);
    this.close();
  }
  close() {
    if (!this.realParent) {
      throw NO_PARENT;
    }
    this.realParent.remove();
    this.realParent = null;
    this.parent = null;
  }
}
class NamedUnit {
  constructor(name, unit) {
    this.name = name;
    this.unit = unit;
  }
}
var units;
((units2) => {
  units2.i32 = {
    read: (reader) => {
      const value = reader.readInt32();
      return value;
    },
    write: (writer, data) => {
    },
    create: (name, data) => {
      const div = document.createElement("div");
      const span = document.createElement("span");
      span.textContent = name;
      const input = document.createElement("span");
      input.textContent = data.toString();
      div.appendChild(span);
      div.appendChild(input);
      return div;
    },
    finish: (element) => {
      return 0;
    }
  };
  units2.f32 = {
    read: (reader) => {
      const value = reader.readFloat32();
      return value;
    },
    write: (writer, data) => {
    },
    create: (name, data) => {
      const div = document.createElement("div");
      const span = document.createElement("span");
      span.textContent = name;
      const input = document.createElement("span");
      input.textContent = data.toString();
      div.appendChild(span);
      div.appendChild(input);
      return div;
    },
    finish: (element) => {
      return 0;
    }
  };
  units2.mut_i32 = {
    read: (reader) => {
      const value = reader.readInt32();
      return value;
    },
    write: (writer, data) => {
      writer.writeInt32(data);
    },
    create: (name, data) => {
      const div = document.createElement("div");
      const span = document.createElement("span");
      span.textContent = name;
      const input = document.createElement("input");
      input.type = "number";
      input.value = data.toString();
      div.appendChild(span);
      div.appendChild(input);
      return div;
    },
    finish: (element) => {
      const input = element.children[1];
      if (!input || input.tagName !== "INPUT") {
        throw new Error("Invalid structure");
      }
      return Math.floor(Number(input.value));
    }
  };
  units2.mut_f32 = {
    read: (reader) => {
      return reader.readFloat32();
    },
    write: (writer, data) => {
      writer.writeFloat32(data);
    },
    create: (name, data) => {
      const div = document.createElement("div");
      const span = document.createElement("span");
      span.textContent = name;
      const input = document.createElement("input");
      input.type = "number";
      input.value = data.toString();
      div.appendChild(span);
      div.appendChild(input);
      return div;
    },
    finish: (element) => {
      const input = element.children[1];
      if (!input || input.tagName !== "INPUT") {
        throw new Error("Invalid structure");
      }
      return Number(input.value);
    }
  };
  units2.button = {
    read: (reader) => {
      return false;
    },
    write: (writer, data) => {
      writer.writeUint32(data ? 1 : 0);
    },
    create: (name, data) => {
      const div = document.createElement("div");
      const btn = document.createElement("button");
      btn.textContent = name;
      div.appendChild(btn);
      btn.onclick = () => {
        btn.classList.add("btn-pressed");
      };
      return div;
    },
    finish: (element) => {
      return element.children[0].classList.contains("btn-pressed");
    }
  };
})(units || (units = {}));
var PANEL_IDS = /* @__PURE__ */ ((PANEL_IDS2) => {
  PANEL_IDS2[PANEL_IDS2["BUILDING_HOME"] = 0] = "BUILDING_HOME";
  PANEL_IDS2[PANEL_IDS2["BUILDING_OIL_FIELD"] = 1] = "BUILDING_OIL_FIELD";
  PANEL_IDS2[PANEL_IDS2["BUILDING_PLANTATION"] = 2] = "BUILDING_PLANTATION";
  PANEL_IDS2[PANEL_IDS2["BUILDING_GROCERY"] = 3] = "BUILDING_GROCERY";
  PANEL_IDS2[PANEL_IDS2["BUILDING_CONSTRUCTION"] = 4] = "BUILDING_CONSTRUCTION";
  PANEL_IDS2[PANEL_IDS2["JOB_OIL_FIELD"] = 5] = "JOB_OIL_FIELD";
  return PANEL_IDS2;
})(PANEL_IDS || {});
const PANEL_MAP = /* @__PURE__ */ new Map();
PANEL_MAP.set(PANEL_IDS.BUILDING_HOME, {
  title: "Home",
  fields: [
    new NamedUnit("Rent", units.mut_i32),
    new NamedUnit("Capacity", units.i32),
    new NamedUnit("Destroy", units.button)
  ]
});
PANEL_MAP.set(PANEL_IDS.BUILDING_OIL_FIELD, {
  title: "Oil field",
  fields: [
    new NamedUnit("Crude", units.f32),
    new NamedUnit("Refined", units.f32),
    new NamedUnit("Entry hour", units.mut_i32),
    new NamedUnit("Leave hour", units.mut_i32),
    new NamedUnit("Salary (per liter)", units.mut_f32),
    new NamedUnit("Sold price (per liter)", units.mut_f32),
    new NamedUnit("Current employees", units.i32),
    new NamedUnit("Goal employees", units.mut_i32)
  ]
});
PANEL_MAP.set(PANEL_IDS.BUILDING_PLANTATION, {
  title: "Plantation",
  fields: [
    new NamedUnit("Test", units.f32)
  ]
});
PANEL_MAP.set(PANEL_IDS.BUILDING_GROCERY, {
  title: "Grocery",
  fields: [
    new NamedUnit("Clients", units.i32),
    new NamedUnit("Cashiers", units.i32),
    new NamedUnit("Stock", units.f32),
    new NamedUnit("Efficiency", units.mut_f32),
    new NamedUnit("Salary (per hour)", units.mut_f32)
  ]
});
PANEL_MAP.set(PANEL_IDS.BUILDING_CONSTRUCTION, {
  title: "Construction",
  fields: [
    new NamedUnit("Type", units.i32),
    new NamedUnit("Completion", units.i32),
    new NamedUnit("Total to reach", units.i32),
    new NamedUnit("Current employees", units.i32),
    new NamedUnit("Goal employees", units.mut_i32)
  ]
});
const resolvers = /* @__PURE__ */ new Map();
function wait(id) {
  return new Promise((resolve) => {
    resolvers.set(id, resolve);
  });
}
function randomUint16() {
  const arr = new Uint16Array(1);
  crypto.getRandomValues(arr);
  return arr[0];
}
function appendArgs(writer, arg) {
  if (arg.building) {
    writer.writeInt32(arg.building.x);
    writer.writeInt32(arg.building.y);
    return;
  }
  if (arg.job) {
    writer.writeInt32(-2147483648);
    writer.writeInt32(arg.job.idx);
    return;
  }
  throw new Error("Missing argument");
}
async function runPanel(args) {
  const panel = new DataPanel();
  panel.open();
  const requestWriter = new DataWriter();
  requestWriter.writeUint8(SERVER_IDS.PANEL);
  requestWriter.writeUint8(0);
  const requestId = randomUint16();
  requestWriter.writeUint16(requestId);
  appendArgs(requestWriter, args);
  sendSocket(requestWriter.toArrayBuffer());
  const reader = await wait(requestId);
  const panelId = reader.readUint32();
  const descriptor = PANEL_MAP.get(panelId);
  const element = document.createElement("div");
  for (let field of descriptor.fields) {
    const value = field.unit.read(reader);
    const e = field.unit.create(field.name, value);
    element.appendChild(e);
  }
  const confirm = await new Promise((resolve) => {
    panel.append(element, (confirm2) => {
      resolve(confirm2);
    });
  });
  if (!confirm) {
    throw new Error("Panel edition cancelled");
  }
  const writer = new DataWriter();
  writer.writeUint8(SERVER_IDS.PANEL);
  writer.writeUint8(1);
  writer.skip(2);
  appendArgs(writer, args);
  for (let i = 0; i < descriptor.fields.length; i++) {
    const u = descriptor.fields[i].unit;
    u.write(
      writer,
      u.finish(element.children[i])
    );
  }
  sendSocket(writer.toArrayBuffer());
}
function resolvePanel(id, reader) {
  const resolve = resolvers.get(id);
  if (!resolve) return;
  resolve(reader);
  resolvers.delete(id);
}
class HandObject {
}
class HandButton extends HandObject {
  constructor(icons, enable, disable, mouseUp, mouseDown = () => {
  }, mouseMove = () => {
  }) {
    super();
    this.icons = icons;
    this._enable = enable;
    this._disable = disable;
    this._mouseUp = mouseUp;
    this._mouseDown = mouseDown;
    this._mouseMove = mouseMove;
  }
  getIcons() {
    return this.icons;
  }
  enable(play) {
    return this._enable(play);
  }
  diseable(play) {
    this._disable(play);
  }
  mouseUp(x, y, btn, play) {
    this._mouseUp(x, y, btn, play);
  }
  mouseDown(x, y, btn, play) {
    this._mouseDown(x, y, btn, play);
  }
  mouseMove(prevX, prevY, x, y, btn, play) {
    this._mouseMove(prevX, prevY, x, y, btn, play);
  }
}
const DIRECTIONS = [
  { value: 0, label: "RIGHT" },
  { value: 1, label: "UP" },
  { value: 2, label: "LEFT" },
  { value: 3, label: "DOWN" }
];
const SUB_OPTIONS = [
  { key: "front", label: "Front" },
  { key: "right", label: "Right" },
  { key: "left", label: "Left" }
];
function bitsToValue(front, right, left) {
  if (front && right && left) return 7;
  if (right && left) return 6;
  if (front && left) return 5;
  if (front && right) return 4;
  if (left) return 3;
  if (right) return 2;
  if (front) return 1;
  return 0;
}
function valueToBits(v) {
  return {
    front: v === 1 || v === 4 || v === 5 || v === 7,
    right: v === 2 || v === 4 || v === 6 || v === 7,
    left: v === 3 || v === 5 || v === 6 || v === 7
  };
}
function findDirections(data) {
  const used = [];
  for (const d of [0, 1, 2, 3]) {
    if (direction.getSide(data, d) !== 0) used.push(d);
  }
  const dir0 = used[0] ?? 0;
  const dir1 = used[1] ?? [0, 1, 2, 3].find((d) => d !== dir0);
  return [dir0, dir1];
}
class TurnSelector {
  constructor(parent) {
    this.callback = null;
    this.currentData = 0;
    this.parent = parent;
    this.parent.querySelector("#ts-ok").addEventListener("click", () => this.confirm());
    this.parent.querySelector("#ts-cancel").addEventListener("click", () => this.cancel());
  }
  // Open the selector with the given data, call callback with updated data or null on cancel
  take(data, callback) {
    this.currentData = data;
    this.callback = callback;
    this.parent.classList.remove("hidden");
    this.buildUI(data);
  }
  // Build both rows from the current data
  buildUI(data) {
    const [dir0, dir1] = findDirections(data);
    this.buildRow(0, dir0, dir1, data);
    this.buildRow(1, dir1, dir0, data);
  }
  // Build a single direction row (select + checkboxes)
  buildRow(rowIndex, direction$1, otherDirection, data) {
    const row = this.parent.querySelector(`#ts-row-${rowIndex}`);
    row.innerHTML = "";
    const lbl = document.createElement("label");
    lbl.textContent = rowIndex === 0 ? "1st direction" : "2nd direction";
    row.appendChild(lbl);
    const select = document.createElement("select");
    for (const d of DIRECTIONS) {
      if (d.value === otherDirection) continue;
      const opt = document.createElement("option");
      opt.value = String(d.value);
      opt.textContent = d.label;
      opt.selected = d.value === direction$1;
      select.appendChild(opt);
    }
    row.appendChild(select);
    const checkDiv = document.createElement("div");
    checkDiv.className = "ts-checkboxes";
    const currentValue = direction.getSide(data, direction$1);
    const bits = valueToBits(currentValue);
    for (const sub of SUB_OPTIONS) {
      const lbl2 = document.createElement("label");
      const cb = document.createElement("input");
      cb.type = "checkbox";
      cb.dataset.key = sub.key;
      cb.checked = bits[sub.key];
      lbl2.appendChild(cb);
      lbl2.append(" " + sub.label);
      checkDiv.appendChild(lbl2);
    }
    row.appendChild(checkDiv);
    select.addEventListener("change", () => this.onDirectionChange(rowIndex));
  }
  // Handle direction change: resolve conflicts and rebuild both rows
  onDirectionChange(changedRow) {
    const otherRow = changedRow === 0 ? 1 : 0;
    const changedSelect = this.parent.querySelector(
      `#ts-row-${changedRow} select`
    );
    const otherSelect = this.parent.querySelector(
      `#ts-row-${otherRow} select`
    );
    const newDir = parseInt(changedSelect.value);
    const currentOther = parseInt(otherSelect.value);
    if (newDir === currentOther) {
      const fallback = [0, 1, 2, 3].find((d) => d !== newDir);
      this.buildRow(changedRow, newDir, fallback, this.currentData);
      this.buildRow(otherRow, fallback, newDir, this.currentData);
    } else {
      this.buildRow(changedRow, newDir, currentOther, this.currentData);
      this.buildRow(otherRow, currentOther, newDir, this.currentData);
    }
  }
  // Read the current state of both rows
  readRows() {
    return [0, 1].map((rowIndex) => {
      const select = this.parent.querySelector(
        `#ts-row-${rowIndex} select`
      );
      const checkDiv = this.parent.querySelector(
        `#ts-row-${rowIndex} .ts-checkboxes`
      );
      const dir = parseInt(select.value);
      let front = false, right = false, left = false;
      checkDiv.querySelectorAll("input[type=checkbox]").forEach((cb) => {
        if (cb.dataset.key === "front") front = cb.checked;
        if (cb.dataset.key === "right") right = cb.checked;
        if (cb.dataset.key === "left") left = cb.checked;
      });
      return { dir, value: bitsToValue(front, right, left) };
    });
  }
  // Apply the selected values to data and call the callback
  confirm() {
    const rows = this.readRows();
    let data = this.currentData;
    for (const d of [0, 1, 2, 3]) {
      if (d !== rows[0].dir && d !== rows[1].dir) {
        const result = direction.setSide(data, d, 0);
        if (result !== 0) data = result;
      }
    }
    for (const { dir, value } of rows) {
      const result = direction.setSide(data, dir, value);
      if (result !== 0) data = result;
    }
    this.close(data);
  }
  cancel() {
    this.close(null);
  }
  close(result) {
    this.parent.classList.add("hidden");
    if (this.callback) {
      this.callback(result);
      this.callback = null;
    }
  }
}
const turnSelector = new TurnSelector(
  document.getElementById("turnSelector")
);
function applyDefaultRightClick(x, y, current, play) {
  const cellType = current & 15;
  console.log(cellType);
  if (cellType === 5) {
    turnSelector.take(current, (next) => {
      if (next === null)
        return;
      sendCommand(COMMAND_CODES.DIRECTION, (writer) => {
        writer.writeInt32(x);
        writer.writeInt32(y);
        writer.writeUint16(next);
      });
    });
    return;
  }
  if (cellType === 2 || cellType === 3) {
    setTimeout(() => runPanel({ building: { x, y } }));
    return;
  }
}
function erase(x, y, btn, play, edit) {
  x = Math.floor(x);
  y = Math.floor(y);
  const current = play.getCell(x, y);
  if (btn === HandPanel.LEFT_BTN) {
    if (current !== null && (current & 15) != 0) {
      sendCommand(COMMAND_CODES.ERASE, (writer) => {
        writer.writeInt32(x);
        writer.writeInt32(y);
      });
    }
    return;
  }
  if (btn === HandPanel.RIGHT_BTN && edit && current !== null) {
    applyDefaultRightClick(x, y, current);
    return;
  }
}
function placeRoad(x, y, btn, play, edit) {
  x = Math.floor(x);
  y = Math.floor(y);
  const current = play.getCell(x, y);
  if (btn === HandPanel.LEFT_BTN) {
    if (current !== null && (current & 15) != 1) {
      sendCommand(COMMAND_CODES.PLACE_SINGLE_ROAD, (writer) => {
        writer.writeInt32(x);
        writer.writeInt32(y);
      });
    }
    return;
  }
  if (btn === HandPanel.RIGHT_BTN && edit && current !== null) {
    applyDefaultRightClick(x, y, current);
    return;
  }
}
function placeParking(x, y, btn, play, edit) {
  x = Math.floor(x);
  y = Math.floor(y);
  const current = play.getCell(x, y);
  if (btn === HandPanel.LEFT_BTN) {
    if (current === null || (current & 15) === 4)
      return;
    sendCommand(COMMAND_CODES.PARKING, (writer) => {
      writer.writeInt32(x);
      writer.writeInt32(y);
    });
    return;
  }
  if (btn === HandPanel.RIGHT_BTN && edit && current !== null) {
    applyDefaultRightClick(x, y, current);
    return;
  }
}
function applyTurn(x, y, btn, play, edit) {
  x = Math.floor(x);
  y = Math.floor(y);
  const current = play.getCell(x, y);
  if (btn === HandPanel.LEFT_BTN) {
    sendCommand(COMMAND_CODES.DIRECTION, (writer) => {
      writer.writeInt32(x);
      writer.writeInt32(y);
      writer.writeUint16(0);
    });
    return;
  }
  if (btn === HandPanel.RIGHT_BTN && edit && current !== null) {
    applyDefaultRightClick(x, y, current);
    return;
  }
}
function ask(play, label, text) {
  const r = prompt(text);
  if (r === null)
    play.handData = null;
  if (play.handData) {
    play.handData[label] = r;
  }
}
const placeHome = new HandButton(
  {
    list: { home: "assets/home.png" },
    first: "home"
  },
  // enable
  (play) => {
    play.handData = {};
    ask(play, "money", "Total money given to employees (to motivate citizen to get this job)");
    ask(
      play,
      "capacity",
      "Capacity (increase construction time)"
    );
    ask(play, "rent", "Rent");
    return "placeHome";
  },
  // diseable
  (play) => {
    play.handData = null;
  },
  // mouseUp
  (x, y, btn, play) => {
  },
  // mouseDown
  (x, y, btn, play) => {
    x = Math.floor(x);
    y = Math.floor(y);
    const current = play.getCell(x, y);
    if (btn === HandPanel.RIGHT_BTN && current !== null) {
      applyDefaultRightClick(x, y, current);
    }
    const data = play.handData;
    if (data === null)
      return;
    sendCommand(COMMAND_CODES.PLACE_HOME, (writer) => {
      writer.writeUint32(x);
      writer.writeUint32(y);
      writer.writeInt32(data.money);
      writer.writeInt32(data.capacity);
      writer.writeInt32(data.rent);
    });
  },
  // mouseMove
  (prevX, prevY, x, y, btn, play) => {
  }
);
const handlist = {
  erase: new HandButton(
    {
      list: { erase: "assets/icons/erase.png" },
      first: "erase"
    },
    // enable
    () => null,
    // diseable
    () => {
    },
    // mouseUp
    (x, y, btn, play) => {
      erase(x, y, btn, play, false);
    },
    // mouseDown
    (x, y, btn, play) => {
      erase(x, y, btn, play, true);
    },
    // mouseMove
    (prevX, prevY, x, y, btn, play) => {
      erase(x, y, btn, play, false);
    }
  ),
  road: new HandButton(
    {
      list: { road: "assets/icons/road.png" },
      first: "road"
    },
    // enable
    () => "road",
    // diseable
    () => {
    },
    // mouseUp
    (x, y, btn, play) => {
      placeRoad(x, y, btn, play, false);
    },
    // mouseDown
    (x, y, btn, play) => {
      placeRoad(x, y, btn, play, true);
    },
    // mouseMove
    (prevX, prevY, x, y, btn, play) => {
      placeRoad(x, y, btn, play, false);
    }
  ),
  parking: new HandButton(
    {
      list: { parking: "assets/icons/parking.png" },
      first: "parking"
    },
    // enable
    () => "parking",
    // diseable
    () => {
    },
    // mouseUp
    (x, y, btn, play) => {
      placeParking(x, y, btn, play, false);
    },
    // mouseDown
    (x, y, btn, play) => {
      placeParking(x, y, btn, play, true);
    },
    // mouseMove
    (prevX, prevY, x, y, btn, play) => {
      placeParking(x, y, btn, play, false);
    }
  ),
  turn: new HandButton(
    {
      list: { rotate: "assets/icons/rotate.png" },
      first: "rotate"
    },
    // enable
    () => "turn",
    // diseable
    () => {
    },
    // mouseUp
    (x, y, btn, play) => {
      applyTurn(x, y, btn, play, false);
    },
    // mouseDown
    (x, y, btn, play) => {
      applyTurn(x, y, btn, play, true);
    },
    // mouseMove
    (prevX, prevY, x, y, btn, play) => {
      applyTurn(x, y, btn, play, false);
    }
  ),
  placeHome
};
function setElementAsBackground(element, div) {
  if (element instanceof HTMLCanvasElement) {
    element.toBlob((blob) => {
      if (!blob) return;
      const url = URL.createObjectURL(blob);
      div.style.backgroundImage = `url(${url})`;
    });
  } else {
    div.style.backgroundImage = `url(${element.src})`;
  }
}
const _HandPanel = class _HandPanel {
  constructor(div) {
    this.initialized = 0;
    this.selected = 0;
    this.list = [];
    this.div = div;
  }
  isInitialized() {
    return this.initialized >= 1;
  }
  async init(loader) {
    if (this.initialized >= 1) {
      throw new Error("HandPanel already initialized");
    }
    this.initialized = 1;
    const pushButton = (btn) => {
      const parentDiv = document.createElement("div");
      const div = document.createElement("div");
      const idx = this.list.length;
      this.list.push(btn);
      parentDiv.onclick = () => {
        this.select(idx, loader);
      };
      parentDiv.appendChild(div);
      this.div.appendChild(parentDiv);
      const icons = btn.getIcons();
      loader.load(icons.list).then(() => {
        setElementAsBackground(loader.get(icons.first), div);
      });
    };
    pushButton(handlist.erase);
    pushButton(handlist.road);
    pushButton(handlist.parking);
    pushButton(handlist.turn);
    pushButton(handlist.placeHome);
    this.select(0, loader);
    this.initialized = 2;
  }
  cleanup() {
    if (this.initialized <= 1) {
      throw new Error("HandPanel not initialized");
    }
    this.div.innerHTML = "";
  }
  select(idx, loader) {
    this.div.children[this.selected].classList.remove("selected");
    this.div.children[idx].classList.add("selected");
    const play = getGameHandler().getState();
    if (!(play instanceof PlayState))
      throw new TypeError("PlayState type was expected");
    this.list[this.selected].diseable(play);
    const icon = this.list[idx].enable(play);
    this.selected = idx;
    if (loader && icon) {
      setElementAsBackground(
        loader.get(icon),
        this.div.children[idx].children[0]
      );
    }
  }
  getButton() {
    return this.list[this.selected];
  }
};
_HandPanel.LEFT_BTN = 0;
_HandPanel.RIGHT_BTN = 1;
_HandPanel.MIDDLE_BTN = 2;
let HandPanel = _HandPanel;
class MouseHandler {
  constructor(playState) {
    this.lastScreenX = NaN;
    this.lastScreenY = NaN;
    this.x = NaN;
    this.y = NaN;
    this.playState = playState;
  }
  onMouseUp(e) {
  }
  onMouseDown(e) {
    const leftDown = (e.buttons & 1) !== 0;
    const rightDown = (e.buttons & 2) !== 0;
    (e.buttons & 4) !== 0;
    const { x, y } = this.getMousePosition(e.clientX, e.clientY);
    const action = this.playState.handPanel.getButton();
    if (leftDown) {
      action.mouseDown(x, y, HandPanel.LEFT_BTN, this.playState);
    }
    if (rightDown) {
      action.mouseDown(x, y, HandPanel.RIGHT_BTN, this.playState);
    }
    this.x = x;
    this.y = y;
    this.lastScreenX = e.clientX;
    this.lastScreenY = e.clientY;
  }
  onMouseMove(e) {
    const leftDown = (e.buttons & 1) !== 0;
    const rightDown = (e.buttons & 2) !== 0;
    const middleDown = (e.buttons & 4) !== 0;
    let pos = this.getMousePosition(e.clientX, e.clientY);
    if (middleDown && isFinite(this.x) && isFinite(this.y)) {
      const dx = pos.x - this.x;
      const dy = pos.y - this.y;
      const camera = this.playState.getCamera();
      this.playState.updateCamera(
        camera.x - dx,
        camera.y - dy,
        camera.z
      );
      pos = this.getMousePosition(e.clientX, e.clientY);
    }
    const action = this.playState.handPanel.getButton();
    const prev = this.getMousePosition(this.lastScreenX, this.lastScreenY);
    if (leftDown) {
      action.mouseMove(
        prev.x,
        prev.y,
        pos.x,
        pos.y,
        HandPanel.LEFT_BTN,
        this.playState
      );
    }
    if (rightDown) {
      action.mouseMove(
        prev.x,
        prev.y,
        pos.x,
        pos.y,
        HandPanel.RIGHT_BTN,
        this.playState
      );
    }
    this.lastScreenX = e.clientX;
    this.lastScreenY = e.clientY;
    this.x = pos.x;
    this.y = pos.y;
  }
  onScroll(e) {
    const camera = this.playState.getCamera();
    this.playState.updateCamera(camera.x, camera.y, camera.z * (1 - e.deltaY / 1e3));
  }
  onTouchStart(e) {
  }
  onTouchEnd(e) {
  }
  onTouchMove(e) {
  }
  getMouse() {
    if (isNaN(this.x))
      return null;
    return { x: this.x, y: this.y };
  }
  getMousePosition(mouseX, mouseY) {
    const camera = this.playState.getCamera();
    const scaleX = innerWidth / GAME_WIDTH;
    const scaleY = innerHeight / GAME_HEIGHT;
    const scale = Math.min(scaleX, scaleY);
    const offsetX = (innerWidth - GAME_WIDTH * scale) / 2;
    const offsetY = (innerHeight - GAME_HEIGHT * scale) / 2;
    let x = mouseX - offsetX;
    let y = mouseY - offsetY;
    x /= scale;
    y /= scale;
    x -= GAME_WIDTH / 2;
    y -= GAME_HEIGHT / 2;
    x /= camera.z;
    y /= camera.z;
    x += camera.x;
    y += camera.y;
    return { x, y };
  }
}
const CHARACTER_SIZE = { x: 1, y: 1 };
function drawCar(car, ctx) {
  ctx.fillStyle = "rgb(144, 238, 144)";
  let x = 0.5;
  let y = 0.5;
  let w = 0;
  let h = 0;
  switch (car.direction) {
    case 0:
      x = car.step;
      y = 0.5;
      w = 0.9;
      h = 0.6;
      break;
    case 1:
      x = 0.5;
      y = 1 - car.step;
      w = 0.6;
      h = 0.9;
      break;
    case 2:
      x = 1 - car.step;
      y = 0.5;
      w = 0.9;
      h = 0.6;
      break;
    case 3:
      x = 0.5;
      y = car.step;
      w = 0.6;
      h = 0.9;
      break;
  }
  ctx.fillRect(x, y, w, h);
}
function drawCharacter(c, ctx) {
  ctx.fillStyle = "rgb(144, 127, 255)";
  ctx.fillRect(0, 0, 1, 1);
}
function loadAssets(loader) {
  loader.load({
    turn: "assets/turn/turn.png",
    all: "assets/turn/all.png",
    select: "assets/turn/select.png",
    share: "assets/turn/share.png",
    home: "assets/home.png",
    constructing: "assets/constructing.png",
    grocery: "assets/grocery.png",
    parking: "assets/parking.png",
    oil: "assets/oil.png",
    plantation: "assets/plantation.png"
  });
}
function evalCalendar(n) {
  const weekdays = [
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday",
    "Sunday"
  ];
  const months = [
    "January",
    "February",
    "March",
    "April",
    "May",
    "June",
    "July",
    "August",
    "September",
    "October",
    "November",
    "December"
  ];
  const MONTH_DAYS = [31n, 28n, 31n, 30n, 31n, 30n, 31n, 31n, 30n, 31n, 30n, 31n];
  let minute = Number(n % 60n);
  let totalHours = n / 60n;
  let hour = Number(totalHours % 24n);
  let totalDays = totalHours / 24n;
  let weekDay = Number(totalDays % 7n);
  const DAYS_400_YEARS = 146097n;
  let cycle = totalDays / DAYS_400_YEARS;
  let year = cycle * 400n;
  totalDays -= cycle * DAYS_400_YEARS;
  for (let i = 0; i < 400; i++) {
    let isLeap = year % 400n === 0n || year % 4n === 0n && year % 100n !== 0n;
    let daysInYear = isLeap ? 366n : 365n;
    if (totalDays >= daysInYear) {
      totalDays -= daysInYear;
      year++;
    } else {
      break;
    }
  }
  let isLeapYear = year % 400n === 0n || year % 4n === 0n && year % 100n !== 0n;
  let month = 0;
  for (let i = 0; i < 12; i++) {
    let daysInMonth = MONTH_DAYS[i];
    if (i === 1 && isLeapYear) {
      daysInMonth = 29n;
    }
    if (totalDays >= daysInMonth) {
      totalDays -= daysInMonth;
      month++;
    } else {
      break;
    }
  }
  let day = Number(totalDays + 1n);
  let hourStr = (hour < 10 ? "0" + hour : "" + hour) + ":" + (minute < 10 ? "0" + minute : "" + minute);
  let suffix = "th";
  if (day % 10 === 1 && day % 100 !== 11) suffix = "st";
  else if (day % 10 === 2 && day % 100 !== 12) suffix = "nd";
  else if (day % 10 === 3 && day % 100 !== 13) suffix = "rd";
  return {
    day: weekdays[weekDay] + ", " + day + suffix + " " + months[month],
    year: "Year " + year.toString().padStart(3, "0"),
    hour: hourStr
  };
}
const natures = [
  "character",
  "car"
];
const statusList = [
  "idle",
  "home",
  "work",
  "fuel",
  "eat"
];
function print(list, ctx, startX, startY) {
  const lineHeight = 1;
  const padding = 0.5;
  const boxWidth = 10;
  const boxHeight = list.length * lineHeight + padding * 2;
  const topY = startY - boxHeight;
  ctx.fillStyle = "rgba(0, 0, 0, 0.8)";
  ctx.fillRect(startX, topY, boxWidth, boxHeight);
  ctx.strokeStyle = "#ffffff";
  ctx.lineWidth = 0.1;
  ctx.strokeRect(startX, topY, boxWidth, boxHeight);
  ctx.font = 0.7 * lineHeight + "px monospace";
  ctx.textAlign = "left";
  ctx.textBaseline = "top";
  list.forEach((item, index) => {
    let displayValue = item.value;
    if (item.type === "float" && typeof item.value === "number") {
      displayValue = item.value.toFixed(3);
    } else if (item.type === "int") {
      displayValue = Math.floor(item.value).toString();
    }
    const text = `${item.label}: ${displayValue}`;
    const textY = topY + padding + index * lineHeight;
    ctx.fillStyle = "white";
    ctx.fillText(text, startX + padding, textY);
  });
}
class EntityDataHandler {
  constructor(playState) {
    this.current = null;
    this.bubble = null;
    this.waiting = [];
    this.playState = playState;
  }
  send(ptr, nature) {
    this.waiting.push({ ptr, nature });
    const writer = new DataWriter();
    switch (nature) {
      case "character":
        writer.writeUint8(SERVER_IDS.GET_CHARACTER);
        break;
      case "car":
        writer.writeUint8(SERVER_IDS.GET_CHARACTER);
        break;
    }
    writer.skip(7);
    writer.writeUint64(ptr);
    sendSocket(writer.toArrayBuffer());
  }
  recv(reader) {
    const isValid = reader.readUint8();
    const nature = natures[reader.readUint16()];
    const ptr = reader.readUint64();
    for (let i = 0; i < this.waiting.length; i++) {
      if (this.waiting[i].nature !== nature || this.waiting[i].ptr !== ptr)
        continue;
      this.waiting.splice(i, 1);
      break;
    }
    if (!isValid) {
      return;
    }
    if (this.current && ptr === this.current.ptr) {
      const c = this.playState.characters.find(
        (c2) => c2.ptr === ptr
      );
      if (c) {
        this.createBubble(c, reader, "character");
      }
    }
    for (let i = 0; i < this.waiting.length; i++) {
    }
  }
  ask(ptr, nature) {
    if (this.current && this.current.ptr === ptr && this.current.nature === nature) {
      return;
    }
    let send = true;
    for (const i of this.waiting) {
      if (i.ptr === ptr && i.nature === nature) {
        send = false;
        break;
      }
    }
    if (send) {
      this.send(ptr, nature);
    }
    this.current = { ptr, nature };
  }
  createBubble(character, reader, nature) {
    switch (nature) {
      case "character": {
        const action = statusList[reader.readUint32()];
        const money = reader.readUint32();
        const food = reader.readFloat32();
        this.bubble = {
          type: "character",
          x: character.x - 1,
          y: character.y - 1,
          action,
          money,
          food
        };
        break;
      }
    }
  }
  drawBubble(ctx) {
    if (!this.bubble)
      return;
    switch (this.bubble.type) {
      case "character":
        print([
          { label: "x", type: "float", value: this.bubble.x },
          { label: "y", type: "float", value: this.bubble.y },
          { label: "action", type: "string", value: this.bubble.action },
          { label: "money", type: "int", value: this.bubble.money },
          { label: "food", type: "float", value: this.bubble.food }
        ], ctx, this.bubble.x, this.bubble.y);
        break;
    }
  }
  unselect() {
    this.current = null;
    this.bubble = null;
  }
}
function modulo(a, n) {
  return (a % n + n) % n;
}
const html_day = document.getElementById("gameDay");
const html_year = document.getElementById("gameYear");
const html_hour = document.getElementById("gameHour");
const html_money = document.getElementById("gameMoney");
function searchChunk(chunks, x, y) {
  let left = 0;
  let right = chunks.length - 1;
  while (left <= right) {
    const mid = left + right >> 1;
    const c = chunks[mid];
    if (c.x === x && c.y === y) {
      return c;
    }
    if (c.x < x || c.x === x && c.y < y) {
      left = mid + 1;
    } else {
      right = mid - 1;
    }
  }
  return null;
}
function getBuildingSize(chunks, chunk, dx, dy) {
  let width = 1;
  let height = 1;
  const base = dy * Chunk.SIZE;
  {
    let ptr = base + dx;
    let pos = dx;
    let cx = chunk.x;
    let cy = chunk.y;
    while (true) {
      ptr++;
      pos++;
      if (pos >= Chunk.SIZE) {
        pos -= Chunk.SIZE;
        cx++;
        const next = searchChunk(chunks, cx, cy);
        if (!next)
          break;
        chunk = next;
        ptr = base + pos;
      }
      const cell = chunk.cells[ptr];
      if ((cell & 15) !== 3)
        break;
      if (cell >> 8 & 15 || cell & 1 << 13)
        break;
      width++;
    }
  }
  {
    let ptr = base + dx;
    let pos = dy;
    let cx = chunk.x;
    let cy = chunk.y;
    while (true) {
      ptr += Chunk.SIZE;
      pos++;
      if (pos >= Chunk.SIZE) {
        pos -= Chunk.SIZE;
        cy++;
        const next = searchChunk(chunks, cx, cy);
        if (!next)
          break;
        chunk = next;
        ptr = pos * Chunk.SIZE + dx;
      }
      const cell = chunk.cells[ptr];
      if ((cell & 15) !== 3)
        break;
      if (cell >> 4 & 15 || cell & 1 << 12)
        break;
      height++;
    }
  }
  return { width, height };
}
class PlayState extends GameState {
  constructor() {
    super();
    this.camX = 0;
    this.camY = 0;
    this.camZ = 50;
    this.frameCount = 0;
    this.cameraUpdates = 0;
    this.mouseHandler = new MouseHandler(this);
    this.viewBox_x = 0;
    this.viewBox_y = 0;
    this.viewBox_w = 1;
    this.viewBox_h = 1;
    this.chunks = [];
    this.entityData = new EntityDataHandler(this);
    this.jobs = [];
    this.cars = [];
    this.characters = [];
    this.handData = null;
    const handPanelDiv = document.getElementById("handPanel");
    this.handPanel = new HandPanel(handPanelDiv);
  }
  enter(data, input, imageLoader) {
    document.getElementById("gameView")?.classList.remove("hidden");
    input.onMouseUp = (e) => this.mouseHandler.onMouseUp(e);
    input.onMouseDown = (e) => this.mouseHandler.onMouseDown(e);
    input.onMouseMove = (e) => this.mouseHandler.onMouseMove(e);
    input.onScroll = (e) => this.mouseHandler.onScroll(e);
    input.onTouchStart = (e) => this.mouseHandler.onTouchStart(e);
    input.onTouchEnd = (e) => this.mouseHandler.onTouchEnd(e);
    input.onTouchMove = (e) => this.mouseHandler.onTouchMove(e);
    this.updateCamera(this.camX, this.camY, this.camZ);
    this.sendAskEntities();
    this.updateCamera(0, 0, 50);
    this.handPanel.init(imageLoader);
    loadAssets(imageLoader);
    window.playState = this;
    this.updateCamera(10, 10, 50);
  }
  test() {
    sendCommand(COMMAND_CODES.TEST, (writer) => {
      writer.writeInt32(18);
      writer.writeInt32(32);
      writer.writeInt32(44);
      writer.writeInt32(-1098);
    });
  }
  handleInputs(input) {
  }
  frame(game) {
    this.handleInputs(game.inputHandler);
    const mouse = this.mouseHandler.getMouse();
    if (mouse) {
      this.selectEntityDataPoint(mouse.x, mouse.y);
    }
    this.frameCount++;
    return null;
  }
  setCalendar(n) {
    const obj = evalCalendar(n);
    html_day.textContent = obj.day;
    html_year.textContent = obj.year;
    html_hour.textContent = obj.hour;
  }
  setMoney(money) {
    html_money.textContent = money + "$";
  }
  async drawGrid(ctx, loader) {
    const rangeW = GAME_WIDTH / this.camZ;
    const rangeH = GAME_HEIGHT / this.camZ;
    const chunks = await askWorker("getChunks", [
      Math.floor(this.camX - rangeW / 2),
      Math.floor(this.camY - rangeH / 2),
      Math.floor(rangeW),
      Math.floor(rangeH)
    ]);
    this.chunks = chunks;
    chunks.sort((a, b) => {
      if (a.x !== b.x) return a.x - b.x;
      return a.y - b.y;
    });
    for (const chunk of chunks) {
      const { x, y, cells } = chunk;
      let j = 0;
      for (let dy = 0; dy < Chunk.SIZE; dy++) {
        for (let dx = 0; dx < Chunk.SIZE; dx++) {
          const cell = cells[j++];
          if (cell === 0)
            continue;
          ctx.save();
          ctx.translate(x + dx, y + dy);
          if (drawCell(cell, ctx, loader)) {
            const size = getBuildingSize(
              chunks,
              chunk,
              dx,
              dy
            );
            drawBuilding(cell >> 4, ctx, loader, size);
          }
          ctx.restore();
        }
      }
    }
  }
  drawCars(ctx) {
    for (const car of this.cars) {
      ctx.save();
      ctx.translate(car.x, car.y);
      drawCar(car, ctx);
      ctx.restore();
    }
  }
  drawCharacters(ctx) {
    for (const character of this.characters) {
      ctx.save();
      ctx.translate(
        character.x - CHARACTER_SIZE.x / 2,
        character.y - CHARACTER_SIZE.y / 2
      );
      drawCharacter(character, ctx);
      ctx.restore();
    }
  }
  async draw(args) {
    const ctx = args.ctx;
    {
      ctx.fillStyle = "#D9E0E6";
      ctx.fillRect(0, 0, GAME_WIDTH, GAME_HEIGHT);
    }
    await askWorker("updateCells", [
      this.viewBox_x,
      this.viewBox_y,
      this.viewBox_w,
      this.viewBox_h
    ]);
    args.followCamera();
    ctx.imageSmoothingEnabled = false;
    await this.drawGrid(ctx, args.imageLoader);
    this.drawCharacters(ctx);
    this.drawCars(ctx);
    this.entityData.drawBubble(ctx);
    args.unfollowCamera();
  }
  exit() {
    if (this.handPanel.isInitialized()) {
      this.handPanel.cleanup();
    }
    document.getElementById("gameView")?.classList.add("hidden");
    window.playState = null;
    askWorker("shutdown", []);
  }
  sendCameraUpdate() {
    const writer = new DataWriter();
    writer.writeUint8(SERVER_IDS.LISTEN);
    writer.skip(3);
    writer.writeInt32(this.viewBox_x);
    writer.writeInt32(this.viewBox_y);
    writer.writeInt32(this.viewBox_w);
    writer.writeInt32(this.viewBox_h);
    sendSocket(writer.toArrayBuffer());
  }
  updateCamera(x, y, z) {
    this.camX = x;
    this.camY = y;
    this.camZ = z;
    const EXPAND = 1.1;
    const width = EXPAND * GAME_WIDTH / this.camZ;
    const height = EXPAND * GAME_HEIGHT / this.camZ;
    this.viewBox_x = Math.floor(this.camX - width / 2);
    this.viewBox_y = Math.floor(this.camY - height / 2);
    this.viewBox_w = Math.floor(width);
    this.viewBox_h = Math.floor(height);
    if (this.cameraUpdates === 0) {
      this.sendCameraUpdate();
    }
    this.cameraUpdates++;
  }
  resetCameraUpdates() {
    if (this.cameraUpdates === 1) {
      this.cameraUpdates = 0;
      return;
    }
    this.sendCameraUpdate();
    this.cameraUpdates = 1;
  }
  getCamera() {
    return { x: this.camX, y: this.camY, z: this.camZ };
  }
  getCell(x, y) {
    const cx = Math.floor(x / Chunk.SIZE);
    const cy = Math.floor(y / Chunk.SIZE);
    for (const chunk of this.chunks) {
      if (chunk.x === cx * Chunk.SIZE && chunk.y === cy * Chunk.SIZE) {
        const cellX = modulo(x, Chunk.SIZE);
        const cellY = modulo(y, Chunk.SIZE);
        return chunk.cells[cellY * Chunk.SIZE + cellX];
      }
    }
    return null;
  }
  sendAskEntities() {
    const writer = new DataWriter();
    writer.writeUint8(SERVER_IDS.UPDATE);
    sendSocket(writer.toArrayBuffer());
  }
  setJobs(jobs) {
    this.jobs = jobs;
  }
  selectEntityDataPoint(x, y) {
    for (let i = 0; i < this.characters.length; i++) {
      const c = this.characters[i];
      const left = c.x - CHARACTER_SIZE.x / 2;
      const right = c.x + CHARACTER_SIZE.x / 2;
      const top = c.y - CHARACTER_SIZE.y / 2;
      const bottom = c.y + CHARACTER_SIZE.y / 2;
      if (left <= x && x <= right && top <= y && y <= bottom) {
        this.entityData.ask(c.ptr, "character");
        return;
      }
    }
    this.entityData.unselect();
  }
}
const ENTITY_ASK_COULDOWN = 20;
let REGION_SIZE = 1;
function net_joinCreated(reader) {
  reader.skip(3);
  REGION_SIZE = reader.readUint32();
  reader.skip(4);
  const sessionHash = reader.read256();
  const playerHash = reader.read256();
  console.log("Session hash:", sessionHash);
  console.log("Player  hash:", playerHash);
  getGameHandler().setState(new PlayState());
  return null;
}
function net_joinAlive(reader) {
  reader.skip(3);
  REGION_SIZE = reader.readUint32();
  reader.skip(4);
  const sessionHash = reader.read256();
  const playerHash = reader.read256();
  console.log("Session hash:", sessionHash);
  console.log("Player  hash:", playerHash);
  getGameHandler().setState(new PlayState());
  return null;
}
function net_areas(reader) {
  reader.skip(3);
  const areasCount = reader.readUint32();
  for (let count = 0; count < areasCount; count++) {
    const x0 = reader.readInt32() * REGION_SIZE;
    const y0 = reader.readInt32() * REGION_SIZE;
    const list = new Uint16Array(REGION_SIZE * REGION_SIZE);
    for (let i = 0; i < REGION_SIZE * REGION_SIZE; i++)
      list[i] = reader.readUint16();
    postWorker("setArea", [x0, y0, REGION_SIZE, REGION_SIZE, list], [list.buffer]);
  }
  const state = getGameHandler().getState();
  if (state instanceof PlayState) {
    state.resetCameraUpdates();
  }
  return null;
}
function net_edits(reader) {
  reader.skip(3);
  const length = reader.readUint32();
  const array = reader.readUint32Array(length - 1);
  postWorker("applyEdits", [array], [array.buffer]);
  return null;
}
let lastEntityAsk = -Infinity;
async function net_getUpdate(reader) {
  reader.skip(3);
  const msgSize = reader.readUint32();
  const prevOffset = reader.getOffset();
  const buffer = reader.readUint8Array(msgSize - 4);
  await askWorker("readEntities", [buffer], [buffer.buffer]);
  reader.setOffset(prevOffset);
  const money = reader.readInt32();
  const jobSize = reader.readInt32();
  let jobs;
  if (jobSize) {
    const offset = reader.getOffset();
    const jobLength = Math.floor(jobSize / 3);
    jobs = new Array(jobLength);
    for (let i = 0; i < jobLength; i++) {
      const type = reader.readUint32();
      const x = reader.readInt32();
      const y = reader.readInt32();
      jobs[i] = { type, x, y };
    }
    console.log(jobs);
    reader.setOffset(offset + jobSize * 4);
  } else {
    jobs = null;
  }
  const calendar = reader.readUint64();
  const carsCount = reader.readUint32();
  const cars = new Array(carsCount);
  for (let i = 0; i < carsCount; i++) {
    const x = reader.readInt32();
    const y = reader.readInt32();
    const step = reader.readFloat32();
    const speed = reader.readFloat32();
    const flag = reader.readInt32();
    const direction2 = flag & 255;
    const state2 = flag >> 8 & 255;
    cars[i] = { x, y, step, speed, direction: direction2, state: state2 };
  }
  if (carsCount % 2) {
    reader.skip(4);
  }
  const charactersCount = reader.readUint32();
  const characters = new Array(charactersCount);
  for (let i = 0; i < charactersCount; i++) {
    const ptr = reader.readUint64();
    const x = reader.readFloat32();
    const y = reader.readFloat32();
    characters[i] = { x, y, ptr };
  }
  const state = getGameHandler().getState();
  if (!(state instanceof PlayState))
    return;
  state.setCalendar(calendar);
  state.setMoney(money);
  state.cars = cars;
  state.characters = characters;
  if (jobs) {
    state.setJobs(jobs);
  }
  const now = Date.now();
  const delta = now - lastEntityAsk;
  if (delta >= ENTITY_ASK_COULDOWN) {
    lastEntityAsk = now;
    state.sendAskEntities();
  } else {
    const couldown = isFinite(lastEntityAsk) ? ENTITY_ASK_COULDOWN - delta : ENTITY_ASK_COULDOWN;
    setTimeout(() => {
      lastEntityAsk = Date.now();
      state.sendAskEntities();
    }, couldown);
  }
}
function net_panel(reader) {
  reader.skip(1);
  const id = reader.readUint16();
  resolvePanel(id, reader);
}
function net_getEntity(reader) {
  const state = getGameHandler().getState();
  if (state instanceof PlayState) {
    state.entityData.recv(reader);
  }
}
function handleMessage(reader) {
  const action = reader.readUint8();
  switch (action) {
    case CLIENT_IDS.JOIN_CREATED:
      return net_joinCreated(reader);
    case CLIENT_IDS.JOIN_ALIVE:
      return net_joinAlive(reader);
    case CLIENT_IDS.AREAS:
      return net_areas(reader);
    case CLIENT_IDS.EDITS:
      return net_edits(reader);
    case CLIENT_IDS.UPDATE:
      net_getUpdate(reader);
      return null;
    case CLIENT_IDS.PANEL:
      net_panel(reader);
      return null;
    case CLIENT_IDS.GET_ENTITY:
      net_getEntity(reader);
      return null;
    default:
      throw new Error("Unknown action " + action);
  }
}
const gameSocket = new WebSocket(window.SERV_ADDRESS);
function sendSocket(data) {
  return gameSocket.send(data);
}
gameSocket.addEventListener("open", () => {
  console.log("Socket open");
});
gameSocket.addEventListener("message", async (event) => {
  const data = event.data;
  let buffer;
  if (data instanceof ArrayBuffer) {
    buffer = data;
  } else if (data instanceof Blob) {
    buffer = await data.arrayBuffer();
  } else if (typeof data === "string") {
    buffer = new TextEncoder().encode(data).buffer;
  } else {
    throw new Error("Unsupported WebSocket message type");
  }
  const reader = new DataReader(buffer);
  const writer = handleMessage(reader);
  if (writer) {
    sendSocket(writer.toArrayBuffer());
  }
});
gameSocket.addEventListener("error", (err) => {
  console.error(err);
});
gameSocket.addEventListener("close", (event) => {
  console.log("Socket closed:", event.code, event.reason);
});
class HomeState extends GameState {
  constructor() {
    super();
  }
  enter(data, input) {
    document.getElementById("homeView")?.classList.remove("hidden");
    document.getElementById("home-createSession")?.addEventListener("click", () => this.createSession("0000000000000000"));
    document.getElementById("home-joinSession")?.addEventListener(
      "click",
      () => {
        const code = prompt("Session");
        if (code !== null)
          this.joinSession(code, "0000000000000000");
      }
    );
    input.onMouseUp = (e) => {
    };
    input.onMouseDown = (e) => {
    };
    input.onMouseMove = (e) => {
    };
    input.onScroll = (e) => {
    };
    input.onTouchStart = (e) => {
    };
    input.onTouchEnd = (e) => {
    };
    input.onTouchMove = (e) => {
    };
  }
  frame(game) {
    return null;
  }
  async draw(args) {
  }
  exit() {
    document.getElementById("homeView")?.classList.add("hidden");
  }
  getCamera() {
    return null;
  }
  createSession(playerHash) {
    const writer = new DataWriter();
    writer.writeUint8(SERVER_IDS.CONNECT);
    writer.skip(7);
    writer.write256("0000000000000000");
    writer.write256(playerHash);
    sendSocket(writer.toArrayBuffer());
  }
  joinSession(code, playerHash) {
    const writer = new DataWriter();
    writer.writeUint8(SERVER_IDS.CONNECT);
    writer.skip(7);
    writer.write256(code);
    writer.write256(playerHash);
    sendSocket(writer.toArrayBuffer());
  }
}
class TestState extends GameState {
  constructor() {
    super();
  }
  enter(data, input, imageLoader) {
    input.onMouseUp = (e) => {
    };
    input.onMouseDown = (e) => {
    };
    input.onMouseMove = (e) => {
    };
    input.onScroll = (e) => {
    };
    input.onTouchStart = (e) => {
    };
    input.onTouchEnd = (e) => {
    };
    input.onTouchMove = (e) => {
    };
  }
  test() {
  }
  frame(game) {
    return new PlayState();
  }
  async draw(args) {
  }
  exit() {
  }
  getCamera() {
    return null;
  }
}
window.switchToTestState = false;
class GameHandler {
  constructor(keyboardMode, mouseEventTarget, keydownEventTarget) {
    this.nextState = null;
    this.imgLoader = new ImageLoader(window.IMG_ROOT_PATH);
    this.inputHandler = new InputHandler(keyboardMode);
    this.inputHandler.startKeydownListeners(keydownEventTarget);
    this.inputHandler.startMouseListeners(mouseEventTarget);
    this.state = new HomeState();
    this.state.enter(void 0, this.inputHandler, this.imgLoader);
  }
  setState(state) {
    this.nextState = state;
  }
  getState() {
    return this.state;
  }
  gameLogic() {
    this.inputHandler.update();
    if (this.nextState) {
      this.state.exit();
      this.state = this.nextState;
      this.state.enter(null, this.inputHandler, this.imgLoader);
      this.nextState = null;
    }
    const next = this.state.frame(this);
    if (next) {
      const data = this.state.exit();
      this.state = next;
      next.enter(data, this.inputHandler, this.imgLoader);
    } else if (window.switchToTestState) {
      window.switchToTestState = false;
      this.state.exit();
      this.state = new TestState();
      this.state.enter(null, this.inputHandler, this.imgLoader);
    }
  }
  async gameDraw(ctx, canvasWidth, canvasHeight, drawMethod) {
    const scaleX = canvasWidth / GAME_WIDTH;
    const scaleY = canvasHeight / GAME_HEIGHT;
    const scale = Math.min(scaleX, scaleY);
    const offsetX = (canvasWidth - GAME_WIDTH * scale) / 2;
    const offsetY = (canvasHeight - GAME_HEIGHT * scale) / 2;
    ctx.save();
    ctx.translate(offsetX, offsetY);
    ctx.scale(scale, scale);
    const camera = this.state.getCamera();
    ctx.fillStyle = "black";
    ctx.fillRect(0, 0, GAME_WIDTH, GAME_HEIGHT);
    const followCamera = () => {
      ctx.save();
      if (camera) {
        ctx.translate(GAME_WIDTH / 2, GAME_HEIGHT / 2);
        ctx.scale(camera.z, camera.z);
        ctx.translate(-camera.x, -camera.y);
      }
    };
    const unfollowCamera = () => {
      ctx.restore();
    };
    await drawMethod(ctx, followCamera, unfollowCamera);
    ctx.restore();
    ctx.fillStyle = "black";
    if (offsetY > 0) ctx.fillRect(0, 0, canvasWidth, offsetY);
    if (offsetY > 0) ctx.fillRect(0, canvasHeight - offsetY, canvasWidth, offsetY);
    if (offsetX > 0) ctx.fillRect(0, 0, offsetX, canvasHeight);
    if (offsetX > 0) ctx.fillRect(canvasWidth - offsetX, 0, offsetX, canvasHeight);
  }
  drawMethod(ctx, followCamera, unfollowCamera) {
    return this.state.draw({ ctx, imageLoader: this.imgLoader, followCamera, unfollowCamera });
  }
}
window.game = null;
window.running = false;
window.useRequestAnimationFrame = true;
window.DEBUG = false;
window.startGame = startGame;
function startGame() {
  const FPS_FREQUENCY = 4;
  const EXCESS_COUNT = 70;
  const EXCESS_LIM = 4 * FPS_FREQUENCY;
  let countedFps = 0;
  let excessCount = 0;
  setInterval(() => {
    const e = document.getElementById("fps");
    const count = countedFps * FPS_FREQUENCY;
    if (excessCount >= 0) {
      if (count > EXCESS_COUNT) {
        excessCount++;
        if (excessCount >= EXCESS_LIM) {
          window.useRequestAnimationFrame = false;
          excessCount = -1;
        }
      } else {
        excessCount = 0;
      }
    }
    if (e) {
      let text = count + "fps";
      if (!window.useRequestAnimationFrame) {
        text += " (async)";
      }
      e.textContent = text;
    }
    countedFps = 0;
  }, 1e3 / FPS_FREQUENCY);
  const canvas = document.getElementById("gameCanvas");
  canvas.oncontextmenu = (e) => {
    e.preventDefault();
  };
  function resizeCanvas() {
    canvas.width = window.innerWidth;
    canvas.height = window.innerHeight;
  }
  resizeCanvas();
  window.addEventListener("resize", resizeCanvas);
  const keyboardMode = localStorage.getItem("keyboardMode");
  let realKeyboardMode;
  if (keyboardMode !== "zqsd" && keyboardMode !== "wasd") {
    realKeyboardMode = "wasd";
  } else {
    realKeyboardMode = keyboardMode;
  }
  const offscreen = new OffscreenCanvas(canvas.width, canvas.height);
  const offCtx = offscreen.getContext("2d");
  const canvasContext = canvas.getContext("2d");
  const game = new GameHandler(
    realKeyboardMode,
    canvas,
    document
  );
  setGameHandler(game);
  async function runGameLoop() {
    game.gameLogic();
    await game.gameDraw(
      offCtx,
      canvas.width,
      canvas.height,
      (ctx, followCamera, unfollowCamera) => game.drawMethod(ctx, followCamera, unfollowCamera)
    );
    canvasContext.drawImage(offscreen, 0, 0);
    if (window.running) {
      if (window.useRequestAnimationFrame) {
        requestAnimationFrame(runGameLoop);
      } else if (window.DEBUG) {
        setTimeout(runGameLoop, 1e3 / 3);
      } else {
        setTimeout(runGameLoop, 1e3 / 60);
      }
    }
    countedFps++;
  }
  window.game = game;
  window.running = true;
  runGameLoop();
}

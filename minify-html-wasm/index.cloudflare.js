import * as imports from "./index_bg.js"

// switch between both syntax for node and for workerd
import wkmod from "./index_bg.wasm"
import * as wasm from "./index_bg.wasm"
if (typeof process !== "undefined" && process.release.name === "node") {
  imports.__wbg_set_wasm(wasm)
} else {
  const instance = new WebAssembly.Instance(wkmod, { "./index_bg.js": imports })
  imports.__wbg_set_wasm(instance.exports)
}

export * from "./index_bg.js"

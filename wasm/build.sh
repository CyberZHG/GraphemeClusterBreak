#!/usr/bin/env bash

emcmake cmake .. -B wasm -DGRAPHEME_CLUSTER_BREAK_BIND_ES=ON
(cd wasm && emmake make GraphemeClusterBreakWASM)

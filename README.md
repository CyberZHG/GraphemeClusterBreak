# Grapheme Cluster Break

[![Unicode 17.0.0](https://img.shields.io/badge/Unicode-17.0.0-blue.svg)](https://www.unicode.org/versions/Unicode17.0.0/)
[![C++ Unit Tests](https://github.com/CyberZHG/GraphemeClusterBreak/actions/workflows/cpp-unit-tests.yml/badge.svg)](https://github.com/CyberZHG/GraphemeClusterBreak/actions/workflows/cpp-unit-tests.yml)
[![Python Build & Test](https://github.com/CyberZHG/GraphemeClusterBreak/actions/workflows/python-build-test.yml/badge.svg)](https://github.com/CyberZHG/GraphemeClusterBreak/actions/workflows/python-build-test.yml)
[![WASM Build & Test](https://github.com/CyberZHG/GraphemeClusterBreak/actions/workflows/wasm-build-test.yml/badge.svg)](https://github.com/CyberZHG/GraphemeClusterBreak/actions/workflows/wasm-build-test.yml)
[![](https://img.shields.io/pypi/v/grapheme-cluster-break)](https://pypi.org/project/grapheme-cluster-break/)
[![](https://img.shields.io/pypi/v/grapheme-cluster-break)](https://www.npmjs.com/package/grapheme-cluster-break)
[![Coverage Status](https://coveralls.io/repos/github/CyberZHG/GraphemeClusterBreak/badge.svg?branch=main)](https://coveralls.io/github/CyberZHG/GraphemeClusterBreak?branch=main)
![](https://visitor-badge.laobi.icu/badge?page_id=cyberzhg.GraphemeClusterBreak)

A high-performance library for Unicode text segmentation according to [UAX #29: Unicode Text Segmentation](https://www.unicode.org/reports/tr29/).

## Features

- Full compliance with Unicode 17.0 text segmentation rules
- **Grapheme Cluster Break**: Segment text into user-perceived characters
- **Word Break**: Segment text into words (optional, compile-time flag)
- **Sentence Break**: Segment text into sentences (optional, compile-time flag)
- Support for extended grapheme clusters (emoji sequences, Indic conjuncts, etc.)
- All official Unicode test cases pass
- Available for **C++**, **Python**, and **JavaScript/WebAssembly**
- Zero external dependencies for the core C++ library

## Text Segmentation Types

### Grapheme Clusters

A grapheme cluster represents what users perceive as a single character, even when it consists of multiple Unicode code points:

| Input | Grapheme Clusters | Code Points |
|-------|-------------------|-------------|
| `"hello"` | `["h", "e", "l", "l", "o"]` | 5 |
| `"é"` (e + combining accent) | `["é"]` | 2 |
| `"👨‍👩‍👧‍👦"` (family emoji) | `["👨‍👩‍👧‍👦"]` | 7 |
| `"🇨🇳"` (flag) | `["🇨🇳"]` | 2 |

### Word Break

Word segmentation identifies word boundaries in text, handling spaces, punctuation, and language-specific rules:

| Input | Words |
|-------|-------|
| `"Hello, world!"` | `["Hello", ",", " ", "world", "!"]` |
| `"can't"` | `["can't"]` |
| `"3.14"` | `["3.14"]` |

### Sentence Break

Sentence segmentation identifies sentence boundaries, handling abbreviations and special punctuation:

| Input | Sentences |
|-------|-----------|
| `"Hello. World!"` | `["Hello. ", "World!"]` |
| `"Dr. Smith went home."` | `["Dr. Smith went home."]` |

## Installation

### Python

```bash
pip install grapheme-cluster-break
```

### JavaScript (NPM)

```bash
npm install grapheme-cluster-break
```

### C++ (CMake)

Add as a subdirectory or use FetchContent:

```cmake
include(FetchContent)
FetchContent_Declare(
    GraphemeClusterBreak
    GIT_REPOSITORY https://github.com/CyberZHG/GraphemeClusterBreak.git
    GIT_TAG v1.0.0
)
FetchContent_MakeAvailable(GraphemeClusterBreak)

target_link_libraries(your_target PRIVATE GraphemeClusterBreak)
```

## Usage

### Python

```python
from grapheme_cluster_break import segment_grapheme_clusters

# Basic usage
clusters = segment_grapheme_clusters("👨‍👩‍👧‍👦")
print(clusters)  # ['👨‍👩‍👧‍👦']

# With combining characters
clusters = segment_grapheme_clusters("é")  # e + combining acute
print(clusters)  # ['é']

# Regional indicators (flags)
clusters = segment_grapheme_clusters("🇨🇳🇺🇸")
print(clusters)  # ['🇨🇳', '🇺🇸']
```

### JavaScript / TypeScript

```javascript
import { init, segmentGraphemeClusters } from "grapheme-cluster-break";

// Initialize the WASM module (required once)
await init();

// Basic usage
const clusters = segmentGraphemeClusters("👨‍👩‍👧‍👦");
console.log(clusters);  // ['👨‍👩‍👧‍👦']

// With combining characters
const combined = segmentGraphemeClusters("é");
console.log(combined);  // ['é']
```

### C++

```cpp
#include "grapheme_break.h"
#include <iostream>

int main() {
    // Grapheme cluster segmentation
    auto clusters = grapheme_break::segmentGraphemeClusters("👨‍👩‍👧‍👦");
    for (const auto& cluster : clusters) {
        std::cout << "[" << cluster << "]";
    }
    // Output: [👨‍👩‍👧‍👦]

    // Code point input
    std::vector<int32_t> codepoints = {0x0915, 0x094D, 0x0937};  // क्ष
    auto clusters2 = grapheme_break::segmentGraphemeClusters(codepoints);
    // Returns: [[0x0915, 0x094D, 0x0937]]

    return 0;
}
```

#### Word Break (requires `GRAPHEME_CLUSTER_BREAK_ENABLE_WORD_BREAK=ON`)

```cpp
#include "word_break.h"
#include <iostream>

int main() {
    auto words = word_break::segmentWords("Hello, world!");
    for (const auto& word : words) {
        std::cout << "[" << word << "]";
    }
    // Output: [Hello][,][ ][world][!]

    return 0;
}
```

#### Sentence Break (requires `GRAPHEME_CLUSTER_BREAK_ENABLE_SENTENCE_BREAK=ON`)

```cpp
#include "sentence_break.h"
#include <iostream>

int main() {
    auto sentences = sentence_break::segmentSentences("Hello. World!");
    for (const auto& sentence : sentences) {
        std::cout << "[" << sentence << "]";
    }
    // Output: [Hello. ][World!]

    return 0;
}
```

## API Reference

### `segmentGraphemeClusters(input, extended = true)`

Segments a string into grapheme clusters.

**Parameters:**
- `input` - The input string (UTF-8 encoded for C++/Python, native string for JavaScript)
- `extended` - If `true` (default), uses extended grapheme cluster rules (GB9a, GB9b, GB9c). If `false`, uses legacy rules.

**Returns:**
- An array/vector of strings, each representing one grapheme cluster.

### `segmentWords(input)` (C++ only, requires compile flag)

Segments a string into words according to Unicode word boundary rules.

**Parameters:**
- `input` - The input string (UTF-8 encoded) or vector of code points

**Returns:**
- An array/vector of strings, each representing one word segment.

### `segmentSentences(input)` (C++ only, requires compile flag)

Segments a string into sentences according to Unicode sentence boundary rules.

**Parameters:**
- `input` - The input string (UTF-8 encoded) or vector of code points

**Returns:**
- An array/vector of strings, each representing one sentence.

## Building from Source

### Prerequisites

- CMake 4.0+
- C++20 compatible compiler
- (Optional) Python 3.8+ with pybind11 for Python bindings
- (Optional) Emscripten for WebAssembly bindings

### Build Commands

```bash
# C++ library only (grapheme cluster break)
cmake -B build
cmake --build build

# With Word Break support
cmake -B build -DGRAPHEME_CLUSTER_BREAK_ENABLE_WORD_BREAK=ON
cmake --build build

# With Sentence Break support
cmake -B build -DGRAPHEME_CLUSTER_BREAK_ENABLE_SENTENCE_BREAK=ON
cmake --build build

# With all features and tests
cmake -B build \
    -DGRAPHEME_CLUSTER_BREAK_ENABLE_TESTS=ON \
    -DGRAPHEME_CLUSTER_BREAK_ENABLE_WORD_BREAK=ON \
    -DGRAPHEME_CLUSTER_BREAK_ENABLE_SENTENCE_BREAK=ON
cmake --build build
ctest --test-dir build

# Python bindings (via pip)
pip install .

# WebAssembly bindings
cd wasm
npm run build
```

### CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `GRAPHEME_CLUSTER_BREAK_ENABLE_TESTS` | `OFF` | Build unit tests |
| `GRAPHEME_CLUSTER_BREAK_ENABLE_WORD_BREAK` | `OFF` | Enable word break segmentation |
| `GRAPHEME_CLUSTER_BREAK_ENABLE_SENTENCE_BREAK` | `OFF` | Enable sentence break segmentation |
| `GRAPHEME_CLUSTER_BREAK_BIND_PYTHON` | `OFF` | Build Python bindings |
| `GRAPHEME_CLUSTER_BREAK_BIND_ES` | `OFF` | Build WebAssembly bindings |
| `GRAPHEME_CLUSTER_BREAK_ENABLE_COVERAGE` | `OFF` | Enable code coverage |
| `GRAPHEME_CLUSTER_BREAK_ENABLE_STRICT` | `OFF` | Enable strict compiler warnings |

## License

MIT License - see [LICENSE](LICENSE) for details.

## Links

- [Unicode UAX #29](https://www.unicode.org/reports/tr29/)
- [Unicode 17.0 Character Database](https://www.unicode.org/Public/17.0.0/ucd/)

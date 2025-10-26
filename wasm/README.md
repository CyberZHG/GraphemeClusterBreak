# Grapheme Cluster Break (JavaScript/WebAssembly)

[![Unicode 17.0.0](https://img.shields.io/badge/Unicode-17.0.0-blue.svg)](https://www.unicode.org/versions/Unicode17.0.0/)

A high-performance JavaScript library (powered by WebAssembly) for segmenting Unicode strings into **grapheme clusters** (user-perceived characters) according to [UAX #29: Unicode Text Segmentation](https://www.unicode.org/reports/tr29/).

## Installation

```bash
npm install grapheme-cluster-break
```

## Usage

### ES Modules

```javascript
import { segmentGraphemeClusters } from "grapheme-cluster-break";

// Basic usage
const clusters = segmentGraphemeClusters("Hello");
console.log(clusters);  // ['H', 'e', 'l', 'l', 'o']

// Emoji ZWJ sequences
const family = segmentGraphemeClusters("👨‍👩‍👧‍👦");
console.log(family);  // ['👨‍👩‍👧‍👦']

// Combining characters
const accent = segmentGraphemeClusters("é");  // e + combining acute
console.log(accent);  // ['é']

// Regional indicators (flags)
const flags = segmentGraphemeClusters("🇨🇳🇺🇸");
console.log(flags);  // ['🇨🇳', '🇺🇸']

// Indic conjuncts
const indic = segmentGraphemeClusters("क्ष");
console.log(indic);  // ['क्ष']

// CJK characters
const cjk = segmentGraphemeClusters("你好世界");
console.log(cjk);  // ['你', '好', '世', '界']

// Hangul
const hangul = segmentGraphemeClusters("한글");
console.log(hangul);  // ['한', '글']
```

### TypeScript

Full TypeScript support with type definitions included:

```typescript
import { segmentGraphemeClusters } from "grapheme-cluster-break";

const clusters: string[] = segmentGraphemeClusters("👨‍👩‍👧‍👦");
```

## API Reference

### `init(): Promise<void>`

Initializes the WebAssembly module. Must be called once before using `segmentGraphemeClusters`.

### `segmentGraphemeClusters(s: string, extended?: boolean): string[]`

Segments a string into grapheme clusters.

**Parameters:**
- `s` - The input string to segment.
- `extended` (optional, default: `true`) - If `true`, uses extended grapheme cluster rules. If `false`, uses legacy rules.

**Returns:**
- An array of strings, each representing one grapheme cluster.

**Throws:**
- `Error` if `init()` has not been called.

## Building from Source

### Prerequisites

- [Emscripten](https://emscripten.org/docs/getting_started/downloads.html)
- Node.js 18+
- CMake 4.0+

### Build Commands

```bash
# Build WASM
npm run build

# Run tests
npm test

# Lint
npm run lint
```

## Browser Usage

The library works in modern browsers that support WebAssembly and ES modules:

```html
<script type="module">
  import { segmentGraphemeClusters } from "./node_modules/grapheme-cluster-break/index.js";

  console.log(segmentGraphemeClusters("👨‍👩‍👧‍👦"));
</script>
```

## License

MIT License

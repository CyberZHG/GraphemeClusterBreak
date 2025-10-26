# Grapheme Cluster Break (Python)

[![Unicode 17.0.0](https://img.shields.io/badge/Unicode-17.0.0-blue.svg)](https://www.unicode.org/versions/Unicode17.0.0/)

A high-performance Python library for segmenting Unicode strings into **grapheme clusters** (user-perceived characters) according to [UAX #29: Unicode Text Segmentation](https://www.unicode.org/reports/tr29/).

## Installation

```bash
pip install grapheme-cluster-break
```

## Usage

```python
from grapheme_cluster_break import segment_grapheme_clusters

# Basic usage
clusters = segment_grapheme_clusters("Hello")
print(clusters)  # ['H', 'e', 'l', 'l', 'o']

# Emoji ZWJ sequences
clusters = segment_grapheme_clusters("👨‍👩‍👧‍👦")
print(clusters)  # ['👨‍👩‍👧‍👦']

# Combining characters
clusters = segment_grapheme_clusters("é")  # e + combining acute accent
print(clusters)  # ['é']

# Regional indicators (flags)
clusters = segment_grapheme_clusters("🇨🇳🇺🇸")
print(clusters)  # ['🇨🇳', '🇺🇸']

# Indic conjuncts
clusters = segment_grapheme_clusters("क्ष")  # Devanagari ksha
print(clusters)  # ['क्ष']

# CJK characters
clusters = segment_grapheme_clusters("你好世界")
print(clusters)  # ['你', '好', '世', '界']

# Hangul
clusters = segment_grapheme_clusters("한글")
print(clusters)  # ['한', '글']
```

## API Reference

### `segment_grapheme_clusters(s, extended=True)`

Segments a UTF-8 string into grapheme clusters.

**Parameters:**
- `s` (`str`) - The input string to segment.
- `extended` (`bool`, optional) - If `True` (default), uses extended grapheme cluster rules. If `False`, uses legacy rules.

**Returns:**
- `list[str]` - A list of strings, each representing one grapheme cluster.

## Building from Source

```bash
# Install build dependencies
pip install scikit-build-core pybind11

# Build and install
pip install .

# Run tests
pip install pytest
pytest python/tests/
```

## License

MIT License

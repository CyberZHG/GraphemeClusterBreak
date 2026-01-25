#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "grapheme_break.h"
using namespace std;
using namespace grapheme_break;

namespace py = pybind11;

PYBIND11_MODULE(_core, m, py::mod_gil_not_used()) {
    m.doc() = "A library for segmenting Unicode strings into grapheme clusters according to UAX #29.";

    m.def("segment_grapheme_clusters",
          py::overload_cast<const std::string&, bool>(&segmentGraphemeClusters),
          py::arg("s"), py::arg("extended") = true,
          R"pbdoc(
Segment a UTF-8 string into grapheme clusters.

This function breaks a UTF-8 encoded string into user-perceived characters
(grapheme clusters) according to Unicode Text Segmentation (UAX #29).

Args:
    s: The input UTF-8 encoded string to segment.
    extended: If True, use extended grapheme cluster rules (default).
              If False, use legacy grapheme cluster rules.

Returns:
    A list of strings, each representing one grapheme cluster.

Example:
    >>> segment_grapheme_clusters("hello")
    ['h', 'e', 'l', 'l', 'o']
    >>> segment_grapheme_clusters("👨‍👩‍👧‍👦")
    ['👨‍👩‍👧‍👦']
)pbdoc");
}

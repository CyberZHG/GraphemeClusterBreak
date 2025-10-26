#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "grapheme_cluster.h"
using namespace std;
using namespace grapheme_cluster;

namespace py = pybind11;

PYBIND11_MODULE(_core, m, py::mod_gil_not_used()) {
    m.def("segment_grapheme_clusters", py::overload_cast<const std::string&, bool>(&segmentGraphemeClusters), py::arg("s"), py::arg("extended") = true);
}

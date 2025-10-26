#include <emscripten/bind.h>
#include "grapheme_cluster.h"
using namespace emscripten;
using namespace grapheme_cluster;

EMSCRIPTEN_BINDINGS(GraphemeCluster) {
    register_vector<std::string>("VectorString");

    function("_segmentGraphemeClusters",
             select_overload<std::vector<std::string>(const std::string&, bool)>(&segmentGraphemeClusters));
}
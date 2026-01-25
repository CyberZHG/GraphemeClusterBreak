#include <emscripten/bind.h>
#include "grapheme_break.h"
using namespace emscripten;
using namespace grapheme_break;

EMSCRIPTEN_BINDINGS(GraphemeCluster) {
    register_vector<std::string>("VectorString");

    function("_segmentGraphemeClusters",
             select_overload<std::vector<std::string>(const std::string&, bool)>(&segmentGraphemeClusters));
}
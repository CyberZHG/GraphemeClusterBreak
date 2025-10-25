#ifndef GRAPHEMEBREAK_GRAPHEME_CLUSTER_H
#define GRAPHEMEBREAK_GRAPHEME_CLUSTER_H

#include <string>
#include <vector>
#include <cstdint>

namespace grapheme_cluster {

    std::vector<std::string> segmentGraphemeClusters(const std::string& s);
    std::vector<std::vector<std::int32_t>> segmentGraphemeClusters(const std::vector<std::int32_t>& codepoints);

}

#endif //GRAPHEMEBREAK_GRAPHEME_CLUSTER_H
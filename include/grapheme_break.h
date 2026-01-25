#ifndef GRAPHEME_CLUSTER_BREAK_GRAPHEME_CLUSTER_H
#define GRAPHEME_CLUSTER_BREAK_GRAPHEME_CLUSTER_H

#include <string>
#include <vector>
#include <cstdint>

namespace grapheme_break {

    /**
     * @brief Segment a UTF-8 string into grapheme clusters.
     *
     * This function breaks a UTF-8 encoded string into user-perceived characters
     * (grapheme clusters) according to Unicode Text Segmentation (UAX #29).
     *
     * @param s The input UTF-8 encoded string to segment.
     * @param extended If true, use extended grapheme cluster rules (default).
     *                 If false, use legacy grapheme cluster rules.
     * @return A vector of UTF-8 strings, each representing one grapheme cluster.
     */
    std::vector<std::string> segmentGraphemeClusters(const std::string& s, bool extended = true);

    /**
     * @brief Segment a sequence of Unicode code points into grapheme clusters.
     *
     * This function breaks a sequence of Unicode code points into user-perceived
     * characters (grapheme clusters) according to Unicode Text Segmentation (UAX #29).
     *
     * @param codepoints The input vector of Unicode code points (as int32_t).
     * @param extended If true, use extended grapheme cluster rules (default).
     *                 If false, use legacy grapheme cluster rules.
     * @return A vector of vectors, where each inner vector contains the code points
     *         of one grapheme cluster.
     */
    std::vector<std::vector<std::int32_t>> segmentGraphemeClusters(const std::vector<std::int32_t>& codepoints, bool extended = true);

}

#endif //GRAPHEME_CLUSTER_BREAK_GRAPHEME_CLUSTER_H
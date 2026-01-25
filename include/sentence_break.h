#ifndef GRAPHEME_CLUSTER_BREAK_SENTENCE_BREAK_H
#define GRAPHEME_CLUSTER_BREAK_SENTENCE_BREAK_H

#include <string>
#include <vector>
#include <cstdint>

namespace sentence_break {

    /**
     * @brief Segment a UTF-8 string into sentences.
     *
     * This function breaks a UTF-8 encoded string into sentences
     * according to Unicode Text Segmentation (UAX #29).
     *
     * @param s The input UTF-8 encoded string to segment.
     * @return A vector of UTF-8 strings, each representing one sentence.
     */
    std::vector<std::string> segmentSentences(const std::string& s);

    /**
     * @brief Segment a sequence of Unicode code points into sentences.
     *
     * This function breaks a sequence of Unicode code points into sentences
     * according to Unicode Text Segmentation (UAX #29).
     *
     * @param codepoints The input vector of Unicode code points (as int32_t).
     * @return A vector of vectors, where each inner vector contains the code points
     *         of one sentence.
     */
    std::vector<std::vector<std::int32_t>> segmentSentences(const std::vector<std::int32_t>& codepoints);

}

#endif //GRAPHEME_CLUSTER_BREAK_SENTENCE_BREAK_H
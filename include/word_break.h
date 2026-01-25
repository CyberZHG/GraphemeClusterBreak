#ifndef GRAPHEMEBREAK_WORD_BREAK_H
#define GRAPHEMEBREAK_WORD_BREAK_H

#include <string>
#include <vector>
#include <cstdint>

namespace word_break {

    /**
     * @brief Segment a UTF-8 string into words.
     *
     * This function breaks a UTF-8 encoded string into words
     * according to Unicode Text Segmentation (UAX #29).
     *
     * @param s The input UTF-8 encoded string to segment.
     * @return A vector of UTF-8 strings, each representing one word.
     */
    std::vector<std::string> segmentWords(const std::string& s);

    /**
     * @brief Segment a sequence of Unicode code points into words.
     *
     * This function breaks a sequence of Unicode code points into words
     * according to Unicode Text Segmentation (UAX #29).
     *
     * @param codepoints The input vector of Unicode code points (as int32_t).
     * @return A vector of vectors, where each inner vector contains the code points
     *         of one word.
     */
    std::vector<std::vector<std::int32_t>> segmentWords(const std::vector<std::int32_t>& codepoints);

}

#endif //GRAPHEMEBREAK_WORD_BREAK_H

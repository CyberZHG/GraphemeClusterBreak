#ifndef GRAPHEMEBREAK_UNICODE_UTILS_H
#define GRAPHEMEBREAK_UNICODE_UTILS_H

#include <string>
#include <vector>
#include <cstdint>

namespace unicode_utils {

    /**
     * @brief Convert a UTF-8 encoded string to a vector of Unicode code points.
     *
     * @param utf8 The input UTF-8 encoded string.
     * @return A vector of Unicode code points (as int32_t).
     */
    std::vector<std::int32_t> utf8ToCodepoints(const std::string& utf8);

    /**
     * @brief Convert a vector of Unicode code points to a UTF-8 encoded string.
     *
     * @param codepoints The input vector of Unicode code points.
     * @return A UTF-8 encoded string.
     */
    std::string codepointsToUtf8(const std::vector<std::int32_t>& codepoints);

}

#endif //GRAPHEMEBREAK_UNICODE_UTILS_H

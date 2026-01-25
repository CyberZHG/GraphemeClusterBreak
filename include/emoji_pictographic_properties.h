#ifndef GRAPHEME_CLUSTER_BREAK_EMOJI_PICTOGRAPHIC_PROPERTIES_H
#define GRAPHEME_CLUSTER_BREAK_EMOJI_PICTOGRAPHIC_PROPERTIES_H

#include <cstdint>

namespace grapheme_break {

    bool isExtendedPictographic(std::int32_t code);

    /** For unit tests only. */
    bool isExtendedPictographicBruteForce(std::int32_t code);

}

#endif //GRAPHEME_CLUSTER_BREAK_EMOJI_PICTOGRAPHIC_PROPERTIES_H
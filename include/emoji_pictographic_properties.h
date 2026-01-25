#ifndef GRAPHEMECLUSTERBREAK_EMOJI_PICTOGRAPHIC_PROPERTIES_H
#define GRAPHEMECLUSTERBREAK_EMOJI_PICTOGRAPHIC_PROPERTIES_H

#include <cstdint>

namespace grapheme_break {

    bool isExtendedPictographic(std::int32_t code);

    /** For unit tests only. */
    bool isExtendedPictographicBruteForce(std::int32_t code);

}

#endif //GRAPHEMECLUSTERBREAK_EMOJI_PICTOGRAPHIC_PROPERTIES_H
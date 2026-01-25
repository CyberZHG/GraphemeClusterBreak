#ifndef GRAPHEMEBREAK_PROPERTIES_H
#define GRAPHEMEBREAK_PROPERTIES_H

#include <cstdint>
#include <ostream>

namespace grapheme_break {

    enum class GraphemeClusterBreakProperty {
        CR,
        LF,
        Control,
        L, V, T, LV, LVT,  // Hangul syllable
        Extend, ZWJ, SpacingMark, Prepend,
        Regional_Indicator,  // Emoji
        Other,
    };

    enum class IndicConjunctBreakProperty {
        InCB_Consonant, InCB_Linker, InCB_Extend,
        InCB_Other,
    };

    std::ostream& operator<<(std::ostream& os, GraphemeClusterBreakProperty property);
    std::ostream& operator<<(std::ostream& os, IndicConjunctBreakProperty property);

    GraphemeClusterBreakProperty findBreakProperty(std::int32_t code);
    IndicConjunctBreakProperty findIndicBreakProperty(std::int32_t code);

    /** For unit tests only. */
    GraphemeClusterBreakProperty findBreakPropertyBruteForce(std::int32_t code);
    /** For unit tests only. */
    IndicConjunctBreakProperty findIndicBreakPropertyBruteForce(std::int32_t code);

}

#endif //GRAPHEMEBREAK_PROPERTIES_H
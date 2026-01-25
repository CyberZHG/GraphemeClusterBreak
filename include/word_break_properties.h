#ifndef GRAPHEME_CLUSTER_BREAK_WORD_BREAK_PROPERTIES_H
#define GRAPHEME_CLUSTER_BREAK_WORD_BREAK_PROPERTIES_H

#include <cstdint>
#include <ostream>

namespace word_break {

    enum class WordBreakProperty {
        CR,
        LF,
        Newline,
        Extend,
        ZWJ,
        Regional_Indicator,
        Format,
        Katakana,
        Hebrew_Letter,
        ALetter,
        Single_Quote,
        Double_Quote,
        MidNumLet,
        MidLetter,
        MidNum,
        Numeric,
        ExtendNumLet,
        WSegSpace,
        Other,
    };

    std::ostream& operator<<(std::ostream& os, WordBreakProperty property);

    WordBreakProperty findWordBreakProperty(std::int32_t code);

    /** For unit tests only. */
    WordBreakProperty findWordBreakPropertyBruteForce(std::int32_t code);

}

#endif //GRAPHEME_CLUSTER_BREAK_WORD_BREAK_PROPERTIES_H

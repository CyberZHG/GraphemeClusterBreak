#ifndef GRAPHEME_CLUSTER_BREAK_SENTENCE_BREAK_PROPERTIES_H
#define GRAPHEME_CLUSTER_BREAK_SENTENCE_BREAK_PROPERTIES_H

#include <cstdint>
#include <ostream>

namespace sentence_break {

    enum class SentenceBreakProperty {
        CR,
        LF,
        Sep,
        Extend,
        Format,
        STerm,
        ATerm,
        Numeric,
        Upper,
        Sp,
        Lower,
        OLetter,
        SContinue,
        Close,
        Other,
    };

    std::ostream& operator<<(std::ostream& os, SentenceBreakProperty property);

    SentenceBreakProperty findSentenceBreakProperty(std::int32_t code);

    /** For unit tests only. */
    SentenceBreakProperty findSentenceBreakPropertyBruteForce(std::int32_t code);

}

#endif //GRAPHEME_CLUSTER_BREAK_SENTENCE_BREAK_PROPERTIES_H
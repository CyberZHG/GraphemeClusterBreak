#ifndef GRAPHEMEBREAK_PROPERTIES_H
#define GRAPHEMEBREAK_PROPERTIES_H

namespace grapheme_break {

enum class GraphemeClusterBreakProperty {
    CR,
    LF,
    Control,
    L, V, T, LV, LVT,  // Hangul syllable
    Extend, ZWJ, SpacingMark, Prepend,
    InCB_Consonant, InCB_Linker, InCB_Extend,  // Indic conjunct break
    Extended_Pictographic, Regional_Indicator,  // Emoji
    Other,
};

}

#endif //GRAPHEMEBREAK_PROPERTIES_H
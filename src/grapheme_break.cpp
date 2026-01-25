#include "grapheme_break.h"
#include "grapheme_break_properties.h"
#include "unicode_utils.h"

#include <format>

namespace grapheme_break {

    static constexpr auto CR = GraphemeClusterBreakProperty::CR;
    static constexpr auto LF = GraphemeClusterBreakProperty::LF;
    static constexpr auto Control = GraphemeClusterBreakProperty::Control;
    static constexpr auto L = GraphemeClusterBreakProperty::L;
    static constexpr auto V = GraphemeClusterBreakProperty::V;
    static constexpr auto T = GraphemeClusterBreakProperty::T;
    static constexpr auto LV = GraphemeClusterBreakProperty::LV;
    static constexpr auto LVT = GraphemeClusterBreakProperty::LVT;
    static constexpr auto Extend = GraphemeClusterBreakProperty::Extend;
    static constexpr auto ZWJ = GraphemeClusterBreakProperty::ZWJ;
    static constexpr auto SpacingMark = GraphemeClusterBreakProperty::SpacingMark;
    static constexpr auto Prepend = GraphemeClusterBreakProperty::Prepend;
    static constexpr auto Extended_Pictographic = GraphemeClusterBreakProperty::Extended_Pictographic;
    static constexpr auto Regional_Indicator = GraphemeClusterBreakProperty::Regional_Indicator;

    static constexpr auto InCB_Consonant = IndicConjunctBreakProperty::InCB_Consonant;
    static constexpr auto InCB_Linker = IndicConjunctBreakProperty::InCB_Linker;
    static constexpr auto InCB_Extend = IndicConjunctBreakProperty::InCB_Extend;

    std::vector<std::string> segmentGraphemeClusters(const std::string& s, const bool extended) {
        if (s.empty()) {
            return {};
        }
        const auto codepoints = unicode_utils::utf8ToCodepoints(s);
        const auto segments = segmentGraphemeClusters(codepoints, extended);
        std::vector<std::string> result(segments.size());
        for (size_t i = 0; i < segments.size(); ++i) {
            result[i] = unicode_utils::codepointsToUtf8(segments[i]);
        }
        return result;
    }

    enum class EmojiModifierState {
        // GB11: \p{Extended_Pictographic} Extend* ZWJ × \p{Extended_Pictographic}
        Initial, Pictographic, ZWJ,
    };

    enum class IndicState {
        // GB9c: \p{InCB=Consonant} [ \p{InCB=Extend} \p{InCB=Linker} ]*
        //       \p{InCB=Linker}
        //       [ \p{InCB=Extend} \p{InCB=Linker} ]* × \p{InCB=Consonant}
        Initial, Consonant, Linker,
    };

    std::vector<std::vector<std::int32_t>> segmentGraphemeClusters(const std::vector<std::int32_t>& codepoints, const bool extended) {
        if (codepoints.empty()) {
            return {};
        }
        const auto n = codepoints.size();
        if (n > std::numeric_limits<std::vector<std::int32_t>::difference_type>::max()) {
            throw std::runtime_error(std::format("Can not process large vector with size: {}", n));
        }
        // GB1: sot ÷ Any
        std::vector<std::int32_t>::difference_type lastBreak = 0;
        auto currentBreakProperty = findBreakProperty(codepoints[0]);
        auto indicState = IndicState::Initial;
        if (extended) {
            if (findIndicBreakProperty(codepoints[0]) == InCB_Consonant) {
                indicState = IndicState::Consonant;
            }
        }
        auto emojiModifierState = EmojiModifierState::Initial;
        if (currentBreakProperty == Extended_Pictographic) {
            emojiModifierState = EmojiModifierState::Pictographic;
        }
        size_t regionIndicatorCount = 0;
        if (currentBreakProperty == Regional_Indicator) {
            regionIndicatorCount = 1;
        }
        std::vector<std::vector<std::int32_t>> result;
        for (size_t i = 0; i + 1 < n; ++i) {
            const auto nextBreakProperty = findBreakProperty(codepoints[i + 1]);
            const auto nextIndicBreakProperty = findIndicBreakProperty(codepoints[i + 1]);

            bool breakCluster = false;
            if (currentBreakProperty == CR && nextBreakProperty == LF) {
                // GB3: CR × LF
            } else if (currentBreakProperty == Control || currentBreakProperty == CR || currentBreakProperty == LF) {
                // GB4: (Control | CR | LF)	÷
                breakCluster = true;
            } else if (nextBreakProperty == Control || nextBreakProperty == CR || nextBreakProperty == LF) {
                // GB5: ÷ (Control | CR | LF)
                breakCluster = true;
            } else if (currentBreakProperty == L && (nextBreakProperty == L || nextBreakProperty == V
                || nextBreakProperty == LV || nextBreakProperty == LVT)) {
                // GB6: L × (L | V | LV | LVT)
            } else if ((currentBreakProperty == LV || currentBreakProperty == V)
                && (nextBreakProperty == V || nextBreakProperty == T)) {
                // GB7: (LV | V) × (V | T)
            } else if ((currentBreakProperty == LVT || currentBreakProperty == T) && nextBreakProperty == T) {
                // GB8: (LVT | T) × T
            } else if (nextBreakProperty == Extend || nextBreakProperty == ZWJ) {
                // GB9: × (Extend | ZWJ)
            } else if (extended && nextBreakProperty == SpacingMark) {
                // GB9a: × SpacingMark
            } else if (extended && currentBreakProperty == Prepend) {
                // GB9b: Prepend ×
            } else if (extended && indicState == IndicState::Linker && nextIndicBreakProperty == InCB_Consonant) {
                // GB9c: \p{InCB=Consonant} [ \p{InCB=Extend} \p{InCB=Linker} ]*
                //       \p{InCB=Linker}
                //       [ \p{InCB=Extend} \p{InCB=Linker} ]* × \p{InCB=Consonant}
            } else if (emojiModifierState == EmojiModifierState::ZWJ && nextBreakProperty == Extended_Pictographic) {
                // GB11: \p{Extended_Pictographic} Extend* ZWJ × \p{Extended_Pictographic}
            } else if (nextBreakProperty == Regional_Indicator && regionIndicatorCount % 2 == 1) {
                // GB12: sot (RI RI)* RI × RI
                // GB13: [^RI] (RI RI)* RI × RI
            } else {
                // GB999: Any ÷ Any
                breakCluster = true;
            }

            // Update indic conjunct break state
            if (extended) {
                switch (indicState) {
                    case IndicState::Initial:
                        break;
                    case IndicState::Consonant:
                        if (nextIndicBreakProperty == InCB_Extend) {
                            indicState = IndicState::Consonant;
                        } else if (nextIndicBreakProperty == InCB_Linker) {
                            indicState = IndicState::Linker;
                        } else {
                            indicState = IndicState::Initial;
                        }
                        break;
                    case IndicState::Linker:
                        if (nextIndicBreakProperty == InCB_Extend || nextIndicBreakProperty == InCB_Linker) {
                            indicState = IndicState::Linker;
                        } else {
                            indicState = IndicState::Initial;
                        }
                        break;
                }
                if (indicState == IndicState::Initial && nextIndicBreakProperty == InCB_Consonant) {
                    indicState = IndicState::Consonant;
                }
            }

            // Update emoji modifier state
            switch (emojiModifierState) {
                case EmojiModifierState::Initial:
                    break;
                case EmojiModifierState::Pictographic:
                    if (nextBreakProperty == Extend) {
                        emojiModifierState = EmojiModifierState::Pictographic;
                    } else if (nextBreakProperty == ZWJ) {
                        emojiModifierState = EmojiModifierState::ZWJ;
                    } else {
                        emojiModifierState = EmojiModifierState::Initial;
                    }
                    break;
                case EmojiModifierState::ZWJ:
                    emojiModifierState = EmojiModifierState::Initial;
                    break;
            }
            if (emojiModifierState == EmojiModifierState::Initial && nextBreakProperty == Extended_Pictographic) {
                emojiModifierState = EmojiModifierState::Pictographic;
            }

            // Update regional indicator state
            if (nextBreakProperty == Regional_Indicator) {
                ++regionIndicatorCount;
            } else {
                regionIndicatorCount = 0;
            }

            if (breakCluster) {
                const auto offset = static_cast<std::vector<std::int32_t>::difference_type>(i + 1);
                result.emplace_back(codepoints.begin() + lastBreak, codepoints.begin() + offset);
                lastBreak = offset;
            }
            currentBreakProperty = nextBreakProperty;
        }
        // GB2: Any ÷ eot
        result.emplace_back(codepoints.begin() + lastBreak, codepoints.end());
        return result;
    }

}

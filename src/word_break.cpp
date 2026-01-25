#include "word_break.h"
#include "word_break_properties.h"
#include "unicode_utils.h"

#include <format>

namespace word_break {

    static constexpr auto CR = WordBreakProperty::CR;
    static constexpr auto LF = WordBreakProperty::LF;
    static constexpr auto Newline = WordBreakProperty::Newline;
    static constexpr auto Extend = WordBreakProperty::Extend;
    static constexpr auto ZWJ = WordBreakProperty::ZWJ;
    static constexpr auto Regional_Indicator = WordBreakProperty::Regional_Indicator;
    static constexpr auto Format = WordBreakProperty::Format;
    static constexpr auto Katakana = WordBreakProperty::Katakana;
    static constexpr auto Hebrew_Letter = WordBreakProperty::Hebrew_Letter;
    static constexpr auto ALetter = WordBreakProperty::ALetter;
    static constexpr auto Single_Quote = WordBreakProperty::Single_Quote;
    static constexpr auto Double_Quote = WordBreakProperty::Double_Quote;
    static constexpr auto MidNumLet = WordBreakProperty::MidNumLet;
    static constexpr auto MidLetter = WordBreakProperty::MidLetter;
    static constexpr auto MidNum = WordBreakProperty::MidNum;
    static constexpr auto Numeric = WordBreakProperty::Numeric;
    static constexpr auto ExtendNumLet = WordBreakProperty::ExtendNumLet;
    static constexpr auto WSegSpace = WordBreakProperty::WSegSpace;
    static constexpr auto Extended_Pictographic = WordBreakProperty::Extended_Pictographic;
    static constexpr auto Other = WordBreakProperty::Other;

    // Helper: AHLetter = ALetter | Hebrew_Letter
    static bool isAHLetter(const WordBreakProperty prop) {
        return prop == ALetter || prop == Hebrew_Letter;
    }

    // Helper: MidNumLetQ = MidNumLet | Single_Quote
    static bool isMidNumLetQ(const WordBreakProperty prop) {
        return prop == MidNumLet || prop == Single_Quote;
    }

    // Helper: Check if property should be ignored for WB4
    static bool isIgnoredForWB4(const WordBreakProperty prop) {
        return prop == Extend || prop == Format || prop == ZWJ;
    }

    std::vector<std::string> segmentWords(const std::string& s) {
        if (s.empty()) {
            return {};
        }
        const auto codepoints = unicode_utils::utf8ToCodepoints(s);
        const auto segments = segmentWords(codepoints);
        std::vector<std::string> result(segments.size());
        for (size_t i = 0; i < segments.size(); ++i) {
            result[i] = unicode_utils::codepointsToUtf8(segments[i]);
        }
        return result;
    }

    std::vector<std::vector<std::int32_t>> segmentWords(const std::vector<std::int32_t>& codepoints) {
        if (codepoints.empty()) {
            return {};
        }
        const auto n = codepoints.size();
        if (n > std::numeric_limits<std::vector<std::int32_t>::difference_type>::max()) {
            throw std::runtime_error(std::format("Can not process large vector with size: {}", n));
        }

        // Precompute properties
        std::vector<WordBreakProperty> props(n);
        for (size_t i = 0; i < n; ++i) {
            props[i] = findWordBreakProperty(codepoints[i]);
        }

        // Find next non-ignored property (for WB4)
        auto findNextProp = [&](const size_t pos) -> WordBreakProperty {
            for (size_t j = pos; j < n; ++j) {
                if (!isIgnoredForWB4(props[j])) {
                    return props[j];
                }
            }
            return props[n - 1];
        };

        // Find property at offset after pos, skipping ignored (for lookahead rules)
        auto findNextNextProp = [&](const size_t pos) -> WordBreakProperty {
            size_t count = 0;
            for (size_t j = pos; j < n; ++j) {
                if (!isIgnoredForWB4(props[j])) {
                    if (count == 1) return props[j];
                    ++count;
                }
            }
            return props[n - 1];
        };

        // WB1: sot ÷ Any
        std::vector<std::int32_t>::difference_type lastBreak = 0;
        size_t regionIndicatorCount = 0;
        if (props[0] == Regional_Indicator) {
            regionIndicatorCount = 1;
        }

        auto prevEffective = Other;
        auto prevPrevEffective = Other;
        std::vector<std::vector<std::int32_t>> result;
        for (size_t i = 0; i + 1 < n; ++i) {
            const auto currentProp = props[i];
            const auto nextProp = props[i + 1];

            const auto nextEffective = findNextProp(i + 1);
            const auto nextNextEffective = findNextNextProp(i + 1);

            if (!isIgnoredForWB4(currentProp)) {
                prevPrevEffective = prevEffective;
                prevEffective = currentProp;
            }

            bool breakWord = false;
            if (currentProp == CR && nextProp == LF) {
                // WB3: CR × LF
            } else if (currentProp == Newline || currentProp == CR || currentProp == LF) {
                // WB3a: (Newline | CR | LF) ÷
                breakWord = true;
            } else if (nextProp == Newline || nextProp == CR || nextProp == LF) {
                // WB3b: ÷ (Newline | CR | LF)
                breakWord = true;
            } else if (currentProp == ZWJ && nextProp == Extended_Pictographic) {
                // WB3c: ZWJ × \p{Extended_Pictographic}
            } else if (currentProp == WSegSpace && nextProp == WSegSpace) {
                // WB3d: WSegSpace × WSegSpace
            } else if (isIgnoredForWB4(nextProp)) {
                // WB4: X (Extend | Format | ZWJ)* → X
            } else if (isAHLetter(prevEffective) && isAHLetter(nextEffective)) {
                // WB5: AHLetter × AHLetter
            } else if (isAHLetter(prevEffective)
                && (nextEffective == MidLetter || isMidNumLetQ(nextEffective))
                && isAHLetter(nextNextEffective)) {
                // WB6: AHLetter × (MidLetter | MidNumLetQ) AHLetter
            } else if (isAHLetter(prevPrevEffective)
                && (prevEffective == MidLetter || isMidNumLetQ(prevEffective))
                && isAHLetter(nextEffective)) {
                // WB7: AHLetter (MidLetter | MidNumLetQ) × AHLetter
            } else if (prevEffective == Hebrew_Letter && nextEffective == Single_Quote) {
                // WB7a: Hebrew_Letter × Single_Quote
            } else if (prevEffective == Hebrew_Letter && nextEffective == Double_Quote && nextNextEffective == Hebrew_Letter) {
                // WB7b: Hebrew_Letter × Double_Quote Hebrew_Letter
            } else if (prevPrevEffective == Hebrew_Letter && prevEffective == Double_Quote && nextEffective == Hebrew_Letter) {
                // WB7c: Hebrew_Letter Double_Quote × Hebrew_Letter
            } else if (prevEffective == Numeric && nextEffective == Numeric) {
                // WB8: Numeric × Numeric
            } else if (isAHLetter(prevEffective) && nextEffective == Numeric) {
                // WB9: AHLetter × Numeric
            } else if (prevEffective == Numeric && isAHLetter(nextEffective)) {
                // WB10: Numeric × AHLetter
            } else if (prevPrevEffective == Numeric && (prevEffective == MidNum || isMidNumLetQ(prevEffective))
                && nextEffective == Numeric) {
                // WB11: Numeric (MidNum | MidNumLetQ) × Numeric
            } else if (prevEffective == Numeric
                && (nextEffective == MidNum || isMidNumLetQ(nextEffective))
                && nextNextEffective == Numeric) {
                // WB12: Numeric × (MidNum | MidNumLetQ) Numeric
            } else if (prevEffective == Katakana && nextEffective == Katakana) {
                // WB13: Katakana × Katakana
            } else if ((isAHLetter(prevEffective) || prevEffective == Numeric || prevEffective == Katakana || prevEffective == ExtendNumLet)
                && nextEffective == ExtendNumLet) {
                // WB13a: (AHLetter | Numeric | Katakana | ExtendNumLet) × ExtendNumLet
            }
            // WB13b: ExtendNumLet × (AHLetter | Numeric | Katakana)
            else if (prevEffective == ExtendNumLet
                && (isAHLetter(nextEffective) || nextEffective == Numeric || nextEffective == Katakana)) {
                // Do not break
            } else if (nextEffective == Regional_Indicator && regionIndicatorCount % 2 == 1) {
                // WB15: sot (RI RI)* RI × RI
                // WB16: [^RI] (RI RI)* RI × RI
            } else {
                // WB999: Any ÷ Any
                breakWord = true;
            }

            // Update regional indicator count
            if (!isIgnoredForWB4(nextProp)) {
                if (nextProp == Regional_Indicator) {
                    ++regionIndicatorCount;
                } else {
                    regionIndicatorCount = 0;
                }
            }

            if (breakWord) {
                const auto offset = static_cast<std::vector<std::int32_t>::difference_type>(i + 1);
                result.emplace_back(codepoints.begin() + lastBreak, codepoints.begin() + offset);
                lastBreak = offset;
            }
        }

        // WB2: Any ÷ eot
        result.emplace_back(codepoints.begin() + lastBreak, codepoints.end());
        return result;
    }

}
